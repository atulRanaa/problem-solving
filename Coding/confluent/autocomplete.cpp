#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <mutex>

using namespace std;

class AutoComplete {
        struct Trie {
            array< unique_ptr<Trie>, 26> child;
            bool is_word = false;
        };

        unique_ptr<Trie> root = make_unique<Trie>();

        mutable mutex mtx;

        Trie* findNode(const string& prefix) {
            Trie* node = root.get();

            for(char ch: prefix) {
                int idx = ch - 'a';
                if(!node->child[idx]) {
                    return nullptr;
                }
                node = node->child[idx].get();
            }

            return node;
        }
        
        void dfs(Trie* node, const string& prefix, vector<string>& ans) {
            if(!node)
                return;
            
            if(node->is_word)
                ans.push_back(prefix);
            
            for(int i = 0; i < 26; i++) {
                dfs(node->child[i].get(), prefix + (char)(i + 'a'), ans);
            }
        }
    public:
        void addWord(const string& word) {
            lock_guard<mutex> lg(mtx);

            Trie* node = root.get();

            for(char ch: word) {
                int idx = ch - 'a';
                if(!node->child[idx]) {
                    node->child[idx] = make_unique<Trie>();
                }
                node = node->child[idx].get();
            }

            node->is_word = true; 
        }

        vector<string> suggestions(const string& query) {
            lock_guard<mutex> lg(mtx);

            vector<string> ans;

            Trie* node = findNode(query);
            if(node) {
                dfs(node, query, ans);
            }
            return ans;
        }
};

int main() {
    AutoComplete feat;

    feat.addWord("atul");
    feat.addWord("add");
    feat.addWord("all");
    feat.addWord("baby");

    auto result = feat.suggestions("a");
    for(auto e: result)
        cout << e << "\n";
    return 0;
}