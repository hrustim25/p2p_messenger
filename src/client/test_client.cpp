#include <grpc++/grpc++.h>

#include "client_to_server.grpc.pb.h"
#include "client_to_server.pb.h"
#include "client_to_client.grpc.pb.h"
#include "client_to_client.pb.h"

#include <iostream>

namespace msgr {
class Client {
public:
    Client(const std::string& server_address)
        : channel_(::grpc::CreateChannel(server_address, ::grpc::InsecureChannelCredentials())),
          stub_(::msgr::grpc::ClientToServerCaller::NewStub(channel_)) {
    }

    std::string Register(const std::string& my_address) {
        ::grpc::ClientContext context;
        ::msgr::grpc::RegistrationRequest request;
        request.set_address(my_address);
        ::msgr::grpc::RegistrationResponse response;

        ::grpc::Status status = stub_->Register(&context, request, &response);
        if (status.ok()) {
            my_id_ = response.user_id();
            return "Registration complete. ID: " + response.user_id();
        } else {
            return "RPC failed.";
        }
    }

    std::string UpdateData(const std::string& new_address) {
        ::grpc::ClientContext context;
        ::msgr::grpc::UpdateDataRequest request;
        request.set_address(new_address);
        request.set_user_id(my_id_);
        ::msgr::grpc::UpdateDataResponse response;

        ::grpc::Status status = stub_->UpdateData(&context, request, &response);
        if (status.ok()) {
            return "Data update complete.";
        } else {
            return "RPC failed.";
        }
    }

    std::string GetClientAddress(const std::string& client_id) {
        ::grpc::ClientContext context;
        ::msgr::grpc::ClientAddressRequest request;
        request.set_sender_id(my_id_);
        request.set_client_id(client_id);
        ::msgr::grpc::ClientAddressResponse response;

        ::grpc::Status status = stub_->GetClientAddress(&context, request, &response);
        if (status.ok()) {
            return "Get client ip complete. IP: " + response.client_address();
        } else {
            return "RPC failed.";
        }
    }

private:
    std::shared_ptr<::grpc::Channel> channel_;
    std::unique_ptr<::msgr::grpc::ClientToServerCaller::Stub> stub_;
    std::string my_id_;
};
};  // namespace msgr

int main(void) {
    std::cout << "Starting gRPC client..." << std::endl;

    std::string server_address("0.0.0.0:1337");

    msgr::Client client(server_address);

    std::cout << "Registration result: " << client.Register("client address") << std::endl;

    std::cout << "Update data result: " << client.UpdateData("new client address") << std::endl;

    std::cout << "Get client ip result: " << client.GetClientAddress("0") << std::endl;

    return 0;
}
