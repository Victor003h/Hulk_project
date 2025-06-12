#include "../Parser/parser.cpp"
#include "../Lexer/Lexer.cpp"
#include "TypeCollectorVisitor.cpp"
#include "TypeBuilderVisitor.cpp"


int main()
{
    std::string input=R"(type Point {
            x = 0;
            y = 0;

            getX() => 2;
            getY() => 1;
            
        };
            )";

    Lexer lexer;
    auto tokens=lexer.scanTokens(input);

    Grammar grammar =Grammar::loadGrammar("grammar.txt");
    Parser parser(grammar);
    auto cst= parser.parse(tokens);

    AstBuilderVisitor collector;
    AstNode* ast=cst.accept(collector);

    TypeCollectorVisitor vis;
    ast->accept(vis);

    TypeBuilderVisitor  builder;
    builder.context=vis.context;
    ast->accept(builder);
    
}