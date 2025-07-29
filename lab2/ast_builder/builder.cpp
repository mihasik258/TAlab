#include "builder.h"
#include <stack>

namespace Ast {

    static int captureGroupCounter = 0;

    AST astBuilder::buildAst(const std::string& expr) {
        captureGroupCounter = 0;
        size_t bracket = 0;
        std::vector<AST> asts;

        astLists(asts, expr, bracket);

        while (bracket-- > 0) {
            auto closeBracket = findBrackets(asts);
            createAst(closeBracket, asts);
        }

        concatinateAsts(asts);
        return asts[0];
    }

    void astBuilder::astLists(std::vector<AST>& asts, const std::string& expr, size_t& bracket) {
        for (size_t i = 0; i < expr.size(); ++i) {
            std::string s;
            if (expr[i] == '{')
            {
                i++;
                while (expr[i] != '}' && expr[i] != ',')
                {
                    s += expr[i];
                    ++i;
                }

                long long int k = stoll(s);
                if (expr[i] == ',')
                {
                    s = "";
                    ++i;
                    while (expr[i] != '}')
                    {
                        s += expr[i];
                        ++i;
                    }
                    long long int v = stoul(s);

                    s ="{}";
                    asts.emplace_back(typeNode::leaf, k, v, s);
                    ++i;
                }
                else
                {
                    s ="{}";
                    asts.emplace_back(typeNode::leaf, k, s);
                    ++i;
                }

                if (i == expr.size()) break;
                else {
                    --i;
                    continue;
                }
            }

            if (expr[i] == '(' && expr[i + 1] == '!') {
                i += 2;
                bracket++;
                asts.emplace_back(typeNode::leaf, "(");
                while (expr[i] != ')') {
                    asts.emplace_back(typeNode::leaf, std::string(1, expr[i++]));
                }
                asts.emplace_back(typeNode::leaf, ")");
                continue;
            }

            if (expr[i] == '(') {
                bracket++;
            }
            if (expr[i] == '[') {
                std::string s = "(";
                asts.emplace_back(typeNode::leaf, s);
                bracket++;
                ++i;

                while (expr[i] != ']') {
                    if (i + 2 < expr.size() && expr[i + 1] == '-' && expr[i + 2] != ']') {
                        char from = expr[i], to = expr[i + 2];
                        for (char c = from; c <= to; ++c) {
                            asts.emplace_back(typeNode::leaf, std::string(1, c));
                            if (c != to) asts.emplace_back(typeNode::leaf, "|");
                        }
                        i += 3;
                    } else {
                        asts.emplace_back(typeNode::leaf, std::string(1, expr[i]));
                        if (expr[i + 1] != ']') asts.emplace_back(typeNode::leaf, "|");
                        ++i;
                    }
                }

                asts.emplace_back(typeNode::leaf, ")");
                if (++i == expr.size()) break;
                else { --i; continue; }
            }
            if (expr[i] == '%' && i + 1 < expr.size()) {
                s = expr[++i];
                asts.emplace_back(typeNode::leaf, s);
            } else if (expr[i] == '$') {
                asts.emplace_back(typeNode::empty, "$");
            } else {
                s = expr[i];
                asts.emplace_back(typeNode::leaf, s);
            }
        }

        if (asts.front().root->label != "(" || asts.back().root->label != ")") {
            asts.emplace_back(typeNode::leaf, ")");
            asts.insert(asts.begin(), AST(typeNode::leaf, "("));
            bracket++;
        }
    }

    std::pair<size_t, size_t> astBuilder::findBrackets(std::vector<AST>& asts) {
        std::stack<size_t> index;
        std::pair<size_t, size_t> closePair = {0, asts.size() - 1};

        for (size_t i = 0; i < asts.size(); ++i) {
            if (asts[i].root->label == "(") {
                index.push(i);
            } else if (asts[i].root->label == ")") {
                size_t open = index.top(); index.pop();
                if ((i - open) < (closePair.second - closePair.first)) {
                    closePair = {open, i};
                }
            }
        }

        return closePair;
    }

    void astBuilder::createAst(std::pair<size_t, size_t>& closeBracket, std::vector<AST>& asts) {
        for (size_t i = closeBracket.first; i < closeBracket.second; ++i) {
            if (asts[i].root->left) continue;

            if (asts[i].root->label == "+") {
                AST newA(typeNode::plus, "+");
                newA.setLeftChild(asts[i - 1].root);
                asts.erase(asts.begin() + i - 1);
                closeBracket.second--;
                asts[i - 1] = newA;
            }

            if (asts[i].root->label == "*") {
                AST newA(typeNode::kleene, "*");
                newA.setLeftChild(asts[i - 1].root);
                asts.erase(asts.begin() + i - 1);
                closeBracket.second--;
                asts[i - 1] = newA;
            }

            if (asts[i].root->label == "/") {
                AST predictionAst = AST(asts[i - 1], asts[i + 1], std::make_shared<Node>(typeNode::prediction, "/"));
                asts.erase(asts.begin() + i - 1);
                asts.erase(asts.begin() + i - 1);
                closeBracket.second -= 2;
                asts[i - 1] = predictionAst;
                --i;
            }
        }

        size_t i = closeBracket.first + 2;
        while (closeBracket.second - closeBracket.first != 2 && i < closeBracket.second) {
            if (asts[i].root->label == "{}") {
                size_t minRepeat = asts[i].root->minRepeat;
                size_t maxRepeat = asts[i].root->maxRepeat;

                AST nextA(typeNode::repeat, minRepeat, maxRepeat, "{}");
                nextA.setLeftChild(asts[i - 1].root);
                asts[i - 1] = nextA;
                asts.erase(asts.begin() + i);
                closeBracket.second--;
            } else {
                ++i;
            }
        }

        i = closeBracket.first + 2;
        while (closeBracket.second - closeBracket.first != 2 && i < closeBracket.second) {
            if (asts[i].root->label != "|" && asts[i - 1].root->label != "|" &&
                asts[i].root->label != "{}" && asts[i - 1].root->label != "{}") {
                AST nextA(asts[i - 1], asts[i], std::make_shared<Node>(typeNode::concat, "."));
                asts.erase(asts.begin() + i - 1);
                closeBracket.second--;
                asts[i - 1] = nextA;
            } else {
                ++i;
            }
        }

        if (asts.size() != 1) {
            for (i = closeBracket.first + 1; i < closeBracket.second; ++i) {
                if (asts[i].root->left) continue;

                if (asts[i].root->label == "|") {
                    AST nextA(asts[i - 1], asts[i + 1], std::make_shared<Node>(typeNode::maybe, "|"));
                    asts.erase(asts.begin() + i - 1);
                    asts.erase(asts.begin() + i - 1);
                    closeBracket.second -= 2;
                    asts[i - 1] = nextA;
                    --i;
                }
            }
        }

        AST groupAst = asts[closeBracket.first + 1];
        for (i = closeBracket.first + 2; i < closeBracket.second - 1; ++i) {
            groupAst = AST(groupAst, asts[i], std::make_shared<Node>(typeNode::concat, "."));
        }

        bool isCapturing = true;
        if (closeBracket.first > 0 && asts[closeBracket.first - 1].root->label == "(" &&
            closeBracket.first + 1 < asts.size() &&
            asts[closeBracket.first + 1].root->label == "!" ) {
            isCapturing = false;
        }

        if (isCapturing) {
            groupAst.root->groupNumber = ++captureGroupCounter;
            groupAst.root->groupStart = closeBracket.first;
            groupAst.root->groupEnd = closeBracket.second;
        }

        asts.erase(asts.begin() + closeBracket.first);
        asts.erase(asts.begin() + closeBracket.second - 1);
    }


    void astBuilder::concatinateAsts(std::vector<AST>& asts) {
        while (asts.size() > 1) {
            AST nextA(asts[0], asts[1], std::make_shared<Node>(typeNode::concat, "."));
            asts.erase(asts.begin());
            asts[0] = nextA;
        }
    }

}
