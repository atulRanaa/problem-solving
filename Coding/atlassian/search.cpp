/*
You are running a classroom and suspect that some of your students are passing around the answer to a multiple-choice question in 2D grids of letters. The word may start anywhere in the grid, and consecutive letters can be either immediately below or immediately to the right of the previous letter.

Given a grid and a word, write a function that returns the location of the word in the grid as a list of coordinates. If there are multiple matches, return any one.

grid1 = [
    ['b', 'b', 'b', 'a', 'l', 'l', 'o', 'o'],
    ['b', 'a', 'c', 'c', 'e', 's', 'c', 'n'],
    ['a', 'l', 't', 'e', 'w', 'c', 'e', 'w'],
    ['a', 'l', 'o', 's', 's', 'e', 'c', 'c'],
    ['w', 'o', 'o', 'w', 'a', 'c', 'a', 'w'],
    ['i', 'b', 'w', 'o', 'w', 'w', 'o', 'w']
]
word1_1 = "access"      # [(1, 1), (1, 2), (1, 3), (2, 3), (3, 3), (3, 4)]
word1_2 = "balloon"     # [(0, 2), (0, 3), (0, 4), (0, 5), (0, 6), (0, 7), (1, 7)]

word1_3 = "wow"         # [(4, 3), (5, 3), (5, 4)] OR 
                        # [(5, 2), (5, 3), (5, 4)] OR 
                        # [(5, 5), (5, 6), (5, 7)]
                          
word1_4 = "sec"         # [(3, 4), (3, 5), (3, 6)] OR 
                        # [(3, 4), (3, 5), (4, 5)]    

word1_5 = "bbaal"       # [(0, 0), (1, 0), (2, 0), (3, 0), (3, 1)]


grid2 = [
  ['a'],
]
word2_1 = "a"

grid3 = [
    ['c', 'a'],
    ['t', 't'],
    ['h', 'a'],
    ['a', 'c'],
    ['t', 'g']
]
word3_1 = "cat"
word3_2 = "hat"

grid4 = [
    ['c', 'c', 'x', 't', 'i', 'b'],
    ['c', 'a', 't', 'n', 'i', 'i'],
    ['a', 'x', 'n', 'x', 'p', 't'],
    ['t', 'x', 'i', 'x', 't', 't']
]
word4_1 = "catnip"      # [(1, 0), (1, 1), (1, 2), (1, 3), (1, 4), (2, 4)] OR
                        # [(0, 1), (1, 1), (1, 2), (1, 3), (1, 4), (2, 4)]


All test cases:

search(grid1, word1_1) => [(1, 1), (1, 2), (1, 3), (2, 3), (3, 3), (3, 4)]
search(grid1, word1_2) => [(0, 2), (0, 3), (0, 4), (0, 5), (0, 6), (0, 7), (1, 7)]
search(grid1, word1_3) => [(4, 3), (5, 3), (5, 4)] OR 
                          [(5, 2), (5, 3), (5, 4)] OR 
                          [(5, 5), (5, 6), (5, 7)]
search(grid1, word1_4) => [(3, 4), (3, 5), (3, 6)] OR
                          [(3, 4), (3, 5), (4, 5)]                           
search(grid1, word1_5) => [(0, 0), (1, 0), (2, 0), (3, 0), (3, 1)]

search(grid2, word2_1) => [(0, 0)]

search(grid3, word3_1) => [(0, 0), (0, 1), (1, 1)]
search(grid3, word3_2) => [(2, 0), (3, 0), (4, 0)]

search(grid4, word4_1) => [(1, 0), (1, 1), (1, 2), (1, 3), (1, 4), (2, 4)] OR
                          [(0, 1), (1, 1), (1, 2), (1, 3), (1, 4), (2, 4)]

Complexity analysis variables:

r = number of rows
c = number of columns
w = length of the word
*/
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

string find(vector<string> &words, string note) {
  int n = words.size();
  vector< vector<int> > freq(n, vector<int> (26, 0));
  
  // a -> 0
  // b -> 1
  // ch - 'a' = index of char
  for(int i = 0; i < n; i++) {
    for(char ch: words[i]) {
      freq[i][ch-'a']++;
    }
  }
  
  vector<int> note_freq(26, 0);
  for(char ch: note)
    note_freq[ch-'a']++;
    
    
  // match 
  for(int i = 0; i < n; i++) {
    
    bool flag = true;
    for(int j = 0; j < 26; j++) {
      if(note_freq[j] < freq[i][j]) {
        flag = false;
        break;
      }
    }
    
    if(flag)
      return words[i];
  }
  
  return "-";
}


vector< pair<int, int> > search(vector<vector<char>> grid, string word) {
  struct point {
    int x, y;
    int itr;
    vector< pair<int, int> > jour;
  };
  
  if (word.size() == 0)
    return {};
  
  queue< point > space;
  
  int n = grid.size();
  int m = grid[0].size();
  
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < m; j++) {
      if(word[0] == grid[i][j]) {
        point p = {i, j, 0, { {i,j} }};
        space.push(p);
      }
    }
  }

  point ans;
  
  while(!space.empty()) {
    point p = space.front();
    space.pop();
    
    int x = p.x;
    int y = p.y;
    int itr = p.itr;

    // cout << x << ", " << y << ":" << itr << "\n";

    if(itr == word.size() - 1) {
        ans = p;
        break;
    }
    if(x >= n || y >= m)
        continue;
    
    if(x+1 < n && itr+1 < word.size() && grid[x+1][y] == word[itr+1]) {
      auto copy_j = p.jour;
      copy_j.push_back({x+1, y});
      point p_next = {x+1, y, itr+1, copy_j};

      space.push(p_next);
    }
    
    if(y+1 < m && itr+1 < word.size() && grid[x][y+1] == word[itr+1]) {
      auto copy_j = p.jour;
      copy_j.push_back({x, y+1});
      point p_next = {x, y+1, itr+1, copy_j};

      space.push(p_next);
    }
  }
  
  return ans.jour;
}

void print(vector< pair<int, int> > v) {
    cout << "Ans: ";
    for(auto e: v)
        cout << "(" << e.first << ", " << e.second << ") ";
    cout << "\n";
}

int main() {
 vector<vector<char>> grid1 = {
    {'b', 'b', 'b', 'a', 'l', 'l', 'o', 'o'},
    {'b', 'a', 'c', 'c', 'e', 's', 'c', 'n'},
    {'a', 'l', 't', 'e', 'w', 'c', 'e', 'w'},
    {'a', 'l', 'o', 's', 's', 'e', 'c', 'c'},
    {'w', 'o', 'o', 'w', 'a', 'c', 'a', 'w'},
    {'i', 'b', 'w', 'o', 'w', 'w', 'o', 'w'},
  };
  string word1_1 = "access";
  string word1_2 = "balloon";
  string word1_3 = "wow";
  string word1_4 = "sec";
  string word1_5 = "bbaal";

  print(search(grid1, word1_1));
  print(search(grid1, word1_2));
  print(search(grid1, word1_3));
  print(search(grid1, word1_4));
  print(search(grid1, word1_5));

  vector<vector<char>> grid2 = {
    {'a'},
  };
  string word2_1 = "a";

  vector<vector<char>> grid3 = {
    {'c', 'a'},
    {'t', 't'},
    {'h', 'a'},
    {'a', 'c'},
    {'t', 'g'},
  };
  string word3_1 = "cat";
  string word3_2 = "hat";

  vector<vector<char>> grid4 = {
    {'c', 'c', 'x', 't', 'i', 'b'},
    {'c', 'a', 't', 'n', 'i', 'i'},
    {'a', 'x', 'n', 'x', 'p', 't'},
    {'t', 'x', 'i', 'x', 't', 't'},
  };
  string word4_1 = "catnip";

  return 0;
}
