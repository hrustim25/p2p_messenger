#pragma once

#include <memory>
#include <mutex>
#include <string>

namespace msgr {

struct AccountData {
    std::string account_id;
};

class AccountHandler {
public:
    AccountHandler();

    void SetAccountData(const AccountData& account_data);

    bool IsRegistered() const;

    const AccountData& GetAccountData() const;

private:
    mutable std::mutex mtx_;
    std::unique_ptr<AccountData> account_data_;
};
}  // namespace msgr
