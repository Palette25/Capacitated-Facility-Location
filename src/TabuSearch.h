/*
* TabuSearch.h -- Use Tabu Search to solve CFL Problem
*/
#ifndef TABU_SEARCH_H
#define TABU_SEARCH_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
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

struct move_p {
	int src_cus;
	int dst_fac;
	bool feasible;
	double cost;
	move_p(int src, int dst, bool fea, double cos){
		src_cus = src;   dst_fac = dst;
		feasible = fea;  cost = cos;
	}
};

class Tabu {
public:
	Tabu(){
		alpha = 1.0;
		epsilon = 0.1;
	}
	void readFile(string filePath);
	// Get greedy result as init solution path
	int performGreedySearch();
	// Perform Tabu Search
	int performTabuSearch();
	// Tabu tools methods
	int performPenalty();
	int getCurrentResult();
	void getNeighborResult(int iteration, int beta);

	int fac_num, cus_num;
	// State vector
	vector<bool> fac_open;
	// Result vector
	vector<int> result;
	vector<int> fea_result;

private:
	int best_feasible_cost, best_cost;
	double alpha, epsilon;
	// Info vectors
	vector<int> capacity;
	vector<int> temp_capacity;
	vector<int> open_cost;
	vector<int> demand;
	vector<vector<int>> assign_cost;
	vector<vector<int>> tabu_list;
};
#endif