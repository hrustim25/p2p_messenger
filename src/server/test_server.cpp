#include <grpc++/grpc++.h>

#include "client_to_server.grpc.pb.h"
#include "client_to_server.pb.h"

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <fstream>

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
        response->set_receiver_address(ips_[request->client_id()]);
        return ::grpc::Status::OK;
    }

private:
    std::map<std::string, std::string> ips_;
};
}  // namespace msgr

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

::grpc::SslServerCredentialsOptions GetSecureServerCredentialsOptions() {
    ::grpc::SslServerCredentialsOptions ssl_options;

    ssl_options.pem_root_certs = ReadCertFile("rootcrt.pem");

    ::grpc::SslServerCredentialsOptions::PemKeyCertPair key_pair;
    key_pair.cert_chain = ReadCertFile("servercrt.pem");
    key_pair.private_key = ReadCertFile("serverkey.pem");

    if (!key_pair.cert_chain.empty() && !key_pair.private_key.empty()) {
        ssl_options.pem_key_cert_pairs.push_back(key_pair);
    }
    return ssl_options;
}

void RunServer(std::string server_address) {
    msgr::ClientToServerCallerImpl service;

    ::grpc::ServerBuilder builder;

    ::grpc::SslServerCredentialsOptions ssl_server_options = GetSecureServerCredentialsOptions();
    if (ssl_server_options.pem_root_certs.empty() ||
        ssl_server_options.pem_key_cert_pairs.empty()) {
        builder.AddListeningPort(server_address, ::grpc::InsecureServerCredentials());
    } else {
        builder.AddListeningPort(server_address, ::grpc::SslServerCredentials(ssl_server_options));
    }
    builder.RegisterService(&service);
    std::unique_ptr<::grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(void) {
    std::cout << "Starting gRPC server..." << std::endl;

    RunServer("localhost:9999");

    return 0;
}
