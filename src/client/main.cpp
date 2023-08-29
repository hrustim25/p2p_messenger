#include "client.h"

#include <iostream>

#include <glog/logging.h>

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);

    LOG(INFO) << "Starting gRPC client...\n";

    // Set up server address
    std::string server_address;
    std::cout << "Write server address:" << std::endl;
    std::cin >> server_address;

    // Launch client
    msgr::Client client(server_address);

    while (true) {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "reg") {
            std::cout << "Registration request is sent. Result:" << std::endl;
            client.Register();
        } else if (cmd == "upd") {
            std::cout << "Update data request is sent. Result: " << std::endl;
            client.UpdateData();
        } else if (cmd == "sendmsg") {
            std::string client_id, msg;
            std::cin >> client_id >> msg;
            std::cout << "Send message request is sent. Result: " << std::endl;
            client.SendMessage(client_id, msg);
        } else if (cmd == "getmsgs") {
            std::string client_id;
            std::cin >> client_id;
            for (auto el : client.GetMessages(client_id)) {
                std::cout << el.sender_id_ << "> " << el.msg_ << std::endl;
            }
        } else if (cmd == "exit") {
            break;
        } else {
            std::cout << "Unknown command." << std::endl;
        }
    }

    return 0;
}
