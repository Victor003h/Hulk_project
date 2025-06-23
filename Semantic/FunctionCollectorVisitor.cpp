#include "context.h"
#include "../Parser/AstNodes.h"
#include"../common/Error.hpp"

class FunctionCollectorVisitor: public Visitor
{
    public:
    Context* context;
    ErrorHandler errorHandler;

    FunctionCollectorVisitor( ErrorHandler& errorHandler)
    :errorHandler(errorHandler){}
    
    void visit(ProgramNode* node)   override
     {
        context= new Context();
        context->loadInternalTypeAndMethod();
        for(auto stmt : node->stmts)
        {
           stmt->accept(*this);
        };
    }
  
    
    void visit(MethodNode* node)
    {
        if(context->exist_Method(node->id.lexeme))
        {
            std::string msg="The method method  "+ node->id.lexeme +" is already defined";
            errorHandler.reportError(node->id,msg);
            return;
        }

        std::vector<Attribute> args;
        for(auto param:node->params)
        {
            if (IdentifierNode* p = dynamic_cast<IdentifierNode*>(param)) {
            args.push_back(Attribute(p->value.lexeme,p->type));
            }
            else
            {
                std::string msg="Unexpected error in builder method  "+ node->id.lexeme;
                errorHandler.reportError(node->id,msg);
                return;
            }
        }
        context->DefineMethod(node->id.lexeme,node->type,args);
         
    };
    void visit(AttributeNode* node)       
    {
        
    };


    void visit(TypeNode* node)  override {}; 
    void visit(BlockNode* node)          {};
    void visit(BinaryExpression* node)   {};
    void visit(LiteralNode* node)        {};
    void visit(IdentifierNode* node)     {}; 
    
    void visit(IfExpression* node)       {};
    void visit(WhileExpression* node)    {};
    void visit(ForExression* node)       {};
    void visit(LetExpression* node)      {}; 
    void visit(UnaryExpression* node)      {};
    void visit(FunCallNode* node)        {};
    void visit(MemberCall* node)        {};
    void visit(DestructiveAssignNode* node) {};
    void visit(TypeInstantiation* node)    {}    ;
};

