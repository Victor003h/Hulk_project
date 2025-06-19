#include<set>
#include "FA.cpp"
class RE
{
    public:
        std::set<char> alphabet;
        TokenType type;

    public:
        RE(std::set<char> alphabet)
        {
            this->alphabet=alphabet;
        }

        virtual bool is_valid(std::string inp)=0;
        virtual NFA ConvertToNFA()=0;
        
        
        RE(){};
};

class EmptyRE:public RE
{
    public:
        EmptyRE():RE{{}}{}

        bool is_valid(std::string inp) override{
            return ConvertToNFA().evualuate(inp);
        }
        NFA ConvertToNFA() override{
            return NFA::emptyRE();
        }

};

class EpsilonRE:public RE 
{
    public:
        EpsilonRE():RE{{'$'}}
        {

        }

        bool is_valid(std::string inp) override{
            return ConvertToNFA().evualuate(inp);
        }
        NFA ConvertToNFA() override{
            return NFA::epsilonRE();
        }
};

class SymbolRE:public RE
{
    private:
        char symbol;
    public:
        SymbolRE(char symbol):
        RE{{symbol}}
        {
            this->symbol=symbol;
        }

        bool is_valid(std::string inp) override{
            return ConvertToNFA().evualuate(inp);
        }
        NFA ConvertToNFA() override{
            return NFA::symbolRE(symbol);
        }
};


class UnionRE:public RE
{
    private:
        RE* left;
        RE* right;
    public:
        UnionRE(RE* left,RE* right):
        RE({left->alphabet})
        {
            this->left=left;
            this->right=right;
            alphabet.merge(right->alphabet);
        }

         bool is_valid(std::string inp) override {
            return ConvertToNFA().evualuate(inp);
        }
        
        NFA ConvertToNFA() override
        {
            NFA a1=(*left).ConvertToNFA();
            NFA a2=(*right).ConvertToNFA();
            return NFA::UnionRE(a1,a2);
        }
};

class ConcatenationRE:public RE
{
    private:
        RE* left;
        RE* right;
    public:
        ConcatenationRE(RE* left,RE* right): RE({left->alphabet})
        {
            this->left=left;
            this->right=right;
            for (char c :right->alphabet)
            {
                alphabet.insert(c);
            }
        }

        bool is_valid(std::string inp) override {
            auto nfa= ConvertToNFA();
            return nfa.evualuate(inp);
        }
        
        NFA ConvertToNFA() override
        {
            NFA a1=(*left).ConvertToNFA();
            NFA a2=(*right).ConvertToNFA();
            return NFA::ConcatenationRE(a1,a2);
        }
};

class ClousureRE:public RE
{
    private:
        RE* value;
    public:
        ClousureRE(RE* value):
        RE{{value->alphabet}}
        {
            this->value=value;
            
            
        }
        bool is_valid(std::string inp) override {
            return ConvertToNFA().evualuate(inp);
        }
        
        NFA ConvertToNFA() override
        {
            NFA a1=(*value).ConvertToNFA();
            return NFA::ClousureRE(a1);
        }

};



