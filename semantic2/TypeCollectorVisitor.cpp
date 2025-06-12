#include "context.h"
#include "visitor.cpp"


class TypeCollectorVisitor: public Visitor
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
  
    void visit(TypeNode* node)  override
    {
    
        if(!context.exist_Type(node->name.lexeme))
            context.CreateType(node->name.lexeme);
        throw std::runtime_error("the type "+node->name.lexeme+"already exists" );
    };

    void visit(BlockNode* node)          {};
    void visit(BinaryExpression* node)   {};
    void visit(LiteralNode* node)        {};
    void visit(IdentifierNode* node)     {}; 
    void visit(AtributeNode* node)       {};
    void visit(MethodNode* node)         {};
    void visit(IfExpression* node)       {};
    void visit(WhileExpression* node)    {};
    void visit(ForExression* node)       {};
    void visit(LetExpression* node)      {};
    
};