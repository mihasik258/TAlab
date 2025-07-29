/*#ifndef MATCH_RESULT_H
#define MATCH_RESULT_H

#include <string>
#include <vector>

class MatchResult {
public:
    std::vector<std::pair<size_t, size_t>> groups; // start, end positions
    std::string source;

    MatchResult(const std::string& str, const std::vector<std::pair<size_t, size_t>>& g)
            : groups(g), source(str) {}

    std::string operator[](size_t i) const {
        if (i >= groups.size()) return "";
        return source.substr(groups[i].first, groups[i].second - groups[i].first);
    }

    size_t size() const { return groups.size(); }

    auto begin() const { return groups.begin(); }
    auto end() const { return groups.end(); }
};

#endif */
