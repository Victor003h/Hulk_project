
#include "lexer.cpp"

int main()
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
        {TokenType::op_Equal,        "=="},      // ("doubleEqual", "==")
        {TokenType::op_NotEqual,     "!="},      // ("notEqual", "!=")
        {TokenType::op_Not,          "!"},       // ("not", "!")
        {TokenType::op_GreaterEqual, ">="},      // ("greaterEq", ">=")
        {TokenType::op_LessEqual,    "<="},      // ("lessEq", "<=")
        {TokenType::op_Plus,         "+"},       // ("plus", "+")
        {TokenType::op_Minus,        "-"},       // ("minus", "-")
        {TokenType::op_Multiply,     "*"},       // ("star", "*")
        {TokenType::op_Divide,       "/"},       // ("div", "/")
        {TokenType::punc_Dot,          "."}        // ("dot", ".")
    };



    Lexer lexer;

    std::string inp=" 5 < 34 or 23.3 <= 4  ";
    std::string inp2=R"(42;
    print(42);
    print((((1 + 2) ^ 3) * 4) / 5);
    print("Hello World");
    print("The message is \\"Hello World\\"");
    print("The meaning of life is " @ 42);
    print(sin(2 * PI) ^ 2 + cos(3 * PI / log(4, 64)));)";


    
     auto toks=lexer.scanTokens(inp);

     for (auto tok: toks)
     {
        std::cout<<tok.to_string()<<std::endl;
     }
 
}
   