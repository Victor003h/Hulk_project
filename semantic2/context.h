#pragma once
#include<vector>
#include<map>
#include <memory>
#include<string>


struct Type;

struct Atribute
{
    std::string name;
    Type type;

    Atribute(std::string name,Type type)
    :name(name),type(type){}

};

struct Method
{
    std::string name;
    std::vector<Atribute> paramaters;
    Type returnType;

    Method(std::string name,std::vector<Atribute> paramaters,Type returnedtype)
    :name(name),paramaters(paramaters),returnType(returnedtype){}

    

};

struct Type
{
    std::string name;
    std::vector<Atribute> atributes;
    std::vector<Atribute> methods;

    Type(std::string name)
    :name(name){}
    Type(){}


    bool DefineAtribute(std::string name,Type type)
    {
        for(auto atr:atributes)
        {
            if(atr.name==name)  return false;

        }
        atributes.push_back(Atribute(name,type));

        return true;
    }
};

class Context
{
    public:
        std::map<std::string,Type> types;
        std::map<std::string,Atribute> atributes;
        std::map<std::string,Method> methods;
        std::shared_ptr<Context>  parent;


        Context(Context* parent=nullptr)
        :parent(parent)
        {

        } 
        
        Type GetType(std::string name)
        {
            return  types[name];
        }

        
        Type GetType_Atributed(std::string symbol)
        {
            return atributes[symbol].type;
        }

        Type GetType_Method(std::string symbol)
        {
            return methods[symbol].returnType;
        }

        Type CreateType(std::string name)
        {
            return  Type(name);
        }

        bool exist_Type(std::string name)
        {
            return types.find(name)!=types.end();
        }

        bool exist_Atribute(std::string name)
        {
            return atributes.find(name)!=atributes.end();
        }
        bool exist_Method(std::string name)
        {
            return methods.find(name)!=methods.end();
        }

        bool DefineAtribute(std::string name,Type type)
        {
            if(atributes.find(name)!=atributes.end()) return false;

            atributes[name]= Atribute(name,type);
            return true;
        }

        bool DefineMethod(std::string name,Type type,std::vector<Atribute> paramaters)
        {
            if(methods.find(name)!=methods.end()) return false;

            methods[name]= Method(name,paramaters,type);
            return true;
        }

};