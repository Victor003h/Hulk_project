#pragma once
#include <string>
#include <filesystem>


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
    UNKNOWN,
    // Puedes agregar más tokens según lo requiera tu lenguaje
};

std::string getStringOfToken(TokenType type) {
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
        ss << "type: \"" << getStringOfToken(type) << "\", ";
        ss << "row: "    << row << ", ";
        ss << "col: "    << col;
        ss << ")";
        return ss.str();
    }
};

