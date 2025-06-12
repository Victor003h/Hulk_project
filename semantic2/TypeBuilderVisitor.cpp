#include "context.h"
#include "visitor.cpp"

class TypeBuilderVisitor :public Visitor
{

    public :
    Context context;
    Type currentType;

    void visit(ProgramNode* node)
    {
        for(auto stmt : node->stmts)
        {
            if (TypeNode* type = dynamic_cast<TypeNode*>(stmt)) {
               visit(type);
            }
        };

    }
    void visit(TypeNode* node) 
    {
        currentType=context.GetType(node->name.lexeme);
        for(auto atribute : node->atributes)
        {
            if (AtributeNode* type = dynamic_cast<AtributeNode*>(atribute)) {
               visit(type);
            }
        };

        for(auto meth : node->methods)
        {
            if (MethodNode* type = dynamic_cast<MethodNode*>(meth)) {
               visit(type);
            }
        };

    }

    void visit(AtributeNode* node)
    {
        Type atrType=context.GetType(node->type);
        currentType.DefineAtribute(node->id.lexeme,atrType);

    }

     virtual void visit(MethodNode* node)
     {  


     };     

    virtual void visit(BlockNode* node)         = 0;
    virtual void visit(BinaryExpression* node)  = 0;
    virtual void visit(LiteralNode* node)         = 0;
    virtual void visit(IdentifierNode* node)      = 0;
    virtual void visit(IfExpression* node)        = 0;
    virtual void visit(WhileExpression* node)     = 0;
    virtual void visit(ForExression* node)        = 0;
    virtual void visit(LetExpression* node)         = 0;

};