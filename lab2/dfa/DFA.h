#ifndef DFA_H
#define DFA_H

#include <map>
#include <memory>
#include <vector>
#include <iostream>
#include <queue>
#include <set>
#include <algorithm>
#include "../nfa/NFA.h"
#include <fstream>
#include <filesystem>

namespace automats
{
    class Dfa
    {
    private:
        struct State
        {
            bool isTerminal;
            std::map <std::string, size_t> id;
            State(bool isTerminal) : isTerminal(isTerminal) {}
            State(bool isTerminal, std::string transition);
            State(bool isTerminal, std::string transition, std::shared_ptr <State> stat);
            State(const State& other)
                    : isTerminal(other.isTerminal),
                      id(other.id)
            {}
        };

        std::shared_ptr <State> start_state;
        size_t startId;
        //size_t currState;
        std::vector <std::shared_ptr <State> > states;
        std::vector <size_t> terminalStatesid;
        static std::set <size_t> getEpsilonClosure(std::shared_ptr <Nfa> nfa, std::set <size_t> currStates);
    public:
        void makeComplete();
        Dfa intersection(const Dfa& other) const;
        Dfa() = default;
        Dfa dopolnenie(const std::set<std::string>& alphabet) const;
        Dfa(std::string s);
        Dfa(const Dfa& other_dfa);
        std::set<std::string> getAlphabet() const;

        static Dfa made(std::shared_ptr <Nfa> nfa);
        void print();
        void minimize();
        Dfa invert();
        int GetNumStates() const;
        size_t GetStartState() const;
        std::vector <size_t> GetFinalStates() const;
        std::vector<std::string> GetTransitionLabels(size_t from, size_t to) const;
        bool checkString(const std::string& input) const;
        size_t doTransition(size_t currentState, char c) const;
        bool checkState();
        void saveAsGraphviz(const std::string& filename);
        const std::vector<std::shared_ptr<State>>& getStates() const { return states; }
        size_t getStartStateId() const { return startId; }
        const std::vector<size_t>& getTerminalStates() const { return terminalStatesid; }
        std::vector<std::pair<bool, std::map<std::string, size_t>>> getStateTransitions() const {
            std::vector<std::pair<bool, std::map<std::string, size_t>>> transitions;
            for (const auto& state : this->states) {  // Используем this->states, чтобы обратиться к приватному члену
                transitions.push_back({state->isTerminal, state->id});
            }
            return transitions;
        }
    };
}

#endif

