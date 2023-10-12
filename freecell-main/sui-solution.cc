#include "search-strategies.h"
#include "mem_watch.h"
#include "memusage.h"


#include <queue>
#include <algorithm>
#include <set>
#include <iostream>
#include <sstream>

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


double StudentHeuristic::distanceLowerBound(const GameState &state) const {
		std::stringstream ss;
	// ss << state;
  // std::string state_string = ss.str();
	// return std::hash<std::string>{}(state_string);

    // int cards_out_of_home = king_value * colors_list.size();
    // for (const auto &home : state.homes) {
    //     auto opt_top = home.topCard();
    //     if (opt_top.has_value())
    //         cards_out_of_home -= opt_top->value;
    // }

    // return cards_out_of_home;


		// todo distance to next card that can be in home_dest  
		// the further we are the bigger the number is
		
		// Bigger the number -> bigger the penalisation 
		//double out = 0; 

		// Get current homeStack
		// based on that find out what card should follow 
    std::array<HomeDestination, nb_homes> home_stacks;
		for (HomeDestination& home: home_stacks){
			if (home.topCard().has_value()){
      	std::cout << "home " << home.topCard().value();
			}
		}
    
		std::array<WorkStack, nb_stacks> work_stacks = state.stacks;
		// Get throughout home_dest and count the penalisation 
		double result = 0.0;
		for (WorkStack& stack : work_stacks) {
			// std::cout << "work stack " << i++ << ": ";
    	while (true) {
			  std::optional<Card> card = stack.getCard();
        if (!card.has_value()) {
            break; // Exit the loop when there are no more cards
        }
        // std::cout << card.value() << "";
				// std::cout << "(" << king_value << ")";
				ss << card.value();
				std::string card_string = ss.str();
				// printf(" [%c,%c]", card_string[card_string.length() - 2], card_string[card_string.length() - 1]);
				std::stringstream card_value_stream;
				card_value_stream << card_string[card_string.length() - 2];
				
				// save the card value as a number
				unsigned card_value;
				if (card_value_stream.str() == "K") {
					card_value = 13;
				} else if (card_value_stream.str() == "Q") {
					card_value = 12;
				} else if (card_value_stream.str() == "J") {
					card_value = 11;  
				} else {
					card_value_stream >> card_value;
				}
				// printf("[card_value: %u]", card_value);
				// std::cout << ", ";

				// declaration for switch
				std::stringstream hcard;
				std::string home_card_string;
				std::stringstream home_card_value_stream;
				double home_card_value = 0.0;	// default - empty home stack
				// find the right home stack
				switch (card_string[card_string.length() - 1])
				{
				case 's':
					// save the home card value as a number
					if (home_stacks[0].topCard().has_value()) {
						hcard << home_stacks[0].topCard().value();	
					} 
					break;
				case 'h':
					// save the home card value as a number
					if (home_stacks[1].topCard().has_value()) {
						hcard << home_stacks[0].topCard().value();	
					} 
					break;
				case 'd':
					// save the home card value as a number
					if (home_stacks[2].topCard().has_value()) {
						hcard << home_stacks[0].topCard().value();	
					} 
					break;
				case 'c':
					// save the home card value as a number
					if (home_stacks[3].topCard().has_value()) {
						hcard << home_stacks[0].topCard().value();	
					} 
					break;
				default:
					break;
				}
				// asign the value of the card on work stack
				home_card_string = hcard.str();
				if (home_card_string[1] == 'K') {
					home_card_value = 13.0;
				} else if (home_card_string[1] == 'Q') {
					home_card_value = 12.0;
				} else if (home_card_string[1] == 'J') {
					home_card_value = 11.0;  
				} else {
					home_card_value_stream << home_card_string[1];
					home_card_value_stream >> home_card_value;
				}
				// return: 13 - diff
				result = 13 - (card_value - home_card_value);
   	 	}
		 	// todo move cards back in order if there is no actuall copy 
			// std::cout << std::endl;
		}

		// printf("kulo(%.0f), ", result);
    return result;
}

typedef std::tuple<double, std::vector<SearchAction>> rated_actions;
class comparator {
	public:
		bool operator()(const rated_actions &lhs, const rated_actions &rhs) {
			double lhs_fst = std::get<0>(lhs);
			double rhs_fst = std::get<0>(rhs);
			return lhs_fst < rhs_fst;
		}
};

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	// A* uses BFS that is better documented in BreadthFirstSearch::solve 

	std::priority_queue<rated_actions, std::vector<rated_actions>, comparator> pq_open;
	// std::vector<SearchAction>	actions(init_state.actions());
	// tup t(1.2, actions);
	// pq_open.push(t);
	// t = make_tuple(1.8, actions);
	// pq_open.push(t);
	// t = make_tuple(1.0, actions);
	// pq_open.push(t);
	// t = make_tuple(756.8, actions);
	// pq_open.push(t);

	// std::cout << std::get<0>(t) << std::endl;

	// while (!pq_open.empty()) {
	// 	std::cout << std::get<0>(pq_open.top()) << std::endl;
	// 	pq_open.pop();
	// }

	// return {};

	// std::queue<std::tuple<std::vector<SearchAction>, double>> open;
  // std::tuple<std::vector<SearchAction>, double> tuple;
	rated_actions t;
	std::vector<SearchAction> solution;
	std::vector<SearchAction> current_actions;
	std::set<size_t> closed;
	std::vector<SearchAction> temp_actions; 
	std::vector<SearchAction> potential_solution; 
	size_t hash_num = 0;
	bool found_potential = false;
	long unsigned int depth = 0; 
	double heuristic;
	
	SearchState working_state(init_state);
	if (working_state.isFinal()){
		return {};
	}
	std::vector<SearchAction> expand_actions = working_state.actions();
	for (const SearchAction& act : expand_actions) {
		std::vector<SearchAction> action;
		action.push_back(act);
		// tuple = std::make_tuple(action, 0);
		// open.push(tuple);
		// pq
		heuristic = compute_heuristic(working_state, *heuristic_);
		t = make_tuple(heuristic, action);
		pq_open.push(t);
		//
	}
	
	compute_heuristic(working_state, *heuristic_);
	
	// while (!open.empty()) {
	while (!pq_open.empty()) {
		SearchState working_state(init_state);
		// if (!open.empty()){
		// 	tuple = open.front();
		// 	current_actions = std::get<0>(tuple);
		// 	open.pop();
		// 	for (const SearchAction& action : current_actions) {
		// 		working_state = action.execute(working_state);
		// 	}
		// }
		// pq
		if (!pq_open.empty()) {
			t = pq_open.top();
			current_actions = std::get<1>(t);
			pq_open.pop();
			for (const SearchAction &action: current_actions) {
				working_state = action.execute(working_state);
			}
		} 
		//

		if (working_state.isFinal()){
			std::cout << std::endl << "Real solution returned:\n" << working_state << std::endl;
			return current_actions;
		}
		if (current_actions.size() > depth){
			if (found_potential == true){
				std::cout << std::endl << "Potential solution returned:\n" << working_state << std::endl;
				return potential_solution;
			}
			found_potential = false;
			depth++;
		}
		
		expand_actions = working_state.actions();		
		for (const SearchAction& action: expand_actions) {
			temp_actions= current_actions; 
			temp_actions.push_back(action);
			SearchState temp_state(init_state); 
			for (const SearchAction& action : temp_actions) {
				temp_state = action.execute(temp_state);
			}
			hash_num = hash(temp_state);
			if (closed.find(hash_num) == closed.end()) {
				if (found_potential == false){
					if (temp_state.isFinal()){
						potential_solution = temp_actions;
						found_potential = true;
					}
				}
				closed.insert(hash_num);
				// tuple = std::make_tuple(temp_actions, 0);
				// open.push(tuple);
				// pq
				// t = make_tuple(0, temp_actions);
				heuristic = compute_heuristic(working_state, *heuristic_);
				t = std::make_tuple(heuristic, temp_actions);
				pq_open.push(t);
				//
			}
		}
		if ((size_t)getCurrentRSS() > mem_limit_ - BFS_MEM_LIMIT_BYTES) {
			return {};
		}
	}
	return {};
}
