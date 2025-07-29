#ifndef MY_REGEX_H
#define MY_REGEX_H

#include "../ast/ast.h"
#include <optional>
#include "../ast_builder/builder.h"
#include "builder.h"
#include "../nfa/NFA.h"
#include "../dfa/DFA.h"
#include "../match/MatchResult.h"
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include <set>

using namespace Ast;
using namespace automats;

namespace regex
{
    struct MatchResult
    {
        std::string sourceString;  // Добавляем хранение исходной строки
        std::vector<std::pair<size_t, size_t>> groupMatches;

        MatchResult() = default;
        explicit MatchResult(const std::string& str) : sourceString(str) {}

        void setSourceString(const std::string& str) {
            sourceString = str;
        }

        void addMatch(int group, size_t start, size_t end) {
            if (group >= groupMatches.size()) {
                groupMatches.resize(group + 1);
            }
            groupMatches[group] = {start, end};
        }

        std::pair<size_t, size_t> getMatch(int group = 0) const {
            if (group < groupMatches.size()) {
                return groupMatches[group];
            }
            return {std::string::npos, std::string::npos};
        }

        // Новый метод для получения подстроки
        std::string getMatchedString(int group = 0) const {
            auto [start, end] = getMatch(group);
            if (start != std::string::npos && end != std::string::npos) {
                return sourceString.substr(start, end - start);
            }
            return "";
        }

        // Новый метод для получения всех совпадений
        std::vector<std::string> getAllMatchedStrings() const {
            std::vector<std::string> result;
            for (const auto& [start, end] : groupMatches) {
                if (start != std::string::npos && end != std::string::npos) {
                    result.push_back(sourceString.substr(start, end - start));
                }
            }
            return result;
        }

        bool empty() const {
            return groupMatches.empty();
        }
    };
    class Regex
    {
    public:
        Regex(const std::string& regExpr);
        size_t compile();
        void search(const std::vector<std::string>& strings, std::vector<std::string>& ans);
        std::optional<MatchResult> search(const std::string& input);
        Regex intersection(const Regex& other) const;
        Regex raznost(const Regex& other) const;
        void dfaPrint();
        bool match(const std::string& input);
        bool match(const std::string& input, MatchResult& result);
        std::string recover() const;
        Regex complement(const std::set<std::string>& fullAlphabet) const;
        Regex complement() const;

    private:
        std::string regExpr;

        AST ast;
        Nfa nfa;
        Dfa dfa;
    };
}

#endif
