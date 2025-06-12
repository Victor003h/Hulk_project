
#include<string>
#include "../Lexer/Token.h"
#include<iostream>
#include<vector>

class AstNode
{
    public:
        virtual ~AstNode(){}
        virtual void print(int indent=0) const =0;
};


class Expression:public AstNode
{
    public:
        virtual ~Expression(){}

};

class BlockNode:public AstNode
{
    public:
        std::vector<AstNode*> exprs;


        BlockNode(std::vector<AstNode*> exprs)
        :exprs(exprs){}

        void print(int indent = 0) const override {
             std::string spacing(indent, ' ');
            std::cout <<spacing <<"Block"<< "\n";
            for(auto exp:exprs)
            {
                exp->print(indent+2);
            }

        }


};

class ProgramNode:public AstNode
{
    public:
        std::vector<AstNode*> stmts;

        ProgramNode(){};

        void print(int indent = 0) const override {
             std::string spacing(indent, ' ');
            std::cout <<spacing <<"Program"<< "\n";
            for(auto exp:stmts)
            {
                exp->print(indent+2);
            }

        }
};


class AtributeNode:public AstNode
{
    public:
        Token id;
        std::string type;
        AstNode* expression;

        AtributeNode(Token id, AstNode* expression)
        : id(id), expression(expression) {}

        void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Assignment: " << id.lexeme << " = " << std::endl;
        expression->print(indent + 2);
    }
        

};

class MethodNode:public AstNode
{
    public:
        Token id;
        std::vector<AstNode*> params;
        AstNode* body;
        
        MethodNode(Token id,std::vector<AstNode*> params,AstNode*body)
        :id(id),params(params),body(body){}


         void print(int indent = 0) const override{
                std::string spacing(indent, ' ');
                std::cout << spacing << "MethodNode "<< id.lexeme<<":" << std::endl;
                std::cout << spacing << "  Params:" << std::endl;
                for (const auto& param : params) {
                    param->print(indent + 4);
                }
                std::cout << spacing << "  Body:" << std::endl;
                body->print(indent + 4);
        }


};


class TypeNode:public AstNode
{
    public:
    Token name;
    std::vector<AstNode*> atributes;
    std::vector<AstNode*> methods;

    TypeNode(Token name,std::vector<AstNode*> atributes,std::vector<AstNode*> methods)
    :name(name),atributes(atributes),methods(methods)
    {

        for(auto atribute : atributes)
        {
            if (AtributeNode* type = dynamic_cast<AtributeNode*>(atribute)) {
               type->type=name.lexeme;
            }
        };

    }


    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "Type Expression:" <<name.lexeme<< std::endl;
        std::cout << spacing << "  Atributes:" << std::endl;
        for (const auto& atrib : atributes) {
            atrib->print(indent + 4);
        }
        std::cout << spacing << "  Methods:" << std::endl;
        for (const auto& meth : methods) {
            meth->print(indent + 4);
        }
        
    }

};

class BinaryExpression:public Expression
{
    public:
        AstNode* left;
        Token op;
        AstNode* right;

        BinaryExpression(AstNode* left,Token op,AstNode* right)
        : left(left),op(op),right(right){}

        void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "AstBinaryExp (" << op.lexeme << "):\n";
        if (left)  left->print(indent + 2);
        if (right) right->print(indent + 2);
    }
};

class LiteralNode: public Expression
{
    public:
        Token value;

        LiteralNode(Token value)
        : value(value){}

        void print(int indent = 0) const override {
            std::string spacing(indent, ' ');
            std::cout << spacing << value.lexeme << "\n";
            
    }

};

class IdentifierNode: public Expression
{
    public:
        Token value;

        IdentifierNode(Token value)
        : value(value){}

        void print(int indent = 0) const override {
            std::string spacing(indent, ' ');
            std::cout << spacing << value.lexeme << "\n";
            
    }

};


class IfExpression:public AstNode
{
    public:
    AstNode* defaultExp;
    std::vector<std::pair<AstNode*,AstNode*>> exprs_cond;

     IfExpression(AstNode* defaultExp,std::vector<std::pair<AstNode*,AstNode*>> exprs_cond)
     :defaultExp(defaultExp),exprs_cond(exprs_cond){}


     void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "If Expression:" << std::endl;
        for (const auto& exp_cond : exprs_cond) {
            std::cout << spacing << "  Condition:" << std::endl;
            exp_cond.first->print(indent+4);
            std::cout << spacing << "  Exp:" << std::endl;
            exp_cond.second->print(indent+4);
        }
        std::cout << spacing << "  Default:" << std::endl;
        defaultExp->print(indent + 4);
    }
    
};

class WhileExpression:public AstNode
{
    public:
    AstNode* condition;
    AstNode* body;

    WhileExpression(AstNode* condition,AstNode* body)
    :condition(condition),body(body){}

    
    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "While Expression:" << std::endl;
        std::cout << spacing << "  Condition:" << std::endl;
        condition->print(indent+4);
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }



};

class ForExression:public AstNode
{
    public:
    AstNode* item ;
    AstNode* iterable;
    AstNode* body;

    ForExression(AstNode* item,AstNode* iterable,AstNode* body)
    :item(item),iterable(iterable),body(body){}

    void print(int indent = 0) const override {
        std::string spacing(indent, ' ');
        std::cout << spacing << "For Expression:" << std::endl;
        std::cout << spacing << "  Item:" << std::endl;
        item->print(indent+4);
        std::cout << spacing << "  Iterable:" << std::endl;
        iterable->print(indent+4);
        std::cout << spacing << "  Body:" << std::endl;
        body->print(indent + 4);
    }

};

class LetExpression:public Expression
{
public:
    std::vector<AstNode*> assignments;
    AstNode* body;

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


};
