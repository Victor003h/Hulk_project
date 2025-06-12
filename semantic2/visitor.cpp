#include "../Parser/AstNodes.cpp"

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
};