#pragma once

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace msgr {

struct MessageData {
    std::string sender_id_;
    std::string msg_;
    uint32_t msg_number_;
};

class MessageHandler {
public:
    enum class MessageHandleStatus {
        OK = 0,
        NUMBER_TOO_HIGH = 1,
        NUMBER_ALREADY_EXISTS = 2,
        DATABASE_ERROR = 3,
    };

public:
    MessageHandler() = default;

    MessageHandleStatus SaveMessage(const std::string& client_id, const MessageData& message);

    uint32_t GetMessageCount(const std::string& client_id) const;

    std::vector<MessageData> GetMessages(const std::string& client_id) const;

private:
    mutable std::mutex mtx_;
};
}  // namespace msgr
