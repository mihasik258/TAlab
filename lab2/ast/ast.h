#ifndef AST_H
#define AST_H
#include <memory>
#include <string>
#include <iostream>
#include <fstream>

namespace Ast
{
    enum class typeNode
    {
        root,
        leaf,
        repeat,
        maybe,
        concat,
        plus,
        kleene,
        prediction,
        empty
    };

    std::string castType(typeNode type);

    struct Node
    {
        typeNode type;
        std::string label;
        size_t minRepeat = 0;
        size_t maxRepeat = 0;
        size_t repeat = 0;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        int groupNumber = -1;
        size_t groupStart = std::string::npos;
        size_t groupEnd = std::string::npos;

        // Конструктор для обычных узлов (без повторений)
        Node(typeNode type, const std::string& label = "")
                : type(type), label(label), left(nullptr), right(nullptr) {}

        // Конструктор для повторяющихся узлов с одинаковым min = max
        Node(typeNode type, size_t repeat, const std::string& label = "")
                : type(type), label(label), minRepeat(repeat), maxRepeat(repeat), left(nullptr), right(nullptr) {}

        // Новый конструктор для диапазона повторений {min,max}
        Node(typeNode type, size_t minRepeat, size_t maxRepeat, const std::string& label = "")
                : type(type), label(label), minRepeat(minRepeat), maxRepeat(maxRepeat), left(nullptr), right(nullptr) {}

        int print(std::ostream& stream = std::cout, size_t spaces = 0);
    };

    class AST
    {
    public:
        std::shared_ptr<Node> root;

        AST() : root(nullptr) {}

        AST(typeNode type, const std::string& label)
                : root(std::make_shared<Node>(type, label)) {}

        AST(typeNode type, size_t repeat, const std::string& label)
                : root(std::make_shared<Node>(type, repeat, label)) {}

        // Новый конструктор для диапазона повторений
        AST(typeNode type, size_t minRepeat, size_t maxRepeat, const std::string& label)
                : root(std::make_shared<Node>(type, minRepeat, maxRepeat, label)) {}

        AST(AST& leftTree, AST& rightTree, std::shared_ptr<Node> node);

        AST& setLeftChild(std::shared_ptr<Node> left_node);
        int DfsPrint(const std::shared_ptr <Node> & node, int& id_counter, std::ostream& out);
        void print(const std::string& filename = "ast.dot");
    };
}

#endif
