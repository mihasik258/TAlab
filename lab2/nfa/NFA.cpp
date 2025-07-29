#include "NFA.h"

namespace automats {
    Nfa Nfa::tompson(std::shared_ptr<Ast::Node> tree) {

        if (tree->type == Ast::typeNode::empty) {
            Nfa a;
            auto start = std::make_shared<State>(false);
            auto accept = std::make_shared<State>(true);
            start->id["$"].push_back(a.states.size() + 1);
            a.states.push_back(start);
            a.states.push_back(accept);
            a.start_states.push_back(0);
            a.terminalStates.push_back(1);
            return a;
        }

        if (tree->left != nullptr) {
            Nfa l = tompson(tree->left);

            if (tree->right != nullptr) {
                Nfa r = tompson(tree->right);
                if (tree->type == Ast::typeNode::maybe) return l.orAutomat(r);
                if (tree->type == Ast::typeNode::concat) return l.addAutomat(r);
                if (tree->type == Ast::typeNode::prediction) return l.predictOperatorAutomat(l, r);
            } else {
                if (tree->type == Ast::typeNode::plus) return l.positiveClosureAutomat(l);
                if (tree->type == Ast::typeNode::kleene) return l.kleeneClosureAutomat(l);
                if (tree->type == Ast::typeNode::repeat) {
                    long long int m = tree->minRepeat;
                    long long int n = tree->maxRepeat;
                    Nfa unit = l;
                    Nfa res;
                    if (m > 0) {
                        res = unit;
                        for (long long int i = 1; i < m; i++) {
                            res = res.addAutomat(unit);
                        }
                    }
                    else {
                        Nfa eps;
                        eps.states.emplace_back(std::make_shared<State>(false));
                        eps.states.emplace_back(std::make_shared<State>(true));
                        eps.start_states   = {0};
                        eps.terminalStates= {1};
                        eps.states[0]->id["$"].push_back(1);
                        eps.states[0]->transitions.insert("$");
                        res = eps;
                    }

                    if (n >= 0 && n > m)
                    {

                        Nfa eps;
                        eps.states.emplace_back(std::make_shared<State>(false));
                        eps.states.emplace_back(std::make_shared<State>(true));
                        eps.start_states   = {0};
                        eps.terminalStates = {1};
                        eps.states[0]->id["$"].push_back(1);
                        eps.states[0]->transitions.insert("$");

                        for (long long int k = 0; k < n - m; k++) {

                            Nfa opt = unit.orAutomat(eps);

                            res = res.addAutomat(opt);
                        }
                    }

                    return res;
                }
            }
        }
            return Nfa(tree->label);
    }

    Nfa::Nfa(std::string s) {
        states.emplace_back(std::make_shared<State>(false));
        states.emplace_back(std::make_shared<State>(false));
        states.emplace_back(std::make_shared<State>(false));
        states.emplace_back(std::make_shared<State>(true));

        states[0]->id["$"].push_back(1);
        states[1]->id[s].push_back(2);
        states[2]->id["$"].push_back(3);
        terminalStates.push_back(3);
        start_states.push_back(0);
    }

    Nfa Nfa::orAutomat(Nfa other) {
        Nfa r(*this);
        r.states.insert(r.states.begin(), std::make_shared<State>(false));

        for (size_t i = 1; i < r.states.size(); i++) {
            for (const auto& e : r.states[i]->id) {
                std::vector<size_t> v(e.second);
                for (size_t j = 0; j < v.size(); j++) v[j]++;
                r.states[i]->id[e.first] = v;
            }
        }

        r.states[0]->id["$"].push_back(1);
        r.states[r.states.size() - 1]->id["$"].push_back(states.size() + other.states.size() + 1);
        r.states[r.states.size() - 1]->isTerminal = 0;

        for (size_t i = 0; i < other.states.size(); i++) {
            r.states.push_back(std::make_shared<State>(false));
        }

        r.states[0]->id["$"].push_back(states.size() + 1);
        r.states[0]->transitions.insert("$");

        for (size_t i = 0; i < other.states.size(); i++) {
            for (const auto& e : other.states[i]->id) {
                for (auto& t : e.second) {
                    r.states[states.size() + i + 1]->id[e.first].push_back(t + states.size() + 1);
                    r.states[states.size() + i + 1]->transitions.insert(e.first);
                }
            }
        }

        r.states.push_back(std::make_shared<State>(true));
        r.states[r.states.size() - 2]->id["$"].push_back(r.states.size() - 1);
        r.terminalStates[0] = r.states.size() - 1;
        return r;
    }

    Nfa Nfa::addAutomat(Nfa other) {
        Nfa r(*this);
        for (size_t i = 0; i < other.states.size(); i++) {
            r.states.push_back(std::make_shared<State>(other.states[i]->isTerminal));
        }
        r.states[states.size() - 1]->isTerminal = false;

        for (size_t i = 0; i < other.states.size() - 1; i++) {
            for (const auto& e : other.states[i]->id) {
                for (auto& t : e.second) {
                    r.states[states.size() + i - 1]->id[e.first].push_back(t + states.size() - 1);
                    r.states[states.size() + i - 1]->transitions.insert(e.first);
                }
            }
        }
        r.terminalStates[0] = r.states.size() - 1;
        r.states[r.states.size() - 2]->id["$"].push_back(r.states.size() - 1);
        r.states[r.states.size() - 2]->transitions.insert("$");
        return r;
    }

    Nfa Nfa::positiveClosureAutomat(Nfa other) {
        Nfa r(*this);
        r.states[r.terminalStates[0]]->isTerminal = false;
        r.states.insert(r.states.begin(), std::make_shared<State>(false));

        for (size_t i = 1; i < r.states.size(); i++) {
            for (const auto& e : r.states[i]->id) {
                std::vector<size_t> v(e.second);
                for (size_t j = 0; j < v.size(); j++) v[j]++;
                r.states[i]->id[e.first] = v;
            }
        }

        r.states.push_back(std::make_shared<State>(true));
        r.states[0]->id["$"].push_back(1);
        r.states[r.states.size() - 2]->id["$"].push_back(1);
        r.states[r.states.size() - 2]->id["$"].push_back(r.states.size() - 1);
        r.terminalStates[0] = r.states.size() - 1;
        return r;
    }

    Nfa Nfa::kleeneClosureAutomat(Nfa inner) {
        Nfa r(*this);
        r.states[r.terminalStates[0]]->isTerminal = false;
        r.states.insert(r.states.begin(), std::make_shared<State>(false));

        for (size_t i = 1; i < r.states.size(); ++i) {
            for (auto& [s, d] : r.states[i]->id) {
                for (auto& x : d) x++;
            }
        }

        r.states.push_back(std::make_shared<State>(true));
        size_t start = 1;
        size_t end = r.states.size() - 2;

        r.states[0]->id["$"].push_back(start);
        r.states[0]->id["$"].push_back(r.states.size() - 1);
        r.states[end]->id["$"].push_back(start);
        r.states[end]->id["$"].push_back(r.states.size() - 1);
        r.terminalStates[0] = r.states.size() - 1;
        return r;
    }

    Nfa Nfa::predictOperatorAutomat(Nfa predict, Nfa actual) {
        Nfa r(predict);
        r.states[r.terminalStates[0]]->isTerminal = false;
        size_t offset = r.states.size();

        for (const auto& st : actual.states) r.states.push_back(std::make_shared<State>(st->isTerminal));

        for (size_t i = 0; i < actual.states.size(); ++i) {
            for (const auto& [s, d] : actual.states[i]->id) {
                for (size_t t : d) {
                    r.states[offset + i]->id[s].push_back(t + offset);
                    r.states[offset + i]->transitions.insert(s);
                }
            }
        }

        r.states[r.terminalStates[0]]->id["$"].push_back(offset);
        r.terminalStates[0] = offset + actual.terminalStates[0];
        return r;
    }

    Nfa::Nfa(const Nfa& other) {
        states.resize(other.states.size());
        for (size_t i = 0; i < states.size(); i++) {
            states[i] = std::make_shared<State>(other.states[i]->isTerminal);
        }

        for (size_t i = 0; i < states.size(); i++) {
            for (const auto& e : other.states[i]->id) states[i]->id[e.first] = e.second;
            states[i]->transitions = other.states[i]->transitions;
        }

        terminalStates = other.terminalStates;
        start_states.push_back(0);
    }

    void Nfa::print() {
        std::cout << "Automat:" << std::endl;
        std::cout << states.size() << std::endl;
        for (size_t i = 0; i < states.size(); i++) {
            std::cout << "State: " << i << " " << states[i]->isTerminal << std::endl;
            for (const auto& e : states[i]->id) {
                std::cout << e.first << " ";
                for (auto& t : e.second) std::cout << t << " ";
                std::cout << std::endl;
            }
        }

        std::cout << "Terminal states:" << std::endl;
        for (size_t t : terminalStates) std::cout << t << " ";
        std::cout << std::endl;
    }
}