#pragma once

#include <grpc++/grpc++.h>

#include "client_to_server.grpc.pb.h"
#include "client_to_server.pb.h"
#include "client_to_client.grpc.pb.h"
#include "client_to_client.pb.h"

#include "message_handler.h"
#include "account_handler.h"

namespace msgr {

class Client {
public:
    struct ClientData {
        std::string address;
        std::string certificate;
    };

public:
    Client(const std::string& server_address);

    void Register();

    void UpdateData();

    ClientData GetClientData(const std::string& client_id);

    bool SendMessage(const std::string& client_id, const std::string& msg);

    std::vector<MessageData> GetMessages(const std::string& client_id) const;

private:
    class ClientToClientService final : public ::msgr::grpc::ClientToClientCaller::Service {
    public:
        ClientToClientService(Client* client, std::shared_ptr<MessageHandler> message_handler);

        ::grpc::Status SendMessage(::grpc::ServerContext* context,
                                   const ::msgr::grpc::SendMessageRequest* request,
                                   ::msgr::grpc::SendMessageResponse* response) override;

    private:
        Client* client_;
        std::shared_ptr<MessageHandler> message_handler_;
    };

private:
    std::string recieve_address_;
    std::shared_ptr<MessageHandler> message_handler_;
    AccountHandler account_handler_;
    ClientToClientService client_service_;
    std::unique_ptr<::grpc::Server> reciever_server_;

    std::string server_cert_;
    std::string client_key_;
    std::string client_cert_;

    std::shared_ptr<::grpc::Channel> channel_;
    std::unique_ptr<::msgr::grpc::ClientToServerCaller::Stub> stub_;
};
}  // namespace msgr
