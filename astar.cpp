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
            int temp=state[pos-1];
            state[pos-1]=state[pos];
            state[pos]=temp;
        }
        if(action=='R'){
            int temp=state[pos+1];
            state[pos+1]=state[pos];
            state[pos]=temp;
        }
        if(action=='U'){
            int temp=state[pos-4];
            state[pos-4]=state[pos];
            state[pos]=temp;
        }
        if(action=='D'){
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

    //Heuristic dependant on number of misplaced tiles
    int misplaced_heuristic(vector<int> state, int path_length){
        int sumDiff=0;
        for(int i=0; i<16; i++){
            int index;
            for(int j=0; j<16; j++){
                if(state[j]==goal[i]){
                    index=j;
                }
            }
            if(index!=i){
                sumDiff++;
            }
        }
        return sumDiff;
    }

    //Heuristic dependant on manhattan distance
    int manhattan_distance_heuristic(vector<int> state, int path_length){
        int sumDist=0;
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
    bool operator()(pair<vector<int>, int> const& lhs, pair<vector<int>, int> const& rhs) const{
        return a.heuristic(lhs.first, lhs.second)>a.heuristic(rhs.first, rhs.second);
    }
};

int main(int argc, char** argv){
    clock_t tStart = clock();
    vector<int> board;
    for (int i=1; i<argc; i++){
        board.push_back(stoi(argv[i]));
    }
    //printBoard(board);
    priority_queue<pair<vector<int>,int>, vector<pair<vector<int>, int>>, my_compare> pq; // my priority queue which serves as the open list
    FifteenPuzzle thePuzzle(board);
    map<vector<int>, pair<vector<int>, char>> predecessor; // My predecessor map which tracks the path taken to final output
    map<vector<int>, int> visited; // My visited map to make sure that we are not revisiting states, serves as my closed list
    pq.push(make_pair(board,0)); // pushes the first board
    pair<vector<int>,int> curr; // My pair that serves as the current state throughout the algorithm
    // BEST SEARCH ALGORITHM
    while(!pq.empty()){
        curr = pq.top();
        //Used for Debugging the chosen paths 
        /*cout << " SELECTED" << endl;
        printBoard(curr.first);*/
        if(thePuzzle.winCondition(curr.first)){
            break;
        }
        pq.pop();
        visited[curr.first]=1;
        vector<char> actions = thePuzzle.possible_actions(curr.first);
        for(auto x:actions){
            vector<int> newBoard = thePuzzle.swap(x,curr.first);
            if(visited[newBoard]==1){
                continue;
            }
            pq.push(make_pair(newBoard, curr.second+1));
            predecessor[newBoard]=make_pair(curr.first,x);
        }
    }

    // Pushing into stack to reverse engineer the path (print backwards)
    stack<char> moves;
    while(predecessor.count(curr.first)!=0){
        moves.push(predecessor[curr.first].second);
        curr.first=predecessor[curr.first].first;
    }
    cout<<"Moves:";
    while(!moves.empty()){
        cout << moves.top();
        moves.pop();
    }
    cout<<endl;
    cout<<"Nodes Explored " << visited.size() <<endl;
    cout<<"Time taken: " << (double)(clock() - tStart)/CLOCKS_PER_SEC << endl;
    cout <<"Memory Used: " << sizeof(pair<vector<int>,int>)*pq.size() << " bytes" << endl;
}