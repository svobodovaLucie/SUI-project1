#include "search-strategies.h"
#include "mem_watch.h"
#include "memusage.h"


#include <queue>
#include <algorithm>
#include <set>
#include <iostream>
#include <sstream>
#include <string>

#define BFS_MEM_LIMIT_BYTES 50000000 //50MB

size_t hash(const SearchState &state){

	std::stringstream ss;
	ss << state;
  std::string state_string = ss.str();
	return std::hash<std::string>{}(state_string);

}

std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {

	// OPEN: queue with next states 
	//            S 
	//       A   B  C  D
	// [(S,D),(S,C),(S,B),(S,A)] 
	std::queue<std::vector<SearchAction>> open;

	// Final solution vector  
	std::vector<SearchAction> solution;

	// vector with currently processed actions 	
	std::vector<SearchAction> current_actions;

	// CLOSED 
	std::set<size_t> closed;
	size_t hash_num = 0;

	// 	helping variable 
	std::vector<SearchAction> temp_actions; 

	// look ahead buffer for optimalization
	std::vector<SearchAction> potentional_solution; 
	bool found_potential = false;
	long unsigned int depth = 0; 
	
	// init open 
	SearchState working_state(init_state);
	if (working_state.isFinal()){
		return {};
	}
	// first expand 
	std::vector<SearchAction> expand_actions = working_state.actions();
	for (const SearchAction& act : expand_actions) {
		std::vector<SearchAction> action;
		action.push_back(act);
		open.push(action);
	}

	// ends if there is no solution 
	while (!open.empty()) {

		SearchState working_state(init_state);
		// Get to currently procesed node 
		if (!open.empty()){
			current_actions = open.front();
			open.pop();
			for (const SearchAction& action : current_actions) {
				working_state = action.execute(working_state);
			}
		}
		// Check if currently expanded node is already a solution	
		if (working_state.isFinal()){
			std::cout << std::endl << "Real solution returned in depth: " << current_actions.size() << "\nfinal_state:\n" << working_state << std::endl;
			return current_actions;
		}
		// optimalization 
		if (current_actions.size() > depth){
			if (found_potential == true){
				std::cout << "Potential solution found in depth: " << current_actions.size() << std::endl;
				return potentional_solution;
			}
			found_potential = false;
			depth++;
		}
		
		// ... GENERATE NEXT STATES and expand them 
		expand_actions = working_state.actions();		
		for (const SearchAction& action: expand_actions) {
			//  current actions (S,A,B) + actuall == (S,A,B,C)
			temp_actions= current_actions; 
			temp_actions.push_back(action);
			
			SearchState temp_state(init_state); 
			for (const SearchAction& action : temp_actions) {
				temp_state = action.execute(temp_state);
			}
			hash_num = hash(temp_state);
			// IF state is already in closed dont expand 
			if (closed.find(hash_num) == closed.end()) {
				// optimalization - if currently expanded node is already a solution	
				if (found_potential == false){
					if (temp_state.isFinal()){
						potentional_solution = temp_actions;
						found_potential = true;
					}
				}
				// store action vector to queue
				closed.insert(hash_num);
				open.push(temp_actions);
			}
		}
		// mem test // 50MB
		if ((size_t)getCurrentRSS() > mem_limit_ - BFS_MEM_LIMIT_BYTES) {
			return {};
		}
	}
	return {};
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {

	// OPEN: queue with next states 
	//            S 
	//       A   B  C  D
	// [(S,D),(S,C),(S,B),(S,A)] 
	std::vector<std::vector<SearchAction>> open;

	// One vector of actions that is currently being processed 
	std::vector<SearchAction> current_actions;

	// Temp variable for expanding  
	std::vector<SearchAction> actions;

	// init open 
	SearchState working_state(init_state);
	actions = working_state.actions();
	if (working_state.isFinal()){
		return {};
	}
	// Expand first state 
	for (const SearchAction& act : actions) {
		std::vector<SearchAction> action;
		action.push_back(act);
		open.push_back(action);
	}

	while(!open.empty()){
		SearchState working_state(init_state); // set to first state 
	
		// stack approach 
		std::vector<SearchAction> current_actions = open.back(); 
		open.pop_back();

		// this approach si slower but much more memory efficent 	
		for (const SearchAction& action : current_actions) {
			working_state = action.execute(working_state);
		}
		if (working_state.isFinal()){
			return current_actions;
		}
		// THIS ESTABLISHES THE MAX HEIGHT 
		if (current_actions.size() < (long unsigned int)DepthFirstSearch::depth_limit_){
			actions = working_state.actions();
			for (const SearchAction& act : actions) {
				std::vector<SearchAction> new_actions = current_actions;
				new_actions.push_back(act);
				open.push_back(new_actions);
			}
		}
		// mem test // 50MB
		if ((size_t)getCurrentRSS() > mem_limit_ - BFS_MEM_LIMIT_BYTES) {
			return {};
		}
	}
	
	return {};
}

double ooo = 0.001;

bool next_values_computed = false;
std::vector<std::string> next_cards = {"0","0","0","0"};

std::vector<std::string> find_next_cards_values(const GameState &state) {
	// std::vector<std::string> 
	next_cards = {"0", "0", "0", "0"};	// h,d,s,c
	// std::vector<int> next_cards_numbers = {0,0,0,0};	// h,d,s,c

	// find out the next cards values
	std::array<HomeDestination, nb_homes> home_stacks;
	// std::cout << "kks: " << state.homes[3].topCard().value() << std::endl;
	// initialize the homes with 0
	// next_cards = {0h, 0d, 0s, 0c};
	for (int i = 0; i < 4; i++){
		if (state.homes[i].topCard().has_value()){
			std::stringstream card_value_str;
			card_value_str << state.homes[i].topCard().value();
			std::string without_last_char = card_value_str.str();	// for storing the number onlu
			without_last_char.pop_back();
			switch (card_value_str.str().back()) {
				case 'h':				
					next_cards[0] = without_last_char;
					break;
				case 'd':				
					next_cards[1] = without_last_char;
					break;
				case 's':				
					next_cards[2] = without_last_char;
					break;
				default:
					next_cards[3] = without_last_char;
					break;
			}
			// next_cards[i] = card_value_str.str();
		}
	}
	
	// next_cards = {0h,1d,2s,0c}	// example
	int val;
	for (int i = 0; i < 4; i++) {
			try {
				val = stoi(next_cards[i]);
				if (val == 11) // 10 -> J
					next_cards[i] = "J";
				else {
					val++;
					next_cards[i] = std::to_string(val);
				}
			} catch(const std::exception& e) {
				std::string x = next_cards[i];
				// std::cout << "bef x: " << x << std::endl;
				x.pop_back();
				// std::cout << "aft x: " << x << std::endl;
				if (x == "J") // J -> Q
					next_cards[i] = "Q";
				else	// Q, K -> K
					next_cards[i] = "K";
			}
			// std::cout << "string: " << next_cards[i] << ", int: " << val << std::endl;
		}

		// add the h,d,c,s
		next_cards[0].append("h");
		next_cards[1].append("d");
		next_cards[2].append("s");
		next_cards[3].append("c");

		// std::cout << "to be found in state: " << std::endl << state << "cards to be found: ";
		// for (int i = 0; i < 4; i++) {
		// 	std::cout << next_cards[i] << ", ";
		// }
		// std::cout << std::endl << std::endl;

	return next_cards;

}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {

	// compute the values of next cards on home stacks
	if (!next_values_computed) {
		next_cards = find_next_cards_values(state);
		next_values_computed = true;
	}

	// std::cout << "Next Cards Values: " << std::endl;
	std::stringstream next_cards_string;
	for (int i = 0; i < 4; i++) {
		// std::cout << next_cards[i] << ", ";
		next_cards_string << next_cards[i];
	}

	// go through the work stacks and find the cards
		std::stringstream ss;
		std::array<WorkStack, nb_stacks> work_stacks = state.stacks;
		// Get throughout home_dest and count the penalisation 
		double result = 0.0;
		for (WorkStack& stack : work_stacks) {
			// std::cout << "work stack " << i++ << ": ";
			std::vector<Card> cards_on_stack = stack.storage();
			size_t num_of_cards_on_stack = cards_on_stack.size();
			std::stringstream cards_on_work_stack;
			for (size_t i = 0; i < num_of_cards_on_stack; i++) {
				Card card = cards_on_stack[i]; 
				std::stringstream card_info;
				card_info << card;
				cards_on_work_stack << card;
				int card_value = card.value;

				size_t found = next_cards_string.str().find(card_info.str());
				if (found != std::string::npos) {
					// found
					// std::cout << "FOUND! i=" << i << ",card on work stack: " << card_info.str()
					// << ", cards to find: " << next_cards_string.str() << std::endl; 
					double manhattan_diff = num_of_cards_on_stack - i - 1.0;
					// std::cout << "manhattan_diff: " << manhattan_diff << std::endl;
					result += manhattan_diff;
				}
				// std::cout << "card value: " << card_value << ", sss: " << sss.str() << std::endl;
			}
			// std::cout << "cards_on_work_stack: " << cards_on_work_stack.str() << std::endl;
			// if (num_of_cards_on_stack > 0) {
			// 	std::cout << "top card: " << stack.storage().back() << std::endl;
			// }

		}

		// std::cout << "heuristic: " << result << std::endl;
		// result = 0;
		return result/2.0;
}
typedef std::tuple<double, std::vector<SearchAction>> rated_actions;
class comparator {
	public:
		bool operator()(const rated_actions &lhs, const rated_actions &rhs) {
			double lhs_fst = std::get<0>(lhs);
			double rhs_fst = std::get<0>(rhs);
			// std::cout << "got lhs_fst: " << lhs_fst << ", rhs_first: " << rhs_fst << std::endl;
			// return lhs_fst <= rhs_fst;	// <= zajisti, ze se ukladaji prvky jako fronta a ne stack
			return lhs_fst <= rhs_fst;
		}
};

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	// A* uses BFS that is better documented in BreadthFirstSearch::solve 

	// priority queue used as open
	std::priority_queue<rated_actions, std::vector<rated_actions>, comparator> pq_open;

	// variable used for storing a tuple (heuristic_value, actions)
	rated_actions t;

	// final solution
	std::vector<SearchAction> solution;

	// actions that has led to the current working state
	std::vector<SearchAction> current_actions;

	// closed - is it valid to use it with A*? TODO
	// closed pouzijeme tak, ze si do closed budeme ukladat hash+hodnotu, ktera je ulozena 
	// 
	std::set<size_t> closed;
	size_t hash_num = 0;

	// helper variable
	std::vector<SearchAction> temp_actions; 

	// vector for storing the values of nearest cards to be pushed to home stacks
	std::vector<std::string> next_cards_values;

	// heuristic variable
	double heuristic;
	// look ahead buffer - I think it is not valid to use it in A*, 
	// but maybe for our simle heuristics it is ok TODO
	// std::vector<SearchAction> potential_solution; 
	// bool found_potential = false;
	// long unsigned int depth = 0; 
	
	// check if the init state is not final
	SearchState working_state(init_state);
	if (working_state.isFinal()){
		return {};
	}

	// std::cout << "3";
	// init the open priority queue
	std::vector<SearchAction> expand_actions = working_state.actions();
	for (const SearchAction& act : expand_actions) {
		std::vector<SearchAction> action;
		action.push_back(act);
		// compute heuristic for the next state
		SearchState next_state(init_state);
		next_state = act.execute(init_state);
		// check if the next state is final
		if (next_state.isFinal()) {
			std::cout << "Potential solution found in depth: " << current_actions.size() + 1 << std::endl;
			// TODO maybe add to potential solution
		}
		// std::cout << next_state << std::endl;
		heuristic = compute_heuristic(next_state, *heuristic_);
		heuristic = 0 - heuristic - current_actions.size() - 1.0; // delka cesty
		t = make_tuple(heuristic, action);
		// add the new rated action to the priority queue
		pq_open.push(t);
	}
	next_values_computed = false;

	// std::cout << std::endl << pq_open.size() << std::endl;

	int i = 0;
	// repeat until the queue is not empty
	while (!pq_open.empty()) {
		// if ( i++ > 3 ) return {};
	
		// get the top rated actions
		t = pq_open.top();
		pq_open.pop();
		current_actions = std::get<1>(t);

		// std::cout << "pq size: " << pq_open.size() << ", current depth: " << current_actions.size() << std::endl;

		// execute actions from the init state
		SearchState working_state(init_state);
		
		for (const SearchAction &action: current_actions) {
			working_state = action.execute(working_state);
		}
			
		// check if the current state is final
		if (working_state.isFinal()){
			std::cout << "Real solution returned in depth: " << current_actions.size() << std::endl;
			// std::cout << "final_state:\n" << working_state << std::endl;
			return current_actions;
		}
		
		// generate next actions and expand the current path
		expand_actions = working_state.actions();	
		
		// add each action in expand_actions to current_actions+action vector and push it to the open queue
		for (const SearchAction& a: expand_actions) {
			temp_actions = current_actions; 
			temp_actions.push_back(a);

			// execute the next state
			SearchState next_state(init_state); 
			for (const SearchAction& act : current_actions) {
				next_state = act.execute(next_state);
			}
			if (next_state.isFinal()) {
					std::cout << "Potential solution found in depth: " << current_actions.size() + 1 << std::endl;
					// TODO maybe add to potential solution
					// return {};
			}

			// compute heuristic for the next state
			// std::cout << next_state << std::endl;
			// std::cout << next_state << std::endl;
			heuristic = compute_heuristic(next_state, *heuristic_);
			heuristic = 0 - heuristic - current_actions.size() - 1.0; // delka cesty
			t = std::make_tuple(heuristic, temp_actions);
			// add the new rated action to the priority queue
			pq_open.push(t);	// TODO dela problem s assertem
			// std::cout << "y";
		}

		/*
		std::priority_queue<rated_actions, std::vector<rated_actions>, comparator> pq_tmp;
		pq_tmp = pq_open;
		std::cout << "pq before: \n";
		int l = 0;
		while (!pq_tmp.empty()) {
		// while (l < 2) {
			rated_actions t = pq_tmp.top();
			std::cout << std::get<0>(t) << ", ";
			pq_tmp.pop();
			l++;
		}
		std::cout << std::endl;
		*/
		// }
		next_values_computed = false;
	}
	
	if ((size_t)getCurrentRSS() > mem_limit_ - BFS_MEM_LIMIT_BYTES) {
		return {};
	}
	return {};
}
	// return {};
// }
