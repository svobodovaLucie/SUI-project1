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
		// todo distance to next card that can be in home_dest  
		// the further we are the bigger the number is
		
		// Bigger the number -> bigger the penalisation 
		//double out = 0; 

		// Get current homeStack
		// based on that find out what card should follow 
    std::array<HomeDestination, nb_homes> home_stacks;
		for (HomeDestination& home: home_stacks){
			if (home.topCard().has_value()){
      	std::cout << "home" << home.topCard().value();
			}
		}
    
		std::array<WorkStack, nb_stacks> work_stacks = state.stacks;
		// Get throughout home_dest and count the penalisation 
		for (WorkStack& stack : work_stacks) {
			
    	while (true) {
			  std::optional<Card> card = stack.getCard();
        if (!card.has_value()) {
            break; // Exit the loop when there are no more cards
        }
        std::cout << card.value() << "\n";
   	 }
		 // todo move cards back in order if there is no actuall copy 

		}

		printf("kulo");
    return 1;
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {

	// Same implementation as BFS (documented there)	
	std::queue<std::vector<SearchAction>> open;
	std::vector<SearchAction> solution;
	SearchState working_state(init_state);
	std::vector<SearchAction> current_actions;
	std::set<SearchState> closed;
	std::vector<SearchAction> potentional_solution; 
	

	compute_heuristic(working_state, *heuristic_);
	

	for (size_t path= 0; ; path++) {

		//printf("heuristic %f ", compute_heuristic(workingState, heuristic));
		
		if (!open.empty()){
			current_actions = open.front();
			open.pop();
			for (const SearchAction& action : current_actions) {
				working_state = action.execute(working_state);
			}
		}
		else if (open.empty() && path > 0){ 
			return {};
		}
		
		std::vector<SearchAction> actions = working_state.actions();		
		for (const SearchAction& action: actions) {

			potentional_solution = current_actions; 
			potentional_solution.push_back(action);
			
			SearchState temp_state(init_state); 
			for (const SearchAction& action : potentional_solution) {
				temp_state = action.execute(temp_state);
			}
			
			if (closed.find(temp_state) == closed.end()) {
				
				if (temp_state.isFinal()){
					printf("Closed: %ld Open: %ld\n",closed.size(), open.size() );
					printf("Finnal mem usage %ld KB\n", (getCurrentRSS()/1000)); //TODO REMOVE 
					return potentional_solution;
				}
				closed.insert(temp_state);
				open.push(potentional_solution);
			}

		}

		if ((size_t)getCurrentRSS() > mem_limit_ - BFS_MEM_LIMIT_BYTES) {
			printf("Closed: %ld Open: %ld\n",closed.size(), open.size() );
			printf("MEM crash %ld\n KB", (getCurrentRSS()/1000)); //TODO REMOVE 
			return {};
		}
	}

	return {};
}
