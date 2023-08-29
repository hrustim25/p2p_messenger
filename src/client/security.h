#pragma once

#include <string>

namespace msgr {
class Security {
public:
    Security() = default;

    static std::string LoadFile(const std::string& path);
    static void GenerateAndSaveCerts();
};
}  // namespace msgr
