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
    Client(const std::string& server_address);

    void Register();

    void UpdateData();

    bool SendMessage(const std::string& client_id, const std::string& msg);

    std::vector<MessageData> GetMessages(const std::string& client_id) const;

    std::string GetRecieveAddress() const;

private:
    std::string GetClientAddress(const std::string& client_id);

    class ClientToClientService final : public ::msgr::grpc::ClientToClientCaller::Service {
    public:
        ClientToClientService(std::shared_ptr<MessageHandler> message_handler);

        ::grpc::Status SendMessage(::grpc::ServerContext* context,
                                   const ::msgr::grpc::SendMessageRequest* request,
                                   ::msgr::grpc::SendMessageResponse* response) override;

    private:
        std::shared_ptr<MessageHandler> message_handler_;
    };

private:
    std::string recieve_address_;
    std::shared_ptr<MessageHandler> message_handler_;
    AccountHandler account_handler_;
    ClientToClientService client_service_;
    std::unique_ptr<::grpc::Server> reciever_server_;

    std::shared_ptr<::grpc::Channel> channel_;
    std::unique_ptr<::msgr::grpc::ClientToServerCaller::Stub> stub_;
};
}  // namespace msgr
