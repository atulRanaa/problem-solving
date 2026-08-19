#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;


class DocumentSearch {
    private:
        struct WordPosition {
            int doc_id;
            int pos;

            WordPosition(int doc_id, int pos): doc_id(doc_id), pos(pos) {};
        };
        unordered_map<int, string> docs;

        // inverted index
        // store word to doc mapping
        unordered_map<string, vector<WordPosition> > inv_index;

        vector<string> tokenizer(const string& text) {
            vector<string> tokens;

            /*
                stringstream ss(text);
                string word;
                while(ss >> word) {
                    tokens.push_back(word);
                }
            */
            
            string word;
            for(const char& ch: text) {
                if(isalnum(ch))
                    word += tolower(ch);
                else {
                    if(!word.empty()) {
                        tokens.push_back(word);
                        word.clear();
                    }
                }
            }

            if(!word.empty())
                tokens.push_back(word);

            return tokens;
        }

        bool phraseSearch(vector<string>& words, int doc_id, int pos) {
            for(size_t i = 1; i < words.size(); i++) {
                auto it = inv_index.find(words[i]);
                if(it == inv_index.end())
                    return false;
                
                bool found = false;
                for(const auto& position: it->second) {
                    if(position.doc_id == doc_id && position.pos == pos + i) {
                        found = true;
                        break;
                    }
                }

                if(!found)
                    return false;
            }

            return true;
        }
    public:
        void addDoc(int docId, const string& text) {
            docs[docId] = text;

            // add in index
            auto tokens = tokenizer(text);
            for(size_t i = 0; i < tokens.size(); i++) {
                inv_index[tokens[i]].emplace_back(docId, i);
            }
        }

        vector<int> search(const string& query) {
            auto tokens = tokenizer(query);
            if(tokens.empty())
                return {};

            auto it = inv_index.find(tokens[0]);
            if(it == inv_index.end())
                return {};

            if(tokens.size() == 1) {
                unordered_set<int> docs;
                for(const auto& pos: it->second)
                    docs.insert(pos.doc_id);

                return vector<int> (docs.begin(), docs.end());
            }
            
            unordered_set<int> result;

            // multiple word search
            for(const auto& pos: it->second) {
                if(phraseSearch(tokens, pos.doc_id, pos.pos)) {
                    result.insert(pos.doc_id);
                }
            }

            return vector<int> (result.begin(), result.end());
        }

};

int main() {
    DocumentSearch dic;

    dic.addDoc(1, "Cloud computing is the on-demand availability");
    dic.addDoc(2, "One integrated service for metrics uptime cloud monitoring");
    dic.addDoc(3, "Monitor entire cloud infrastructure");

    // search("cloud") -> [1, 2, 3]
    // search("cloud monitoring") -> [2]
    // search("Cloud computing is") -> [1]

    auto result = dic.search("cloud computing is");
    for(auto& e: result)
        cout << e << " ";
    cout << "\n";
    return 0;
}