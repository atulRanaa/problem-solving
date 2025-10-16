#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <iostream>

using namespace std;

struct Function {
    std::string name;
    std::vector<std::string> argumentTypes;
    bool isVariadic;
    
    Function(std::string n, std::vector<std::string> args, bool var)
        : name(std::move(n)), argumentTypes(std::move(args)), isVariadic(var) {}
};

class FunctionLibrary {

    public:
        void registerFunc(Function);
        std::vector<Function> findMatch(std::vector<std::string> argumentTypes);
};

class FunctionLibraryA: public FunctionLibrary {
    private:
        std::vector<Function> funcs;

        bool match(const Function& func, vector<string> argumentTypes) {
            if (!func.isVariadic) {
                if(argumentTypes.size() != func.argumentTypes.size())
                    return false;

                for(size_t argi = 0; argi < func.argumentTypes.size(); argi++) {
                    if(func.argumentTypes[argi] != argumentTypes[argi]) {
                        return false;
                    }
                }

                return true;
            }

            if(argumentTypes.size() < func.argumentTypes.size())
                return false;
            
            for(size_t i = 0; i < func.argumentTypes.size(); i++) {
                if(func.argumentTypes[i] != argumentTypes[i]) {
                    return false;
                }
            }

            for(size_t i = func.argumentTypes.size() - 1; i < argumentTypes.size(); i++) {
                if(func.argumentTypes.back() != argumentTypes[i]) {
                    return false;
                }
            }

            return true;
        }
    public:
        void registerFunc(Function f) {
            funcs.push_back(f);
        }

        std::vector<Function> findMatch(std::vector<string> argumentTypes) {
            vector<Function> ans;

            for(size_t i = 0; i < funcs.size(); i++) {
                const auto& func = funcs[i];
                if(match(func, argumentTypes)) {
                    ans.push_back(func);
                }
            }

            return ans;
        }
};


int main() {
    FunctionLibraryA lib;

    lib.registerFunc(Function("FuncA", {"String", "Integer", "Integer"}, false));
    lib.registerFunc(Function("FuncB", {"String", "Integer"}, true));
    lib.registerFunc(Function("FuncC", {"Integer"}, true));
    lib.registerFunc(Function("FuncD", {"Integer", "Integer"}, true));
    lib.registerFunc(Function("FuncE", {"Integer", "Integer", "Integer"}, false));
    lib.registerFunc(Function("FuncF", {"String"}, false));
    lib.registerFunc(Function("FuncG", {"Integer"}, false));

    // auto result = lib.findMatch({"String"});
    auto result = lib.findMatch({"Integer", "Integer", "Integer", "Integer"});
    for(auto& e: result)
        cout << e.name << "\n";
    
    return 0;
}