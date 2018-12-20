#include "TabuSearch.h"

void Tabu::readFile(string filePath){
	ifstream in(filePath.c_str());
	in >> fac_num >> cus_num;
	// Read variables in order
	for(int i=0; i<fac_num; i++){
		int temp_cap, temp_cost;
		in >> temp_cap >> temp_cost;
		capacity.push_back(temp_cap);
		temp_capacity.push_back(temp_cap);
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
		result.push_back(0);
		fea_result.push_back(0);
	}

	for(int i=0; i<fac_num; i++){
		vector<int> temp;
		for(int j=0; j<cus_num; j++){
			temp.push_back(-1);
		}
		tabu_list.push_back(temp);
	}
}

bool sort_customer(customer c1, customer c2){
	return c1.cost < c2.cost;
}

bool sort_move(move_p c1, move_p c2){
	return c1.cost < c2.cost;
}

// Get greedy result as init solution path
int Tabu::performGreedySearch(){
	int total_cost = 0;
	vector<customer> store;
	// Greedy choose facility for every customer
	for(int i=0; i<cus_num; i++){
		store.clear();
		
		for(int j=0; j<fac_num; j++){
			customer target = customer(assign_cost[j][i], j);
			store.push_back(target);
		}
		// Sort customer by their assign cost, keep cost less customer first
		sort(store.begin(), store.end(), sort_customer);
		// Pick facillity in order
		for(int j=0; j<fac_num; j++){
			int fac_id = store[j].id;
			if(capacity[fac_id] >= demand[i]){
				capacity[fac_id] -= demand[i];
				result[i] = fac_id;
				total_cost += store[j].cost;
				// If target facility not open, open it
				if(!fac_open[fac_id]){
					fac_open[fac_id] = true;
					total_cost += open_cost[fac_id];
				}
				break;
			}
		}
	}
	return total_cost;
}

// Perform Tabu Search
int Tabu::performTabuSearch(){
	best_feasible_cost = performGreedySearch();
	best_cost = best_feasible_cost;
	for(int i=0; i<fea_result.size(); i++)
		fea_result[i] = result[i];

	int count = 0;
	double temp_result = 0;
	for(int i=0; i<cus_num*2; i++){
		getNeighborResult(i, 3 + fac_num / 2);
		if(fabs(temp_result - best_cost) < 0.00001){
			count++;
		}else {
			count = 0;
			temp_result = best_cost;
		}
		if(count > fac_num)
			break;
	}
	return best_feasible_cost;
}

// Tabu tools methods
int Tabu::performPenalty(){
	int res = 0;
	vector<int> temp_cap(temp_capacity);
	for(int i=0; i<cus_num; i++){
		temp_cap[result[i]] -= demand[i];
	}
	for(int i=0; i<fac_num; i++){
		// Perform penalty
		if(temp_cap[i] < 0){
			res -= temp_cap[i];
		}
	}
	return res;
}

// Return current cost
int Tabu::getCurrentResult(){
	int res = 0;
	bool open[fac_num];
	for(int i=0; i<fac_num; i++){
		open[i] = false;
	}
	// Create temp records
	for(int i=0; i<cus_num; i++){
		res += assign_cost[result[i]][i];
		open[result[i]] = true;
	}
	// Adding open cost
	for(int i=0; i<fac_num; i++){
		if(open[i]) res += open_cost[i];
	}

	return res;
}

// Get seatch neighbor result
void Tabu::getNeighborResult(int iteration, int beta){
	vector<move_p> path;

	for(int i=0; i<cus_num; i++){
		int temp = result[i];
		for(int j=0; j<fac_num; j++){
			// Swap facility number
			if(temp != j){
				result[i] = j;
				int targetResult = getCurrentResult();
				int penalty = performPenalty();
				bool ability = penalty > 0 ? false : true;
				int cost = targetResult + penalty * alpha;
				move_p neighmove = move_p(i, j, ability, cost);
				// Judge tabu list and neighbor path feasible
				// Tabu enable conditions: Less Penalty, available and cost less, unavailable but cost less than best
				if(tabu_list[j][i] < penalty || (ability && cost < best_feasible_cost) || (!ability && cost < best_cost)){
					path.push_back(neighmove);
				}
			}
		}
		result[i] = temp;
	}
	sort(path.begin(), path.end(), sort_move);

	// Find small cost and enable neighbor
	for(int i=0; i<path.size(); i++){
		if(path[i].feasible){
			if(path[i].cost < best_feasible_cost){
				int origin = result[path[i].src_cus];
				result[path[i].src_cus] = path[i].dst_fac;
				for(int i=0; i<fea_result.size(); i++)
					fea_result[i] = result[i];
				best_feasible_cost = path[i].cost;
				result[path[i].src_cus] = origin;
			}
			break;
		}
	}
	// Update best solution and tabu list
	if(path[0].cost < best_cost)
		best_cost = path[0].cost;

	tabu_list[path[0].dst_fac][path[0].src_cus] = iteration + beta;

	result[path[0].src_cus] = path[0].dst_fac;

	if(path[0].feasible){
		alpha /= (1+epsilon);
	}else {
		alpha *= (1+epsilon);
	}
}

int main(){
	for(int i=1; i<=71; i++){
		string targetName = "p" + to_string(i);
		string resultFile = "../result/Tabu/" + targetName;
		ofstream out(resultFile.c_str());

		Tabu tb;
		string targetFile = "../testData/" + targetName;

		tb.readFile(targetFile);

		int result = tb.performTabuSearch();

	    for(int i=0; i<tb.fac_num; i++){
	    	tb.fac_open[i] = false;
	    }
	    for(int i=0; i<tb.fac_num; i++){
	    	tb.fac_open[tb.fea_result[i]] = true;
	    }

	    out << result << "\n";
	    for(int i=0; i<tb.fac_num; i++){
	    	out << tb.fac_open[i] << " ";
	    }
	    out << "\n";
	    for(int i=0; i<tb.cus_num; i++){
	    	out << tb.result[i] << " ";
	    }
		out << "\n";
	}
}