#pragma once
#include<vector>
#include<map>
#include<set>
#include <memory>
#include<string>



struct Atribute
{
    std::string name;
    std::string type;

    Atribute(std::string name,std::string type)
    :name(name),type(type){}

};

struct Method
{
    std::string m_name;
    std::vector<Atribute> m_paramaters;
    std::string m_returnType;

    Method(std::string name,std::vector<Atribute> paramaters,std::string returnedtype)
    {
        this->m_name=name;
        this->m_paramaters=paramaters;
        this->m_returnType=returnedtype;
    }
    

};

struct Type
{
    std::string name;
    std::map<std::string,Atribute*> atributes;
    std::vector<std::pair<std::string,Atribute*>>arguments;
    std::map<std::string,Method*> methods;
    Type* parent=nullptr;

    Type(std::string name)
    :name(name){}
    Type(){}


    void DefineAtribute(std::string name,std::string type)
    {
        atributes.emplace(name, new Atribute(name,type));
    }

    void DefineArgument(std::string name,std::string type)
    {
        arguments.push_back({name, new Atribute(name,type)});
    }


    bool exist_Atribute(std::string name)
    {
        return atributes.find(name)!=atributes.end();
    }
     
    bool exist_Argument(std::string name)
    {
        for(auto[argname,_] :arguments)
        {

            if(argname==name)   return true;

        }
        return false;
    }


    bool exist_Method(std::string name)
    {
        if(methods.find(name)!=methods.end())   return true;
        if(parent==nullptr) return false;
        return parent->GetMethod(name);
    }

    
    void DefineMethod(std::string name,std::string type,std::vector<Atribute> paramaters)
    {
        
        methods.emplace(name,new Method(name,paramaters,type));
        
    }

    Method* GetMethod(std::string name)
    {
        if(methods.find(name)!=methods.end())   return methods[name];
        if(parent==nullptr) return nullptr;
        return parent->GetMethod(name);
        
    }



     bool isDescendantOf(const Type* other) const {
        const Type* current = this;
        while (current != nullptr) {
            if (current == other)
                return true;
            current = current->parent;
        }
        return false;
    }

    bool operator==(const Type &other) const {
        return name == other.name;
    }

    bool operator<(const Type &other) const {
        // Si son iguales, no se considera menor.
        if (*this == other)
            return false;

        // Si este tipo es descendiente de 'other', se considera menor.
        if (this->isDescendantOf(&other)) 
            return true;
        
       return false;
    }


};



class Context
{
    public:
        std::map<std::string,Type*> types;
        std::map<std::string,Method*> methods;
        std::map<std::string,Type*> atributes;
        Context*  parent;
        Context*  child;
        const std::vector<std::string> INTERNAL_TYPES = {"Object", "Number", "String", "Boolean", "Null"};


        Context(Context* parent=nullptr)
        :parent(parent)
        {
                    
        } 
        
        Type* GetType(std::string name) 
        {
            if(types.find(name)!=types.end())   return types[name];
            if(parent==nullptr) return nullptr;
            return parent->GetType(name);
        }
        
        Type* GetTypeOfAtribute(std::string name) 
        {
            if(atributes.find(name)!=atributes.end())   return atributes[name];
            if(parent==nullptr) return nullptr;
            return parent->GetTypeOfAtribute(name);
        }

        
        // Type GetType_Method(std::string symbol)
        // {
        //     auto type= methods[symbol].returnType;
        //     return types[type];

        // }

        Type* CreateType(std::string name)
        {
            Type* t= new Type(name);
            types.emplace(name,t);
            return  t;
        }

        bool exist_Type(std::string name)
        {
            if(types.find(name)!=types.end())   return true;
            if(parent==nullptr) return false;
            return parent->exist_Type(name);
        }

        bool exist_Atribute(std::string name) //global
        {
            if(atributes.find(name)!=atributes.end())   return true;
            if(parent==nullptr) return false;
            return parent->exist_Atribute(name);
        }

        bool exist_local_Atribute(std::string name) 
        {
            return atributes.find(name)!=atributes.end();
        
        }


        bool Define_local_Atribute(std::string name,std::string typeName)  ////// 
        {
            if(!exist_Type(typeName)) return false;/////// inneceisario?
            Type* type =GetType(typeName);
            atributes.emplace(name,type);
            return true;
        }

        
        bool exist_Method(std::string name)
        {
            if(methods.find(name)!=methods.end())   return true;
            if(parent==nullptr) return false;
            return parent->exist_Method(name);
            
        }
        Method* GetMethod(std::string name)
        {
            if(methods.find(name)!=methods.end())   return methods[name];
            if(parent==nullptr) return nullptr;
            return parent->GetMethod(name);
            
        }
        

        bool DefineMethod(std::string name,std::string type,std::vector<Atribute> paramaters)
        {
            if(methods.find(name)!=methods.end()) return false;

            methods.emplace(name,new Method(name,paramaters,type));
            return true;
        }

        Context* createChildContext()
        {
            child= new Context(this);
            return child;
        }

        Context* RemoveContext()
        {
            return parent;
        }

        void loadInternalType()
        {
            Type* object= CreateType(INTERNAL_TYPES[0]);
            for(size_t i=1 ; i< INTERNAL_TYPES.size();i++)
            {
                auto t=CreateType(INTERNAL_TYPES[i]);
                t->parent=object;
            }

        }

        std::string getLeastCommonAncestor(std::vector<std::string>exp_types)
        {
            Type* current=GetType(exp_types[0]);


            for(size_t i=1 ;i<exp_types.size();i++)
            {
                current=LeastCommonAncestors(current,GetType(exp_types[i]));
            }
            return current->name;
            
    
        }
       
        Type* LeastCommonAncestors(Type*first, Type* second ) {
        
            if(first==second) return first; 
        
            Type* a = first;
            Type* b = second;

            int depthA = 0, depthB = 0;
            for(const Type* curr = a; curr != nullptr; curr = curr->parent)
                depthA++;
            for(const Type* curr = b; curr != nullptr; curr = curr->parent)
                depthB++;

            // Igualamos las profundidades moviendo el nodo más profundo hacia arriba.
            while(depthA > depthB) {
                a = a->parent;
                depthA--;
            }
            while(depthB > depthA) {
                b = b->parent;
                depthB--;
            }

            while(a != b) {
                a = a->parent;
                b = b->parent;
            }

            // a y b son iguales en este punto (o nullptr) y serán el LCA.
            return a;
    }

};
