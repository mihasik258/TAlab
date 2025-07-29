#include "ast.h"

namespace Ast {

    std::string castType(typeNode type) {
        switch (type) {
            case typeNode::root:       return "root";
            case typeNode::leaf:       return "leaf";
            case typeNode::repeat:     return "repeat";
            case typeNode::maybe:      return "maybe";
            case typeNode::concat:     return "concat";
            case typeNode::plus:       return "plus";
            case typeNode::kleene:     return "kleene";
            case typeNode::prediction: return "prediction";
            case typeNode::empty:      return "empty";
            default:                   return "leaf";
        }
    }

    AST::AST(AST& leftTree, AST& rightTree, std::shared_ptr<Node> node)
            : root(std::move(node)) {
        root->left = leftTree.root;
        root->right = rightTree.root;
    }

    AST& AST::setLeftChild(std::shared_ptr<Node> left_node) {
        root->left = left_node;
        return *this;
    }

    int AST::DfsPrint(const std::shared_ptr <Node> & node, int& id_counter, std::ostream& out)
    {
        if (!node) return -1;

        int my_id = id_counter++;

        out << "  node" << my_id
            << " [label=\"" << castType(node->type) << "\\n";

        if (node->type == typeNode::repeat)
        {
            if (node->maxRepeat == -1)
            {
                out << "\\ncount=" << node->minRepeat;
            }
            else
            {
                out << "\\ncount_left=" << node->minRepeat << "\\ncount_right=" << node->maxRepeat;
            }
        }

        if (!node->label.empty())
        {
            out << "\\n" << node->label;
        }

        out << "\"]" << ";\n";

        if (node->left) {
            int left_id = DfsPrint(node->left, id_counter, out);
            out << "  node" << my_id << " -> node" << left_id << ";\n";
        }

        if (node->right) {
            int right_id = DfsPrint(node->right, id_counter, out);
            out << "  node" << my_id << " -> node" << right_id << ";\n";
        }

        return my_id;
    }

    void AST::print(const std::string& filename)
    {
        std::ofstream out(filename);
        if (!out.is_open())
        {
            throw std::runtime_error("Не удалось открыть файл для записи: " + filename);
        }

        out << "digraph AST {\n";
        int counter = 0;
        DfsPrint(this->root, counter, out);
        out << "}\n";
    }
}
