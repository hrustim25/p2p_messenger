#include "message_handler.h"

#include <sqlite_orm/sqlite_orm.h>

#include <glog/logging.h>

namespace msgr {

MessageHandler::MessageHandleStatus MessageHandler::SaveMessage(const std::string& client_id,
                                                                const MessageData& message) {
    uint32_t msg_count = GetMessageCount(client_id);
    if (message.msg_number_ < msg_count) {
        return MessageHandleStatus::NUMBER_ALREADY_EXISTS;
    }
    if (message.msg_number_ > msg_count) {
        return MessageHandleStatus::NUMBER_TOO_HIGH;
    }

    std::unique_lock lock(mtx_);

    std::string chat_table_name = "chat_" + client_id;
    LOG(INFO) << "Saving message with client id: " << client_id << "\n";
    try {
        auto storage = sqlite_orm::make_storage(
            "message_db.sqlite",
            sqlite_orm::make_table(chat_table_name,
                                   sqlite_orm::make_column("msg_id", &MessageData::msg_number_,
                                                           sqlite_orm::primary_key()),
                                   sqlite_orm::make_column("sender_id", &MessageData::sender_id_),
                                   sqlite_orm::make_column("msg", &MessageData::msg_)));
        storage.sync_schema();

        storage.insert(message);
        LOG(INFO) << "Saved successfully";
    } catch (std::system_error& err) {
        LOG(ERROR) << "Error occured while saving message: " << err.what() << "\n";
        return MessageHandleStatus::DATABASE_ERROR;
    }

    return MessageHandleStatus::OK;
}

uint32_t MessageHandler::GetMessageCount(const std::string& client_id) const {
    std::unique_lock lock(mtx_);

    uint32_t msg_count;
    std::string chat_table_name = "chat_" + client_id;
    LOG(INFO) << "Loading message count with client id: " << client_id << "\n";
    try {

        auto storage = sqlite_orm::make_storage(
            "message_db.sqlite",
            sqlite_orm::make_table(chat_table_name,
                                   sqlite_orm::make_column("msg_id", &MessageData::msg_number_,
                                                           sqlite_orm::primary_key()),
                                   sqlite_orm::make_column("sender_id", &MessageData::sender_id_),
                                   sqlite_orm::make_column("msg", &MessageData::msg_)));
        storage.sync_schema();

        msg_count = storage.count<MessageData>();
        LOG(INFO) << "Loaded message count successfully\n";
    } catch (std::system_error& err) {
        LOG(ERROR) << "Error occured while loading message count: " << err.what() << "\n";
        return 0;
    }
    return msg_count;
}

std::vector<MessageData> MessageHandler::GetMessages(const std::string& client_id) const {
    std::unique_lock lock(mtx_);

    std::vector<MessageData> result;
    std::string chat_table_name = "chat_" + client_id;
    LOG(INFO) << "Loading messages with client id: " << client_id << "\n";
    try {
        auto storage = sqlite_orm::make_storage(
            "message_db.sqlite",
            sqlite_orm::make_table(chat_table_name,
                                   sqlite_orm::make_column("msg_id", &MessageData::msg_number_,
                                                           sqlite_orm::primary_key()),
                                   sqlite_orm::make_column("sender_id", &MessageData::sender_id_),
                                   sqlite_orm::make_column("msg", &MessageData::msg_)));
        storage.sync_schema();

        result = storage.get_all<MessageData>();
        LOG(INFO) << "Loaded messages successfully\n";
    } catch (std::system_error& err) {
        LOG(ERROR) << "Error occured while loading messages: " << err.what() << "\n";
        return {};
    }
    return result;
}
}  // namespace msgr
