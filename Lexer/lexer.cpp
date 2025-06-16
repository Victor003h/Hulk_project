#include<string>
#include<set>
#include <iostream>
#include "regular_expressions.h"
#include <map>
#include "../common/Error.hpp"

class Lexer
{
    private:
        std::vector<pair<TokenType,std::string>>tokens_regExp;
        std::map<TokenType,DFA> automatas={};
        DFA finaldfa;
        int currentLine=0;
        int currentCol=0;
        

    public:
        ErrorHandler errorHandler;

        Lexer( ErrorHandler& errorHandler):errorHandler(errorHandler)
        {
            

            std::vector<std::pair<TokenType, std::string>> tokens = {
                {TokenType::op_Modulo,       "%"},   // ("modOp", "%")
                {TokenType::punc_at,           "@"},   // ("at", "@")
                {TokenType::punc_doubleAt,     "@@"},  // ("doubleAt", "@@")
                {TokenType::kw_extends,      "extends"}, // ("extends", "extends")
                {TokenType::punc_LeftBracket,  "["},   // ("lbracket", "[")
                {TokenType::punc_RightBracket, "]"},   // ("rbracket", "]")
                {TokenType::punc_LeftBrace,    "{"},   // ("lbrace", "{")
                {TokenType::punc_RightBrace,   "}"},   // ("rbrace", "}")
                {TokenType::punc_LeftParen,    "("},   // ("lparen", "(")
                {TokenType::punc_RightParen,   ")"},   // ("rparen", ")")
                {TokenType::op_Greater,      ">"},   // ("greater", ">")
                {TokenType::op_Less,         "<"},   // ("less", "<")
                {TokenType::punc_Semicolon,    ";"},   // ("semicolon", ";")
                {TokenType::punc_Colon,        ":"},   // ("colon", ":")
                {TokenType::punc_Comma,        ","},   // ("comma", ",")
                {TokenType::kw_type,         "type"},// ("type", "type")
                {TokenType::arrow,        "=>"},  // ("arrow", "=>")
                {TokenType::Assignment,   "="},   // ("equal", "=")
                // Palabras clave y otros tokens de la lista original
                {TokenType::kw_if,           "if"},        // sustituido a "if"
                {TokenType::kw_else,         "else"},      // sustituido a "else"
                {TokenType::kw_elif,         "elif"},
                {TokenType::kw_protocol,     "protocol"},
                {TokenType::kw_in,           "in"},
                {TokenType::kw_let,          "let"},
                {TokenType::kw_function,     "function"},
                {TokenType::kw_inherits,     "inherits"},
                {TokenType::kw_extends,      "extends"},   // se repite según la lista original
                {TokenType::kw_while,        "while"},     // sustituido a "while"
                {TokenType::kw_for,          "for"},       // sustituido a "for"
                {TokenType::kw_true_,        "true"},
                {TokenType::kw_false_,       "false"},
                {TokenType::kw_new_,         "new"},
                {TokenType::kw_null_,        "null"},
                {TokenType::kw_is,         "is"},
                {TokenType::kw_as,         "as"},
                {TokenType::op_destruc,    ":="},
                {TokenType::op_LogicalOr,    "||"},      // ("doubleOr", "||")
                {TokenType::op_Or,           "|"},       // ("or", "|")
                {TokenType::op_And,          "&"},       // ("and", "&")
                {TokenType::op_LogicalAnd,    "&&"},      // ("doubleOr", "||")
                {TokenType::op_Equal,        "=="},      // ("doubleEqual", "==")
                {TokenType::op_NotEqual,     "!="},      // ("notEqual", "!=")
                {TokenType::op_Not,          "!"},       // ("not", "!")
                {TokenType::op_GreaterEqual, ">="},      // ("greaterEq", ">=")
                {TokenType::op_LessEqual,    "<="},      // ("lessEq", "<=")
                {TokenType::op_Plus,         "+"},       // ("plus", "+")
                {TokenType::op_Minus,        "-"},       // ("minus", "-")
                {TokenType::op_Multiply,     "*"},       // ("star", "*")
                {TokenType::op_Divide,       "/"},       // ("div", "/")
                {TokenType::op_Exp,         "^"},         
                {TokenType::punc_Dot,          "."}        // ("dot", ".")
            };


            this->tokens_regExp=tokens;
            std::vector<NFA> nfas={};
            
            for (auto pair :tokens)
            {
                RE* n= linear_reguex(pair.second);
                n->type=pair.first;
                NFA nfa= n->ConvertToNFA();
                for(int fs:nfa.final_states)
                {
                    nfa.m_final_token_types[fs]=pair.first;
                }
                nfas.push_back(nfa);



                // if( existAutamata(pair.first) )
                // {
                    
                //     DFA* dfa = DFA::loadFromFile(getString(pair.first));
                //     automatas[pair.first]=*dfa;
                // }
                // else
                // {
                //     RE* n= linear_reguex(pair.second);
                //     n->type=pair.first;
                //     NFA nfa= n->ConvertToNFA();
                //     for(int fs:nfa.final_states)
                //     {
                //         nfa.m_final_token_types[fs]=pair.first;
                //     }

                //     DFA dfa=nfa.convertToDFA();
                //     dfa.saveToFile(getString(pair.first));
                //     //nfas.push_back(nfa);
                //     automatas[pair.first]=dfa;
                // }
               
            }

            auto digitis_re=digits_reguex();
            NFA nfa_d=digitis_re->ConvertToNFA();
            for(int fs:nfa_d.final_states)
            {
                nfa_d.m_final_token_types[fs]=Number;
            }
            nfas.push_back(nfa_d);


            auto identifier_re=buildIdentifierRE();
            NFA nfa_i=identifier_re->ConvertToNFA();
            for(int fs:nfa_i.final_states)
            {
                nfa_i.m_final_token_types[fs]=Identifier;
            }
            nfas.push_back(nfa_i);


            NFA nfa=nfas[0];

            for (size_t i=1; i<nfas.size();i++)
            {
                nfa= NFA::UnionRE(nfa,nfas[i]);
            }



            
            DFA dfa= nfa.convertToDFA();
            finaldfa=dfa;
            
            

            // if( existAutamata(Number) )
            // {
            //     DFA* dfa = DFA::loadFromFile(getString(Number));
            //     automatas[Number]=*dfa;
            // }
            // else
            // {
            //     auto digitis_re=digits_reguex();
            //     NFA nfa_d=digitis_re->ConvertToNFA();
            //     for(int fs:nfa_d.final_states)
            //     {
            //         nfa_d.m_final_token_types[fs]=Number;
            //     }
            //     DFA dfa=nfa_d.convertToDFA();
            //     dfa.saveToFile(getString(Number));
            //     //nfas.push_back(nfa_d);
            //     automatas[Number]= dfa;
            // }

            // if( existAutamata(Identifier) )
            // {
            //     DFA* dfa = DFA::loadFromFile(getString(Identifier));
            //     automatas[Identifier]=*dfa;
            // }
            // else
            // {
            //    auto identifier_re=buildIdentifierRE();
            //     NFA nfa_i=identifier_re->ConvertToNFA();
            //     for(int fs:nfa_i.final_states)
            //     {
            //         nfa_i.m_final_token_types[fs]=Identifier;
            //     }
            //     DFA dfa=nfa_i.convertToDFA();
            //     dfa.saveToFile(getString(Identifier));
            //     //nfas.push_back(nfa_i);
            //     automatas[Identifier]=dfa;
            // }

            // NFA nfa=nfas[0];

            // for (int i=1; i<nfas.size();i++)
            // {
            //     nfa= NFA::UnionRE(nfa,nfas[i]);
            // }
            
            // DFA dfa= nfa.convertToDFA();
            
            // automatas.push_back(dfa);
                        
        }

        
        bool existAutamata(TokenType type)
        {
            std::string namefile=getStringOfToken(type);

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

            while(cr<inp.size())
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
                

               // Token token= scanToken(inp,cr);
                Token token= scanToken2(inp,cr);
                if(token.type==TokenType::Error)
                {
                    errorHandler.reportError(token,"Lexical error: unrecognized token ");
                }
                cr+=token.lexeme.length();
                currentCol+=token.lexeme.length();
                tokens.push_back(token);
            }
            tokens.push_back(Token("EOF",EOFs,currentLine,currentCol));

            return tokens;
        }

        // Token scanToken(std::string inp,int cr)
        // {
        //     std::pair<TokenType,int> matched={Error,-1};

        //     for (auto pair :automatas)
        //     {
        //         int currentpos= cr;
        //         int currentState=pair.second.getStartState();
                
        //         while(cr<inp.length())
        //         {
        //             char n=inp[currentpos];
        //             int nextstate=pair.second.nextState(n,currentState);
        //             if (nextstate==-1)  break;
        //             currentState=nextstate;
        //             currentpos++;
            
        //         }
        //         if (pair.second.is_final_state(currentState) &&matched.second< currentpos-cr)
        //         {
        //             matched={pair.first,currentpos-cr};
        //         }
        //     }

        //     if (matched.second==-1)
        //     {
        //         return Token("error",Error,this->currentLine,this->currentCol);
        //     }

        //     return Token(inp.substr(cr,matched.second),matched.first,currentLine,currentCol);

        // }
    
        Token scanToken2(std::string inp,int cr)
        {
           // std::pair<TokenType,int> matched={Error,-1};

            int currentpos=cr;
            int currentState=finaldfa.start_state;


            while(cr <inp.size())
            {
                char n=inp[currentpos];
                int nextstate=finaldfa.nextState(n,currentState);
                if (nextstate==-1)  break;
                currentState=nextstate;
                currentpos++;
        
            }
            if (finaldfa.is_final_state(currentState) )
            {
                 TokenType t=finaldfa.m_final_token_types[currentState];
                 return Token(inp.substr(cr,currentpos-cr),t,currentLine,currentCol);
            }
         
            return Token("error",Error,this->currentLine,this->currentCol);
        }

    };

// void testTokenize(Lexer lex,std::string type,std::string inp, bool expected)
// {
//     bool res=lex.is_valid_token(type,inp);
//     std::cout<<"Input: "<<inp<<", res: "<<res<<", esperado: "<<expected <<std::endl;
// }


