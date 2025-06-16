#include "../Parser/parser.cpp"
#include "../Lexer/Lexer.cpp"
#include "SemanticAnalizer.cpp"
#include "TypeCollectorVisitor.cpp"
#include "TypeBuilderVisitor.cpp"
#include "VariableDefinedVisitor.cpp"
#include "FunctionCollectorVisitor.cpp"

int main()
{
    std::string input=R"( 
    let x=2, x=4 in x+x;
    )";
    
    
    ErrorHandler error;

    Lexer lexer(error);
  
    auto tokens=lexer.scanTokens(input);
    if(lexer.errorHandler.hasErrors())
       {
           return 0;
        }

    //Grammar grammar =Grammar::loadGrammar("grammar.txt");
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
    }
    //FunctionCollectorVisitor vis;
    //ast->accept(vis);

    // TypeBuilderVisitor  builder;
    // builder.context=vis.context;
    // ast->accept(builder);

  // VariableDefindedVisitor  def(vis.context);
  // ast->accept(def);

}
