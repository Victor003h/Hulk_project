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
        context.CreateType(node->name.lexeme);
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