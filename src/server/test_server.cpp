#include <grpc++/grpc++.h>

#include "client_to_server.grpc.pb.h"
#include "client_to_server.pb.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>

namespace msgr {

class ClientToServerCallerImpl final : public ::msgr::grpc::ClientToServerCaller::Service {
public:
    ::grpc::Status Register(::grpc::ServerContext* context,
                            const ::msgr::grpc::RegistrationRequest* request,
                            ::msgr::grpc::RegistrationResponse* response) override {
        std::string user_id = std::to_string(ips_.size());
        ips_[user_id] = request->address();
        response->set_user_id(user_id);
        return ::grpc::Status::OK;
    }

    ::grpc::Status UpdateData(::grpc::ServerContext* context,
                              const ::msgr::grpc::UpdateDataRequest* request,
                              ::msgr::grpc::UpdateDataResponse* response) override {
        ips_[request->user_id()] = request->address();
        return ::grpc::Status::OK;
    }

    ::grpc::Status GetClientAddress(::grpc::ServerContext* context,
                                    const ::msgr::grpc::ClientAddressRequest* request,
                                    ::msgr::grpc::ClientAddressResponse* response) override {
        response->set_receiver_address(ips_[request->receiver_id()]);
        return ::grpc::Status::OK;
    }

private:
    std::map<std::string, std::string> ips_;
};
}  // namespace msgr

void RunServer(std::string server_address) {
    msgr::ClientToServerCallerImpl service;

    ::grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<::grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(void) {
    std::cout << "Starting gRPC server..." << std::endl;

    RunServer("0.0.0.0:9999");

    return 0;
}
