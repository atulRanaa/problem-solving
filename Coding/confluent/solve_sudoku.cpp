#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

class SudokuSolver {
    private:
        bool canPlace(vector< vector<int> > &sudoku, int i, int j, int num) {

            for(int k = 0; k < 9; k++) {
                if(sudoku[i][k] == num || sudoku[k][j] == num)
                    return false;
            }
            
            int row = (i / 3) * 3;
            int col = (j / 3) * 3;
            for(int row_i = 0; row_i < 3; row_i++) {
                for(int col_i = 0; col_i < 3; col_i++) {
                    if(sudoku[row_i + row][col_i + col] == num)
                        return false;
                }
            }
            
            return true;
        }
    public:
        bool solve(vector< vector<int> > &sudoku) {
            
            for(int i = 0; i < 9; i++) {
                for(int j = 0; j < 9; j++) {
                    if(sudoku[i][j] != 0)
                        continue;

                    for(int k = 1; k <= 9; k++) {
                        if(canPlace(sudoku, i, j, k)) {
                            sudoku[i][j] = k;

                            if(solve(sudoku))
                                return true;

                            sudoku[i][j] = 0;
                        }
                    }

                    return false;
                }
            }

            return true;
        }

        bool is_valid(vector< vector<int> > &sudoku) {
            vector< bitset<10> > row(9);
            vector< bitset<10> > col(9);
            vector< bitset<10> > dia(9);


            for(int i = 0; i < 9; i++) {
                for(int j = 0; j < 9; j++) {
                    int box = (i/3) * 3 + j/3;

                    int val = sudoku[i][j];
                    if(row[i][val] || col[j][val] || dia[box][val])
                        return false;

                    row[i][val] = true;
                    col[j][val] = true;
                    dia[box][val] = true;
                }
            }

            return true;
        }
};


int main() {
    SudokuSolver solver;

    vector< vector<int> > sudoku = {
        {1,0,0,0,0,0,0,0,0},
        {0,0,6,0,0,0,0,9,0},
        {0,0,0,0,0,5,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0},
        {0,2,0,0,0,0,7,0,0},
        {0,0,8,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,4,0},
        {0,0,0,0,3,0,0,0,0},
    };

    solver.solve(sudoku);
    
    for(const auto& row: sudoku) {
        for(auto e: row)
            cout << e << " ";
        cout << "\n";
    }

    cout << solver.is_valid(sudoku) << "\n";

    return 0;
}