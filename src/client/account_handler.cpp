#include "account_handler.h"

#include <sqlite_orm/sqlite_orm.h>

#include <iostream>

namespace msgr {

AccountHandler::AccountHandler() {
    std::unique_lock lock(mtx_);
    try {
        auto storage = sqlite_orm::make_storage(
            "account.sqlite",
            sqlite_orm::make_table("account",
                                   sqlite_orm::make_column("account_id", &AccountData::account_id,
                                                           sqlite_orm::primary_key())));
        storage.sync_schema();
        int entry_count = storage.count<AccountData>();
        if (entry_count) {
            account_data_ = std::make_unique<AccountData>(storage.get_all<AccountData>()[0]);
        }
    } catch (std::system_error& err) {
        std::cerr << "error occured: " << err.what() << std::endl;
        throw;
    }
}

void AccountHandler::SetAccountData(const AccountData& account_data) {
    std::unique_lock lock(mtx_);
    try {
        auto storage = sqlite_orm::make_storage(
            "account.sqlite",
            sqlite_orm::make_table("account",
                                   sqlite_orm::make_column("account_id", &AccountData::account_id,
                                                           sqlite_orm::primary_key())));
        storage.sync_schema();

        storage.remove_all<AccountData>();
        storage.replace(account_data);

        account_data_ = std::make_unique<AccountData>(account_data);
    } catch (std::system_error& err) {
        std::cerr << "error occured: " << err.what() << std::endl;
    }
}

bool AccountHandler::IsRegistered() const {
    return account_data_.get() != nullptr;
}

const AccountData& AccountHandler::GetAccountData() const {
    return *account_data_;
}
}  // namespace msgr
