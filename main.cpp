#include "./Parser/parser.cpp"
#include "./Lexer/Lexer.cpp"
#include "./Semantic/SemanticAnalizer.cpp"
#include "./CodeGen/CodeGenerationContext.hpp"
#include <iostream>

// 329*2 
// compute=200  // 395
int main() {
    std::string input=R"( 
    type Point(x,y) {
    x = x;
    y = y;

    getX() => 3;
    getY() => 2;

    
};
    )";
    
    ErrorHandler error;

    Lexer lexer(error);
  
    auto tokens=lexer.scanTokens(input);
    if(lexer.errorHandler.hasErrors())
       {
           return 0;
        }

   // Grammar grammar =Grammar::loadGrammar("grammar.txt");
     Parser parser(error);
     auto cst= parser.parse(tokens);
     
    if(cst==nullptr || parser.errorHandler.hasErrors())
    {
        parser.errorHandler.printErrors();
        return 0;
    }


    AstBuilderVisitor collector;
    AstNode* ast=cst->accept(collector);

    ast->print();


    SemanticAnalizer semantic;
    semantic.errorHandler=error;

    semantic.check(ast);
    if(semantic.errorHandler.hasErrors())
    {
      semantic.errorHandler.printErrors();
      return 0;
    };


    CodeGenerationContext codegen;

    codegen.generateIR(ast);

    codegen.dumpIR();
    return 0;

}
