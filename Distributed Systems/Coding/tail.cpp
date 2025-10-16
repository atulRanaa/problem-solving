#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

class TailCmd {
        const int BUFFER_SIZE = 4096;
    public:

        vector<string> tail(const string& filename, int n) {
            if(n <= 0)
                return {};

            std::ifstream file(filename, std::ios::binary);
            if(!file) {
                cerr << "unable to open the file\n";
                return {};
            }
        
            file.seekg(0, std::ios::end);
            streampos file_size = file.tellg();

            if(file_size == 0)
                return {};

            vector<string> result;

            streampos pos = file_size;
            vector<char> buffer(BUFFER_SIZE);
            string line;

            while(pos > 0 && result.size() < n) {
                streampos chunk_size = min(pos, static_cast<std::streampos>(BUFFER_SIZE));

                pos -= chunk_size;
                file.seekg(pos);
                file.read(buffer.data(), chunk_size);

                for(auto i = chunk_size - static_cast<std::streampos>(1); i >= 0; --i) {
                    if(buffer[i] == '\n') {
                        reverse(line.begin(), line.end());
                        result.push_back(line);
                        line.clear();

                        if (result.size() == n)
                            break;
                    } else {
                        line += buffer[i];
                    }
                }
            }

            reverse(result.begin(), result.end());
            return result;
        }


        vector<string> tailSimple(const string& filename, int n) {
            ifstream file(filename, std::ios::binary);

            deque<string> result;
            string line;
            while(getline(file, line)) {
                result.push_back(line);

                if(result.size() > n)
                    result.pop_front();
            }

            return vector<string> (result.begin(), result.end());
        }
};

int main() {
    TailCmd cmd;

    auto result = cmd.tail("/Users/ranaatul/Code/problem-solving-directory/Distributed Systems/3. design.md", 1000000);
    for(auto e: result) {
        cout << e << "\n";
    }
    return 0;
}