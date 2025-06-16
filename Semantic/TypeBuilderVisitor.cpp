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

        void visit(AtributeNode* node) override
        {

            //Type atrType=context.GetType(node->type);

            currentType->DefineAtribute(node->id.lexeme,node->type);

        }

        void visit(MethodNode* node) override
        {  
            Type* returnType=context->GetType(node->type);
            std::vector<Atribute> args;
            
            std::vector<Atribute> arg;
            for(auto param:node->params)
            {
                if (IdentifierNode* p = dynamic_cast<IdentifierNode*>(param)) {
                args.push_back(Atribute(p->value.lexeme,p->type));
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


};
