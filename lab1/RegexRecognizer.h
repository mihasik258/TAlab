#ifndef REGEX_RECOGNIZER_H
#define REGEX_RECOGNIZER_H

#include <string>
#include <unordered_map>
#include "IMyRecognizer.h"

class RegexRecognizer : public IMyRecognizer {
public:
    bool validate(const std::string& url) override;
    const std::unordered_map<std::string, int>& getServerStats() const override;
    void clearServerStats() override;

private:
    mutable std::unordered_map<std::string, int> serverStats; // Статистика по серверам
};

#endif
