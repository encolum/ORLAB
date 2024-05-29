#include<iostream>
#include<string>
#include<fstream>
#include<cmath>
#include<filesystem>
#include<sstream>
#include<random>
#include<algorithm>
#include<numeric>
#include<vector>
#include <chrono>
using namespace std;

// Folder_file
vector<string> folder_file(const string &folder_path){
    string extension = ".sop";
    vector<string> files;
    for (const auto& entry: filesystem::directory_iterator(folder_path))
    {
        if (entry.is_regular_file() && entry.path().extension() == extension)
        {
            files.push_back(entry.path().filename().string());
        }
    }
    return files;

}

// preprocesor

vector<vector<double>> preprocessor(const string &filename)
{
    //Doc file
    ifstream file(filename);
    string line;
    vector<vector<double>> coordinates;
    bool foundHeader = false; // Biến đánh dấu nếu đã tìm thấy dòng header

    // Bỏ qua các dòng cho đến NODE_COORD_SECTION
    while (getline(file, line)) {
        if (line == "NODE_COORD_SECTION") {
            foundHeader = true;
            break;
        }
    }

    // Đọc tọa độ cho đến khi gặp GTSP_SET_SECTION
    if (foundHeader) {
        while (getline(file, line)) {
            if (line == "GTSP_SET_SECTION: set_id set_profit id-vertex-list")
                break;
            vector<double> point;
            istringstream iss(line);
            int index;
            double x, y;
            if (iss >> index >> x >> y) {
                point.push_back(x);
                point.push_back(y);
                coordinates.push_back(point);
            }
        }
    }

    file.close();

    // Tao ma tran khoang cach
    int n = coordinates.size();
    vector<vector<double>> c(n,vector<double>(n));
    for (int i = 0 ; i < n ; i ++){
        for (int j = 0 ; j < n ; j++){
            double dx = coordinates[i][0] - coordinates[j][0];
            double dy = coordinates[i][1] - coordinates[j][1]; 
            c[i][j] = ceil(sqrt(dx*dx + dy*dy));
        }
    }
    return c ;
}


//Accepted probability
double accepted_probability(double delta, double temperature){
    double accepted_prob = exp(-delta/temperature);
    if (delta > 0){
        return accepted_prob;
    }
    else
        return 1.0;
}

// Fix start node
vector<int> fix_start_node(const vector<int> &solution, int start_node){
    auto it = find(solution.begin(), solution.end(), start_node); //find để tìm vị trí của startNode trả về 1 con trỏ iterator
    int index = distance(solution.begin(), it); //tính index của startnode  (index thuộc (0, n-1))
    if (index == solution.size()) // nếu index == n : chứng tỏ ko có điểm startnode trong solution --> trả về solution nguyên bản
        return solution;
    vector<int> newSolution;
    newSolution.insert(newSolution.end(), solution.begin() + index, solution.end()); //Thêm các phần tử từ index đến cuối solution vào newSolution. (like append)
    newSolution.insert(newSolution.end(), solution.begin(), solution.begin() + index); //Thêm các phần tử từ đầu đến index-1 của solution vào cuối của newSolution. (like append)
    return newSolution;

}

// Total distance

double total_distance(const vector<vector<double>> &c, const vector<int> &solution){
    double total_distance = 0 ;
    int n = solution.size();
    for (int i = 0 ; i < n -1  ; i++){
        total_distance += c[solution[i]][solution[i+1]];
        
    }
    total_distance += c[solution.back()][solution.front()];
    return total_distance;
}

// Generate neighbor

pair<vector<int>, double> generateNeighbor(const vector<vector<double>>& c, const vector<int>& solution) {
    int numsNode = solution.size();
    vector<int> neighbor = solution;
    double totalDist = total_distance(c, neighbor);
    int moveType = rand() % 3 + 1 ; // Generates a random number between 1 and 3
    // int moveType = 3 ;
    if (moveType == 1) { // K-opt
        int i = rand() % (numsNode - 4) + 1;
        int j = i + 3;
        int ni = neighbor[i], nj = neighbor[j];
        int prevI = neighbor[i - 1], nextI = neighbor[(i + 1) % numsNode];
        int prevJ = neighbor[(j - 1 + numsNode) % numsNode], nextJ = neighbor[(j + 1) % numsNode];
        reverse(neighbor.begin() + i, neighbor.begin() + j + 1);
        totalDist -= c[prevI][ni] + c[nj][nextJ];
        totalDist += c[prevI][nj] + c[ni][nextJ];
        // totalDist = total_distance(c,neighbor);

    } else if (moveType == 2) { // Swap
        int i = rand() % (numsNode - 2) + 1;
        int j = rand() % (numsNode - 2) + 1;
        int ni = neighbor[i], nj = neighbor[j];
        int prevI = neighbor[(i - 1)], nextI = neighbor[(i + 1) %numsNode];
        int prevJ = neighbor[(j - 1)], nextJ = neighbor[(j + 1) % numsNode];

        if (abs(i - j) > 1) {
            totalDist -= c[prevI][ni] + c[ni][nextI] + c[prevJ][nj] + c[nj][nextJ];
            totalDist += c[prevI][nj] + c[nj][nextI] + c[prevJ][ni] + c[ni][nextJ];
        } else if (abs(i - j) == 1) {
            if (i < j) {
                totalDist -= c[prevI][ni] + c[ni][nj] + c[nj][nextJ];
                totalDist += c[prevI][nj] + c[nj][ni] + c[ni][nextJ];
            } else {
                totalDist -= c[prevJ][nj] + c[nj][ni] + c[ni][nextI];
                totalDist += c[prevJ][ni] + c[ni][nj] + c[nj][nextI];
            }
        }

        swap(neighbor[i], neighbor[j]);
    }
    else { // Insertion
        int i = rand() % (numsNode - 2) + 1;
        int j = rand() % (numsNode - 2) + 1;
        int ni = neighbor[i], nj = neighbor[j];
        int prevI = neighbor[(i - 1)], nextI = neighbor[(i + 1) %numsNode];
        int prevJ = neighbor[(j - 1)], nextJ = neighbor[(j + 1) % numsNode];

        if (abs(i - j) > 1) {
            if (j > i){
                totalDist = totalDist - c[prevI][ni] - c[nj][nextJ] - c[prevJ][nj] ;
                totalDist += c[prevI][nj] + c[nj][ni] + c[prevJ][nextJ] ;
            
            }
            else if (i > j){
                
                totalDist = totalDist - c[prevJ][nj] - c[nj][nextJ] - c[ni][nextI];
                totalDist = totalDist + c[prevJ][nextJ] + c[ni][nj] + c[nj][nextI];

            }

        
        }
               
        else if (abs(i - j) == 1) {
            if (i < j){
                    totalDist = totalDist - c[prevI][ni]  - c[nj][nextJ];
                    totalDist = totalDist + c[prevI][nj]  + c[ni][nextJ];

                }
               
                else{
                    totalDist = totalDist - c[prevJ][nj] - c[ni][nextI];
                    totalDist = totalDist + c[prevJ][ni] + c[nj][nextI];

                }
        }
        if (i < j){
        neighbor.insert(neighbor.begin() + i, neighbor[j]); //Insert phần tử ở vị trí i vào vị trí j
        neighbor.erase(neighbor.begin() + j + (j < i ? 0 : 1));
    }
    else{
        neighbor.insert(neighbor.begin() + i + 1 , neighbor[j]); //Insert phần tử ở vị trí i vào vị trí j
        neighbor.erase(neighbor.begin() + j );
}
    
}
    return make_pair(neighbor, totalDist);
}
    //Solve
    pair<vector<int>, double> solve_SA_TSP(const string& file, double cooling_rate, double temp_control){
        vector<vector<double>> c = preprocessor(file);
        if (c.empty()) {
        cout << "Khong the xu ly file: " << file << endl;
        return make_pair(vector<int>(), -1.0); // Trả về một giá trị đặc biệt để đánh dấu lỗi
    }
    int numsNode = c.size();
    vector<int> currentSolution(numsNode);
    iota(currentSolution.begin(), currentSolution.end(), 0);
    random_shuffle(currentSolution.begin(), currentSolution.end());
    double currentDistance = total_distance(c, currentSolution);
    vector<int> bestSolution = fix_start_node(currentSolution, 0);
    double bestDistance = total_distance(c, bestSolution);
    double temperature = -(bestDistance * temp_control / -0.69314718056) ; // ln(0.5)
    mt19937 gen(time(0)); //random
    uniform_real_distribution<double> dis(0.0, 1.0);

    while (temperature > 0.001) {
        pair<vector<int>, double> neighborPair = generateNeighbor(c, currentSolution);
        vector<int> neighbor = fix_start_node(neighborPair.first, 0);
        double neighborDistance = total_distance(c, neighbor);

        if (neighborPair.second != neighborDistance) {
            cout << "g" << endl;
            cout << neighborPair.second << " " << neighborDistance << endl;
            cout << "Current: ";
            for (int node : currentSolution) {
                cout << node << " ";
            }
            cout << endl;
            cout << "Neighbor: ";
            for (int node : neighbor) {
                cout << node << " ";
            }
            cout << endl;
        }

        double delta = neighborPair.second - currentDistance;
        double acceptedProb = accepted_probability(delta, temperature);

        if (acceptedProb > dis(gen)) {
            currentSolution = neighbor;
            currentDistance = neighborPair.second;
            if (currentDistance < bestDistance) {
                bestSolution = currentSolution;
                bestDistance = currentDistance;
            }
        }

        temperature *= cooling_rate;
    }

    return make_pair(bestSolution, bestDistance);
}

int main() {
    string folder_path  ="D:\\TSP";
    vector<string> files = folder_file(folder_path);
    double cooling = 0.999999;
    double temp_control = 0.5 ;
    vector<string> filess;
    for (auto &file : files) {
        auto a = std::filesystem::path(folder_path) / file ;
        string s = a.string();
        filess.push_back(s);
    }

        
    for (int i =0 ; i < filess.size(); i++)
    {   
        auto start = chrono::high_resolution_clock::now();
        pair<vector<int>, double> result = solve_SA_TSP(filess[i], cooling, temp_control);
        auto end = chrono::high_resolution_clock::now();
        vector<int> bestSolution = result.first;
        double bestDistance = result.second;
        std::chrono::duration<double> duration = end - start;
        cout << "File name: " << filess[i] << endl ;
        cout << "Best distance: " << bestDistance << endl;
        cout << "Time: " <<  duration.count() << "seconds" << endl;
        cout << endl;
    }
}
        