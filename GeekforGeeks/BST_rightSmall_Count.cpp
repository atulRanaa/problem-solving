#include <iostream>
using namespace std;

struct Node {
    int value;
    Node* left;
    Node* right;
    
    int num_of_lefts;
    
    Node(int val): value(val), left(0), right(0), num_of_lefts(0){}
};

struct BST {
    Node* root;
    BST(): root(0){}
    
    int insert_element(int value)
    {
        if (0 == root) {
            root = new Node(value);
            return 0;
        }
        int result = 0;
        Node* runner = root;
        while (0 != runner) {
            if (runner->value < value) {
                ++result;
                result += runner->num_of_lefts;
                if (0 == runner->right) {
                    runner->right = new Node(value);
                    return result;
                } else {
                    runner = runner->right;
                }
            } else {
                ++runner->num_of_lefts;
                if (0 == runner->left) {
                    runner->left = new Node(value);
                    return result;
                } else {
                    runner = runner->left;
                }
            }
        }
        return 0;
    }
};

void count_small(int array[], int size, int result[])
{
    BST tree;
    for (int i = (size - 1); i >= 0; --i) {
        result[i] = tree.insert_element(array[i]);
    }
}
int main (){

    freopen("input.txt","r",stdin);
    freopen("output.txt","w",stdout);
    int n; cin>>n;
    int ar[n];
    for(int i=0; i<n; i++) cin>>ar[i];
    int result[n] = {0};

    count_small(ar, n, result);
    for (int i = 0; i < n; ++i) cout << result[i] << ' ';
    return 0;
}