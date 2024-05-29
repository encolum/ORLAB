#include<iostream>
#include<fstream>
#include<algorithm>
#include<random>
#include<sstream>
#include<cmath>

using namespace std;

// Preprocessor

vector<vector<double>> preprocessor(const string &filename){
ifstream file(filename);
string line;
bool foundheader = false;
vector<vector<double>> coordinates;
while (getline(file, line)) {
if (line == "NODE_COORD_SECTION") {
foundheader = true;
break;
}


}
while (getline(file,line)){
    if (line == "GTSP_SET_SECTION: set_id set_profit id-vertex-list")
        break;
    vector<double> point;
    istringstream iss(line);  // Khởi tạo một istringstream iss từ dòng line.
    int z;
    double x,y;
    if (iss >> z >> x >> y){
        point.push_back(x);
        point.push_back(y);
        coordinates.push_back(point);
    }
}
file.close();

// Tạo ma trận cost
int n = coordinates.size();
vector<vector<double>> c(n,vector<double>(n));
for (int i  = 0 ; i < n ; i++){
    for (int j =  0 ; j < n ; j++){
        double dx = coordinates[i][0] - coordinates[j][0];
        double dy = coordinates[i][1] - coordinates[j][1];
        c[i][j] = ceil(sqrt(dx*dx + dy*dy));
    }
}
return c;
}

// Fix start node
vector<int> fix_start_node(const vector<int> &solution, int start_node){
    auto it = find(solution.begin(), solution.end(), start_node);
    int index = distance(solution.begin() , it);
    if (index == solution.size())
    {
    return solution;
    }
    vector<int> newSolution;
    newSolution.insert(newSolution.end(), solution.begin() + index, solution.end());
    newSolution.insert(newSolution.end(), solution.begin(), solution.begin() + index);
    return newSolution;
}

// Total Distance


double total_distance(const vector<vector<double>> &c, const vector<int> &solution){
    int n = solution.size();
    double total_distance = 0;
    for (int i = 0 ; i < n - 1; i++){
        total_distance += c[solution[i]][solution[i+1]];
    }
    total_distance += c[solution.front()][solution.back()];
    return total_distance;
}
// Generate neighbor or perturbation ???
vector<int> generateNeighbor(const vector<int>& solution) {
    int numsNode = solution.size();
    vector<int> neighbor = solution;
    int moveType = rand() % 3 + 1 ; // Generates a random number between 1 and 3
    // int moveType = 3 ;
    if (moveType == 1) { // K-opt
    int i = rand() % (numsNode - 4) + 1;
    int j = i + 3;
    int ni = neighbor[i], nj = neighbor[j];
    reverse(neighbor.begin() + i, neighbor.begin() + j + 1);


    } else if (moveType == 2) { // Swap
        int i = rand() % (numsNode - 2) + 1;
        int j = rand() % (numsNode - 2) + 1;
        int ni = neighbor[i], nj = neighbor[j];

        swap(neighbor[i], neighbor[j]);
    }
    else { // Insertion
        int i = rand() % (numsNode - 2) + 1;
        int j = rand() % (numsNode - 2) + 1;
        if (i < j){
        neighbor.insert(neighbor.begin() + i, neighbor[j]); //Insert phần tử ở vị trí i vào vị trí j
        neighbor.erase(neighbor.begin() + j + (j < i ? 0 : 1));
    }
    else{
        neighbor.insert(neighbor.begin() + i + 1 , neighbor[j]); //Insert phần tử ở vị trí i vào vị trí j
        neighbor.erase(neighbor.begin() + j );
    }

    }
        return neighbor;
    }

vector<int> generate_initial_solution(const vector<vector<double>> &c) {
int n = c.size();
vector<int> solution(n);
iota(solution.begin(), solution.end(), 0);

// Greedy initialization
random_device rd;
mt19937 gen(rd());
shuffle(solution.begin(), solution.end(), gen);


return solution;
}

// Tabu search

pair<vector<int>, double> tabu_search(const vector<vector<double>> c,  long long num_iterations , int tabu_tenure ){
    int n = c.size();
    vector<int> current_solution = generate_initial_solution(c);
    vector<int> best_solution = fix_start_node(current_solution,0);
    double best_distance = total_distance(c,best_solution);

    vector<vector<int>> tabu_list;
    for (int i = 0 ; i < num_iterations; i++){
        vector<int> neighbor = generateNeighbor(current_solution);
        neighbor = fix_start_node(neighbor,0);
        double neighbor_distance = total_distance(c,neighbor);

        if (neighbor_distance < best_distance){
            if (find(tabu_list.begin(), tabu_list.end(), neighbor) == tabu_list.end()){ //Hàm find trả về 1 con trỏ
                current_solution = neighbor;
                best_distance = neighbor_distance;
                best_solution = neighbor;
                tabu_list.push_back(current_solution);
                if(tabu_list.size() > tabu_tenure)
                    tabu_list.erase(tabu_list.begin());

            }
            

        }
    }
    return make_pair(best_solution, best_distance);
}
pair<vector<int>, double> run_tabu_search(const vector<vector<double>>& c, int num_runs = 100, long long num_iterations = 1000000, int tabu_tenure = 30) {
    pair<vector<int>, double> best_result = make_pair(vector<int>(), INFINITY);

    for (int i = 0; i < num_runs; i++) {
        pair<vector<int>, double> result = tabu_search(c, num_iterations, tabu_tenure);
        if (result.second < best_result.second) {
            best_result = result;
        }
    }

    return best_result;
}



int main() {
    string file = "11berlin52_T40_p1.sop";
    vector<vector<double>> c = preprocessor(file);

    pair<vector<int>, double> result = run_tabu_search(c);
    vector<int> best_solution = result.first;
    double best_distance = result.second;

    cout << "Best distance: " << best_distance << endl;

    return 0;
}
