#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <exception>
#include <utility>

class Config
{
public:
    Config() {};
    Config(const std::string &);

    std::string getConfig(const std::string &) const;

private:
    std::unordered_map<std::string, std::string> configs;
    
    void parse(const std::string &);
};