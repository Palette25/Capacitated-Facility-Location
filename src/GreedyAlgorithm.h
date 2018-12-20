/*
* GreedyAlgrithm.h -- Use Greedy Search to solve CFL Problem
*/
#ifndef GREEDY_ALGORITHM_H
#define GREEDY_ALGORITHM_H

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <windows.h>

using namespace std;

struct customer {
	int cost;
	int id;
	customer(int c, int i){
		cost = c;
		id = i;
	}
};

class Greedy {
public:
	Greedy(){}
	void readFile(string filePath);
	int performGreedySearch();

	int fac_num, cus_num;
	// State vector
	vector<bool> fac_open;
	// Result vector
	vector<int> result;

private:
	// Info vectors
	vector<int> capacity;
	vector<int> open_cost;
	vector<int> demand;
	vector<vector<int>> assign_cost; 
};

#endif