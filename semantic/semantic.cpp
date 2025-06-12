#include <string>
#include "../semantic/symbolTable.cpp"
#include "../parser/parser.cpp"

using namespace std;

class SemanticAnalyzer : public AstVisitor {
private:
    SymbolTable symbolTable;
    vector<string> errors; // Almacenará mensajes de error semántico.
    
    void reportError(const string& msg) {
        errors.push_back(msg);
        cerr << "Error semántico: " << msg << endl;
    }
public:
    // Devuelve la lista de errores acumulados.
    const vector<string>& getErrors() const { return errors; }
    
    bool isNumeric(const DataType& dt) {
        return dt == TYPE_NUMBER ;
    }

    virtual void visit(NumberNode* node) override {}
    
    virtual void visit(StringNode* node) override {}

    virtual void visit(VariableNode* node) override {
        TypeInfo info;
        if (!symbolTable.lookup(node->name, info)) {
            reportError("La variable '" + node->name + "' no está declarada.");
            node->inferredType = TYPE_ERROR;
        } else {
            node->inferredType = info.dt;
        }
    }

    virtual void visit(UnaryExpressionNode* node) override {
        if (node->right)
            node->right->accept(this);
        else {
            reportError("El operando para el unario '" + node->op + "' es nulo.");
            node->inferredType = TYPE_ERROR;
            return;
        }
        
        if (node->op == "-" || node->op == "+") {
            if (!isNumeric(node->right->inferredType)) {
                reportError("El operador unario '" + node->op + "' se aplica a un operando no numérico.");
                node->inferredType = TYPE_ERROR;
            } else {
                node->inferredType = node->right->inferredType;
            }
        } else {
            reportError("Operador unario '" + node->op + "' no soportado.");
            node->inferredType = TYPE_ERROR;
        }
    }
    
    virtual void visit(ExpressionNode* node) override {
        if (node->left)  node->left->accept(this);
        if (node->right) node->right->accept(this);

        if (node->op == "+")
        {
            if (node->right->inferredType == TYPE_STRING || node->left->inferredType == TYPE_STRING)
            {
                if (node->left->inferredType != TYPE_STRING || node->right->inferredType != TYPE_STRING) {
                    reportError("Incompatibilidad de tipos en la operación '" + node->op + "'.");
                    node->inferredType = TYPE_ERROR;
                } else {
                    node->inferredType = TYPE_STRING;
                }
            }
        }
        
        if (node->op == "+" || node->op == "-" || node->op == "*" || node->op == "/") {
            if (node->left->inferredType != TYPE_NUMBER || node->right->inferredType != TYPE_NUMBER) {
                reportError("Incompatibilidad de tipos en la operación '" + node->op + "'.");
                node->inferredType = TYPE_ERROR;
            } else {
                node->inferredType = TYPE_NUMBER;
            }
        } else if (node->op == "==") {
            if (node->left->inferredType != node->right->inferredType) {
                reportError("Incompatibilidad de tipos en la comparación '" + node->op + "'.");
                node->inferredType = TYPE_ERROR;
            } else {
                node->inferredType = TYPE_NUMBER;
            }
        } else {
            reportError("Operador desconocido: " + node->op);
            node->inferredType = TYPE_ERROR;
        }

        // Inferir el tipo combinando los tipos de ambos operandos.
        node->inferredType = combineTypes(node->left->inferredType, node->right->inferredType);
    }
    
    virtual void visit(AtributeNode* node) override {
        if (node->expression) node->expression->accept(this);
        TypeInfo info;
        if (!symbolTable.lookup(node->variable, info)) {
            // Adicionar la variable al ambito actual
            TypeInfo newInfo { node->expression->inferredType };
            if (!symbolTable.addSymbol(node->variable, newInfo)) {
                reportError("La variable '" + node->variable + "' ya está declarada en este ámbito.");
                node->inferredType = TYPE_ERROR;
            } else {
                node->inferredType = newInfo.dt;
            }
        } else {
            // La variable ya está declarada. Comprobar compatibilidad de tipos.
            DataType resultType = combineTypes(info.dt, node->expression->inferredType);
            if (resultType == TYPE_ERROR || info.dt != resultType) {
                reportError("Incompatibilidad de tipos en asignación a '" + node->variable + "'.");
                node->inferredType = TYPE_ERROR;
            } else {
                node->inferredType = info.dt;
            }
        }
    }
    
    virtual void visit(LetExpressionNode* node) override {
        symbolTable.enterScope();
        for (ASTNode* n : node->assignments) {
            n->accept(this);
        }
        if (node->body) node->body->accept(this);
        node->inferredType = (node->body ? node->body->inferredType : TYPE_ERROR);
        symbolTable.exitScope();
    }

    virtual void visit(FunctionCallNode* node) override {
        node->inferredType = TYPE_ERROR;
    }
    
    virtual void visit(StatementNode* node) override {
        if (node->expression) {
            node->expression->accept(this);
            node->inferredType = node->expression->inferredType;
        } else {
            node->inferredType = TYPE_ERROR;
        }
    }
};


// int main() {

//     vector<Token> tokens;
    
//     /*
//     tokens.push_back(Token{TokenType::Identifier, "x", 1, 1, ""});
//     tokens.push_back(Token{TokenType::Operator, "=", 1, 3, ""});
//     tokens.push_back(Token{TokenType::Number, "42", 1, 5, ""});
//     tokens.push_back(Token{TokenType::Operator, "+", 1, 8, ""});
//     tokens.push_back(Token{TokenType::Number, "5", 1, 10, ""});
//     tokens.push_back(Token{TokenType::Operator, "*", 1, 12, ""});
//     tokens.push_back(Token{TokenType::LParent, "(", 1, 14, ""});
//     tokens.push_back(Token{TokenType::Number, "3", 1, 15, ""});
//     tokens.push_back(Token{TokenType::Operator, "-", 1, 17, ""});
//     tokens.push_back(Token{TokenType::Number, "1", 1, 19, ""});
//     tokens.push_back(Token{TokenType::RParent, ")", 1, 20, ""});
//     tokens.push_back(Token{TokenType::Semicolon, ";", 1, 21, ""});
//     tokens.push_back(Token{TokenType::Keyword, "", -1, -1, "EOF"});
//     */

//     " x=42+5 * (3-1);  ";
    
//     tokens.push_back(Token{TokenType::Keyword, "let", 1, 1});
//     tokens.push_back(Token{TokenType::Identifier, "x", 1, 6});
//     tokens.push_back(Token{TokenType::Operator, "=", 1, 7});
//     tokens.push_back(Token{TokenType::Number, "8", 1, 8});
//     tokens.push_back(Token{TokenType::Comma, ",", 1, 9});
//     tokens.push_back(Token{TokenType::Identifier, "y", 1, 10});
//     tokens.push_back(Token{TokenType::Operator, "=", 1, 11});
//     tokens.push_back(Token{TokenType::Number, "2", 1, 12});
//     tokens.push_back(Token{TokenType::Keyword, "in", 1, 13});
//     tokens.push_back(Token{TokenType::Identifier, "x", 1, 14});
//     tokens.push_back(Token{TokenType::Operator, "*", 1, 15});
//     tokens.push_back(Token{TokenType::Operator, "+", 1, 15});
//     tokens.push_back(Token{TokenType::Number, "2", 1, 16});
//     tokens.push_back(Token{TokenType::Semicolon, ";", 1, 17});
    

//     Parser parser(tokens);
//     ASTNode* ast = parser.parse();
    
//     cout << "\nAST:" << endl;
//     printAST(ast);
    
//     SemanticAnalyzer analyzer;
//     if (ast)
//         ast->accept(&analyzer);
    
    
//     return 0;
// }