#include "client.h"

#include <iostream>
#include <fstream>

namespace msgr {

static std::string ReadCertFile(const std::string& filename) {
    std::ifstream cert_fstream(filename);
    std::string result;
    if (!cert_fstream.is_open()) {
        return result;
    }
    while (cert_fstream.good()) {
        char ch;
        cert_fstream.get(ch);
        if (cert_fstream.eof()) {
            break;
        }
        result += ch;
    }
    return result;
}

Client::Client(const std::string& server_address)
    : message_handler_(std::make_shared<msgr::MessageHandler>()),
      account_handler_(),
      client_service_(message_handler_) {

    ::grpc::SslCredentialsOptions ssl_options;
    ssl_options.pem_root_certs = ReadCertFile("servercrt.pem");
    if (ssl_options.pem_root_certs.empty()) {
        channel_ = ::grpc::CreateChannel(server_address, ::grpc::InsecureChannelCredentials());
    } else {
        channel_ = ::grpc::CreateChannel(server_address, ::grpc::SslCredentials(ssl_options));
    }
    stub_ = ::msgr::grpc::ClientToServerCaller::NewStub(channel_);

    std::cout << "Write client recieve address:" << std::endl;
    std::cin >> recieve_address_;

    ::grpc::ServerBuilder builder;
    builder.AddListeningPort(recieve_address_, ::grpc::InsecureServerCredentials());
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

    std::string client_address = GetClientAddress(client_id);

    std::shared_ptr<::grpc::Channel> client_channel(
        ::grpc::CreateChannel(client_address, ::grpc::InsecureChannelCredentials()));
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

std::string Client::GetRecieveAddress() const {
    return recieve_address_;
}

std::string Client::GetClientAddress(const std::string& client_id) {
    ::grpc::ClientContext context;
    ::msgr::grpc::ClientAddressRequest request;
    request.set_receiver_id(account_handler_.GetAccountData().account_id);
    request.set_client_id(client_id);
    ::msgr::grpc::ClientAddressResponse response;

    ::grpc::Status status = stub_->GetClientAddress(&context, request, &response);
    if (status.ok()) {
        std::cout << "Get client ip complete. IP: " << response.receiver_address() << std::endl;
        return response.receiver_address();
    } else {
        std::cout << "RPC failed. Error message: " << status.error_message() << std::endl;
        return "";
    }
}

Client::ClientToClientService::ClientToClientService(
    std::shared_ptr<MessageHandler> message_handler)
    : message_handler_(message_handler) {
}

::grpc::Status Client::ClientToClientService::SendMessage(
    ::grpc::ServerContext* context, const ::msgr::grpc::SendMessageRequest* request,
    ::msgr::grpc::SendMessageResponse* response) {
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
