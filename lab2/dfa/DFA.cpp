#include "DFA.h"

namespace automats {
    Dfa::Dfa(const Dfa& other)
    {
        // Копируем все состояния
        states.reserve(other.states.size());
        std::map<std::shared_ptr<State>, std::shared_ptr<State>> stateMap;

        for (const auto& st : other.states)
        {
            auto newState = std::make_shared<State>(*st);
            states.push_back(newState);
            stateMap[st] = newState;
        }

        // Копируем startId
        startId = other.startId;
        //currState = other.currState;

        // Обновляем переходы, чтобы ссылались на новые состояния
        for (size_t i = 0; i < other.states.size(); ++i)
        {
            for (const auto& [sym, to] : other.states[i]->id)
            {
                states[i]->id[sym] = to;  // индексы остаются такими же
            }
        }

        // Копируем terminal состояния
        terminalStatesid = other.terminalStatesid;

        // Копируем указатель на стартовое состояние
        start_state = states[startId];
    }

    void Dfa::print() {
        std::cout << "Determ Automat:\n";
        std::cout << states.size() << "\n";

        for (size_t i = 0; i < states.size(); ++i) {
            std::cout << "State: " << i << " " << states[i]->isTerminal << "\n";
            for (const auto &elem : states[i]->id) {
                std::cout << elem.first << " " << elem.second << "\n";
            }
        }

        std::cout << "Terminal states:\n";
        for (size_t id : terminalStatesid) {
            std::cout << id << " ";
        }
        std::cout << "\n";

        std::cout << "Start state: " << startId << "\n";
    }

    Dfa Dfa::made(std::shared_ptr<Nfa> nfa) {
        Dfa dfa;
        std::map<std::set<size_t>, size_t> dfa_state_map;
        std::queue<std::set<size_t>> states_queue;
        std::vector<std::shared_ptr<Nfa::State>> &nfa_states = nfa->states;

        std::set<size_t> start_closure = getEpsilonClosure(nfa, {nfa->start_states.begin(), nfa->start_states.end()});
        size_t start_id = dfa.states.size();
        //std::cout << "This is start state: " << start_id << "\n";

        dfa.startId = start_id;
        dfa_state_map[start_closure] = start_id;
        dfa.states.push_back(std::make_shared<State>(false));
        states_queue.push(start_closure);

        while (!states_queue.empty()) {
            std::set<size_t> current = states_queue.front();
            states_queue.pop();
            size_t current_id = dfa_state_map[current];

            std::map<std::string, std::set<size_t>> transitions;

            for (size_t id : current) {
                for (const auto &trans : nfa_states[id]->id) {
                    if (trans.first == "$") continue;
                    transitions[trans.first].insert(trans.second.begin(), trans.second.end());
                }
            }

            for (const auto&[symbol, targets] : transitions) {
                std::set<size_t> next = getEpsilonClosure(nfa, targets);

                if (!dfa_state_map.count(next)) {
                    size_t new_id = dfa.states.size();
                    dfa_state_map[next] = new_id;
                    dfa.states.push_back(std::make_shared<State>(false));
                    states_queue.push(next);
                }

                dfa.states[current_id]->id[symbol] = dfa_state_map[next];
            }
        }

        for (const auto&[nfa_set, id] : dfa_state_map) {
            for (size_t state : nfa_set) {
                if (nfa_states[state]->isTerminal) {
                    dfa.states[id]->isTerminal = true;
                    dfa.terminalStatesid.push_back(id);
                    break;
                }
            }
        }

        return dfa;
    }

    std::set<size_t> Dfa::getEpsilonClosure(std::shared_ptr<Nfa> nfa, std::set<size_t> currStates) {
        std::set<size_t> closure = currStates;
        std::queue<size_t> queue;

        for (size_t s : currStates) queue.push(s);

        while (!queue.empty()) {
            size_t state = queue.front();
            queue.pop();

            auto &transitions = nfa->states[state]->id;
            if (transitions.count("$")) {
                for (size_t next : transitions["$"]) {
                    if (!closure.count(next)) {
                        closure.insert(next);
                        queue.push(next);
                    }
                }
            }
        }

        return closure;
    }

    void Dfa::minimize() {
        if (states.empty()) return;

        std::vector<std::set<size_t>> partitions;
        std::map<size_t, size_t> stateToPartition;

        std::set<size_t> terminal, nonTerminal;
        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i]->isTerminal)
                terminal.insert(i);
            else
                nonTerminal.insert(i);
        }

        if (!terminal.empty()) partitions.push_back(terminal);
        if (!nonTerminal.empty()) partitions.push_back(nonTerminal);

        for (size_t i = 0; i < partitions.size(); ++i) {
            for (size_t state : partitions[i]) {
                stateToPartition[state] = i;
            }
        }

        bool changed = true;
        while (changed) {
            changed = false;
            std::vector<std::set<size_t>> newPartitions;

            for (const auto& group : partitions) {
                if (group.size() <= 1) {
                    newPartitions.push_back(group);
                    continue;
                }

                std::map<std::map<std::string, size_t>, std::set<size_t>> splitter;

                for (size_t state : group) {
                    std::map<std::string, size_t> transitionsSignature;
                    for (const auto& [symbol, target] : states[state]->id) {
                        transitionsSignature[symbol] = stateToPartition[target];
                    }
                    splitter[transitionsSignature].insert(state);
                }

                if (splitter.size() == 1) {
                    newPartitions.push_back(group);
                } else {
                    for (auto& [_, subset] : splitter) {
                        newPartitions.push_back(subset);
                    }
                    changed = true;
                }
            }

            partitions = std::move(newPartitions);
            stateToPartition.clear();
            for (size_t i = 0; i < partitions.size(); ++i) {
                for (size_t state : partitions[i]) {
                    stateToPartition[state] = i;
                }
            }
        }

        // 2. Построение нового автомата
        std::vector<std::shared_ptr<State>> newStates(partitions.size());
        std::vector<size_t> newTerminalStates;
        size_t newStartId = stateToPartition[startId];

        for (size_t i = 0; i < partitions.size(); ++i) {
            bool isTerminal = false;
            for (size_t state : partitions[i]) {
                if (states[state]->isTerminal) {
                    isTerminal = true;
                    break;
                }
            }
            newStates[i] = std::make_shared<State>(isTerminal);
            if (isTerminal) newTerminalStates.push_back(i);
        }

        for (size_t oldState = 0; oldState < states.size(); ++oldState) {
            size_t newState = stateToPartition[oldState];
            for (const auto& [symbol, target] : states[oldState]->id) {
                newStates[newState]->id[symbol] = stateToPartition[target];
            }
        }

        states = std::move(newStates);
        terminalStatesid = std::move(newTerminalStates);
        startId = newStartId;
    }

/*
    void Dfa::setState() {
        currState = startId;
    }*/

    Dfa Dfa::invert() {
        Dfa inverted;
        inverted.states.resize(states.size());

        for (size_t i = 0; i < states.size(); ++i) {
            inverted.states[i] = std::make_shared<State>(!states[i]->isTerminal);
            if (!states[i]->isTerminal) inverted.terminalStatesid.push_back(i);
        }

        for (size_t i = 0; i < states.size(); ++i) {
            for (const auto&[sym, next] : states[i]->id) {
                inverted.states[next]->id[sym] = i;
            }
        }

        inverted.startId = startId;
        return inverted;
    }
    bool Dfa::checkString(const std::string& input) const {
        size_t currentState = startId;

        for (char c : input) {
            currentState = doTransition(currentState, c);
            if (currentState == size_t(-1)) {
                return false;
            }
        }

        return states[currentState]->isTerminal;
    }

    size_t Dfa::doTransition(size_t currentState, char c) const {
        if (currentState == size_t(-1)) return size_t(-1);

        std::string str(1, c);
        auto it = states[currentState]->id.find(str);
        if (it != states[currentState]->id.end()) {
            return it->second;
        }
        return size_t(-1);
    }
/*
    size_t Dfa::doTransition(char c) {
        if (currState == size_t(-1)) return size_t(-1);

        std::string str(1, c);
        auto it = states[currState]->id.find(str);
        if (it != states[currState]->id.end()) {
            currState = it->second;
            return currState;
        } else {
            currState = size_t(-1);
            return currState;
        }
    }


    bool Dfa::checkState() {
        if (currState == size_t(-1)) return false;
        return states[currState]->isTerminal;
    }
*/

    void Dfa::saveAsGraphviz(const std::string &filename) {
        //std::cout << "Current working directory: " << std::filesystem::current_path() << "\n";
        std::ofstream out(filename);
        if (!out) {
            std::cerr << "Error: Cannot open file '" << filename << "' for writing.\n";
            return;
        }

        out << "digraph DFA {\n";
        out << "    rankdir=LR;\n";
        out << "    node [shape=circle];\n";

        // Стартовая точка
        out << "    start [shape=point];\n";
        out << "    start -> " << startId << ";\n";

        // Терминальные состояния
        for (size_t id = 0; id < states.size(); ++id) {
            if (states[id]->isTerminal) {
                out << "    " << id << " [shape=doublecircle];\n";
            }
        }

        // Переходы
        for (size_t id = 0; id < states.size(); ++id) {
            for (const auto&[symbol, nextId] : states[id]->id) {
                std::string label = symbol;
                for (char &ch : label) {
                    if (ch == '\"' || ch == '\\') {
                        ch = '_';
                    }
                }
                out << "    " << id << " -> " << nextId << " [label=\"" << label << "\"];\n";
            }
        }

        out << "}\n";

        if (out.fail()) {
            std::cerr << "Error: Failed to write to file '" << filename << "'.\n";
        } else {
            //std::cout << "Graphviz file saved successfully to '" << filename << "'.\n";
        }
    }
    int Dfa::GetNumStates() const
    {
        return static_cast <int> (states.size());
    }

    size_t Dfa::GetStartState() const
    {
        return startId;
    }

    std::vector <size_t> Dfa::GetFinalStates() const
    {
        return terminalStatesid;
    }

    std::vector <std::string> Dfa::GetTransitionLabels(size_t from, size_t to) const
    {
        std::vector<std::string> labels;
        // приведение через .at() даст ошибку, если from вне диапазона
        const auto &tr_map = states.at(from)->id;
        for (const auto& kv : tr_map)
        {
            if (kv.second == to)
            {
                labels.push_back(kv.first);
            }
        }
        return labels;
    }
    void Dfa::makeComplete() {
        std::set<std::string> alphabet;
        for (const auto& state : states) {
            for (const auto& [sym, _] : state->id) {
                alphabet.insert(sym);
            }
        }

        std::shared_ptr<State> trapState = std::make_shared<State>(false);
        size_t trapId = states.size();
        bool needTrap = false;

        for (auto& state : states) {
            for (const auto& sym : alphabet) {
                if (state->id.find(sym) == state->id.end()) {
                    state->id[sym] = trapId;
                    needTrap = true;
                }
            }
        }

        if (needTrap) {
            for (const auto& sym : alphabet) {
                trapState->id[sym] = trapId;
            }
            states.push_back(trapState);
        }
    }
    Dfa Dfa::intersection(const Dfa& other) const {
        using StatePair = std::pair<size_t, size_t>;
        std::map<StatePair, size_t> stateMap;
        std::queue<StatePair> queue;

        Dfa result;
        std::vector<std::shared_ptr<State>> resultStates;
        std::vector<size_t> resultFinals;

        std::set<std::string> alphabet;

        for (const auto& state : this->states) {
            for (const auto& [sym, _] : state->id) {
                alphabet.insert(sym);
            }
        }

        Dfa copyA = *this;
        Dfa copyB = other;

        copyA.makeComplete();
        copyB.makeComplete();

        auto& A = copyA;
        auto& B = copyB;

        StatePair start = {A.startId, B.startId};
        stateMap[start] = 0;
        queue.push(start);

        bool isFinal = std::find(A.terminalStatesid.begin(), A.terminalStatesid.end(), start.first) != A.terminalStatesid.end() &&
                       std::find(B.terminalStatesid.begin(), B.terminalStatesid.end(), start.second) != B.terminalStatesid.end();

        resultStates.push_back(std::make_shared<State>(isFinal));
        if (isFinal) resultFinals.push_back(0);

        while (!queue.empty()) {
            StatePair current = queue.front(); queue.pop();
            size_t currId = stateMap[current];

            for (const std::string& sym : alphabet) {
                size_t nextA = A.states[current.first]->id.at(sym);
                size_t nextB = B.states[current.second]->id.at(sym);
                StatePair next = {nextA, nextB};

                if (stateMap.find(next) == stateMap.end()) {
                    size_t newId = resultStates.size();
                    bool isFinalNext =
                            std::find(A.terminalStatesid.begin(), A.terminalStatesid.end(), nextA) != A.terminalStatesid.end() &&
                            std::find(B.terminalStatesid.begin(), B.terminalStatesid.end(), nextB) != B.terminalStatesid.end();
                    resultStates.push_back(std::make_shared<State>(isFinalNext));
                    if (isFinalNext) resultFinals.push_back(newId);
                    stateMap[next] = newId;
                    queue.push(next);
                }

                resultStates[currId]->id[sym] = stateMap[next];
            }
        }

        result.startId = 0;
        result.states = std::move(resultStates);
        result.terminalStatesid = std::move(resultFinals);

        return result;
    }
    Dfa automats::Dfa::dopolnenie(const std::set<std::string>& alphabet) const {
        Dfa copy = *this;

        // 1. Создаем sink-состояние
        size_t sinkIndex = copy.states.size();
        auto sinkState = std::make_shared<State>(false);
        for (const auto& sym : alphabet) {
            sinkState->id[sym] = sinkIndex;
        }

        // 2. Делаем автомат полным по заданному алфавиту
        bool needSink = false;
        for (size_t i = 0; i < copy.states.size(); ++i) {
            for (const auto& sym : alphabet) {
                if (!copy.states[i]->id.count(sym)) {
                    copy.states[i]->id[sym] = sinkIndex;
                    needSink = true;
                }
            }
        }
        if (needSink) {
            copy.states.push_back(sinkState);
        }

        // 3. Инвертируем терминальные состояния
        std::vector<size_t> newFinals;
        for (size_t i = 0; i < copy.states.size(); ++i) {
            if (!copy.states[i]->isTerminal) {
                newFinals.push_back(i);
            }
            copy.states[i]->isTerminal = !copy.states[i]->isTerminal;
        }
        copy.terminalStatesid = std::move(newFinals);

        return copy;
    }
    std::set<std::string> Dfa::getAlphabet() const {
        std::set<std::string> alphabet;
        for (const auto& state : states) {
            for (const auto& [sym, _] : state->id) {
                alphabet.insert(sym);
            }
        }
        return alphabet;
    }

}
