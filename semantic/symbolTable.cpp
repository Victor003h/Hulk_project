#include "../common/DataTypes.h"

struct TypeInfo {
    DataType dt;
};

class SymbolTable {
private:
    stack<unordered_map<string, TypeInfo>> scopes;
public:
    SymbolTable() {
        scopes.push(unordered_map<string, TypeInfo>());
    }
    
    void enterScope() {
        scopes.push(unordered_map<string, TypeInfo>());
    }
    
    void exitScope() {
        if (!scopes.empty())
            scopes.pop();
    }
    
    bool addSymbol(const string &name, const TypeInfo &info) {
        auto &currentScope = scopes.top();
        if (currentScope.find(name) != currentScope.end())
            return false;
        currentScope[name] = info;
        return true;
    }
    
    bool lookup(const string &name, TypeInfo &info) {
        stack<unordered_map<string, TypeInfo>> temp = scopes;
        while (!temp.empty()) {
            auto &currentScope = temp.top();
            if (currentScope.find(name) != currentScope.end()) {
                info = currentScope[name];
                return true;
            }
            temp.pop();
        }
        return false;
    }
};
