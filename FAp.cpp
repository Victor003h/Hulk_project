// #include<vector>
// #include<set>
// #include<map>
// #include<queue>
// #include <iostream>
// #include <fstream>
// #include <filesystem>
// #include "Token.h"

// using namespace std;

// struct CharRange
// {
//     char from;
//     char to;
    
//     bool contains(char c) const {
//         return c >= from && c <= to;
//     }

//      bool operator<(const CharRange& other) const {
//         return std::tie(from, to) < std::tie(other.from, other.to);
//     }
    

// };



// class DFA
// {
//     public:
//         int start_state;
//         int total_states;
//         std::vector<int> final_states;
//         std::set<char> alphabet;
//         std::map<int, std::map<char, int>> transitions;
//         std::map<int, TokenType> m_final_token_types;

  
//     public: DFA(int start_state,int total_states,std::vector<int> final_states,
//             std::set<char>alphabet,std::map<int, std::map<char, int >> transitions)
//         {
//             this->total_states=total_states;
//             this->start_state=start_state;
//             this->final_states=final_states;
//             this->alphabet=alphabet;
//             this->transitions=transitions;
//         }
//         DFA(){};


//         int getStartState()
//         {
//             return start_state;
//         }
       
//         int nextState(char l,int current_state)
//         {

//             if (!is_valid(l)) return -1;
            
//             if(transitions[current_state].find(l)==transitions[current_state].end())   return -1;

//             auto trs=transitions[current_state][l];
            
//             return trs;
//         }
        
//         bool is_valid(const char symbol)
//         {
//             if (alphabet.find(symbol)==alphabet.end())
//             {
               
//                 return false;
//             }
//             return true;
//         }

//         bool is_final_state(int current_state)
//         {
//             for(int f_s :final_states){
//                 if(current_state==f_s)  return true;
//             }
//             return false;
//         }

//         bool evualuate(std::string input)
//         {
               
//             int current_state=start_state;
//             for(char symbol :input)
//             {
//                 current_state=nextState(symbol,current_state);
//             }

//             return is_final_state(current_state);
//         }
        
//         static bool existsDfa()
//         {
//             return std::filesystem::exists("dfa.bin");
//         }
       

//         static void save_binary(const DFA& dfa, const std::string& filename="dfa.bin") {
//         std::ofstream out(filename, std::ios::binary);

//         out.write(reinterpret_cast<const char*>(&dfa.start_state), sizeof(int));
//         out.write(reinterpret_cast<const char*>(&dfa.total_states), sizeof(int));

//         int final_size = dfa.final_states.size();
//         out.write(reinterpret_cast<const char*>(&final_size), sizeof(int));
//         for (int f : dfa.final_states) {
//             out.write(reinterpret_cast<const char*>(&f), sizeof(int));
//         }

//         int alphabet_size = dfa.alphabet.size();
//         out.write(reinterpret_cast<const char*>(&alphabet_size), sizeof(int));
//         for (char c : dfa.alphabet) {
//             out.write(&c, sizeof(char));
//         }

//         int num_states = dfa.transitions.size();
//         out.write(reinterpret_cast<const char*>(&num_states), sizeof(int));
//         for (const auto& [state, map] : dfa.transitions) {
//             out.write(reinterpret_cast<const char*>(&state), sizeof(int));
//             int num_transitions = map.size();
//             out.write(reinterpret_cast<const char*>(&num_transitions), sizeof(int));
//             for (const auto& [symbol, next] : map) {
//                 out.write(&symbol, sizeof(char));
//                 out.write(reinterpret_cast<const char*>(&next), sizeof(int));
//             }
//         }
//     }

//         static DFA load_binary(const std::string& filename="dfa.bin")
//         {
//             std::ifstream in(filename, std::ios::binary);
//             DFA dfa;

//             in.read(reinterpret_cast<char*>(&dfa.start_state), sizeof(int));
//             in.read(reinterpret_cast<char*>(&dfa.total_states), sizeof(int));

//             int final_size;
//             in.read(reinterpret_cast<char*>(&final_size), sizeof(int));
//             dfa.final_states.resize(final_size);
//             for (int& f : dfa.final_states) {
//                 in.read(reinterpret_cast<char*>(&f), sizeof(int));
//             }

//             int alphabet_size;
//             in.read(reinterpret_cast<char*>(&alphabet_size), sizeof(int));
//             for (int i = 0; i < alphabet_size; ++i) {
//                 char c;
//                 in.read(&c, sizeof(char));
//                 dfa.alphabet.insert(c);
//             }

//             int num_states;
//             in.read(reinterpret_cast<char*>(&num_states), sizeof(int));
//             for (int i = 0; i < num_states; ++i) {
//                 int state;
//                 in.read(reinterpret_cast<char*>(&state), sizeof(int));
//                 int num_transitions;
//                 in.read(reinterpret_cast<char*>(&num_transitions), sizeof(int));
//                 for (int j = 0; j < num_transitions; ++j) {
//                     char sym;
//                     int dest;
//                     in.read(&sym, sizeof(char));
//                     in.read(reinterpret_cast<char*>(&dest), sizeof(int));
//                     dfa.transitions[state][sym] = dest;
//                 }
//             }

//             return dfa;
//     }

// };
 


// class NFA{
//     public:

//         int start_state;
//         int total_states;
//         std::set<char> alphabet;
//         std::map<int, std::vector<std::pair<CharRange, std::vector<int>>>> transitions;       
//         std::vector<int> final_states;
//         std::map<int, TokenType> m_final_token_types;
    
//     public:
//         NFA(int start_state,int total_states,std::vector<int> final_states,
//             std::set<char>alphabet,std::map<int, std::vector<std::pair<CharRange, std::vector<int>>>> transitions)
//         {
//             this->total_states=total_states;
//             this->start_state=start_state;
//             this->final_states=final_states;
//             this->alphabet=alphabet;
//             this->transitions=transitions;

           
//         }
        

//         bool is_final_state(int current_state)
//         {
//             for(int f_s :final_states){
//                 if(current_state==f_s)  return true;
//             }
//             return false;
//         }

//         std::vector<int> epsilonTransitions(int state)
//         {
//             if(transitions[state].find('$')==transitions[state].end())   return {};
//             return transitions[state]['$'];
//         }
        
        
//         std::vector<int> nextStates(char l,int current_state)
//         {

//             if (!is_valid(l)) return {};
//             //std::vector<int> res=epsilonTransitions(current_state);
            
//             if(transitions[current_state].find(l)==transitions[current_state].end())   return {};

            
//             auto trs=transitions[current_state][l];
//             //for (int e : trs)   res.push_back(e);
            
//             return trs;
//         }
        
//         bool is_valid(const char symbol)
//         {
//             if (alphabet.find(symbol)==alphabet.end())
//             {
//                 std::cout<<"el caracter" <<symbol<< " no pertenece a el alfabeto"<<std::endl ;
//                 return false;
//             }
//             return true;
//         }

//         bool evualuate(std::string input)
//         {
//             std::vector<int> cs={start_state};
//             for(char symbol :input)
//             {
//                 std::vector<int> aux={};

//                 for (size_t i=0;i<cs.size();i++)
//                 {
//                     auto eps=eClousure(cs[i],false);
//                     for(int i:eps) cs.push_back(i);

//                     auto s=nextStates(symbol,cs[i]);
//                     for(int i:s) aux.push_back(i); 
//                 }
//                 cs=aux;
//             }

//            for (size_t i=0;i<cs.size();i++)
//             {
//                 auto eps=eClousure(cs[i],false);
//                 for(int i:eps) cs.push_back(i);

//                 if(is_final_state(cs[i]))    return true;
//             }
//             return false;
//          }

        

//         DFA convertToDFA()
//         {
//             std::map<std::set<int>,int> dfn_states={};
//             std::queue<std::set<int>> queue={};
//             std::map<int, std::map<char, int>> trs={};

            
//             int cr=0;
//             auto start=eClousure_set({start_state});
//             queue.push(start);
//             dfn_states[start]=cr;
//             cr++;

//             while (!queue.empty())
//             {

//                 std::set<int> T=queue.front();
//                 for (char sym:alphabet)
//                 {
//                     if (sym=='$') continue;
//                     auto u=eClousure_set(move(T,sym));
//                     if(dfn_states.find(u)==dfn_states.end())
//                     {
//                         if (u.size()==0)    dfn_states[u]=-1;
//                         else{
//                             dfn_states[u]=cr;
//                             cr++;
//                         }
                        
//                         queue.push(u);
//                     }
//                     trs[dfn_states[T]][sym]=dfn_states[u];
//                 }
//                 queue.pop();
//             }

//             std::map<int, TokenType> dfa_final_tokens;
//             std::vector<int> finalStates ={};
//             for (auto pair : dfn_states)
//             {
//                 int dfa_state_id = pair.second;
//                 TokenType token = UNKNOWN; 
//                 for (int state_nfa: pair.first)
//                 {
                    
//                     if (is_final_state(state_nfa))
//                     {
//                         auto t=m_final_token_types[state_nfa];
//                         if(morePriority(t,token))    
//                         {
//                             token=t;
//                         }

//                         finalStates.push_back(pair.second);
//                     }
//                 }

//                 dfa_final_tokens[dfa_state_id]=token;

//             }
//            std::set<char> d_al=alphabet;
//             if(d_al.find('$')==d_al.end())  d_al.erase('$');

//             DFA dfa(0,dfn_states.size(),finalStates,alphabet,trs);
//             dfa.m_final_token_types=dfa_final_tokens;
//             return dfa;

//         }
        
        

//         std::set<int> eClousure(int state,bool initial)
//         {
//             std::set<int> res={};
//             std::queue<int> pending={};
//             pending.push(state);
            

//             while(!pending.empty())
//             {
//                 int cr=pending.front();
//                 pending.pop();

//                 if(res.find(cr)!=res.end()) continue;

//                 auto r=transitions[cr]['$'];
//                 for(int i:r)    
//                 {
//                     pending.push(i);
//                 }
//                 if (!initial && cr==state)   continue;
//                 res.insert(cr);

//             }
//             return res;

//         }
       
//         std::set<int> move(std::set<int>set,char symbol)
//         {
//             std::set<int> res={};

//             for (int state: set)
//             {
//                 auto s=transitions[state][symbol];
//                 for (int i :s){
//                     res.insert(i);
//                 }
//             }
//             return res;
//         }
        
//         std::set<int>eClousure_set(std::set<int> set)
//         {
//             std::set<int> res={};
//             for (int st: set)
//             {
//                 auto eclo=eClousure(st,true);
//                 for (int i : eclo)
//                 {
//                     res.insert(i);
//                 }

//             }
//             return res;
//         }
    
    
//          // factory for regular expressions
//         static NFA emptyRE()
//         {
//             return NFA(0,0,{},{},{});
//         }
       
//         static NFA epsilonRE()
//         {
//             std::map<int,std::map<char,std::vector<int>>> tras={};
//             tras[0]['$']={1};
//             return NFA(0,2,{1},{'$'},tras);
//         }

//         static NFA symbolRE(char symbol) {
//         std::map<int,std::map<char,std::vector<int>>> tras = {
//         {0, {{symbol, {1}}}}
//         };
//         return NFA(0, 2, {1}, {symbol}, tras);
//     }

       

//         static  NFA UnionRE(NFA a1,NFA a2)
//         {
//             int startstate= a1.total_states+a2.total_states;
//             std::vector<int> finals=a1.final_states;
//             int totalstates=a1.total_states+a2.total_states+2;

//             std::set<char> alphabet=a1.alphabet;
//             alphabet.merge(a2.alphabet);

//             std::map<int, std::map<char, std::vector<int>>> transitions={};
//             transitions=a1.transitions;
//             for (auto value: a2.transitions)
//             {
//                int new_state=value.first+a1.total_states;
//                for (auto value2: value.second)
//                {
//                     std::vector<int> new_states=value2.second;
//                     for(size_t i=0 ;i<new_states.size();i++)
//                     {
//                         new_states[i]+=a1.total_states;
//                     }
//                     transitions[new_state][value2.first]=new_states;

//                }
//             }

//             if(alphabet.find('$')==alphabet.end())
//             {
//                 alphabet.insert('$');
//             }
//             transitions[startstate]['$']={a1.start_state,a2.start_state+a1.total_states};

//             auto final_token_types=a1.m_final_token_types;
//             for (auto pair: a2.m_final_token_types)
//             {
//                 final_token_types[pair.first+a1.total_states]=pair.second;
//             }

            

//             for (int fs: a2.final_states)
//             {
//                 finals.push_back(fs+a1.total_states);
//             }
            
//             NFA res(startstate,totalstates,finals,alphabet,transitions);
//             res.m_final_token_types=final_token_types;
//             return res;
//         }
    










//         static NFA ConcatenationRE(NFA a1,NFA a2)
//         {
//             int startstate= a1.start_state;
//             int totalstates=a1.total_states+a2.total_states;

//             std::set<char> alphabet=a1.alphabet;
//             alphabet.merge(a2.alphabet);

//             std::vector<int>final_states={};
//             for (int fs:a2.final_states)
//             {
//                 final_states.push_back(fs+a1.total_states);
//             }

//             std::map<int, std::map<char, std::vector<int>>> transitions={};
//             transitions=a1.transitions;

//             for (auto value: a2.transitions)
//             {
//                int new_state=value.first+a1.total_states;
//                for (auto value2: value.second)
//                {
//                     std::vector<int> new_states=value2.second;
//                     for(size_t i=0 ;i<new_states.size();i++)
//                     {
//                         new_states[i]+=a1.total_states;
//                     }
//                     transitions[new_state][value2.first]=new_states;
//                }
//             }
//             if(alphabet.find('$')==alphabet.end())
//             {
//                 alphabet.insert('$');
//             }
//             for (int fs:a1.final_states)
//             {
//                 transitions[fs]['$']={a2.start_state+a1.total_states};
//             }

//             return NFA(startstate,totalstates,final_states,alphabet,transitions);
            
//         }
    
//         static NFA ClousureRE(NFA a1)
//         {

//             int startstate=a1.total_states;
//             int finalstate=a1.total_states+1;
//             int totalStates=a1.total_states+2;

//             auto transitions=a1.transitions;
//             auto alphabet=a1.alphabet;

//             if(alphabet.find('$')==alphabet.end())
//             {
//                 alphabet.insert('$');
//             }


//             for (int fs : a1.final_states)
//             {
//                 transitions[fs]['$']={a1.start_state,finalstate};
//             }
//             transitions[startstate]['$']={finalstate,a1.start_state};

//             return NFA(startstate,totalStates,{finalstate},alphabet,transitions);

//         }
    

//     };
    


