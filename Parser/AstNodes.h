
#pragma once
#include<string>
#include "../Lexer/Token.h"
#include<iostream>
#include<vector>


class ProgramNode;
class TypeNode;
class BlockNode;
class BinaryExpression;
class LiteralNode;
class IdentifierNode;
class AtributeNode;
class MethodNode;
class IfExpression;
class WhileExpression;
class ForExression;
class LetExpression;
class UnaryExpression;
class FunCallNode;
class MemberCall;

class Visitor {
public:
    virtual ~Visitor() = default;

    virtual void visit(ProgramNode* node)       = 0;
    virtual void visit(TypeNode* node)          = 0;
    virtual void visit(BlockNode* node)         = 0;
    virtual void visit(BinaryExpression* node)  = 0;
    virtual void visit(LiteralNode* node)         = 0;
    virtual void visit(IdentifierNode* node)      = 0;
    virtual void visit(AtributeNode* node)        = 0;
    virtual void visit(MethodNode* node)          = 0;
    virtual void visit(IfExpression* node)        = 0;
    virtual void visit(WhileExpression* node)     = 0;
    virtual void visit(ForExression* node)        = 0;
    virtual void visit(LetExpression* node)         = 0;
    virtual void visit(UnaryExpression* node)         = 0;
    virtual void visit(FunCallNode* node)        = 0;
    virtual void visit(MemberCall* node)        = 0;

};

class AstNode {
public:
    virtual ~AstNode() {}
    virtual void print(int indent = 0) const = 0;
    std::string type="Object";
    virtual std::string getType()=0;
    virtual void setType(std::string type)=0;
    virtual void accept(Visitor &visitor)=0; // Método virtual puro para el Visitor
};

class Expression : public AstNode {
public:
    virtual ~Expression() {}
    // Si Expression no se instancia directamente, puede dejarse sin implementación de accept,
    // para que las clases concretas (BinaryExpression, LiteralNode, etc.) lo definan.
    virtual void accept(Visitor &visitor) = 0;
    virtual std::string getType()=0;
    virtual void setType(std::string type)=0;

};

class BlockNode : public AstNode {
public:
    std::vector<AstNode*> exprs;
    BlockNode(std::vector<AstNode*> exprs)
        : exprs(exprs) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Block" << "\n";
        for (auto exp : exprs) {
            exp->print(indent + 2);
        }
    }
    std::string getType(){return "";};
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string type){};


};

class ProgramNode : public AstNode {
public:
    std::vector<AstNode*> stmts;
    ProgramNode() {};

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Program" << "\n";
        for (auto exp : stmts) {
            exp->print(indent + 2);
        }
    }

    std::string getType(){return "";};
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string type){};

};

class AtributeNode : public AstNode {
public:
    Token id;
    std::string type="Object";
    AstNode* expression;

    AtributeNode(Token id, AstNode* expression)
        : id(id), expression(expression) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Assignment: " << id.lexeme << " = " << std::endl;
        expression->print(indent + 2);
    }
    std::string getType()override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}

};

class MethodNode : public AstNode {
public:
    Token id;
    std::vector<AstNode*> params;
    AstNode* body;
    std::string type="Object";
    
    MethodNode(Token id, std::vector<AstNode*> params, AstNode* body)
        : id(id), params(params), body(body) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "MethodNode " << id.lexeme << ":" << std::endl;
        std::cout << spacing << "  Params:" << std::endl;
        for (const auto& param : params) {
            param->print(indent + 4);
        }
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    
    void setType(std::string t){type=t;}
};

class TypeNode : public AstNode {
public:
    Token name;
    std::vector<AstNode*> atributes;
    std::vector<AstNode*> methods;


    TypeNode(Token name, std::vector<AstNode*> atributes, std::vector<AstNode*> methods)
        : name(name), atributes(atributes), methods(methods) {
        
    }

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Type Expression: " << name.lexeme << std::endl;
        std::cout << spacing << "  Atributes:" << std::endl;
        for (const auto& atrib : atributes) {
            atrib->print(indent + 4);
        }
        std::cout << spacing << "  Methods:" << std::endl;
        for (const auto& meth : methods) {
            meth->print(indent + 4);
        }
    }
    std::string getType(){return "";};
    void setType(std::string t){}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
};

class BinaryExpression : public Expression {
public:
    AstNode* left;
    Token op;
    AstNode* right;
    std::string type="Object";

    BinaryExpression(AstNode* left, Token op, AstNode* right)
        : left(left), op(op), right(right) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "AstBinaryExp (" << op.lexeme << "):" << "\n";
        if (left) left->print(indent + 2);
        if (right) right->print(indent + 2);
    }
    std::string getType()override  {return type;}
    
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class LiteralNode : public Expression {
public:
    Token value;
    std::string type;


    LiteralNode(Token value)
        : value(value) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << value.lexeme << "\n";
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class IdentifierNode : public Expression {
public:
    Token value;
    std::string type="Object";

    IdentifierNode(Token value)
        : value(value) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << value.lexeme << "\n";
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class IfExpression : public AstNode {
public:
    AstNode* defaultExp;
    std::vector<std::pair<AstNode*, AstNode*>> exprs_cond;
    std::string type="Object";

    IfExpression(AstNode* defaultExp, std::vector<std::pair<AstNode*, AstNode*>> exprs_cond)
        : defaultExp(defaultExp), exprs_cond(exprs_cond) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "If Expression:" << std::endl;
        for (const auto& exp_cond : exprs_cond) {
            std::cout << spacing << "  Condition:" << std::endl;
            exp_cond.first->print(indent + 4);
            std::cout << spacing << "  Exp:" << std::endl;
            exp_cond.second->print(indent + 4);
        }
        std::cout << spacing << "  Default:" << std::endl;
        defaultExp->print(indent + 4);
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class WhileExpression : public AstNode {
public:
    AstNode* condition;
    AstNode* body;
    std::string type="Object";

    WhileExpression(AstNode* condition, AstNode* body)
        : condition(condition), body(body) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "While Expression:" << std::endl;
        std::cout << spacing << "  Condition:" << std::endl;
        condition->print(indent + 4);
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class ForExression : public AstNode {
public:
    AstNode* item;
    AstNode* iterable;
    AstNode* body;
    std::string type="Object";

    ForExression(AstNode* item, AstNode* iterable, AstNode* body)
        : item(item), iterable(iterable), body(body) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "For Expression:" << std::endl;
        std::cout << spacing << "  Item:" << std::endl;
        item->print(indent + 4);
        std::cout << spacing << "  Iterable:" << std::endl;
        iterable->print(indent + 4);
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }
    std::string getType() override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};

class LetExpression : public Expression {
public:
    std::vector<AstNode*> assignments;
    AstNode* body;
    std::string type="Object";

    LetExpression(const std::vector<AstNode*>& assignments, AstNode* body)
        : assignments(assignments), body(body) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Let Expression:" << std::endl;
        std::cout << spacing << "  Assignments:" << std::endl;
        for (const auto& assign : assignments) {
            assign->print(indent + 4);
        }
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }
    std::string getType()  override  {return type;}
    
    void accept(Visitor &visitor) override {
        visitor.visit(this);
    }
    void setType(std::string t){type=t;}
};



class UnaryExpression :public Expression
{
    public:
    Token value;
    AstNode* exp;
    std::string type="Object";

        UnaryExpression(Token value,AstNode* exp)
        : value(value),exp(exp){}


         void print(int indent = 0) const override 
         {
            std::string spacing(indent, ' ');
            std::cout << spacing << "Unary Expression: " <<value.lexeme<< std::endl;
            std::cout << spacing << "  exp:" << std::endl;
            exp->print(indent + 4);
         }
        std::string getType()  override  {return type;}
    

        void accept(Visitor &visitor) override {
            visitor.visit(this);

        }
        void setType(std::string t){type=t;}
};


class FunCallNode : public Expression {
public:
    Token id; 
    std::vector<AstNode*> arguments; 
    std::string type = "Object"; 

    FunCallNode(const Token& id, const std::vector<AstNode*>& arguments)
        : id(id), arguments(arguments) {}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "FunCall: " << id.lexeme <<std::endl ;
         std::cout << spacing << "Arg: " <<std::endl  ;
        for (size_t i = 0; i < arguments.size(); ++i) {
            arguments[i]->print(indent+4);
        }
     
    }

    std::string getType() override {
        return type;
    }

    void accept(Visitor &visitor) override {
            visitor.visit(this);

        }
        void setType(std::string t){type=t;}
};

class MemberCall:public AstNode
{
    public:
    AstNode* obj;
    AstNode* member;
    std::string type;

    MemberCall(AstNode* obj,AstNode* member)
    : obj(obj),member(member)
    {}


     void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "MemberCall: "  <<std::endl ;
        std::cout << spacing << "object: " <<std::endl ;
        obj->print(indent+4);
        std::cout << spacing << "Member: " <<std::endl  ;
        member->print(indent+4);
    }

    std::string getType() override {
        return type;
    }

    void accept(Visitor &visitor) override {
            visitor.visit(this);

        }
        void setType(std::string t){type=t;}
};


