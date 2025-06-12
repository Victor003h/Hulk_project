#pragma once
#include <string>
#include <filesystem>


enum TokenType {
    Number,
    EOFs,
    Identifier,
    String,


    // Operators 
    op_Plus,           // +
    op_Minus,          // -
    op_Multiply,       // *
    op_Exp , 
    op_Divide,         // /
    op_Modulo,         // %
    op_Equal,          // ==
    op_NotEqual, 
    op_Less,           // <
    op_LessEqual,      // <=
    op_Greater,        // >
    op_GreaterEqual,   // >=
    op_LogicalAnd,
    op_LogicalOr,
    op_LogicalNot,
    op_And,            // &
    op_Or,             // |
    op_Not,            // !
    op_destruc,

    // Punctuators
    punc_LeftParen,      // (
    punc_RightParen,     // )
    punc_LeftBrace,      // {
    punc_RightBrace,     // }
    punc_LeftBracket,    // [
    punc_RightBracket,   // ]
    punc_Semicolon,      // ;
    punc_Comma,          // ,
    punc_Dot,            // .
    punc_Colon,
    punc_at,
    punc_doubleAt,

    // Asignación 
    Assignment,     // =


    // Keywords
    kw_if,             // "if"
    kw_else,           // "else"
    kw_for,            // "for"
    kw_while,          // "while"
    kw_break,
    kw_Switch,
    kw_Case,
    kw_Default,
    kw_Continue,
    kw_Return,
    kw_elif,
    kw_protocol,
    kw_in,
    kw_let,
    kw_function,
    kw_inherits,
    kw_true_,         
    kw_false_,
    kw_new_,
    kw_null_,
    kw_extends,
    kw_type,
    kw_is,
    kw_as,
    

    
    //
    arrow,

    //Internal
    Keyword,
    Operator,
    Punctuator,
    Error,
    UNKNOWN,


 
};

std::string getStringOfToken(TokenType type) {
    switch (type) {
        // Literales e identificadores
        case TokenType::Number:         return "Number";
        case TokenType::Identifier:     return "Identifier";

        // Operadores aritméticos
        case TokenType::op_Plus:        return "op_Plus";
        case TokenType::op_Minus:       return "op_Minus";
        case TokenType::op_Multiply:    return "op_Multiply";
        case TokenType::op_Divide:      return "op_Divide";
        case TokenType::op_Exp:         return "op_Exp";
        case TokenType::op_Modulo:      return "op_Modulo";

        // Delimitadores
        case TokenType::punc_LeftParen:   return "punc_LeftParen";
        case TokenType::punc_RightParen:  return "punc_RightParen";
        case TokenType::punc_LeftBrace:   return "punc_LeftBrace";
        case TokenType::punc_RightBrace:  return "punc_RightBrace";
        case TokenType::punc_LeftBracket: return "punc_OpenBracket";
        case TokenType::punc_RightBracket:return "punc_CloseBracket";
        case TokenType::punc_Semicolon:   return "punc_Semicolon";
        case TokenType::punc_Comma:       return "punc_Comma";
        case TokenType::punc_Dot:         return "punc_Dot";

        // Asignación y comparación
        case TokenType::Assignment:   return "Assignment";
        case TokenType::op_Equal:     return "op_Equal";
        case TokenType::op_NotEqual:  return "op_NotEqual";

        // Operadores relacionales
        case TokenType::op_Less:          return "op_Less";
        case TokenType::op_LessEqual:     return "op_LessEqual";
        case TokenType::op_Greater:       return "op_Greater";
        case TokenType::op_GreaterEqual:  return "op_GreaterEqual";

        // Operadores lógicos / bit a bit
        case TokenType::op_And:       return "op_And";
        case TokenType::op_Or:        return "op_Or";
        case TokenType::op_Not:       return "op_Not";

        // Palabras clave de la nueva lista
        case TokenType::kw_if:        return "kw_if";
        case TokenType::kw_else:      return "kw_else";
        case TokenType::kw_for:       return "kw_for";
        case TokenType::kw_while:     return "kw_while";
        case TokenType::kw_break:     return "kw_break";
        case TokenType::kw_Switch:    return "kw_Switch";
        case TokenType::kw_Case:      return "kw_Case";
        case TokenType::kw_Default:   return "kw_Default";
        case TokenType::op_LogicalAnd:return "op_LogicalAnd";
        case TokenType::op_LogicalOr: return "op_LogicalOr";
        case TokenType::op_LogicalNot:return "op_LogicalNot";
        case TokenType::kw_Continue:  return "kw_Continue";
        case TokenType::kw_Return:    return "kw_Return";

        // Tokens sin sustitución (se mantienen igual)
        case TokenType::punc_at:         return "punc_at";
        case TokenType::punc_doubleAt:   return "punc_doubleAt";
        case TokenType::kw_extends:      return "kw_extends";
        case TokenType::punc_Colon:      return "punc_Colon";
        case TokenType::kw_type:         return "kw_type";
        case TokenType::arrow:           return "arrow";
        case TokenType::kw_is:           return "kw_is";
        case TokenType::kw_as:           return "kw_as";
        case TokenType::op_destruc:      return "op_destruc";

        // Palabras clave adicionales
        case TokenType::kw_elif:         return "kw_elif";
        case TokenType::kw_protocol:     return "kw_protocol";
        case TokenType::kw_in:           return "kw_in";
        case TokenType::kw_let:          return "kw_let";
        case TokenType::kw_function:     return "kw_function";
        case TokenType::kw_inherits:     return "kw_inherits";
        case TokenType::kw_true_:        return "kw_true_";
        case TokenType::kw_false_:       return "kw_false_";
        case TokenType::kw_new_:         return "kw_new_";
        case TokenType::kw_null_:        return "kw_null_";


        case TokenType::EOFs:        return "EOFs";
        default:                       return "Unknown";
    }
}

TokenType GetType(TokenType token)
{
    
    std::string st=getStringOfToken(token).substr(0,3);
    if(st=="kw_")   return Keyword;
    if(st=="op_")   return Operator;
    if(st=="punc")   return Punctuator;

    return token;

}

int tokenPrecedence(TokenType token) {
    switch(token) {
        case TokenType::Keyword:    return 3; 
        case TokenType::Identifier: return 2; 
        case TokenType::Number:     return 1;
        case TokenType::Operator:   return 0;
        case TokenType::UNKNOWN:    return -2;
        default:                    return -1; 
    }
    }

bool morePriority(TokenType token1, TokenType token2) {
    return tokenPrecedence(GetType(token1)) > tokenPrecedence(GetType(token2));
}






struct Token{
    
    std::string lexeme;
    TokenType type;
    int line,column;

    Token(std::string lexeme,TokenType type,int row,int col)
    {
        this->lexeme=lexeme;
        this->column=col;
        this->type=type;
        this->line=row;
    }
    Token(){}
    std::string to_string() const 
    {
        std::stringstream ss;
        ss << "Token(";
        ss << "lexeme: \"" << lexeme << "\", ";
        ss << "type: \"" << getStringOfToken(type) << "\", ";
        ss << "row: "    << line << ", ";
        ss << "col: "    << column;
        ss << ")";
        return ss.str();
    }
};

