#include<vector>
#include<iostream>
#include "AstNodes.cpp"
#include "grammarItems.h"


class ParseTreeVisitor;

class ParseNode
{
    public:
        std::vector<ParseNode*> m_children;
        ParseNode* m_parent;
        Symbol* m_value;
        Token m_token;

        ParseNode(Symbol*value)
        {
            m_value=value;
            m_parent=nullptr;
        }

        AstNode* accept(ParseTreeVisitor &visitor);    
};


class ParseTree
{
    public:
        ParseNode* m_root;
    
        ParseTree(ParseNode* root)
        : m_root(root){};


        AstNode* accept(ParseTreeVisitor &visitor)
        {
            return m_root->accept(visitor);
        }
        

    void printParseTree(ParseNode* node, int level = 0) 
    {
        if (!node)
            return;

        // Imprimir la indentación (dos espacios por nivel)
        for (int i = 0; i < level; ++i)
            std::cout << "  ";

        // Imprimir el valor del nodo; se asume que m_value no es nulo.
        if (node->m_value)
            std::cout << node->m_value->value << "\n";
        else
            std::cout << "null\n";

        // Recorrer e imprimir recursivamente a cada hijo, incrementando el nivel.
        for (ParseNode* child : node->m_children) {
            printParseTree(child, level + 1);
        }
}


};


class ParseTreeVisitor
{
    public:
        virtual ~ParseTreeVisitor(){};
        virtual AstNode* visit(ParseNode* node)=0;

};

class AstBuilderVisitor: public ParseTreeVisitor
{
    public:
        AstNode* visit(ParseNode* node)
       {

            if(node->m_value->value=="Program")
            {
                ProgramNode* prg= new ProgramNode();

                prg->stmts=stmsList(node->m_children[0]);
              
                return prg;
            }


            if(node->m_value->value=="Statement")
            {
                return node->m_children[1]->accept(*this);
            }

            if(node->m_value->value=="ifExp")
            {
                AstNode* defaultExp=node->m_children[0]->accept(*this);
                std::vector<std::pair<AstNode*,AstNode*>> exprs_cond=ElifList(node->m_children[2]);
                exprs_cond.insert(exprs_cond.begin(),{node->m_children[5]->accept(*this),node->m_children[3]->accept(*this)});
                
                return new IfExpression(defaultExp,exprs_cond);
            }

            if(node->m_value->value=="While_loop")
            {

                AstNode* condition=node->m_children[2]->accept(*this);
                AstNode* body=node->m_children[0]->accept(*this);
                return new WhileExpression(condition,body);
            }
            if(node->m_value->value=="For_loop")
            {

                AstNode* item=new IdentifierNode(node->m_children[4]->m_token);
                AstNode* iterable=node->m_children[2]->accept(*this);
                AstNode* body=node->m_children[0]->accept(*this);
                return new ForExression(item,iterable,body);
            }


            if(node->m_value->value=="VarDeclaration")
            {
                std::vector<AstNode*> asgs= AssignmentList(node->m_children[2]);
                auto body=node->m_children[0]->accept(*this);
                return new LetExpression(asgs,body);    
            }
            
            if(node->m_value->value=="Expbody")
            {
                return node->m_children[0]->accept(*this);    
            }
            
            if(node->m_value->value=="Block")
            {
                std::vector<AstNode*> exp= ExpList(node->m_children[1]);
                return new BlockNode(exp);    
            }
         
            if(node->m_value->value=="AssignmentDecl")
            {
                auto body=node->m_children[0]->accept(*this);
                return  new AtributeNode(node->m_children[2]->m_token,body);

            }

            if(node->m_value->value=="Expression")
            {
                return node->m_children[0]->accept(*this);
            }

            if(node->m_value->value=="FuncDef")
            {
                Token id= node->m_children[1]->m_token;
                auto [params,body]=MethodSignaturePrime(node->m_children[0]);
                return new MethodNode(id,params,body);
            
            }
            if(node->m_value->value=="TypeDef")
            {
                Token name= node->m_children[3]->m_token;
                auto [atr,meth]= GetMemberList(node->m_children[1]);
                return new TypeNode(name,atr,meth);
                
            }


            if(node->m_value->value=="ExpAditiva")
            {
                AstNode* left=node->m_children[1]->accept(*this);
                return ExpAditivaPrime(left,node->m_children[0]);
            }
            
            if(node->m_value->value=="ExpMultiplicativa")
            {
                AstNode* left=node->m_children[1]->accept(*this);
                return ExpMultiplicativaPrime(left,node->m_children[0]);
            }
            
            if(node->m_value->value=="ExpExpon")
            {
                AstNode* left=node->m_children[1]->accept(*this);
                return ExpExponPrime(left,node->m_children[0]);
            }
            
            if(node->m_value->value=="Primary")
            {
                if(node->m_children.size()==3)
                {
                    return node->m_children[1]->accept(*this);
                }
                if(node->m_children[0]->m_value->value=="Number")
                {
                    
                    return new LiteralNode(node->m_children[0]->m_token);
                }
                if(node->m_children[0]->m_value->value=="Identifier")
                {
                    return new IdentifierNode(node->m_children[0]->m_token);
                }
                
            }

            std::cout<<"Unknow node "<<node->m_value->value<<std::endl;            
            return nullptr;
       }

       std::pair<std::vector<AstNode*> ,std::vector<AstNode*>> GetMemberList(ParseNode*node)
       {
            std::vector<AstNode*> atributes;
            std::vector<AstNode*> methods;
            if(node->m_children.empty() )    return {atributes,methods};
            
            auto [isAtr,member]=GetMember(node->m_children[1]);
            if(isAtr)   atributes.push_back(member);
            else methods.push_back(member);

            auto [atr,meth]=GetMemberList(node->m_children[0]);
             for(auto child:atr)
            {
                atributes.push_back(child);
            }
            for(auto child:meth)
            {
                methods.push_back(child);
            }
            return {atributes,methods};
            
       }

       std::pair<bool,AstNode*> GetMember(ParseNode* node)
       {
            Token id=node->m_children[2]->m_token;
            if(node->m_children[1]->m_children.size()==2)
            {
                AstNode* exp=node->m_children[1]->m_children[0]->accept(*this);

                return {true,new AtributeNode(id,exp)};   
            }

            auto [params,body]=MethodSignaturePrime(node->m_children[1]->m_children[0]);
            return {false,new MethodNode(id,params,body)};

       }

       std::pair<std::vector<AstNode*> ,AstNode*> MethodSignaturePrime(ParseNode* node)
       {
            std::vector<AstNode*> params= ParamList(node->m_children[2]);
            AstNode* body= node->m_children[0]->m_children[0]->accept(*this);
            return {params,body};

       }

       std::vector<std::pair<AstNode*,AstNode*>> ElifList(ParseNode* node)
       {
            std::vector<std::pair<AstNode*,AstNode*>> list;
            if(node->m_children.empty())    return list;
            list.push_back({node->m_children[3]->accept(*this),node->m_children[1]->accept(*this)});
            auto asglist= ElifList(node->m_children[0]);
            for(auto child:asglist)
            {
                list.push_back(child);
            }
            return list;

       }

       std::vector<AstNode*> ParamList(ParseNode* node)
       {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;
            list.push_back(new IdentifierNode(node->m_children[1]->m_token));
            auto parmlist2=ParamListTail(node->m_children[0]);
             for(auto child:parmlist2)
            {
                list.push_back(child);
            }
            return list;

       }

        std::vector<AstNode*> ParamListTail(ParseNode* node)
        {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;
             list.push_back(new IdentifierNode(node->m_children[1]->m_token));
            auto asglist= ParamListTail(node->m_children[0]);
            for(auto child:asglist)
            {
                list.push_back(child);
            }
            return list;
        }

       std::vector<AstNode*> ExpList(ParseNode* node)
       {
            std::vector<AstNode*> list;
            list.push_back(node->m_children[1]->accept(*this));
            auto asglist2=ExpListTail(node->m_children[0]);
             for(auto child:asglist2)
            {
                list.push_back(child);
            }
            return list;

       }

        std::vector<AstNode*> ExpListTail(ParseNode* node)
        {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;
            list.push_back(node->m_children[2]->accept(*this));
            auto asglist= ExpListTail(node->m_children[0]);
            for(auto child:asglist)
            {
                list.push_back(child);
            }
            return list;
        }

        std::vector<AstNode*> AssignmentList(ParseNode* node)
        {
            std::vector<AstNode*> list;
            list.push_back(node->m_children[1]->accept(*this));
            auto asglist2=AssignmentListTail(node->m_children[0]);
             for(auto child:asglist2)
            {
                list.push_back(child);
            }
            return list;

        }

        std::vector<AstNode*> AssignmentListTail(ParseNode* node)
        {
            std::vector<AstNode*> list;
            if(node->m_children.empty())    return list;

            list.push_back(node->m_children[1]->accept(*this));
            
            auto asglist= AssignmentListTail(node->m_children[0]);
            for(auto child:asglist)
            {
                list.push_back(child);
            }
            return list;
        }
       
        std::vector<AstNode*> stmsList(ParseNode* node)
       {
            std::vector<AstNode*> list;
            if(node->m_children.empty())
                return list;
            
            list.push_back(node->m_children[1]->accept(*this));
            
            auto stmsl=stmsList(node->m_children[0]);
            for(auto child:stmsl)
            {
                list.push_back(child);
            }
            return list;
       }

        AstNode* ExpAditivaPrime(AstNode* inherited,ParseNode* node)
        {
            if(node->m_children.empty())    return inherited;

            AstNode* right=node->m_children[1]->accept(*this);
            Token op=node->m_children[2]->m_token;
            AstNode* temp = new BinaryExpression(inherited,op,right);
            return ExpAditivaPrime(temp,node->m_children[0]);
        }
       
        AstNode* ExpMultiplicativaPrime(AstNode* inherited,ParseNode* node)
        {
            if(node->m_children.empty())    return inherited;

            AstNode* right=node->m_children[1]->accept(*this);
            Token op=node->m_children[2]->m_token;
            AstNode* temp = new BinaryExpression(inherited,op,right);
            return ExpMultiplicativaPrime(temp,node->m_children[0]);
        }
        
        AstNode* ExpExponPrime(AstNode* inherited,ParseNode* node)
        {
            if(node->m_children.empty())    return inherited;

            AstNode* right=node->m_children[1]->accept(*this);
            Token op=node->m_children[2]->m_token;
            AstNode* temp = new BinaryExpression(inherited,op,right);
            return ExpExponPrime(temp,node->m_children[0]);
        }

        
    };

AstNode*  ParseNode::accept(ParseTreeVisitor &visitor)
{
    return visitor.visit(this);
}
