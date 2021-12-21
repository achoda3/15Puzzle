#include<iostream>
#include<string>
#include<vector>
#include<queue>
#include<utility>
#include<set>
#include<map>
#include<stack>
#include <time.h>
using namespace std;

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

    // Custom heuristic, I sum up the Difference between goal and current state (count of offset numbers)
    // and the sum of distances of each number and where it would be in the goal
    // as well as the total path length so far
    int heuristic(vector<int> state, int path_length){
        int sumDist=0, sumDiff=0;
        for(int i=0; i<16; i++){
            int index;
            for(int j=0; j<16; j++){
                if(state[j]==goal[i]){
                    index=j;
                }
            }
            sumDist+=abs(index-i);
            if(index!=i){
                sumDiff++;
            }
        }
        // Used for debugging the Heuristic calculations
        /*cout << " H " << sumDist+sumDiff+path_length << endl;
        printBoard(state);*/
        return sumDiff+sumDist+path_length;
    }

    int man_heuristic(vector<int> state){
        int sumDist=0, sumDiff=0;
        for(int i=0; i<16; i++){
            int index;
            for(int j=0; j<16; j++){
                if(state[j]==goal[i]){
                    index=j;
                }
            }
            sumDist+=abs(index-i);
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


// Recursive Function that takes in the current board and depth, checks if the win condition is reached, and if not performs itself on all the current Boards neighbours in a depth search manner, passes 
// visited set to keep track of visited set and size, and predecessor map to get the path taken. Also passes last Node so that the main knows where to start the predecessor path.
int recursiveDFS(vector<int> currBoard, int currDepth, int thresh, FifteenPuzzle& thePuzzle, set<vector<int>>& visited, map<vector<int>, pair<vector<int>, char>>& predecessor, vector<int>& lastNode, vector<int> prevNode){
    int f = currDepth + thePuzzle.man_heuristic(currBoard);
    if(thePuzzle.winCondition(currBoard)){
        return -1;
    } else if(f > thresh){
        return f;
    } else {
        int min = INT_MAX;
        bool found = false;
        //printBoard(currBoard);
        vector<char> actions = thePuzzle.possible_actions(currBoard);
        //cout <<"actions " <<endl;
        /*for(auto x:actions){
            cout << x;
        }*/
        for(auto x:actions){
            vector<int> child = thePuzzle.swap(x,currBoard);
            if(prevNode==child){
                //printBoard(currBoard);
                continue;
            }
            predecessor[child]=make_pair(currBoard,x);
            lastNode=child;
            int temp = recursiveDFS(child, currDepth+1, thresh, thePuzzle, visited, predecessor, lastNode, currBoard);
            if(temp==-1){
                found = true;
                break;
            } else if(temp<min){
                min = temp;
            }
        }
        if(found == true){
            return -1;
        } else {
            return min;
        }
    }
}

// The Depth First Search Function that calls the Recursive function and creates the visited set to be used for it, as well as saves the size of it to be returned
int DFS(vector<int> board, int thresh, int& Size, FifteenPuzzle& thePuzzle, map<vector<int>, pair<vector<int>, char>>& predecessor, vector<int>& lastNode){
    set<vector<int>> visited;
    int returnValue;
    while(1){
        returnValue =  recursiveDFS(board, 0, thresh, thePuzzle, visited, predecessor, lastNode, vector<int> ());
        if(returnValue==-1){
            return -1;
        }
        thresh=returnValue;
        cout<<thresh << endl;;
    }
    Size = visited.size();
    return returnValue;
}

int main(int argc, char** argv){
    clock_t tStart = clock();
    vector<int> board;
    for (int i=1; i<argc; i++){
        board.push_back(stoi(argv[i]));
    }

    // Iterative Deepening Search
    FifteenPuzzle thePuzzle(board);
    map<vector<int>, pair<vector<int>, char>> predecessor; // My predecessor map which tracks the path taken to final output
    int length;
    int total=0;
    int size=0;
    bool found=true;
    vector<int> lastNode;
    length = DFS(board, thePuzzle.man_heuristic(board), size, thePuzzle, predecessor, lastNode);
    if(found){
        stack<char> moves;
        while(predecessor.count(lastNode)!=0){
            moves.push(predecessor[lastNode].second);
            lastNode=predecessor[lastNode].first;
        }
        cout<<"Moves:";
        while(!moves.empty()){
            cout << moves.top();
            moves.pop();
        }
    } else {
        cout <<"Not found!" << endl;
    }
    cout<<endl;
    cout<<"Nodes Explored " << total <<endl;
    cout<<"Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout <<"Memory Used: " << sizeof(vector<int>) * total << " bytes" << endl;

}