#include "../Parser/parser.cpp"
#include "../Lexer/Lexer.cpp"
#include "../Semantic/SemanticAnalizer.cpp"
#include "CodeGenerationContext.hpp"


int main() {
 
    std::string input=R"( 
    let x=2, x=4 in x+x;
    )";

    Lexer lexer;
    auto tokens=lexer.scanTokens(input);

    //Grammar grammar =Grammar::loadGrammar("grammar.txt");
    Parser parser;
    auto cst= parser.parse(tokens);

    AstBuilderVisitor collector;
    AstNode* ast=cst.accept(collector);
    ast->print();
   
    SemanticAnalizer semantic;
   
    semantic.check(ast);

    CodeGenerationContext codegen;

    codegen.generateIR(ast);
    
    codegen.dumpIR();

}