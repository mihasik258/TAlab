#ifndef IMYRECOGNIZER_H
#define IMYRECOGNIZER_H

#include <string>
#include <unordered_map>

class IMyRecognizer {
public:
    virtual ~IMyRecognizer() = default;
    virtual bool validate(const std::string& url) = 0;
    virtual const std::unordered_map<std::string, int>& getServerStats() const = 0;
    virtual void clearServerStats() = 0;
};

#endif
