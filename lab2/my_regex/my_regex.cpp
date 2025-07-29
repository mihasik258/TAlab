#include "my_regex.h"

namespace regex {
    Regex::Regex(const std::string &regExpr) {
        this->regExpr = regExpr;
    }

    size_t Regex::compile() {
        this->ast = astBuilder::buildAst(this->regExpr);
        //ast.print();
        this->nfa = automats::Nfa::tompson(ast.root);

        std::shared_ptr<automats::Nfa> p{std::make_shared<Nfa>(this->nfa)};

        this->dfa = Dfa::made(p);
        //this->dfa.print();
        this->dfa.saveAsGraphviz("dfa1.dot");
        this->dfa.minimize();
        this->dfa.saveAsGraphviz("dfa2.dot");

        //this->dfa.print();
        return 0;
    }
    void Regex::search(const std::vector<std::string>& strings, std::vector<std::string>& ans) {
        for (const auto& s : strings) {
            for (size_t i = 0; i < s.size(); ++i) {
                size_t currentState = dfa.GetStartState();
                for (size_t j = i; j < s.size(); ++j) {
                    currentState = dfa.doTransition(currentState, s[j]);

                    if (currentState == size_t(-1)) {
                        break; // Недопустимый переход
                    }

                    if (dfa.getStates()[currentState]->isTerminal) {
                        ans.push_back(s);
                        goto next_string;
                    }
                }
            }
            next_string:;
        }
    }

    std::optional<MatchResult> Regex::search(const std::string& input) {
        MatchResult result(input);  // Инициализируем с исходной строкой

        for (size_t start = 0; start < input.size(); ++start) {
            size_t currentState = dfa.GetStartState();
            for (size_t end = start; end < input.size(); ++end) {
                currentState = dfa.doTransition(currentState, input[end]);

                if (currentState == size_t(-1)) {
                    break;
                }

                if (dfa.getStates()[currentState]->isTerminal) {
                    result.addMatch(0, start, end + 1);  // Группа 0 - полное совпадение
                    return result;
                }
            }
        }

        return std::nullopt;
    }
    void Regex::dfaPrint() {
        this->dfa.print();
    }
    static std::string unite(const std::string &a, const std::string &b)
    {
        if (a.empty())
        {
            return b;
        }
        if (b.empty())
        {
            return a;
        }
        return "(" + a + "|" + b + ")";
    }

// Конкатенация двух regex-строк, учитывая пустую строку как ε
    static std::string concat(const std::string& a, const std::string& b)
    {
        if (a.empty())
        {
            return b;
        }
        if (b.empty())
        {
            return a;
        }
        return a + b;
    }

    static std::string plus(const std::string &a)
    {
        if (a.empty())
        {
            // ε* = ε, но для корректности возвращаем ε
            return "";
        }
        if (a.size() == 1 || (a.size() == 2 && a[0] == '\\'))
        {
            return a + "*";
        }
        return "(" + a + ")*";
    }


    std::string Regex::recover() const
    {
        // Получим из dfa:
        //   N = число состояний
        //   start = индекс начального состояния
        //   finals = множество терминальных
        //   trans[u][v] = regex-строка (символ или union-символов) для ребра u→v
        int N = dfa.GetNumStates();
        int start = dfa.GetStartState();
        std::vector<bool> is_final(N, false);
        for (int f : dfa.GetFinalStates())
        {
            is_final[f] = true;
        }

        // инициализируем матрицу R[0][i][j]
        // R[k][i][j] — regex для всех путей i→j, промежуточные состояния ≤ k
        // мы храним только «текущий» слой R (и предыдущий), но здесь удобнее
        // делать 3D-вектор размера (N+1)xNxN
        std::vector<std::vector<std::vector<std::string>>> R(
                N+1,
                std::vector<std::vector<std::string>>(N, std::vector<std::string>(N))
        );

        // заполняем R[0]
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++) {
                // получим все символы, приводящие i→j
                auto syms = dfa.GetTransitionLabels(i, j);
                std::string r;
                for (auto const& lbl : syms)
                {
                    // lbl уже std::string, просто объединяем
                    r = unite(r, lbl);
                }
                // если i==j, добавляем ε (пустую строку)
                if (i == j)
                {
                    r = unite(r, "");
                }
                R[0][i][j] = r;
            }
        }

        // теперь рекуррентная формула K-путей
        for (int k = 1; k <= N; k++)
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    // R[k][i][j] = R[k-1][i][j]
                    std::string r = R[k-1][i][j];
                    // + R[k-1][i][k-1] · (R[k-1][k-1][k-1])^* · R[k-1][k-1][j]
                    const std::string &rik = R[k-1][i][k-1];
                    const std::string &rkk = R[k-1][k-1][k-1];
                    const std::string &rkj = R[k-1][k-1][j];
                    if (!rik.empty() && !rkj.empty())
                    {
                        std::string middle = concat(rik, concat(plus(rkk), rkj));
                        r = unite(r, middle);
                    }
                    R[k][i][j] = r;
                }
            }
        }

        // у нас может быть несколько финальных состояний;
        // итоговый regex — объединение путей start→f для всех f∈finals
        std::string result;
        for (int f = 0; f < N; f++)
        {
            if (!is_final[f]) continue;
            result = unite(result, R[N][start][f]);
        }
        return result;
    }

    bool Regex::match(const std::string& input, MatchResult& result) {
        result.setSourceString(input);
        size_t currentState = dfa.GetStartState();

        for (size_t i = 0; i < input.size(); ++i) {
            currentState = dfa.doTransition(currentState, input[i]);
            if (currentState == size_t(-1)) {
                return false;
            }
        }

        bool isMatch = dfa.getStates()[currentState]->isTerminal;
        if (isMatch) {
            result.addMatch(0, 0, input.size());  // Полное совпадение со всей строкой
        }
        return isMatch;
    }
    bool Regex::match(const std::string& input) {
        MatchResult dummy;
        return match(input, dummy);
    }
    Regex Regex::intersection(const Regex& other) const {
        Regex result("unused");
        result.dfa = this->dfa.intersection(other.dfa);
        return result;
    }

    Regex Regex::complement(const std::set<std::string>& fullAlphabet) const {
        Regex result = *this;
        result.dfa = dfa.dopolnenie(fullAlphabet);
        return result;
    }
    Regex Regex::complement() const {
        Regex result = *this;
        std::set<std::string> alphabet = dfa.getAlphabet();
        result.dfa = dfa.dopolnenie(alphabet);
        result.dfa.saveAsGraphviz("dfa1.dot");
        return result;
    }

    Regex Regex::raznost(const Regex& other) const {
        std::set<std::string> alphabet = dfa.getAlphabet();
        Regex result = *this;
        result = intersection(other.complement(alphabet));
        result.dfa.saveAsGraphviz("dfa1.dot");
        return result;
    }
}