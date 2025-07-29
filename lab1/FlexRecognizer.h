#ifndef FLEX_RECOGNIZER_H
#define FLEX_RECOGNIZER_H

#include "IMyRecognizer.h"
#include <string>
#include <unordered_map>

class FlexRecognizer : public IMyRecognizer {
public:
    bool validate(const std::string& url) override;
    const std::unordered_map<std::string, int>& getServerStats() const override;
    void clearServerStats() override;
};

#endif

