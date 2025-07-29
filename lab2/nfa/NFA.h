#ifndef NFA_H
#define NFA_H

#include <map>
#include <memory>
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
#include "../ast/ast.h"

namespace automats
{
    class Nfa
    {
        friend class Dfa;

    public:
        Nfa() = default;
        Nfa(std::string s);
        Nfa(const Nfa& other);
        static Nfa tompson(std::shared_ptr<Ast::Node> tree);
        void print();
        std::map<int, std::pair<size_t, size_t>> groupBoundaries;

    protected:
        struct State
        {
            bool isTerminal;
            std::map<std::string, std::vector<size_t>> id;
            std::set<std::string> transitions;
            std::set<int> groupStart;
            std::set<int> groupEnd;

            State(bool isTerminal) :
                    isTerminal(isTerminal),
                    groupStart(),
                    groupEnd() {}

            State(bool isTerminal, std::string transition) :
                    isTerminal(isTerminal),
                    groupStart(),
                    groupEnd() {
                id[transition] = {};
                transitions.insert(transition);
            }

            State(const State& stat) :
                    isTerminal(stat.isTerminal),
                    id(stat.id),
                    transitions(stat.transitions),
                    groupStart(stat.groupStart),
                    groupEnd(stat.groupEnd) {}
        };

        std::vector<size_t> start_states;
        std::vector<std::shared_ptr<State>> states;
        std::vector<size_t> terminalStates;

        Nfa positiveClosureAutomat(Nfa other);
        Nfa addAutomat(Nfa other);
        Nfa orAutomat(Nfa other);
        Nfa predictOperatorAutomat(Nfa predict, Nfa actual);
        Nfa kleeneClosureAutomat(Nfa inner);
    };
}
#endif
