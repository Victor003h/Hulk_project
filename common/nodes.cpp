#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include "DataTypes.h"
using namespace std;

class NumberNode;
class UnaryExpressionNode;
class StringNode;
class VariableNode;
class ExpressionNode;
class LetExpressionNode;
class FunctionCallNode;
class StatementNode;
class AtributeNode;

// Interfaz del visitor.
class AstVisitor {
public:
    virtual void visit(NumberNode* node) = 0;
    virtual void visit(UnaryExpressionNode* node) = 0;
    virtual void visit(StringNode* node) = 0;
    virtual void visit(VariableNode* node) = 0;
    virtual void visit(ExpressionNode* node) = 0;
    virtual void visit(LetExpressionNode* node) = 0;
    virtual void visit(FunctionCallNode* node) = 0;
    virtual void visit(StatementNode* node) = 0;
    virtual void visit(AtributeNode* node) = 0;
    virtual ~AstVisitor() {}
};


class ASTNode {
public:
    DataType inferredType;
    virtual void accept(AstVisitor *visitor) = 0;
    virtual ~ASTNode() = default;
};

class ExpressionNode : public ASTNode {
public:
    ASTNode* left;
    string op;
    ASTNode* right;

    ExpressionNode(ASTNode* l, const string& oper, ASTNode* r)
        : left(l), op(oper), right(r) {}
    void accept(AstVisitor *visitor) override {
        visitor->visit(this);
    }
};

class AtributeNode : public ASTNode {
public:
    string variable;
    ASTNode* expression;

    AtributeNode(const string& var, ASTNode* expr)
        : variable(var), expression(expr) {}
    void accept(AstVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StatementNode : public ASTNode {
public:
    ASTNode* expression;

    StatementNode(ASTNode* expr)
        : expression(expr) {}
    void accept(AstVisitor *visitor) override {
        visitor->visit(this);
    }
};

class NumberNode : public ASTNode {
public:
    std::string value;
    NumberNode(const std::string& val) : value(val) {
        inferredType = TYPE_NUMBER;
    }
    void accept(AstVisitor *visitor) override {
        visitor->visit(this);
    }
};

class UnaryExpressionNode : public ASTNode {
public:
    std::string op;
    ASTNode* right;
    UnaryExpressionNode(const string& oper, ASTNode* r) : op(oper), right(r)  {
        inferredType = TYPE_NUMBER;
    }
    void accept(AstVisitor *visitor) override {
        visitor->visit(this);
    }
};

class StringNode : public ASTNode{
public:
    string value;
    StringNode(const std::string& val) : value(val) {
        inferredType = TYPE_STRING;
    }
    void accept(AstVisitor *visitor) override {
        visitor->visit(this);
    }
};

class VariableNode : public ASTNode {
public:
    std::string name;
    VariableNode(const std::string& id) : name(id) {}
    void accept(AstVisitor *visitor) override {
        visitor->visit(this);
    }
};

class FunctionCallNode : public ASTNode{
public:
  string name;
  vector<ASTNode*> arguments;
  
  FunctionCallNode(const string &name, const vector<ASTNode*>& args):
    name(name), arguments(args) {}
  void accept(AstVisitor *visitor) override {
        visitor->visit(this);
    }
};

class LetExpressionNode : public ASTNode {
public:
    vector<ASTNode*> assignments;  
    ASTNode* body;       

    LetExpressionNode(vector<ASTNode*> assign, ASTNode* bodyExpr)
        : assignments(assign), body(bodyExpr) { }
    void accept(AstVisitor *visitor) override {
        visitor->visit(this);
    }
};
