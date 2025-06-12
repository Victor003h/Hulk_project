#include "context.h"
#include "visitor.cpp"


class FunctionCollectorVisitor: public Visitor
{
    public:
    Context context;

    
    void visit(ProgramNode* node)   override
     {
        context= new Context();
        for(auto stmt : node->stmts)
        {
            if (TypeNode* type = dynamic_cast<TypeNode*>(stmt)) {
               visit(type);
            }
        };
    }
  
    

    void visit(MethodNode* node)
    {
        Type returnType=context.GetType(node->type);
        std::vector<Atribute> args;
        for(auto param:node->params)
        {
            if (IdentifierNode* p = dynamic_cast<IdentifierNode*>(param)) {
            args.push_back(Atribute(p->value.lexeme,p->type));
            }
            else
            {
                throw std::runtime_error("Unexpected error in builder method  "+ node->id.lexeme);
            }
        }
        context.DefineMethod(node->id.lexeme,node->type,args);
         
    };


    void visit(TypeNode* node)  override {}; 
    void visit(BlockNode* node)          {};
    void visit(BinaryExpression* node)   {};
    void visit(LiteralNode* node)        {};
    void visit(IdentifierNode* node)     {}; 
    void visit(AtributeNode* node)       {};
    
    void visit(IfExpression* node)       {};
    void visit(WhileExpression* node)    {};
    void visit(ForExression* node)       {};
    void visit(LetExpression* node)      {}; 

};




