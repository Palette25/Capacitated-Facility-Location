#include "GreedyAlgorithm.h"

void Greedy::readFile(string filePath){
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

bool sort_customer(customer c1, customer c2){
	return c1.cost < c2.cost;
}

int Greedy::performGreedySearch(){
	int total_cost = 0;
	vector<customer> store;
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

int main(){
	for(int i=1; i<=71; i++){
		string targetName = "p" + to_string(i);
		string resultFile = "../result/Greedy/" + targetName;
		ofstream out(resultFile.c_str());

		Greedy gr;
		string targetFile = "../testData/" + targetName;

		gr.readFile(targetFile);

		int result = gr.performGreedySearch();

	    out << result << "\n";
	    for(int i=0; i<gr.fac_num; i++){
	    	out << gr.fac_open[i] << " ";
	    }
	    out << "\n";
	    for(int i=0; i<gr.cus_num; i++){
	    	out << gr.result[i] << " ";
	    }
		out << "\n";
	}

	return 0;
}