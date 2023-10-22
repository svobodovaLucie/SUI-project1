/*
 * File:         sui-solution.cc
 * Institution:  FIT BUT 2023/2024
 * Course:       SUI - Artificial Intelligence and Machine Learning
 * Authors:      Lucie Svobodová, xsvobo1x@stud.fit.vutbr.cz
 *               Jakub Kuzník, xkuzni04@stud.fit.vutbr.cz
 * 
 * Implementation of BFS, DFS and A* algorithms, and A* heuristic.
 */
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
#define AST_MEM_LIMIT_BYTES 10000000 //10MB

size_t hash(const SearchState &state){
	// creates hash from state as a string
	std::stringstream state_string;
	state_string << state;
	return std::hash<std::string>{}(state_string.str());
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

	//	helper variable 
	std::vector<SearchAction> temp_actions; 

	// look ahead buffer for optimalization
	std::vector<SearchAction> potentional_solution; 
	bool found_potential = false;
	long unsigned int depth = 0; 

	// mem check counter
	char mem_cnt = 1;
	
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
			return current_actions;
		}
		// optimalization 
		if (current_actions.size() > depth){
			if (found_potential == true){
				return potentional_solution;
			}
			found_potential = false;
			depth++;
		}
		
		// GENERATE NEXT STATES and expand them 
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
		if (!(mem_cnt++ % 100)) {
			if ((size_t)getCurrentRSS() > mem_limit_ - BFS_MEM_LIMIT_BYTES) {
				return {};
			}
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

bool next_values_computed = false;
std::string next_cards_string;

void find_next_cards_values(const GameState &state) {
	// find out the next cards values
	std::array<HomeDestination, nb_homes> home_stacks;
	std::vector<int> current_cards_values = {0,0,0,0};
	for (int i = 0; i < 4; i++){
		if (state.homes[i].topCard().has_value()){
			switch (state.homes[i].topCard().value().color) {
				case Color::Heart:
					current_cards_values[0] = state.homes[i].topCard().value().value;
					break;
				case Color::Diamond:
					current_cards_values[1] = state.homes[i].topCard().value().value;
					break;
				case Color::Spade:
					current_cards_values[2] = state.homes[i].topCard().value().value;
					break;
				case Color::Club:
					current_cards_values[3] = state.homes[i].topCard().value().value;
					break;
			}
		}
	}

	std::vector<std::string> colors = {"h","d","s","c"};
	next_cards_string = "";
	// convert vector of integers to a string with incremented values for each color
	for (int i = 0; i < 4; i++) {
		// convert values bigger than 10 to J/Q/K
		if (current_cards_values[i] >= 10) {
			switch (current_cards_values[i]) {
			case 10:
				next_cards_string += "J";
				break;
			case 11:
				next_cards_string += "Q";
				break;
			default:
				next_cards_string += "K";
				break;
			}
		} else {
			// increment the value and append it to the string
			current_cards_values[i]++;
			next_cards_string += std::to_string(current_cards_values[i]);
		}
		// append the color
		next_cards_string += colors[i];
	}
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
	// compute the values of next cards on home stacks
	if (!next_values_computed) {
		find_next_cards_values(state);
		next_values_computed = true;
	}

	std::array<WorkStack, nb_stacks> work_stacks = state.stacks;
	double result = 0.0;
	int cards_found = 0;
	// iterate through the work stacks and find the wanted cards
	for (WorkStack& stack : work_stacks) {
		std::vector<Card> cards_on_stack = stack.storage();
		size_t num_of_cards_on_stack = cards_on_stack.size();
		// check each card on the stack if it is the one to be found
		for (size_t i = 0; i < num_of_cards_on_stack; i++) {
			std::stringstream card_info;
			card_info << cards_on_stack[i];
			size_t found = next_cards_string.find(card_info.str());
			if (found != std::string::npos) {
				// found
				result += num_of_cards_on_stack - i - 1.0;
				if (cards_found >= 3) {
					return result*2;
				}
				cards_found++;
			}
		}
	}
	return result*2;
}

// type used for the priority queue used in A*
typedef std::tuple<double, std::vector<SearchAction>> rated_actions;
class comparator {
	public:
		bool operator()(const rated_actions &lhs, const rated_actions &rhs) {
			double lhs_fst = std::get<0>(lhs);
			double rhs_fst = std::get<0>(rhs);
			return lhs_fst <= rhs_fst;
		}
};


std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	// priority queue used as open
	std::priority_queue<rated_actions, std::vector<rated_actions>, comparator> pq_open;

	// variable used for storing a tuple (heuristic_value, actions)
	rated_actions t;

	// final solution
	std::vector<SearchAction> solution;

	// actions that has led to the current working state
	std::vector<SearchAction> current_actions;

	// closed list
	std::set<size_t> closed;
	size_t hash_num = 0;

	// helper variable
	std::vector<SearchAction> temp_actions; 

	// heuristic variable
	double heuristic;

	// check if the init state is not final
	SearchState working_state(init_state);
	if (working_state.isFinal()){
		return {};
	}

	// mem check counter
	char mem_cnt = 1;

	// initialise the open priority queue
	std::vector<SearchAction> expand_actions = working_state.actions();
	// add each action executable from the init state to the actions
	for (const SearchAction& act : expand_actions) {
		std::vector<SearchAction> action;
		action.push_back(act);
		// compute heuristic for the next state
		SearchState next_state(init_state);
		next_state = act.execute(init_state);
		// check if the next state is final
		if (next_state.isFinal()) {
			std::cout<<"solution: 0"<<std::endl;
			return action;
		}
		heuristic = compute_heuristic(next_state, *heuristic_);
		heuristic = 0 - heuristic - current_actions.size() - 1.0; // delka cesty, heuristiky jsou zaporne
		t = make_tuple(heuristic, action);
		// push the new rated action to the priority queue
		pq_open.push(t);
	}
	// no need to recalculate the card values for current state
	next_values_computed = false;

	// repeat until the queue is not empty or until the solution is found
	while (!pq_open.empty()) {

		// get the top rated actions
		t = pq_open.top();
		pq_open.pop();
		current_actions = std::get<1>(t);

		// execute actions from the init state
		SearchState working_state(init_state);
		for (const SearchAction &action: current_actions) {
			working_state = action.execute(working_state);
		}

		// generate next actions and expand the current path
		expand_actions = working_state.actions();	
		
		// add each action in expand_actions to current_actions
		for (const SearchAction& a: expand_actions) {
			temp_actions = current_actions; 
			temp_actions.push_back(a);
			// execute the next state
			SearchState next_state(init_state); 
			for (const SearchAction& act : temp_actions) {
				next_state = act.execute(next_state);
			}
			// check if the next state is final
			if (next_state.isFinal()) {
					std::cout<<"solution: "<<temp_actions.size()<<std::endl;
					return temp_actions;
			}

			// compute the hash and check closed list
			hash_num = hash(next_state);
			if (closed.find(hash_num) == closed.end()) {	// not found
				// compute heuristic for the next state
				heuristic = compute_heuristic(next_state, *heuristic_);
				heuristic = 0 - heuristic - current_actions.size() - 1.0; // delka cesty
				t = std::make_tuple(heuristic, temp_actions);
				// push the new rated action to the priority queue and to the closed list
				pq_open.push(t);
				closed.insert(hash_num);
			}
		}
		// new state -> need to recompute the card values
		next_values_computed = false;

		// mem test // 10MB
		if (!(mem_cnt++ % 100)) {
			if ((size_t)getCurrentRSS() > mem_limit_ - AST_MEM_LIMIT_BYTES) {
				std::cout<<"solution: not found"<<std::endl;
				return {};
			}
		}
	}
	return {};
}