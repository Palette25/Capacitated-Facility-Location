/*
* RandomLocalSearch.h -- Use random local search to solve CFLP
*/
#ifndef RANDOM_LOCAL_SEARCH_H
#define RANDOM_LOCAL_SEARCH_H

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <windows.h>

using namespace std;

class RLS{
public:
	RLS(){
		bestCost = (double)INT_MAX;
		tempCost = 0;
	}
	void readFile(string filePath);
	int performRandomLocalSearch();

	// Random Search tools methods
	vector<bool> randomStartState();
	vector<bool> chooseBetterNeighbor(vector<bool>& curr_state);
	// Cost calculating for facility state
	double calculateCostForState(vector<bool>& state);
	// Greedy assign customers to state, decide state available or not
	bool greedyAssignCustomer(vector<bool>& state);
	void calcluateCost();

	int fac_num, cus_num;
	// State vector
	vector<bool> fac_open;
	// Result vector
	vector<int> result;

private:
	double bestCost, tempCost;
	vector<bool> bestState;
	vector<int> usedCapacity;
	// Info vectors
	vector<int> capacity;
	vector<int> open_cost;
	vector<int> demand;
	vector<vector<int>> assign_cost; 

};

#endif