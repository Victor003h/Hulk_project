#include <iostream>
#include "parser.cpp"
#include "../Lexer/Lexer.cpp"





void printProductions(const std::vector<Production>& productions) {
    std::cout << "Producciones:\n";
    int index =0;
    for (const auto& prod : productions) {
        std::cout <<index++ << " "<<  prod.left.value << " -> ";
        if (prod.right.empty()) {
            std::cout << "ε";  // Representa la cadena vacía.
        } else {
            for (const auto& sym : prod.right) {
                std::cout << sym.value << " ";
            }
        }
        std::cout << "\n";
        
    }
}




int main() {

         ErrorHandler error;

        Lexer lexer(error);
    
        std::string inp=R"(type Point {
            x = 0;
            y = 0;

            getX() => 2;
            getY() => 1;
            
        };
            )";

    
        auto tokens=lexer.scanTokens(inp);
        if(lexer.errorHandler.hasErrors())
        {
            return 0;
        }
        // std::vector<Token>tokens={

        //     Token("",TokenType::Identifier,1,1),
        //     Token("x",TokenType::Identifier,1,1),
        //     Token("x",TokenType::Identifier,1,1),
        //     Token("x",TokenType::Identifier,1,1),
        //     Token("x",TokenType::Identifier,1,1),
        //     Token("+",TokenType::op_Plus,1,1),
        //     Token("11",TokenType::Number,1,1),
          
            
        //     Token("EOFs",TokenType::EOFs,1,1),

        // };

    
   // Grammar grammar(Program, terminals, nonTerminals, productions);
   // Grammar grammar =Grammar::loadGrammar("grammar.txt");


    //grammar.printParsingTable();

    Parser* parser=new Parser(lexer.errorHandler);
    
    //printProductions(parser->m_grammar.m_productions);
  //  parser->m_grammar.printParsingTable();
    auto tree= parser->parse(tokens);
    if(tree==nullptr || parser->errorHandler.hasErrors())
    {
        parser->errorHandler.printErrors();
        return 0;

    }
    AstBuilderVisitor visitor;
    AstNode* ast=tree->accept(visitor);
    ast->print();
    std::cout<<"\n";


   // tree.printParseTree(tree.m_root);

}
