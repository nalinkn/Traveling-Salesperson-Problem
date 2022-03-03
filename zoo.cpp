//  3E33912F8BAA7542FC4A1585D2DB6FE0312725B9
//  zoo.cpp
//  p4-zoo
//
//  Created by Nalin Krishnan on 11/19/21.
//

#include <algorithm>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <numeric>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <getopt.h>
#include <iomanip>
#include "xcode_redirect.hpp"


using namespace std;


class Zoo {
private:
    //make different struct for tsp, only need xCoord and yCoord
    struct CageMST {
        double min_edge_weight = numeric_limits<double>::infinity();
        int32_t xCoord;
        int32_t yCoord;
        u_int32_t parentIndex = numeric_limits<int>::max();
        char category;
        bool visited = false;
    };
    
    struct CageFAST {
        int32_t xCoord;
        int32_t yCoord;
    };
    
    struct CageOPT {
        double min_edge_weight = numeric_limits<double>::infinity(); //reset
        int32_t xCoord;
        int32_t yCoord;
        u_int32_t parentIndex = numeric_limits<int>::max(); //reset
        bool visited = false; //reset
    };
    
    double fastDistance (CageFAST* a, CageFAST* b) {
        
        double aX = static_cast<double>(a->xCoord);
        double aY  = static_cast<double>(a->yCoord);
        double bX = static_cast<double>(b->xCoord);
        double bY = static_cast<double>(b->yCoord);
            
        double xDiff = (bX  - aX) * (bX - aX);
        double yDiff = (bY - aY) * (bY - aY);
            
        double dist = sqrt(xDiff + yDiff);
        return  dist;
        
    }
    
    double optDistance (CageOPT* a, CageOPT* b) {
        double aX = static_cast<double>(a->xCoord);
        double aY  = static_cast<double>(a->yCoord);
        double bX = static_cast<double>(b->xCoord);
        double bY = static_cast<double>(b->yCoord);
            
        double xDiff = (bX  - aX) * (bX - aX);
        double yDiff = (bY - aY) * (bY - aY);
            
        double dist = sqrt(xDiff + yDiff);
        return  dist;
    }
    
    double mstDistance (CageMST* a, CageMST* b) {
        if (a->category == 'w' && b->category == 's') {
            return numeric_limits<double>::infinity();
        }
        else if (a->category == 's' && b->category == 'w') {
            return numeric_limits<double>::infinity();
        }
        else {
            double aX = static_cast<double>(a->xCoord);
            double aY  = static_cast<double>(a->yCoord);
            double bX = static_cast<double>(b->xCoord);
            double bY = static_cast<double>(b->yCoord);
            
            double xDiff = (bX  - aX) * (bX - aX);
            double yDiff = (bY - aY) * (bY - aY);
            
            double dist = xDiff + yDiff;
            return  dist;
        }
    }
    
public:
    void get_options(int argc, char** argv);
    string mode = "";
    u_int32_t cages = 0;
    vector<CageMST*> master;
    vector<CageFAST*> master_fast;
    vector<CageOPT*> master_opt;
    double currCost;
    double bestCost;
    vector<u_int32_t> currPath;
    vector<u_int32_t> bestPath;
    
    void readFile() {
        cin >> cages;
        int32_t num;
        if (mode == "MST") {
            while (cin >> num) {
                CageMST* cage = new CageMST;
                cage->xCoord = num;
                cin >> cage->yCoord;
                if (cage->xCoord < 0 && cage->yCoord < 0) {
                    cage->category = 'w';
                }
                else {
                    if (cage->xCoord == 0 && cage->yCoord <= 0) {
                        cage->category = 'b';
                    }
                    else if (cage->yCoord == 0 && cage->xCoord <= 0) {
                        cage->category = 'b';
                    }
                    else {
                        cage->category = 's';
                    }
                }
                master.push_back(cage);
            }
        }
        else if (mode == "FASTTSP") {
            while (cin >> num) {
                CageFAST* cage = new CageFAST;
                cage->xCoord = num;
                cin >> cage->yCoord;
                master_fast.push_back(cage);
            }
        }
        else {
            while (cin >> num) {
                CageOPT* cage = new CageOPT;
                cage->xCoord = num;
                cin >> cage->yCoord;
                master_opt.push_back(cage);
            }
        }
    }
    
    bool promising(size_t permLength) {
        master_opt[currPath[permLength + 1]]->min_edge_weight = 0;
        CageOPT* curr = master_opt[currPath[permLength + 1]];
        u_int32_t currIndex = 0;
        curr->visited = true;
        double totalWeight = 0;
        u_int32_t count = 0;
       
        for (size_t i = permLength + 1; i < currPath.size(); ++i) {
            if (i != permLength + 1) {
                double currWeight = numeric_limits<double>::infinity();
                for (size_t j = permLength + 2; j < currPath.size(); ++j) {
                    if (!master_opt[currPath[j]]->visited) {
                        if (master_opt[currPath[j]]->min_edge_weight < currWeight) {
                            curr = master_opt[currPath[j]];
                            currIndex = currPath[j];
                            currWeight = master_opt[currPath[j]]->min_edge_weight;
                        }
                    }
                }
                curr->visited = true;
                
                totalWeight += curr->min_edge_weight;
                count++;
            }
           
            for (size_t j = permLength + 2; j < currPath.size(); ++j) {
                if (!master_opt[currPath[j]]->visited) {
                    double dist = optDistance(curr, master_opt[currPath[j]]);
                    if(master_opt[currPath[j]]->min_edge_weight > dist)  {
                        master_opt[currPath[j]]->min_edge_weight = dist;
                        master_opt[currPath[j]]->parentIndex = currIndex;
                    }
                }
            }
        }
        
        //find shortest edge from master[currPath[0]] to master[currPath[permLength + 1]] and onwards
        double firstCloser = numeric_limits<double>::infinity();
        double lastCloser = numeric_limits<double>::infinity();
        for (size_t i = permLength + 1; i < currPath.size(); ++i) {
            if (optDistance(master_opt[currPath[i]], master_opt[currPath[0]]) < firstCloser) {
                firstCloser = optDistance(master_opt[currPath[i]], master_opt[currPath[0]]);
            }
            if (optDistance(master_opt[currPath[i]], master_opt[currPath[permLength]]) < lastCloser) {
                lastCloser = optDistance(master_opt[currPath[i]], master_opt[currPath[permLength]]);
            }
            master_opt[currPath[i]]->visited = false;
            master_opt[currPath[i]]->min_edge_weight = numeric_limits<double>::infinity();
            master_opt[currPath[i]]->parentIndex = numeric_limits<int>::max();
        }
        
        if (permLength == currPath.size() - 1) {
            firstCloser = 0;
            lastCloser = optDistance(master_opt[currPath[permLength]], master_opt[currPath[0]]);
        }
        
        totalWeight += firstCloser + lastCloser + currCost;
        if (totalWeight < bestCost) {
            return true;
        }
        
        return false;
    }
    
    void genPerms(size_t permLength) {
        if (permLength == currPath.size()) {
            //do something with path
            currCost += optDistance(master_opt[currPath[permLength - 1]], master_opt[currPath[0]]);
            //is it distance to starting point instead?
            if (currCost < bestCost) {
                bestPath = currPath;
                bestCost = currCost;
            }
            currCost -= optDistance(master_opt[currPath[permLength - 1]], master_opt[currPath[0]]);
            return;
        }
        if (!promising(permLength)) {
            return;
        }
        for (size_t i = permLength; i < currPath.size(); i++) {
            
            swap(currPath[permLength], currPath[i]);
            double dist = optDistance(master_opt[currPath[permLength]], master_opt[currPath[permLength - 1]]);
            currCost += dist;
            genPerms(permLength + 1);
            currCost -= dist;
            swap(currPath[permLength], currPath[i]);
            
        }
    }
    
    void findMST() {
        master[0]->min_edge_weight = 0;
        CageMST* curr = master[0];
        u_int32_t currIndex = 0;
        curr->visited = true;
        double totalWeight = 0;
        for (size_t i = 0; i < cages; ++i) {
            if (i != 0) {
                double currWeight = numeric_limits<double>::infinity();
                for (size_t j = 1; j < cages; ++j) {
                    if (!master[j]->visited) {
                        if (master[j]->min_edge_weight < currWeight) {
                            curr = master[j];
                            currIndex = static_cast<u_int32_t>(j);
                            currWeight = master[j]->min_edge_weight;
                        }
                    }
                }
                if (currWeight == numeric_limits<double>::infinity()) {
                    cerr << "Cannot construct MST" << "\n";
                    exit(1);
                }
                curr->visited = true;
                
                totalWeight += sqrt(curr->min_edge_weight);
                
            }
           
            for (size_t j = 1; j < cages; ++j) {
                if (!master[j]->visited) {
                    double dist = mstDistance(curr, master[j]);
                    if(master[j]->min_edge_weight > dist)  {
                        master[j]->min_edge_weight = dist;
                        master[j]->parentIndex = currIndex;
                    }
                }
            }
        }
        
        cout << totalWeight << "\n";
        for (size_t i = 1; i < cages; ++i) {
            if (i > master[i]->parentIndex) {
                cout << master[i]->parentIndex << " " << i << "\n";
            }
            else {
                cout << i << " " << master[i]->parentIndex << "\n";
            }
            //iterating all the cages
            //check if its parent index is less/greater than i
            //print based on that
        }
    }
    
//    bool promisingFAST() {
//        return false;
//    }
//
//    bool promisingOPT() {
//        return false;
//    }
    
    void fastTSP () {
        vector<u_int32_t> cycle;
        cycle.reserve(master_fast.size() + 1);
        cycle.push_back(0);
        cycle.push_back(1);
        cycle.push_back(2);
        cycle.push_back(0);
        double totalWeight = fastDistance(master_fast[0], master_fast[1]) + fastDistance(master_fast[1], master_fast[2]) + fastDistance(master_fast[2], master_fast[0]);;
        for (size_t i = 3; i < master_fast.size(); ++i) {
            double minWeight = numeric_limits<double>::infinity();
            size_t currIndex = 0;
            size_t size = cycle.size();
            for (size_t j = 0; j < size - 1; ++j) {
                    //make separate distance function for TSP (no constraint for wild cages)
                double dist = fastDistance(master_fast[cycle[j]], master_fast[i]) + fastDistance(master_fast[i], master_fast[cycle[j + 1]]) - fastDistance(master_fast[cycle[j]], master_fast[cycle[j + 1]]);
                if (dist < minWeight) {
                        minWeight = dist;
                        currIndex = j + 1;
                }
                if (j == size - 2) {
                    cycle.insert(cycle.begin() + static_cast<u_int32_t>(currIndex), static_cast<u_int32_t>(i));
                }
            }
            totalWeight += minWeight;
        }
        cycle.pop_back();
        cout << totalWeight << "\n";
        for (size_t i = 0; i < cycle.size(); ++i) {
            cout << cycle[i] << " ";
        }
        cout << "\n";
    }
    
    void optTSP () {
        currPath.push_back(0);
        currPath.push_back(1);
        currPath.push_back(2);
        currPath.push_back(0);
        //start with path 0, 1, 2, 0
        double totalWeight = optDistance(master_opt[0], master_opt[1]) + optDistance(master_opt[1], master_opt[2]) + optDistance(master_opt[2], master_opt[0]);
        for (size_t i = 3; i < master_opt.size(); ++i) {
            double minWeight = numeric_limits<double>::infinity();
            size_t currIndex = 0;
            size_t size = currPath.size();
            for (size_t j = 0; j < size - 1; ++j) {
                    //make separate distance function for TSP (no constraint for wild cages)
                double dist = optDistance(master_opt[currPath[j]], master_opt[i]) + optDistance(master_opt[i], master_opt[currPath[j + 1]]) - optDistance(master_opt[currPath[j]], master_opt[currPath[j + 1]]);
                if (dist < minWeight) {
                        minWeight = dist;
                        currIndex = j + 1;
                }
                if (j == size - 2) {
                    currPath.insert(currPath.begin() + static_cast<u_int32_t>(currIndex), static_cast<u_int32_t>(i));
                }
            }
            totalWeight += minWeight;
        }
        
        bestCost = totalWeight;
        currPath.pop_back();
        bestPath = currPath;
        
        genPerms(1);
        
        
        cout << bestCost << "\n";
        for (size_t i = 0; i < bestPath.size(); ++i) {
            cout << bestPath[i] << " ";
        }
        cout << "\n";
        
    }
};

void printHelp(char **argv) {
    cout << "Usage: " << argv[0] << " [-m MST|FASTTSP|OPTTSP]|-h" << endl;
    cout << "This program reads the map of a zoo and provides either the " << endl;
    cout << "most optimal set of paths to provide food, the fast-to-find " << endl;
    cout << "and almost optimal canal system to provide water, or the " << endl;
    cout << "optimal canal system to provide water to the animals" << endl;
}

void Zoo::get_options(int argc, char** argv) {
    
    int choice;
    int option_index = 0;
    struct option long_options[] = {
        { "mode",   required_argument,  nullptr, 'm'  },
        { "help",   no_argument,        nullptr, 'h'  },
        { nullptr,  0,                  nullptr, '\0' }
    };
    
    while ((choice = getopt_long(argc, argv, "m:h", long_options, &option_index)) != -1) {
        switch(choice) {
            case 'h':
                printHelp(argv);
                exit(0);
                
            case 'm':
                mode = optarg;
                if (mode != "MST" && mode != "FASTTSP" && mode != "OPTTSP") {
                    cerr << "Invalid mode" << endl;
                    exit(1);
                }
                break;
                
            default:
                cerr << "Invalid command line option" << endl;
                exit(1);
        }
    }
    
    if (mode == "") {
        cerr << "No mode specified" << endl;
        exit(1);
    }
    
}

int main (int argc, char** argv) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    
    cout << setprecision(2);
    cout << fixed;
    
    Zoo zoo;
    zoo.get_options(argc, argv);
    zoo.readFile();
    if (zoo.mode == "MST") {
        zoo.findMST();
        for (size_t i = 0; i < zoo.master.size(); ++i) {
            delete zoo.master[i];
        }
    }
    else if (zoo.mode == "FASTTSP") {
        zoo.fastTSP();
        for (size_t i = 0; i < zoo.master_fast.size(); ++i) {
            delete zoo.master_fast[i];
        }
    }
    else {
        zoo.optTSP();
        for (size_t i = 0; i < zoo.master_opt.size(); ++i) {
            delete zoo.master_opt[i];
        }
    }
}
