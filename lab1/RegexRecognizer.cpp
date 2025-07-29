#include "RegexRecognizer.h"
#include <regex>
#include <iostream>

const std::regex pattern(R"(^http://([a-zA-Z]{1,20})(\.[a-zA-Z]{1,20})*\.[a-zA-Z]{1,5}$)", std::regex_constants::optimize);

bool RegexRecognizer::validate(const std::string& url){
    std::smatch match;
    if (std::regex_match(url, match, pattern)) {
        if (match.size() > 1) {
            std::string serverName = match[1].str();
            serverStats[serverName]++;
        }
        return true;
    }
    return false;
}

const std::unordered_map<std::string, int>& RegexRecognizer::getServerStats() const {
    return serverStats;
}

void RegexRecognizer::clearServerStats() {
    serverStats.clear();
}
