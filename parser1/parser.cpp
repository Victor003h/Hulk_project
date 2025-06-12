#include <iostream>
#include <vector>
#include "../common/token.h"
#include "../Lexer/Token.cpp"
#include "../common/nodes.cpp"
#include "../common/Error.cpp"

using namespace std;

class Parser {
private:
    const vector<Token>& tokens;
    size_t current;
    ErrorHandler errorHandler;

public:
    Parser(const vector<Token>& tokens): tokens(tokens), current(0){}

    ASTNode* parse()
    {
        if (isEnd())
        {
            return nullptr;
        }
        return parseStatement();
    }

private:


    bool isEnd()
    {
        return current >= tokens.size();
    }

    Token peek()
    {
        if(current < tokens.size()) return tokens[current];
        return Token{ "",TokenType::Keyword, -1, -1};
    }

    Token advance()
    {
        if(!isEnd()) return tokens[current++];
        return tokens[current];
    }

    bool check(TokenType type)
    {
        if(current < tokens.size()) 
        {
            if(type==Operator)
            {
                return GetType(tokens[current].type)==Operator;
            }
            return tokens[current].type == type;
        }
        return false;
    }

    bool match(TokenType type)
    {
        if(check(type)) {advance(); return true;}
        return false;
    }

    bool matchOperator(TokenType type)
    {

        if (check(TokenType::Operator) ||check(TokenType::Assignment))
        {
            advance();
            return true;
        }
        return false;
    }

    ASTNode* parseStatement() {
        ASTNode* expr = parseExpression();
        if (!expr) {
            errorHandler.reportError(peek(), "Error al parsear la expresión.");
            return nullptr;
        }
        if (!match(TokenType::punc_Semicolon)) {
            errorHandler.reportError(peek(), "Se esperaba ';' al final de la declaración.");
            return nullptr;
        }
        StatementNode* statement = new StatementNode(expr);
        return statement;
    }

    ASTNode* parseExpression() {
        if (check(TokenType::kw_let) ) {
            return parseLetExpression();
        } else {
            return parseAdditiveExpression(); 
        }
    }

    ASTNode* parseAdditiveExpression()
    {
        ASTNode* term = parseTerm();
        if (!term) return nullptr;
        return parseExpressionPrime(term);
    }

    ASTNode* parseTerm()
    {
        ASTNode* factor = parseFactor();
        if(!factor) return nullptr;
        return parseTermPrime(factor);
    }

    ASTNode* parseFactor() {
        if (check(TokenType::Number)|| check(TokenType::Operator)) {
            return parseUnary();
        }
        else if (check(TokenType::String)) {
            Token strToken = advance();
            return new StringNode(strToken.lexeme);
        }
        else if (check(TokenType::Identifier)) {
            Token idToken = advance();
            if (check(TokenType::punc_LeftParen)) {
                advance(); 
                vector<ASTNode*> args = parseArgumentList();
                cout << tokens[current].lexeme;
                if (!match(TokenType::punc_RightParen)) {
                    errorHandler.reportError(peek(), "Se esperaba ')' después de la lista de argumentos.");
                    return nullptr;
                }
                return new FunctionCallNode(idToken.lexeme, args);
            } else {
                return new VariableNode(idToken.lexeme);
            }
        }
        else if (match(TokenType::punc_LeftParen)) {
            ASTNode* expr = parseExpression();
            if (!match(TokenType::punc_RightParen)) {
                errorHandler.reportError(peek(), "Se esperaba ')' después de la expresión entre paréntesis.");
                return nullptr;
            }
            return expr;
        }
        errorHandler.reportError(peek(), "Se esperaba un número, identificador o paréntesis.");
        return nullptr;
    }


    ASTNode* parsePrimary()
    {
        Token currentToken = peek();
        if(match(TokenType::Number))
        {
            return new NumberNode(currentToken.lexeme);
        }
        if(match(TokenType::Identifier))
        {
            return new VariableNode(currentToken.lexeme);
        }
        errorHandler.reportError(currentToken, "Se esperaba un numero o identificador.");
        return nullptr;
    }
   
    ASTNode* parseUnary()
    {
        if (check(TokenType::Operator)) {
            std::string op = peek().lexeme;
            if (op == "-" || op == "+") {
                // Consume el operador
                advance();
                // Llama recursivamente para obtener el operando
                ASTNode* right = parseUnary();
                // Puedes crear un nodo de expresión unaria
                return new UnaryExpressionNode(op, right);
            }
        }
        return parsePrimary();
    }
   
    void error(const Token& token, const string& message)
    {
        cerr << "Error in line " << token.line << ", column " << token.column << ": " << message << endl;
    }

   
    ASTNode* parseLetExpression() {
        advance();

        vector<ASTNode*> assignments;

        ASTNode* assign = parseAssignment();
        assignments.push_back(assign);

        while (match(TokenType::punc_Comma))
        {
            ASTNode* assign = parseAssignment();
            assignments.push_back(assign);      // PUSH_BACK ?????
        }


        
        /*
        if (!check(TokenType::Identifier)) {
            error(peek(), "Se esperaba un identificador luego de 'let'.");
            return nullptr;
        }
        Token idToken = advance();

        if (!matchOperator("=")) {
            error(peek(), "Se esperaba '=' luego del identificador en la declaración let.");
            return nullptr;
        }

        ASTNode* initializer = parseExpression();
        if (!initializer) {
            error(peek(), "Error al parsear la expresión de inicialización.");
            return nullptr;
        }
        
        */
        if (!check(TokenType::kw_in)) {
            errorHandler.reportError(peek(), "Se esperaba 'in' después de las asignaciones.");
            return nullptr;
        }
        advance(); // Consume "in"


        ASTNode* body = parseExpression();
        if (!body) {
            errorHandler.reportError(peek(), "Error al parsear el cuerpo de la expresión let.");
            return nullptr;
        }

        

        return new LetExpressionNode(assignments, body);
    }

    ASTNode* parseAssignment() {

        if (!check(TokenType::Identifier)) {
            errorHandler.reportError(peek(), "Se esperaba un identificador en la declaración let.");
            return nullptr;
        }
        Token idToken = advance();

        if (!matchOperator(TokenType::Assignment)) {
            errorHandler.reportError(peek(), "Se esperaba '=' luego del identificador en la declaración let.");
            return nullptr;
        }

        ASTNode* value = parseExpression();
        if (!value) {
            errorHandler.reportError(peek(), "Error al parsear la expresión de inicialización.");
            return nullptr;
        }        

        return new AtributeNode(idToken.lexeme, value);
    }

    

    ASTNode* parseExpressionPrime(ASTNode* left)
    {
        if(check(TokenType::Operator))
        {
            string op = peek().lexeme;
            if(op == "+" || op == "-")
            {
                advance();
                ASTNode* term = parseTerm();
                if(!term)
                {
                    errorHandler.reportError(peek(), "Se esperaba un término después del operador.");
                    return nullptr;
                }
                ASTNode* newLeft = new ExpressionNode(left, op, term);
                return parseExpressionPrime(newLeft);
            }
        }
        return left;
    }

    
    ASTNode* parseTermPrime(ASTNode* left) {
        if (check(TokenType::Operator)) {
            string op = peek().lexeme;
            if (op == "*" || op == "/") {
                advance();
                ASTNode* factor = parseFactor();
                if (!factor) {
                    errorHandler.reportError(peek(), "Se esperaba un factor después del operador.");
                    return nullptr;
                }
                ASTNode* newLeft = new ExpressionNode(left, op, factor);
                return parseTermPrime(newLeft);
            }
        }
        return left;
    }

    
    vector<ASTNode*> parseArgumentList()
    {
        vector<ASTNode*> arguments;

        if (check(TokenType::punc_RightParen)) return arguments;

        ASTNode* arg = parseExpression();
        if (arg == nullptr)
        {
            errorHandler.reportError(peek(), "Error al parsear un argumento en la lista.");
            return arguments;
        }
        arguments.push_back(arg);

        while(match(TokenType::punc_Comma))
        {
            arg = parseExpression();
            if (arg == nullptr)
            {
                errorHandler.reportError(peek(), "Error al parsear un argumento en la lista.");
                break;
            }
            arguments.push_back(arg);
        }
        return arguments;
    }

    
};


void printAST(ASTNode* node, const std::string &indent = "", bool isLast = true) {
    if (!node) return;

    // Imprime la indentación y la "rama" actual.
    std::cout << indent;
    std::cout << (isLast ? "'-- " : "|-- ");

    // Si es un nodo LetExpression, imprimimos sus asignaciones y luego el body.
    if (auto letExpr = dynamic_cast<LetExpressionNode*>(node)) {
        std::cout << "Let Expression" << "\n";
        // Recorremos las asignaciones
        for (size_t i = 0; i < letExpr->assignments.size(); i++) {
            printAST(letExpr->assignments[i],
                     indent + (isLast ? "    " : "|   "),
                     i == letExpr->assignments.size() - 1);
        }
        // Indicamos el comienzo del cuerpo (in)
        std::cout << indent << (isLast ? "'-- " : "|-- ") << "in" << "\n";
        printAST(letExpr->body, indent + (isLast ? "    " : "|   "), true);
    }
    // Si es un nodo de asignación (AssignmentNode)
    else if (auto assign = dynamic_cast<AtributeNode*>(node)) {
        std::cout << "Assignment: " << assign->variable << " =" << "\n";
        printAST(assign->expression, indent + (isLast ? "    " : "|   "), true);
    }
    // Si es un statement (StatementNode)
    else if (auto stmt = dynamic_cast<StatementNode*>(node)) {
        std::cout << "Statement" << "\n";
        printAST(stmt->expression, indent + (isLast ? "    " : "|   "), true);
    }
    // Si es una expresión binaria (ExpressionNode)
    else if (auto expr = dynamic_cast<ExpressionNode*>(node)) {
        std::cout << "Expr: " << expr->op << "\n";
        printAST(expr->left, indent + (isLast ? "    " : "|   "), false);
        printAST(expr->right, indent + (isLast ? "    " : "|   "), true);
    }
    // Si es una llamada a función
    else if (auto func = dynamic_cast<FunctionCallNode*>(node)) {
        std::cout << "FuncCall: " << func->name << "\n";
        for (size_t i = 0; i < func->arguments.size(); i++) {
            printAST(func->arguments[i],
                     indent + (isLast ? "    " : "|   "),
                     i == func->arguments.size() - 1);
        }
    }
    // Si es un número
    else if (auto num = dynamic_cast<NumberNode*>(node)) {
        std::cout << "Number: " << num->value << "\n";
    }
    // Si es una variable
    else if (auto var = dynamic_cast<VariableNode*>(node)) {
        std::cout << "Variable: " << var->name << "\n";
    }
    else {
        std::cout << "Nodo desconocido\n";
    }
}


