#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<utility>
#include<set>
#include<map>
#include<stack>
#include <time.h>
#include <climits>
using namespace std;

int expanded=0; // To keep track of nodes expanded

// Function I used for debugging by printing the board 
void printBoard(vector<int> board){
    for(auto x:board){
        cout << x << " ";
    }
    cout << endl;
}

// Class I created for encapsulating a lot of the functions and usage, kinda helped me from reusing variables like goal and actions
// and provided a neat way to interface with my functions
class FifteenPuzzle{
    private:
    vector<int> board; // used for gathering the initial board
    vector<int> goal={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0}; // used to compare for the distances and difference as well as win condition
    vector<char> actions = {'L', 'U', 'R', 'D'}; //list of all possible actions

    // function I used to get the position of the blank
    int blank_pos(vector<int> state){
        for(int i=0; i<16; i++){
            if(state[i]==0){
                return i;
            }
        }
        return -1;
    }

    public:

    // Default constructor I needed to make so I could create an object in my comparator class without having to create a board
    FifteenPuzzle(){

    }

    // Constructor I use to pass the initial board configuration
    FifteenPuzzle(vector<int> board_passed){
        for(int i=0; i<16; i++){
            board.push_back(board_passed[i]);
        }
    }

    // Function used to check if we have the win condition or not
    bool winCondition(vector<int> state){
        for(int i=0; i<16; i++){
            if(state[i]!=goal[i]){
                return false;
            }
        }
        return true;
    }

    // A function that takes in a state and returns a vector of character that lists all available moves from that state
    vector<char> possible_actions(vector<int> state){
        int pos = blank_pos(state);
        //cout << "pos" << pos << endl;
        vector<char> possible_actions;
        bool L=false, U=false, R=false, D=false;
        if(pos%4==0){
            L=true;
        } 
        if(pos<4){
            U=true;
        }
        if(pos%4==3){
            R=true;
        }
        if(pos>11){
            D=true;
        }

        if(!L){
            possible_actions.push_back(actions[0]);
        }
        if(!U){
            possible_actions.push_back(actions[1]);
        }
        if(!R){
            possible_actions.push_back(actions[2]);
        }
        if(!D){
            possible_actions.push_back(actions[3]);
        }
        

        //used to debug the actions available
        /*for(auto x:possible_actions){
            cout<< x << " ";
        }*/
        
        return possible_actions;
    }

    // Function that takes in a state and an action and actually performs the swap between blank and chosen number and returns new state
    vector<int> swap(char action, vector<int> state){
        int pos = blank_pos(state);
        if(action=='L'){
            //cout << (pos-1) <<endl;
            int temp=state[pos-1];
            state[pos-1]=state[pos];
            state[pos]=temp;
        }
        if(action=='R'){
            //cout << (pos+1) <<endl;
            int temp=state[pos+1];
            state[pos+1]=state[pos];
            state[pos]=temp;
        }
        if(action=='U'){
            //cout << (pos-4) <<endl;
            int temp=state[pos-4];
            state[pos-4]=state[pos];
            state[pos]=temp;
        }
        if(action=='D'){
            //cout << (pos+4) <<endl;
            int temp=state[pos+4];
            state[pos+4]=state[pos];
            state[pos]=temp;
        }
        return state;
    }

    // Manhattan Distance
    int man_heuristic(vector<int> state){
        int sumDist=0;
        for(int i=0; i<16; i++){
            int index;
            for(int j=0; j<16; j++){
                if(state[j]==goal[i]){
                    index=j;
                }
            }
            sumDist+=abs((index/4)-(i/4))+abs((index%4)-(i%4));
        }
        // Used for debugging the Heuristic calculations
        /*cout << " H " << sumDist+sumDiff+path_length << endl;
        printBoard(state);*/
        return sumDist;
    }

};

// Global Object to be used by Comparator
FifteenPuzzle a;

// Custom comparator that the priority queue uses to order the different avaialble options and paths to take
class my_compare
{
    public:
    bool operator()(vector<int> const& lhs, vector<int> const& rhs) const{
        return a.man_heuristic(lhs)>a.man_heuristic(rhs);
    }
};

// Recursive Iteratively Deepening Search Function
int idSearch(stack<pair<vector<int>, char>>& path, char prevMove, int g, int& threshold, FifteenPuzzle& thePuzzle){
    vector<int> currBoard = (path.top()).first;
    int f = g + thePuzzle.man_heuristic(currBoard);
    // If greater then threshold return
    if(f>threshold){
        return f;
    }
    // If won, return 1 (FOUND)
    if(thePuzzle.winCondition(currBoard)){
        return -1;
    }
    // Set min to max integer
    int min = INT_MAX;
    vector<char> actions = thePuzzle.possible_actions(currBoard);
    for(auto x:actions){
        vector<int> newNode = thePuzzle.swap(x,currBoard);
        // Checking for move backs
        if(prevMove=='L' && x=='R' || prevMove=='R' && x=='L' || prevMove=='U' && x=='D' || prevMove=='D' && x=='U'){
            continue;
        }
        path.push(make_pair(newNode, x));
        expanded++;
        // call the function for children nodes
        int temp = idSearch(path, x, g+1, threshold, thePuzzle);
        // if found, return 
        if(temp==-1){
            return -1;
        }
        // If not found, return the min of all their thresholds
        if(temp<min){
            min=temp;
        }
        path.pop();
    }
    // If no children nodes were applicable, return your own threshold
    if(min==INT_MAX){
        return f;
    }
    // Else return min
    return min;
}

// Function to call the Recursive function for greater thresholds until found
int idastar(vector<int> initBoard, FifteenPuzzle& thePuzzle){
    int threshold = thePuzzle.man_heuristic(initBoard);
    // stack that contains the path of boards and moves
    stack<pair<vector<int>, char>> path;
    // Push the initial Board with ' ' to find end while printing 
    path.push(make_pair(initBoard, ' '));
    // Until we find solution, can't handle unsolvable puzzles 
    while(1){
        int temp = idSearch(path, ' ', 0, threshold, thePuzzle);
        // If Found
        if(temp==-1){
            int pathSize = path.size(); // Stack to print the path in reverse (Moves in stack "path" are in reverse order)
            stack<char> printStack;
            while((path.top()).second!=' '){
                printStack.push(path.top().second);
                path.pop();
            }
            cout <<"MOVES: ";
            while(!printStack.empty()){
                cout<<printStack.top();
                printStack.pop();
            }
            return pathSize;
        }
        // If not, update threshold and re-run
        threshold=temp;
    }
}

int main(int argc, char** argv){
    clock_t tStart = clock();
    vector<int> board;
    for (int i=1; i<argc; i++){
        board.push_back(stoi(argv[i]));
    }
    // Iterative Deepening Search
    FifteenPuzzle thePuzzle(board);
    bool found = false;
    int pathSize = idastar(board, thePuzzle);
    cout<<endl;
    cout<<"Nodes expanded " <<  expanded <<endl;
    cout<<"Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout <<"Memory Used: " << sizeof(pair<vector<int>,int>)*(pathSize*sizeof(vector<int>)) << " bytes" << endl;

}