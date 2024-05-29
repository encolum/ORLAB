#include<iostream>
#include<fstream>
#include<filesystem>
#include<algorithm>
#include<string>
#include<vector>
#include<cmath>
#include<sstream>
#include<random>
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

//Local search

pair<vector<int>, double> local_search(const vector<vector<double>> &c, const vector<int> &solution ){
    vector<int> best_solution = fix_start_node(solution,0);
    // pair<vector<int>, double> neighbor_pair = generateNeighbor(c, solution)
    double best_distance = total_distance(c,best_solution);
     for (int i = 0 ; i < 10000; i++){
        vector<int> neighbor= generateNeighbor(best_solution);
        double neighbor_distance = total_distance(c,neighbor);
        if (neighbor_distance < best_distance){
            best_distance = neighbor_distance;
            best_solution = neighbor;
        }
    }
    return make_pair(best_solution, best_distance);
}
//perturbation?? is this necessary ??

vector<int> perturbate(const vector<int>& solution, int perturbation_size) {
int n = solution.size();
vector<int> perturbed_solution = solution;
int moveType = rand() % 2 + 1 ;

if (moveType == 1){
    // Shaking
for (int i = 0; i < perturbation_size; i++) {
    int rand1 = rand() % (n - 1) + 1;
    int rand2 = rand() % (n - 1) + 1;
    swap(perturbed_solution[rand1], perturbed_solution[rand2]);
}

}
else{
    // Kick
for (int i = 0 ; i < perturbation_size/2; i++){
    int rand1 = rand() % (n - 2) + 1;
    int rand2 = rand() % (n - 2) + 1;
    reverse(perturbed_solution.begin() + rand1, perturbed_solution.begin() + rand2 + 1);

}
}

 return perturbed_solution;
}

pair<vector<int>, double> iterated_local_search(const vector<vector<double>> &c, long long num_iterations, int perturbation_size) {
int n = c.size();
vector<int> current_solution = generate_initial_solution(c);
pair<vector<int>, double> current_pair = local_search(c, current_solution);
vector<int> best_solution = current_pair.first;
double best_distance = current_pair.second;


for (int i = 0; i < num_iterations; i++) {
    vector<int> perturbed_solution = perturbate(best_solution, perturbation_size);
    pair<vector<int>, double> new_pair = local_search(c, perturbed_solution);
    vector<int> new_solution = new_pair.first;
    double new_distance = new_pair.second;

    if (new_distance < best_distance) {
        best_solution = new_solution;
        best_distance = new_distance;
    }
}

return make_pair(best_solution, best_distance);
}

int main(){
string file = "11berlin52_T40_p1.sop";
vector<vector<double>> c = preprocessor(file);
long long num_iterations = 10000;
int perturbed_size = 5;
pair<vector<int>, double> result = iterated_local_search(c, num_iterations, perturbed_size);
vector<int> best_solution = result.first;
double best_distance = result.second;
cout << "Best distance: " << best_distance << endl;

}