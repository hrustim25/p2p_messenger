#include "client.h"
#include "security.h"

#include <iostream>

namespace msgr {

Client::Client(const std::string& server_address)
    : message_handler_(std::make_shared<msgr::MessageHandler>()),
      account_handler_(),
      client_service_(this, message_handler_) {

    // Load certificates and keys
    server_cert_ = Security::LoadFile("servercrt.pem");
    client_key_ = Security::LoadFile("clientkey.pem");
    client_cert_ = Security::LoadFile("clientcert.pem");
    if (client_key_.empty() || client_cert_.empty()) {
        Security::GenerateAndSaveCerts();
        client_key_ = Security::LoadFile("clientkey.pem");
        client_cert_ = Security::LoadFile("clientcert.pem");
    }

    // Set up client credentials (for server)
    ::grpc::SslCredentialsOptions ssl_options;
    ssl_options.pem_root_certs = server_cert_;
    ssl_options.pem_private_key = client_key_;
    ssl_options.pem_cert_chain = client_cert_;

    if (ssl_options.pem_root_certs.empty()) {
        channel_ = ::grpc::CreateChannel(server_address, ::grpc::InsecureChannelCredentials());
    } else {
        channel_ = ::grpc::CreateChannel(server_address, ::grpc::SslCredentials(ssl_options));
    }
    stub_ = ::msgr::grpc::ClientToServerCaller::NewStub(channel_);

    std::cout << "Write client recieve address:" << std::endl;
    std::cin >> recieve_address_;

    // Set up client credentials (for clients)
    ::grpc::SslServerCredentialsOptions ssl_server_options;
    ::grpc::SslServerCredentialsOptions::PemKeyCertPair server_key_pair;
    server_key_pair.private_key = client_key_;
    server_key_pair.cert_chain = client_cert_;
    ssl_server_options.pem_key_cert_pairs.push_back(server_key_pair);
    ssl_server_options.client_certificate_request = ::grpc_ssl_client_certificate_request_type::
        GRPC_SSL_REQUEST_CLIENT_CERTIFICATE_BUT_DONT_VERIFY;

    ::grpc::ServerBuilder builder;
    builder.AddListeningPort(recieve_address_, ::grpc::SslServerCredentials(ssl_server_options));
    builder.RegisterService(&client_service_);
    reciever_server_ = builder.BuildAndStart();
    std::cout << "Client listening on " << recieve_address_ << std::endl;

    if (account_handler_.IsRegistered()) {
        std::cout << "Loaded account. Id: " << account_handler_.GetAccountData().account_id
                  << std::endl;
        UpdateData();
    }
}

void Client::Register() {
    if (account_handler_.IsRegistered()) {
        std::cout << "Already registered." << std::endl;
        return;
    }
    ::grpc::ClientContext context;
    ::msgr::grpc::RegistrationRequest request;
    request.set_address(recieve_address_);
    ::msgr::grpc::RegistrationResponse response;

    ::grpc::Status status = stub_->Register(&context, request, &response);
    if (status.ok()) {
        if (response.status() != "-1") {
            AccountData acc_data{response.user_id()};
            account_handler_.SetAccountData(acc_data);
            std::cout << "Registration complete. ID: " << response.user_id() << std::endl;
        } else {
            std::cout << "Registration failed. Server response: " << response.status() << std::endl;
        }
    } else {
        std::cout << "RPC failed. Error message: " << status.error_message() << std::endl;
    }
}

void Client::UpdateData() {
    if (!account_handler_.IsRegistered()) {
        std::cout << "You have to register first." << std::endl;
        return;
    }

    /*update recieve address*/

    ::grpc::ClientContext context;
    ::msgr::grpc::UpdateDataRequest request;
    request.set_address(recieve_address_);
    request.set_user_id(account_handler_.GetAccountData().account_id);
    ::msgr::grpc::UpdateDataResponse response;

    ::grpc::Status status = stub_->UpdateData(&context, request, &response);
    if (status.ok()) {
        if (response.status() != "-1") {
            std::cout << "Data update complete." << std::endl;
        } else {
            std::cout << "Data update failed. Server response: " << response.status() << std::endl;
        }
    } else {
        std::cout << "RPC failed. Error message: " << status.error_message() << std::endl;
    }
}

bool Client::SendMessage(const std::string& client_id, const std::string& msg) {
    if (!account_handler_.IsRegistered()) {
        std::cout << "You have to register first." << std::endl;
        return false;
    }

    ClientData client_data = GetClientData(client_id);

    ::grpc::SslCredentialsOptions ssl_options;
    ssl_options.pem_root_certs = client_data.certificate;
    ssl_options.pem_private_key = client_key_;
    ssl_options.pem_cert_chain = client_cert_;

    std::shared_ptr<::grpc::Channel> client_channel(
        ::grpc::CreateChannel(client_data.address, ::grpc::SslCredentials(ssl_options)));
    std::unique_ptr<::msgr::grpc::ClientToClientCaller::Stub> current_stub(
        ::msgr::grpc::ClientToClientCaller::NewStub(client_channel));

    ::grpc::ClientContext context;
    ::msgr::grpc::SendMessageRequest request;
    request.set_source_id(account_handler_.GetAccountData().account_id);
    request.set_destination_id(client_id);
    request.set_msg(msg);
    request.set_msg_number(message_handler_->GetMessageCount(client_id));
    ::msgr::grpc::SendMessageResponse response;

    ::grpc::Status status = current_stub->SendMessage(&context, request, &response);
    if (status.ok()) {
        MessageData msg_entity{request.source_id(), request.msg(), request.msg_number()};
        MessageHandler::MessageHandleStatus message_status =
            message_handler_->SaveMessage(client_id, msg_entity);

        std::cout << "Message delivery success." << std::endl;
        return true;
    } else {
        std::cout << "RPC failed. Error: " << status.error_message() << std::endl;
        return false;
    }
}

std::vector<MessageData> Client::GetMessages(const std::string& client_id) const {
    return message_handler_->GetMessages(client_id);
}

Client::ClientData Client::GetClientData(const std::string& client_id) {
    ::grpc::ClientContext context;
    ::msgr::grpc::ClientAddressRequest request;
    request.set_receiver_id(account_handler_.GetAccountData().account_id);
    request.set_client_id(client_id);
    ::msgr::grpc::ClientAddressResponse response;

    ::grpc::Status status = stub_->GetClientAddress(&context, request, &response);
    if (status.ok()) {
        std::cout << "Get client ip complete. IP: " << response.receiver_address() << std::endl;
        return ClientData{.address = response.receiver_address(),
                          .certificate = response.receiver_certificate()};
    } else {
        std::cout << "RPC failed. Error message: " << status.error_message() << std::endl;
        return ClientData{};
    }
}

Client::ClientToClientService::ClientToClientService(
    Client* client, std::shared_ptr<MessageHandler> message_handler)
    : client_(client), message_handler_(message_handler) {
}

::grpc::Status Client::ClientToClientService::SendMessage(
    ::grpc::ServerContext* context, const ::msgr::grpc::SendMessageRequest* request,
    ::msgr::grpc::SendMessageResponse* response) {
    if (context->auth_context()->FindPropertyValues("x509_pem_cert").empty()) {
        return ::grpc::Status(::grpc::StatusCode::UNAUTHENTICATED, "Valid certificate not found");
    }
    ClientData client_data = client_->GetClientData(request->source_id());
    if (client_data.certificate !=
        context->auth_context()->FindPropertyValues("x509_pem_cert")[0]) {
        return ::grpc::Status(::grpc::StatusCode::UNAUTHENTICATED, "Certificates do not match");
    }

    MessageData msg_entity{request->source_id(), request->msg(), request->msg_number()};

    MessageHandler::MessageHandleStatus message_status =
        message_handler_->SaveMessage(request->source_id(), msg_entity);
    if (message_status == MessageHandler::MessageHandleStatus::OK) {
        response->set_status("ok");
        std::cout << "From " << msg_entity.sender_id_ << " got message: " << msg_entity.msg_
                  << std::endl;
    } else {
        if (message_status == MessageHandler::MessageHandleStatus::NUMBER_ALREADY_EXISTS) {
            response->set_status("Message with this number already exists.");
        } else {
            response->set_status("Message number is too high.");
        }
    }
    return ::grpc::Status::OK;
}
}  // namespace msgr
