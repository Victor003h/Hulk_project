// #include<set>
// #include "FA.cpp"

// Nueva versión mejorada de la jerarquía de clases RE para facilitar
// construcción desde un parser y mejorar eficiencia

#include <memory>
#include <set>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "FA.cpp" // Asegúrate de incluir tu definición de NFA

enum class REKind {
    Empty, Epsilon, Symbol, Union, Concat, Clousure,Range,Any
};

class RE {
public:
    TokenType type;
    std::set<char> alphabet;

    RE(std::set<char> alphabet) : alphabet(std::move(alphabet)) {}
    virtual ~RE() = default;

    virtual bool is_valid(const std::string& inp) const = 0;
    virtual NFA ConvertToNFA() const = 0;
};

using REPtr = std::shared_ptr<RE>;

class EmptyRE : public RE {
public:
    EmptyRE() : RE({}) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evualuate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::emptyRE();
    }
};

class EpsilonRE : public RE {
public:
    EpsilonRE() : RE({'$'}) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evualuate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::epsilonRE();
    }
};

class AnyRE : public RE {
public:
    AnyRE() : RE({'~'}) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evualuate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::anyRE();
    }
};


class SymbolRE : public RE {
    char symbol;
public:
    SymbolRE(char symbol) : RE({symbol}), symbol(symbol) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evualuate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::symbolRE(symbol);
    }
};

class UnionRE : public RE {
    REPtr left, right;
public:
    UnionRE(REPtr l, REPtr r) : RE(l->alphabet) {
        left = std::move(l);
        right = std::move(r);
        alphabet.insert(right->alphabet.begin(), right->alphabet.end());
    }
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evualuate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::UnionRE(left->ConvertToNFA(), right->ConvertToNFA());
    }
};

class ConcatenationRE : public RE {
    REPtr left, right;
public:
    ConcatenationRE(REPtr l, REPtr r) : RE(l->alphabet) {
        left = std::move(l);
        right = std::move(r);
        alphabet.insert(right->alphabet.begin(), right->alphabet.end());
    }
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evualuate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::ConcatenationRE(left->ConvertToNFA(), right->ConvertToNFA());
    }
};

class ClousureRE : public RE {
    REPtr value;
public:
    ClousureRE(REPtr val) : RE(val->alphabet), value(std::move(val)) {}
    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evualuate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::ClousureRE(value->ConvertToNFA());
    }
};


class RangeRE : public RE {
    char from;
    char to;


public:
    RangeRE(char from, char to) : RE({}), from(from), to(to) {
        for (char c = from; c <= to; ++c)
            alphabet.insert(c);
    }

    bool is_valid(const std::string& inp) const override {
        return ConvertToNFA().evualuate(inp);
    }
    NFA ConvertToNFA() const override {
        return NFA::RangeRE(from,to);
    }
};














// class RE
// {
//     public:
//         std::set<char> alphabet;
//         TokenType type;

//     public:
//         RE(std::set<char> alphabet)
//         {
//             this->alphabet=alphabet;
//         }

//         virtual bool is_valid(std::string inp)=0;
//         virtual NFA ConvertToNFA()=0;
        
        
//         RE(){};
// };

// class EmptyRE:public RE
// {
//     public:
//         EmptyRE():RE{{}}{}

//         bool is_valid(std::string inp) override{
//             return ConvertToNFA().evualuate(inp);
//         }
//         NFA ConvertToNFA() override{
//             return NFA::emptyRE();
//         }

// };

// class EpsilonRE:public RE 
// {
//     public:
//         EpsilonRE():RE{{'$'}}
//         {

//         }

//         bool is_valid(std::string inp) override{
//             return ConvertToNFA().evualuate(inp);
//         }
//         NFA ConvertToNFA() override{
//             return NFA::epsilonRE();
//         }
// };


// class SymbolRE:public RE
// {
//     private:
//         char symbol;
//     public:
//         SymbolRE(char symbol):
//         RE{{symbol}}
//         {
//             this->symbol=symbol;
//         }

//         bool is_valid(std::string inp) override{
//             return ConvertToNFA().evualuate(inp);
//         }
//         NFA ConvertToNFA() override{
//             return NFA::symbolRE(symbol);
//         }
// };


// class UnionRE:public RE
// {
//     private:
//         RE* left;
//         RE* right;
//     public:
//         UnionRE(RE* left,RE* right):
//         RE({left->alphabet})
//         {
//             this->left=left;
//             this->right=right;
//             alphabet.merge(right->alphabet);
//         }

//          bool is_valid(std::string inp) override {
//             return ConvertToNFA().evualuate(inp);
//         }
        
//         NFA ConvertToNFA() override
//         {
//             NFA a1=(*left).ConvertToNFA();
//             NFA a2=(*right).ConvertToNFA();
//             return NFA::UnionRE(a1,a2);
//         }
// };

// class ConcatenationRE:public RE
// {
//     private:
//         RE* left;
//         RE* right;
//     public:
//         ConcatenationRE(RE* left,RE* right): RE({left->alphabet})
//         {
//             this->left=left;
//             this->right=right;
//             for (char c :right->alphabet)
//             {
//                 alphabet.insert(c);
//             }
//         }

//         bool is_valid(std::string inp) override {
//             auto nfa= ConvertToNFA();
//             return nfa.evualuate(inp);
//         }
        
//         NFA ConvertToNFA() override
//         {
//             NFA a1=(*left).ConvertToNFA();
//             NFA a2=(*right).ConvertToNFA();
//             return NFA::ConcatenationRE(a1,a2);
//         }
// };

// class ClousureRE:public RE
// {
//     private:
//         RE* value;
//     public:
//         ClousureRE(RE* value):
//         RE{{value->alphabet}}
//         {
//             this->value=value;
            
            
//         }
//         bool is_valid(std::string inp) override {
//             return ConvertToNFA().evualuate(inp);
//         }
        
//         NFA ConvertToNFA() override
//         {
//             NFA a1=(*value).ConvertToNFA();
//             return NFA::ClousureRE(a1);
//         }

// };



