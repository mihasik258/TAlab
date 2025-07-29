#include "SmcRecognizer.h"
#include <iostream>

SmcRecognizer::SmcRecognizer() : _fsm(*this), isAcceptable(false), serverLen(0), domainLen(0), zoneLen(0) {}

bool SmcRecognizer::validate(const std::string& url) {
    _fsm.enterStartState();
    _fsm.enterStartState();
    serverName.clear();
    buffer.clear();
    serverLen = 0;
    domainLen = 0;
    zoneLen = 0;
    std::string sample = "http://";
    if (url.rfind(sample, 0) != 0) {
        Unacceptable();
        return false;
    }
    _fsm.H(url[0]);
    _fsm.T(url[1]);
    _fsm.T(url[2]);
    _fsm.P(url[3]);
    _fsm.Colon(url[4]);
    _fsm.Slash(url[5]);
    _fsm.Slash(url[6]);
    size_t pos = 7;
    bool foundFirstDot = false;
    bool foundSecondDot = false;
    for (; pos < url.size(); ++pos) {
        char c = url[pos];
        if (std::isalpha(c)) {
            if (!foundFirstDot) {
                serverLen++;
            } else {
                domainLen++;
            }
            _fsm.Letter(c);
        } else if (c == '/') {
            _fsm.Slash(c);
        } else if (c == '.') {
            if (!foundFirstDot) {
                foundFirstDot = true;
                _fsm.Point(c);
            } else if (!foundSecondDot) {
                foundSecondDot = true;
                _fsm.Point(c);
            } else{
                domainLen = 0;
            }
        } else {
            Unacceptable();
            return false;
        }
    }

    _fsm.EOS();
    _fsm.setState(Map1::Start);
    return isAcceptable;
}

const std::unordered_map<std::string, int>& SmcRecognizer::getServerStats() const {
    return serverStats;
}

void SmcRecognizer::clearServerStats() {
    serverStats.clear();
}

bool SmcRecognizer::serverNameLen() const {
    return serverLen >= 1 && serverLen <= 20;
}

bool SmcRecognizer::domainNameLen() const {
    return domainLen >= 1 && domainLen <= 20;
}
bool SmcRecognizer::domainZoneNameLen() const {
    return domainLen >= 1 && domainLen <= 5;
}

bool SmcRecognizer::zoneNameLen() const {
    return zoneLen >= 1 && zoneLen <= 5;
}

void SmcRecognizer::addSymb(char symb) {
    buffer.push_back(symb);
}
void SmcRecognizer::zeroLen() {
    zoneLen = 0;
    domainLen = 0;
}
void SmcRecognizer::addToTable() {
    if (!serverName.empty()) {
        serverStats[serverName]++;
    }
}

void SmcRecognizer::Acceptable() {
    isAcceptable = true;
}

void SmcRecognizer::Unacceptable() {
    isAcceptable = false;
}
void SmcRecognizer::saveServerName() {
    serverName = buffer;
}
