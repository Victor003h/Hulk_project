#include "context.h"
#include <stdexcept> 
#include "../Parser/AstNodes.h"
class TypeBuilderVisitor :public Visitor
{

    public :
        Context* context;
        Type* currentType;

        

        void visit(ProgramNode* node) override
        {
            for(auto stmt : node->stmts)
            {
                if (TypeNode* type = dynamic_cast<TypeNode*>(stmt)) {
                visit(type);
                }
            };

        }
       
        void visit(TypeNode* node) override
        {
            currentType=context->GetType(node->name.lexeme);
            
            for(auto atribute : node->atributes)
            {
                if (AttributeNode* type = dynamic_cast<AttributeNode*>(atribute)) {
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

        void visit(AttributeNode* node) override
        {

            //Type atrType=context.GetType(node->type);

            currentType->DefineAttribute(node->id.lexeme,node->type);

        }

        void visit(MethodNode* node) override
        {  

            std::vector<Attribute> args;
            
            std::vector<Attribute> arg;
            for(auto param:node->params)
            {
                if (IdentifierNode* p = dynamic_cast<IdentifierNode*>(param)) {
                args.push_back(Attribute(p->value.lexeme,p->type));
                }
                else
                {
                    return;
                    //throw std::runtime_error("Unexpected error in builder method  "+ node->id.lexeme);
                }
            }
            context->DefineMethod(node->id.lexeme,node->type,args);
        };     


        void visit(BlockNode* node)    {};
        void visit(BinaryExpression* node)  {};
        void visit(LiteralNode* node)         {};
        void visit(IdentifierNode* node)     {};
        void visit(IfExpression* node)        {};
        void visit(WhileExpression* node)     {};
        void visit(ForExression* node)        {};
        void visit(LetExpression* node)       {};
        void visit(UnaryExpression* node)       {};
        void visit(FunCallNode* node)        {};
        void visit(MemberCall* node)        {};
        void visit(DestructiveAssignNode* node) {};
        void visit(TypeInstantiation* node)    {}    ;


};
