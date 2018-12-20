#include "RandomLocalSearch.h"

void RLS::readFile(string filePath){
	ifstream in(filePath.c_str());
	in >> fac_num >> cus_num;
	// Read variables in order
	for(int i=0; i<fac_num; i++){
		int temp_cap, temp_cost;
		in >> temp_cap >> temp_cost;
		capacity.push_back(temp_cap);
		open_cost.push_back(temp_cost);
	}

	for(int i=0; i<cus_num; i++){
		float temp_demand;
		in >> temp_demand;
		demand.push_back((int)temp_demand);
	}

	for(int i=0; i<fac_num; i++){
		vector<int> temp;
		for(int j=0; j<cus_num; j++){
			float ass_cost;
			in >> ass_cost;
			temp.push_back((int)ass_cost);
		}
		assign_cost.push_back(temp);
	}

	// Init State
	for(int i=0; i<fac_num; i++){
		fac_open.push_back(false);
	}

	for(int i=0; i<cus_num; i++){
		result.push_back(-1);
	}
}

int RLS::performRandomLocalSearch(){
	int searchTime = 100;

	for(int i=0; i<searchTime; i++){
		vector<bool> startState = randomStartState();
		// Start Random Search
		bool endFlag = false;
		double curr_cost = tempCost;
		while(!endFlag){
			vector<bool> nextState = chooseBetterNeighbor(startState);
			if(curr_cost <= tempCost){
				endFlag = true;
			}else {
				startState = nextState;
				curr_cost = tempCost;
			}
		}
		calculateCostForState(startState);
		if(tempCost < bestCost){
			bestCost = tempCost;
			bestState = startState;
		}
	}
	calculateCostForState(bestState);
	return bestCost;
}

// Random Search tools methods
vector<bool> RLS::randomStartState(){
	vector<bool> result;
	srand((int)time(0));

	for(int i=0; i<fac_num; i++){
		int rand_result = rand() % 10;
		if(rand_result > 5){
			result.push_back(true);
		}else {
			result.push_back(false);
		}
	}
	return result;
}

vector<bool> RLS::chooseBetterNeighbor(vector<bool>& curr_state){
	vector<bool> bestNext = curr_state;
	double best_cost = tempCost;

	vector<int> open;
	vector<int> close;

	for (int i = 0; i < fac_num; i++) {
        if (curr_state[i]) {
            open.push_back(i);
        } else {
            close.push_back(i);
        }
    }

    vector<bool> next;
    double nextCost = 0;
    // close a open facility
    for (int i : open) {
        next = curr_state;
        next[i] = false;
        nextCost = calculateCostForState(next);
        if (nextCost < best_cost) {
            best_cost = nextCost;
            bestNext = next;
        }
    }

    // open a closed facility
    for (int i : close) {
        next = curr_state;
        next[i] = true;
        nextCost = calculateCostForState(next);
        if (nextCost < best_cost) {
            best_cost = nextCost;
            bestNext = next;
        }
    }

    // close a open facility and open a closed facility
    for (int i : open) {
        next = curr_state;
        next[i] = false;
        for (int j : close) {
            next[j] = true;
            nextCost = calculateCostForState(next);
            if (nextCost < best_cost) {
                best_cost = nextCost;
                bestNext = next;
            }
        }
    }
    return bestNext;
}

// Cost calculating for facility state
double RLS::calculateCostForState(vector<bool>& state){
	bool success = greedyAssignCustomer(state);
	if(success){
		calcluateCost();
		return tempCost;
	}else {
		return (double)INT_MAX;
	}
}

// Greedy assign customers to state, decide state available or not
bool RLS::greedyAssignCustomer(vector<bool>& state){
	usedCapacity.clear();
	for(int i=0; i<fac_num; i++){
		usedCapacity.push_back(0);
		fac_open[i] = false;
	}

	for (int j = 0; j < cus_num; j++) {
        int whichFacility = -1;
        for (int i = 0; i < fac_num; i++) {
            if (state[i]) {
                if (capacity[i] >= demand[j] + usedCapacity[i]) {
                    whichFacility = i;
                    break;
                }
            }
        }

        if (whichFacility == -1) {
            return false;
        }

        for (int i = 0; i < fac_num; i++) {
            if (state[i]) {
                if (usedCapacity[i] + demand[j] <= capacity[i]
                        && assign_cost[i][j] < assign_cost[whichFacility][j]) {
                    whichFacility = i;
                }
            }
        }

        result[j] = whichFacility;
        usedCapacity[whichFacility] += demand[j];
        fac_open[whichFacility] = true;
    }

    return true;
}

void RLS::calcluateCost(){
	int openCost = 0, serveCost = 0;
	for(int i=0; i<fac_num; i++){
		if(fac_open[i]){
			openCost += open_cost[i];
		}
	}

	for(int i=0; i<cus_num; i++){
		serveCost += assign_cost[result[i]][i];
	}

	tempCost = openCost + serveCost;
}

int main(){
	string resultFile = "../result/RLS.txt";
	ofstream out(resultFile.c_str());
	out << "|      | Result | Time(ms) |" << '\n';
	out << "| ---- | ------ | ------- |" << '\n';

	for(int i=1; i<=71; i++){
		string targetName = "p" + to_string(i);

		LARGE_INTEGER cpuFreq;
	    LARGE_INTEGER startTime;
	    LARGE_INTEGER endTime;
	    double runTime=0.0;
	    QueryPerformanceFrequency(&cpuFreq);
		
		RLS rls;
		string targetFile = "../testData/" + targetName;
		string resultP = "../result/RLS/" + targetName;
		ofstream outP(resultP.c_str());

		rls.readFile(targetFile);

		QueryPerformanceCounter(&startTime);
		int result = rls.performRandomLocalSearch();

		QueryPerformanceCounter(&endTime);
	    runTime = (((endTime.QuadPart - startTime.QuadPart) * 1000.0f) / cpuFreq.QuadPart);

		out << "| " << targetName << "    | ";
	    out << result << "    | " << runTime << "    |";
		out << "\n";

		outP << result << "\n";
	    for(int i=0; i<rls.fac_num; i++){
	    	outP << rls.fac_open[i] << " ";
	    }
	    outP << "\n";
	    for(int i=0; i<rls.cus_num; i++){
	    	outP << rls.result[i] << " ";
	    }
		outP << "\n";
	}

	return 0;
}