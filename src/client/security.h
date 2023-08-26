#pragma once

#include <string>

class Security {
public:
    Security() = default;

    static std::string LoadFile(const std::string& path);
    static void GenerateAndSaveCerts();
};
