#ifndef SMC_RECOGNIZER_H
#define SMC_RECOGNIZER_H

#include "IMyRecognizer.h"
#include "SmcRecognizer_sm.h"
#include <unordered_map>
#include <string>

class SmcRecognizer : public IMyRecognizer {
private:
    mutable SmcRecognizerContext _fsm;
    mutable std::unordered_map<std::string, int> serverStats;
    mutable std::string buffer;
    mutable std::string serverName;
    mutable bool isAcceptable;
    mutable int serverLen;
    mutable int domainLen;
    mutable int zoneLen;
public:
    SmcRecognizer();

    bool validate(const std::string& url) override;
    const std::unordered_map<std::string, int>& getServerStats() const override;
    void clearServerStats() override;

    bool serverNameLen() const;
    bool domainNameLen() const;
    bool domainZoneNameLen() const;
    bool zoneNameLen() const;

    void addSymb(char symb);
    void addToTable();
    void Acceptable();
    void Unacceptable();
    void saveServerName();
    void zeroLen();
};

#endif // SMC_RECOGNIZER_H
