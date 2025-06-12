#include "grammar.h"

class Parser
{
    public:
        Grammar m_grammar;
        ErrorHandler errorHandler;
    
        

    public:
        Parser(Grammar grammar)
        {
            m_grammar=grammar;
        
        }

        ParseTree parse(std::vector<Token> tokens)
        {
            return m_grammar.parse(tokens,errorHandler);
        }

        // ASTNode* convertToAst(ParseTree tree)
        // {

        // }

};