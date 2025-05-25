#include<string>
#include<set>
#include <iostream>
#include "regular_expressions.h"
#include <map>
enum TokenType {
    Number,
    EOFs,
    Identifier,
    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    OpenBracket,
    CloseBracket,
    Semicolon,
    Comma,
    Dot,
    Assignment,   // =
    Equal,        // ==
    NotEqual,     // !=
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    And,
    Or,
    Not,
    If,
    Else,
    For,
    While,
    Break,
    Switch,
    Case,
    Default,
    LogicalAnd,
    LogicalOr,
    LogicalNot,
    Continue,
    Return,
    Error,
    // Puedes agregar más tokens según lo requiera tu lenguaje
};


struct Token{
    
    std::string lexeme;
    TokenType type;
    int row,col;

    Token(std::string lexeme,TokenType type,int row,int col)
    {
        this->lexeme=lexeme;
        this->col=col;
        this->type=type;
        this->row=row;
    }
    std::string to_string() const 
    {
        std::stringstream ss;
        ss << "Token(";
        ss << "lexeme: \"" << lexeme << "\", ";
        ss << "row: "    << row << ", ";
        ss << "col: "    << col;
        ss << ")";
        return ss.str();
    }
};

class Lexer
{
    private:
        std::vector<pair<TokenType,std::string>>tokens_regExp;
        std::map<TokenType,DFA> automatas={};
        int currentLine=0;
        int currentCol=0;
        

    public:
        Lexer( std::vector<pair<TokenType,std::string>>tokens)
        {

            this->tokens_regExp=tokens;
            std::vector<NFA> nfas={};
            for (auto pair :tokens)
            {
                if( existAutamata(pair.first) )
                {
                    
                    DFA* dfa = DFA::loadFromFile(getString(pair.first));
                    automatas[pair.first]=*dfa;
                }
                else
                {
                    RE* n= linear_reguex(pair.second);
                    NFA nfa= n->ConvertToNFA();
                    DFA dfa=nfa.convertToDFA();
                    dfa.saveToFile(getString(pair.first));
                    //nfas.push_back(nfa);
                    automatas[pair.first]=dfa;
                }
               
            }
            if( existAutamata(Number) )
            {
                DFA* dfa = DFA::loadFromFile(getString(Number));
                automatas[Number]=*dfa;
            }
            else
            {
                auto digitis_re=digits_reguex();
                NFA nfa_d=digitis_re->ConvertToNFA();
                DFA dfa=nfa_d.convertToDFA();
                dfa.saveToFile(getString(Number));
                //nfas.push_back(nfa_d);
                automatas[Number]= dfa;
            }

            if( existAutamata(Identifier) )
            {
                DFA* dfa = DFA::loadFromFile(getString(Identifier));
                automatas[Identifier]=*dfa;
            }
            else
            {
               auto identifier_re=buildIdentifierRE();
                NFA nfa_i=identifier_re->ConvertToNFA();
                DFA dfa=nfa_i.convertToDFA();
                dfa.saveToFile(getString(Identifier));
                //nfas.push_back(nfa_i);
                automatas[Identifier]=dfa;
            }

            // NFA nfa=nfas[0];

            // for (int i=1; i<nfas.size();i++)
            // {
            //     nfa= NFA::UnionRE(nfa,nfas[i]);
            // }
            
            // DFA dfa= nfa.convertToDFA();
            
            // automatas.push_back(dfa);
            
        }

        std::string getString(TokenType type) {
            switch (type) {
                case Number:       return "Number";
                case Identifier:   return "Identifier";
                case Plus:         return "Plus";
                case Minus:        return "Minus";
                case Multiply:     return "Multiply";
                case Divide:       return "Divide";
                case Modulo:       return "Modulo";
                case LeftParen:    return "LeftParen";
                case RightParen:   return "RightParen";
                case LeftBrace:    return "LeftBrace";
                case RightBrace:   return "RightBrace";
                case Semicolon:    return "Semicolon";
                case Comma:        return "Comma";
                case Dot:          return "Dot";
                case Assignment:   return "Assignment";
                case Equal:        return "Equal";
                case NotEqual:     return "NotEqual";
                case Less:         return "Less";
                case LessEqual:    return "LessEqual";
                case Greater:      return "Greater";
                case GreaterEqual: return "GreaterEqual";
                case And:          return "And";
                case Or:           return "Or";
                case Not:          return "Not";
                case If:           return "If";
                case Else:         return "Else";
                case For:          return "For";
                case While:        return "While";
                case Return:       return "Return";
                case EOFs:          return "EOFs";
                default:           return "Unknown";
                }

        }
        
        bool existAutamata(TokenType type)
        {
            std::string namefile=getString(type);

             if (!std::filesystem::exists("automatas")) return false;
             if (!std::filesystem::exists("automatas/" + namefile ))  return false;
             return true;

        }

        bool is_valid_token(TokenType type,std::string inp)
        {
            return automatas[type].evualuate(inp);
        }
        
        RE* digits_reguex()
        {
           // RE* digit = unionOfSymbols("0123456789");
            RE* digit = unionOfSymbols("0123456789");
           
            RE* digits= new ConcatenationRE(digit,new ClousureRE(digit));
            RE* opcfrac= new UnionRE(new ConcatenationRE(new SymbolRE('.'),digits),new EpsilonRE());
            RE* number= new ConcatenationRE(digits,opcfrac);
            return number;
        }
        
        RE* linear_reguex(const std::string &s) 
        {

            if (s.empty())
                return new EpsilonRE(); 

            RE* currentRE = new SymbolRE(s[0]);
            
        
            for (size_t i = 1; i < s.size(); ++i) {
                currentRE = new ConcatenationRE(currentRE, new SymbolRE(s[i]));
            }
            
            return currentRE;
        }

   
        RE* unionOfSymbols(const std::string &symbols) 
        {
            if (symbols.empty())
                return nullptr; 

            RE* result = new SymbolRE(symbols[0]);
            
            // Para cada símbolo adicional, lo unimos al resultado actual.
            for (size_t i = 1; i < symbols.size(); ++i) {
                result = new UnionRE(result, new SymbolRE(symbols[i]));
            }
            return result;
        }

        RE* buildIdentifierRE() 
        {
           
            RE* firstChar = unionOfSymbols("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_");
            //RE* firstChar = unionOfSymbols("af");


            RE* subsequentChar = unionOfSymbols("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_");
            //RE* subsequentChar = unionOfSymbols("af8");

           
            RE* subsequentStar = new ClousureRE(subsequentChar);

            
            RE* identifierRE = new ConcatenationRE(firstChar, subsequentStar);

            return identifierRE;
        }

        std::vector<Token> scanTokens(std::string inp)
        {
            
            std::vector<Token> tokens={};
            int cr=0;

            while(cr<inp.length())
            {
                if(inp[cr]==' ')
                {
                    currentCol+=1;
                    cr++;
                    continue;
                }
                if(inp[cr]=='\n')
                {
                    currentLine++;
                    currentCol=0;
                    cr++;
                    continue;
                }
                if(inp[cr]=='\t')
                {
                    currentCol+=4;
                    cr++;
                    continue;
                }
                if(inp[cr]=='\r')
                {
                    currentCol+=1;
                    cr++;
                    continue;
                }
                
                Token token= scanToken(inp,cr);
                cr+=token.lexeme.length();
                currentCol+=token.lexeme.length();
                tokens.push_back(token);
            }
            tokens.push_back(Token("EOF",EOFs,currentLine,currentCol));

            return tokens;
        }

        Token scanToken(std::string inp,int cr)
        {
            std::pair<TokenType,int> matched={Error,-1};

            for (auto pair :automatas)
            {
                int currentpos= cr;
                int currentState=pair.second.getStartState();
                
                while(cr<inp.length())
                {
                    char n=inp[currentpos];
                    int nextstate=pair.second.nextState(n,currentState);
                    if (nextstate==-1)  break;
                    currentState=nextstate;
                    currentpos++;
            
                }
                if (pair.second.is_final_state(currentState) &&matched.second< currentpos-cr)
                {
                    matched={pair.first,currentpos-cr};
                }
            }

            if (matched.second==-1)
            {
                return Token("error",Error,this->currentLine,this->currentCol);
            }

            return Token(inp.substr(cr,matched.second),matched.first,currentLine,currentCol);

        }
    
   
    };



// void testTokenize(Lexer lex,std::string type,std::string inp, bool expected)
// {
//     bool res=lex.is_valid_token(type,inp);
//     std::cout<<"Input: "<<inp<<", res: "<<res<<", esperado: "<<expected <<std::endl;
// }
RE* unionOfSymbols(const std::string &symbols) 
        {
            if (symbols.empty())
                return nullptr; 

            RE* result = new SymbolRE(symbols[0]);
            
            // Para cada símbolo adicional, lo unimos al resultado actual.
            for (size_t i = 1; i < symbols.size(); ++i) {
                result = new UnionRE(result, new SymbolRE(symbols[i]));
            }
            return result;
        }


RE* digits_reguex()
        {
           // RE* digit = unionOfSymbols("0123456789");
            RE* digit = unionOfSymbols("0123456789");
           
            RE* digits= new ConcatenationRE(digit,new ClousureRE(digit));
            RE* opcfrac= new UnionRE(new ConcatenationRE(new SymbolRE('.'),digits),new EpsilonRE());
            RE* number= new ConcatenationRE(digits,opcfrac);
            return number;
        }

int main()
{
    std::vector<pair<std::string,std::string>>tokens_regExp;
    std::vector<tuple<int,std::string,int>>t;

    std::vector<pair<TokenType,std::string>> tokens={};


    // Palabras reservadas (keywords) comunes
    tokens.push_back({If, "if"});
    tokens.push_back({Else, "else"});
    tokens.push_back({While, "while"});
    tokens.push_back({For, "for"});
    tokens.push_back({Return, "return"});
    tokens.push_back({Break, "break"});
    tokens.push_back({Continue, "continue"});
    tokens.push_back({Switch, "switch"});
    tokens.push_back({Case, "case"});
    tokens.push_back({Default, "default"});


    // // Operadores aritméticos
    tokens.push_back({Plus, "+"});
    tokens.push_back({Minus, "-"});
    tokens.push_back({Multiply, "*"});
    tokens.push_back({Divide, "/"});
    tokens.push_back({Modulo, "%"});
    
    // // Operadores de asignación y compuestos
     tokens.push_back({Assignment, "="});

    
    // // Operadores relacionales
    tokens.push_back({Equal, "=="});
    tokens.push_back({NotEqual, "!="});
    tokens.push_back({Less, "<"});
    tokens.push_back({Greater, ">"});
    tokens.push_back({LessEqual, "<="});
    tokens.push_back({GreaterEqual, ">="});
    
    // // Operadores lógicos
    tokens.push_back({LogicalAnd, "&&"});
    tokens.push_back({LogicalOr, "||"});
    tokens.push_back({LogicalNot, "!"});
    
    // // Operadores bit a bit (muy usados en ciertos lenguajes)
    // tokens.push_back({"bitwise_and", "&"});
    // tokens.push_back({"bitwise_or", "|"});
    // tokens.push_back({"bitwise_xor", "^"});
    // tokens.push_back({"bitwise_not", "~"});
    // tokens.push_back({"shift_left", "<<"});
    // tokens.push_back({"shift_right", ">>"});
    
    // // Delimitadores y signos de puntuación
    tokens.push_back({LeftParen, "("});
    tokens.push_back({RightParen, ")"});
    tokens.push_back({OpenBracket, "["});
    tokens.push_back({CloseBracket, "]"});
    tokens.push_back({LeftBrace, "{"});
    tokens.push_back({RightBrace, "}"});
    tokens.push_back({Semicolon , ";"});
    tokens.push_back({Comma, ","});
    tokens.push_back({Dot, "."});
    
    // // Otros símbolos útiles
    // tokens.push_back({"colon", ":"});


   

    Lexer lexer(tokens);

     std::string inp=" 5 < 34 or 23.3 <= 4  ";

     auto toks=lexer.scanTokens(inp);

     for (auto tok: toks)
     {
        std::cout<<tok.to_string()<<std::endl;
     }
 
}
   