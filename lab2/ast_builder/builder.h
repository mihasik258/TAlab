#ifndef AST_BUILDER_H
#define AST_BUILDER_H

#include "../ast/ast.h"
#include <string>
#include <vector>

namespace Ast {

    class astBuilder {
    public:
        static AST buildAst(const std::string& expr);

    private:
        static void astLists(std::vector<AST>& asts, const std::string& expr, size_t& bracket);
        static std::pair<size_t, size_t> findBrackets(std::vector<AST>& asts);
        static void createAst(std::pair<size_t, size_t>& closeBracket, std::vector<AST>& asts);
        static void concatinateAsts(std::vector<AST>& asts);
    };

}

#endif // AST_BUILDER_
