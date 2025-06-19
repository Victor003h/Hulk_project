#include "context.h"
#include "../Parser/AstNodes.h"
#include"../common/Error.hpp"

class VariableDefindedVisitor:public Visitor
{

       public:

        Context* context;
        Type* currentType=nullptr;
        ErrorHandler errorHandler;

         VariableDefindedVisitor(Context* context,ErrorHandler& errorHandler)
        :context(context),errorHandler(errorHandler){}

        void visit(ProgramNode* node)   override
        {
            for (auto stmt: node->stmts)
            {
                stmt->accept(*this);
            }
        }
       
    
        void visit(TypeNode* node)  override
        {
        
            if(context->exist_Type(node->name.lexeme))
            {
                std::string msg="The class"+ node->name.lexeme+" already exists";
                errorHandler.reportError(node->name,msg);
                return;
            }
            
           auto lastType=currentType;

           context=context->createChildContext();

           Type* parent=context->GetType("Object");
            
           if(node->parentName.lexeme!="")
           {

                for(auto tp:context->INTERNAL_TYPES)
                {
                    if(tp==node->parentName.lexeme)
                    {
                        std::string msg="Can't inherit" +node->name.lexeme + "from a basic type like [Number, String, Vector, Node, Boolean] ";
                        errorHandler.reportError(node->parentName,msg);
                        return;
                    }
                }


                if(!context->exist_Type(node->parentName.lexeme))
                {
                    std::string msg="The class "+ node->parentName.lexeme+"  is not defined";
                    errorHandler.reportError(node->parentName,msg);
                    return;
                }
                parent= context->GetType(node->parentName.lexeme);

                if(node->parent_args.size()!= parent->atributes.size())
                {
                    std::string msg="Invalid value of arguments";
                    errorHandler.reportError(node->parentName,msg);
                    return;
                }
                
           }

            currentType=context->CreateType(node->name.lexeme);
            currentType->parent=parent;

            for(auto arg:node->args)
            {
                auto id= static_cast<IdentifierNode*>(arg);
                context->Define_local_Atribute(id->value.lexeme,id->getType());
                if(currentType->exist_Argument(id->value.lexeme))
                {
                    std::string msg="The argument "+ node->parentName.lexeme+"  already exist";
                    errorHandler.reportError(id->value,msg);
                    return;
                }
                currentType->DefineArgument(id->value.lexeme,id->type);
            }

            
            for(auto atribute : node->atributes)
            {
                atribute->accept(*this);
            };

            for(auto atribute : node->atributes)
            {
                auto atr=static_cast<AtributeNode*>(atribute);
                context->Define_local_Atribute(atr->id.lexeme,atr->getType());
            };


            for(auto meth : node->methods)
            {
                meth->accept(*this);
            };

            context =context->RemoveContext();
            auto temp=context->CreateType(node->name.lexeme);
            
            temp->parent=currentType->parent;
            temp->arguments=currentType->arguments;
            temp->atributes=currentType->atributes;
            temp->methods=currentType->methods;
            currentType=lastType;

        };

        void visit(AtributeNode* node)
        {
           
            if(  currentType!=nullptr )
            {
                if(currentType->exist_Atribute(node->id.lexeme))
                {
                    std::string msg="The atribute name"+ node->id.lexeme+" already exists in this type";
                    errorHandler.reportError(node->id,msg);
                    return;
                }

                     

                node->expression->accept(*this);
             
                currentType->DefineAtribute(node->id.lexeme,node->expression->getType());   
                //  bool ok=context->Define_local_Atribute(node->id.lexeme,node->expression->getType());
                // if(!ok)
                // {
                //     std::string msg="The type"+ node->expression->getType()+" does not already exists";
                //     errorHandler.reportError(node->id,msg);
                //     return;   
                // }

                 return;
            }

            if(context->exist_local_Atribute(node->id.lexeme))
            {
                std::string msg="The name "+ node->id.lexeme+" already exists";
                errorHandler.reportError(node->id,msg);
                return;   

            }
              
            node->expression->accept(*this);
            bool ok=context->Define_local_Atribute(node->id.lexeme,node->getType());
            if(!ok)
            {
                std::string msg="The type"+ node->expression->getType()+" does not already exists";
                errorHandler.reportError(node->id,msg);
                return;
            }
            
        };
        
        void visit(MethodNode* node)         
        {
            
            if(currentType!=nullptr )
            {
                if(context->exist_Method(node->id.lexeme))
                {
                    std::string msg="The method method  "+ node->id.lexeme +" is already defined";
                    errorHandler.reportError(node->id,msg);
                    return;
                }
            }
            context= context->createChildContext();           
            std::vector<Atribute> args;

            for(auto param:node->params)
            {
                if (IdentifierNode* p = dynamic_cast<IdentifierNode*>(param)) 
                {
                
                    if(context->exist_local_Atribute(p->value.lexeme))
                    {
                        if(context->exist_Method(node->id.lexeme))
                        {
                            std::string msg="the param name"+ node->id.lexeme+ "already exits ";
                            errorHandler.reportError(node->id,msg);
                            return;
                        }
                    }
                    context->Define_local_Atribute(p->value.lexeme,p->getType());
                    args.push_back(Atribute(p->value.lexeme,p->getType()));

                }
                else
                {
                    std::string msg="Unexpected error in builder method  "+ node->id.lexeme;
                    errorHandler.reportError(node->id,msg);
                    return;
                }
            }
            
            node->body->accept(*this);
          //  node->type=node->body->getType();
          
    
            context= context->RemoveContext(); 

            if(currentType!=nullptr)
                currentType->DefineMethod(node->id.lexeme,node->type,args);
               
        };

        void visit(LetExpression* node)
        {
            context= context->createChildContext();

            for (auto decl: node->assignments)
            {
                decl->accept(*this);
            }
            node->body->accept(*this);

           // node->type=node->body->getType();

            context=context->RemoveContext();
            
        };

        void visit(BlockNode* node) 
        {
            for(auto exp :node->exprs)
            {
                exp->accept(*this);
            }
        };
       
        void visit(BinaryExpression* node)  
        {
            node->left->accept(*this);
            node->right->accept(*this);
            //std::set<std::string> boolop={"<","<=",">",">=","==","!="};
           // std::set<std::string> aritop={"+","-","/","*","^","%"};

            //  if(boolop.find(node->op.lexeme)!=boolop.end())
            //  {
            //     if(node->left->getType()=="Object" &&node->right->getType()=="Number")
            //         node->left->setType("Number");
            //     else if (node->right->getType()=="Object" &&node->left->getType()=="Number")
            //             node->left->setType("Number");
                
            //  }

            
            // if(node->left->getType()!=node->right->getType())
            // {
            //     std::string msg="los tipos no coinciden ";
            //     errorHandler.reportError(node->op,msg);
            //     return;
            // }
            // if(boolop.find(node->op.lexeme)!=boolop.end())
            //     node->type="Boolean";
            // else
            //     node->type=node->left->getType();
    
        };
        
        void visit(IdentifierNode* node)     
        {
            if(node->value.lexeme=="self" )
            {
                if(!currentType)
                     errorHandler.reportError(node->value, "'self' used outside of a class");
                else
                    {
                        node->type=currentType->name;
                    }
                
                return;
            }

            if(!context->exist_Atribute(node->value.lexeme))
            {
                std::string msg="The name"+ node->value.lexeme+ " does not exists";
                errorHandler.reportError(node->value,msg);
                return;
            }

            auto type=context->GetTypeOfAtribute(node->value.lexeme);
            node->type=type->name;            

        }; 

         
       
       
        void visit(IfExpression* node)       
        {
            std::vector<std::string> exp_types;
            for(auto [cond,exp] :node->exprs_cond)
            {
                cond->accept(*this);
                if(cond->getType()!="Boolean")
                {
                    std::string msg="the condition ,bust be bool  ";
                    errorHandler.reportError(Token(),msg);
                    return;
                }
                exp->accept(*this);
                exp_types.push_back(exp->getType());
            }

            node->defaultExp->accept(*this);
            exp_types.push_back(node->defaultExp->getType());
            
          //  node->type=context->getLeastCommonAncestor(exp_types);
        };

        void visit(WhileExpression* node)   
        {
            node->condition->accept(*this);
            // if(node->condition->getType()!="Boolean")
            // {
            //     std::string msg="the condition ,bust be bool  ";
            //     errorHandler.reportError(Token(),msg);
            //     return;
            // }
            node->body->accept(*this);

        };
        
       
     
        void visit(FunCallNode* node) 
        {
            
            Method* meth=nullptr;
            if(currentType!=nullptr)
                {
                    meth=currentType->GetMethod(node->id.lexeme);
                    if(!meth)
                    {
                        std::string msg="The function "+ node->id.lexeme+ " is not defined";
                        errorHandler.reportError(node->id,msg);
                        return;
                    }
                }

           else
           {
             if( !(context->exist_Method(node->id.lexeme)))
            {
                std::string msg="The function "+ node->id.lexeme+ " is not defined";
                errorHandler.reportError(node->id,msg);
                return;
            }
    
            meth=context->GetMethod(node->id.lexeme);


           }

             if(meth->m_paramaters.size()!=node->arguments.size())
             {
                std::string msg="The function "+ node->id.lexeme + " must receive  argm";
                errorHandler.reportError(node->id,msg);
                return;

             }
            for (size_t i = 0; i < node->arguments.size(); i++)
            {
                auto arg=node->arguments[i];
                arg->accept(*this);

                // if(arg->getType()!=meth->m_paramaters[i].type)
                // {
                //     std::string msg="invalid type  of argument in function "+ node->id.lexeme;
                //     errorHandler.reportError(node->id,msg);
                //     return;
                // }
            }
        };

    void visit(MemberCall* node) 
    {
        // Primero resolvemos obj
        node->obj->accept(*this);

        // Obtener el tipo del objeto base
        Type* objType = context->GetType(node->obj->getType());

        if (!objType) 
        {
            std::string msg = "Unknown type for object in member call.";
            errorHandler.reportError(0, 0, msg);
            return;
        }



        if (auto id = dynamic_cast<IdentifierNode*>(node->member)) 
        {
            // Atributo
            if (currentType == nullptr || currentType->name!= objType->name) 
            {
                std::string msg = "Attributes of '" + objType->name + "' can only be accessed from within the class using 'self'";
                errorHandler.reportError(0, 0, msg);
                return;
            }

            if (!objType->exist_Atribute(id->value.lexeme)) {
                std::string msg = "The name '" + id->value.lexeme + "' is not an attribute of type " + objType->name;
                errorHandler.reportError(id->value, msg);
                return;
            }
            id->type=objType->atributes[id->value.lexeme]->type;
            node->type = id->type;
        } 
        else if (auto meth = dynamic_cast<FunCallNode*>(node->member)) 
        {
            if (!objType->exist_Method(meth->id.lexeme)) 
            {
                std::string msg = "The type " + objType->name + " has no method named " + meth->id.lexeme + "'";
                errorHandler.reportError(meth->id, msg);
                return;
            }

            auto lastType=currentType;
            currentType=objType;
            meth->accept(*this);
            currentType=lastType;
            // Seteamos el tipo resultante
            node->type = meth->getType();
        } 
        else 
        {
            std::string msg = "Invalid member access.";
            errorHandler.reportError(0, 0, msg);
        }

    }


        void visit(DestructiveAssignNode* node) 
        {
            node->lhs->accept(*this);
            node->rhs->accept(*this);
           
        }
       
        void visit(TypeInstantiation* node)
        {
            if(!context->exist_Type(node->typeName.lexeme))
            {
                std::string msg="The class "+ node->typeName.lexeme+"  is not defined";
                errorHandler.reportError(node->typeName,msg);
                return;
            }

            Type* type= context->GetType(node->typeName.lexeme);
            Type* parent=type->parent;
        
            
            if(parent->name!="Object")
               
            if(node->arguments.size()!=type->arguments.size())
            {
                std::string msg="Invalid number of arguments , must receive "+ type->arguments.size();
                errorHandler.reportError(node->typeName,msg);
                return;

            }

            for (size_t i = 0; i < node->arguments.size(); i++)
            {
                node->arguments[i]->accept(*this);
                
                
                if(node->arguments[i]->getType()!=type->arguments[i].second->type)
                {
                    std::string msg="The type arguments does not match ";
                    errorHandler.reportError(node->typeName,msg);
                    return;
                }

            }

        }    

        void visit(ForExression* node)       {};

        void visit(UnaryExpression* node)      {};

        void visit(LiteralNode* node)        {};
        
};