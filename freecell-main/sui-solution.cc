#include "search-strategies.h"
#include <queue>

#include <algorithm>
#include <set>

#include "mem_watch.h"
#include "memusage.h"

#define BFS_MEM_LIMIT_BYTES 50000000 //50MB


std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {

	// OPEN: queue with next states 
	//            S 
	//       A   B  C  D
	// [(S,D),(S,C),(S,B),(S,A)] 
	std::queue<std::vector<SearchAction>> open;

	// Finall solution vector  
	std::vector<SearchAction> solution;

	// init first state 
	SearchState working_state(init_state);
	
	// vector with currently processed actions 	
	std::vector<SearchAction> current_actions;

	// CLOSED 
	std::set<SearchState> closed;

	// look ahead buffer 	
	std::vector<SearchAction> potentional_solution; 

	for (size_t path= 0; ; path++) {

		SearchState working_state(init_state);  
		
		// ... GET CURRENT STATE
		// if que is not empty go to the state otherwise it is first try 
		// This is to way to get to currently procesed node 
		if (!open.empty()){
			current_actions = open.front();
			open.pop();
			for (const SearchAction& action : current_actions) {
				working_state = action.execute(working_state);
			}
		}
		// There is no solution
		else if (open.empty() && path > 0){ 
			return {};
		}
		
		// ... GENERATE NEXT STATES and expand them 
		std::vector<SearchAction> actions = working_state.actions();		
		for (const SearchAction& action: actions) {

			// story whole new path to the queue 
			//  current actions (S,A,B) + actuall == (S,A,B,C)
			potentional_solution = current_actions; 
			potentional_solution.push_back(action);
			
			SearchState temp_state(init_state); 
			for (const SearchAction& action : potentional_solution) {
				temp_state = action.execute(temp_state);
			}
			
			// IF state is already in closed dont expand 
			if (closed.find(temp_state) == closed.end()) {
				
				// Check if currently expanded node is already a solution	
				if (temp_state.isFinal()){
					printf("Closed: %ld Open: %ld\n",closed.size(), open.size() );
					printf("Finnal mem usage %ld KB\n", (getCurrentRSS()/1000)); //TODO REMOVE 
					return potentional_solution;
				}
				
				// store action vector to queue
				closed.insert(temp_state);
				open.push(potentional_solution);
			}

		}

		// mem test // 50MB
		if ((size_t)getCurrentRSS() > mem_limit_ - BFS_MEM_LIMIT_BYTES) {
			printf("Closed: %ld Open: %ld\n",closed.size(), open.size() );
			printf("MEM crash %ld\n KB", (getCurrentRSS()/1000)); //TODO REMOVE 
			return {};
		}
	}

	return {};
}

typedef std::tuple<SearchAction, size_t, SearchState> action_depth_type;
std::vector<action_depth_type> stack;
std::vector<action_depth_type> solution_with_depth;
std::vector<SearchAction> solution;

bool DLS(const SearchState &init_state, size_t max_depth) {

	SearchState working_state(init_state);
	std::cout << "DLS\n";

	while (!stack.empty()) {
		// pop from the stack to variable ad
		action_depth_type ad = stack.back();
		stack.pop_back();

		// pop from solution (there is an old value) until the right depth
		while (!solution_with_depth.empty() && (std::get<1>(solution_with_depth.back()) >= std::get<1>(ad))) {
			std::cout << "popping\n";
			solution_with_depth.pop_back();
			solution.pop_back();
		}

		// add current action to solution
		solution_with_depth.push_back(ad);
		solution.push_back(std::get<0>(ad));

		std::cout << "CURRENT STATE: \n" << working_state << std::endl;
		std::cout << "CURRENT ACTION: " << std::get<0>(ad) << ", depth: " << std::get<1>(ad) << std::endl; 
		
		// set new working state
		working_state = (std::get<0>(ad).execute(std::get<2>(ad)));
		
		// check final state
		if (working_state.isFinal()) {
			// proc tohle nekdy nastane????
			std::cout << "DLS FINAL - TODO remove" << std::endl;
			return true;
		}
		
		// check if the current depth is lower or equal to the max depth
		// if yes, generate new actions from the current state and add them to the stack
		if (std::get<1>(ad) <= max_depth) {	// < nebo <=
			std::cout << "FINALIBEFORE:";
			for (SearchAction &ac: solution) std::cout << ac << ", "; std::cout << "\n";
			// generate actions from the current working state
			std::vector <SearchAction> actions = working_state.actions();
			// execute each action and check if the generated state is not final
			for (SearchAction &action: actions) {
				SearchState generated_state(working_state);
				generated_state = action.execute(generated_state);
				// check if the state is final
				if (generated_state.isFinal()) {
					std::cout << "WORKING_STATE: " << working_state << std::endl;
					std::cout << "GENERATED_STATE: " << generated_state << std::endl;
					std::cout << "FINALIONE... action: " << action << ", AD: (" << std::get<0>(ad) << "," << std::get<1>(ad) << "), ";
					for (SearchAction &ac: solution) std::cout << ac << ", ";
					std::cout << "\n";
					std::cout << "WORKING STATE: \n" << generated_state << std::endl;
					// push the final action to the solution list
					solution.push_back(action);
					std::cout << "FINALIFINAL after push: ";
					for (SearchAction &ac: solution) std::cout << ac << ", "; std::cout << "\n";
					// return success
					return true;
				}
				// the action was not final -> add the action to the stack with depth = current depth + 1
				action_depth_type new_ad(action, std::get<1>(ad)+1, working_state);
				stack.push_back(new_ad);
			}
		}
	}
	std::cout << "DLS FAILED" << std::endl;
	return false;
	
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	std::cout << "==================== NEW PROBLEM ====================\n";
	SearchState working_state(init_state);
	std::cout << "Init state: \n" << working_state << std::endl;

	bool result;
	// run DLS and increment the depth
	for (size_t current_max_depth = 1; current_max_depth <= DepthFirstSearch::depth_limit_; ++current_max_depth) {
		// clear potential solutions
		while(!solution.empty())
			solution.pop_back();

		while(!solution_with_depth.empty()) 
			solution_with_depth.pop_back();

		// get all available actions from init state
		std::vector<SearchAction> actions = working_state.actions();
		std::cout << "97\n";
		SearchState generated_state(working_state);
		// for each action generate a state and check if it is final -> else add to the stack
		for (SearchAction &action: actions) {
			// rozgenerovat a checknout
			generated_state = action.execute(working_state);
			if (generated_state.isFinal()) {
				// return solution
				solution.push_back(action);
				std::cout << "SUCCESS IN IDS\n";
				return solution;
			}
			std::cout << "not final in IDS\n";
			// if not final -> push action to the stack
			action_depth_type ad(action, 1, working_state);
			stack.push_back(ad);
		}
		// run DLS with the current stack and current max depth
		result = DLS(working_state, current_max_depth);
		if (result) {
			std::cout << "SUCCESS" << std::endl;
			return solution;
		} else {
			std::cout << "ELSE..." << std::endl;
			if (stack.empty()) {
				// stack is empty -> end the search with empty solution
				while (!solution.empty())
					solution.pop_back();
				break;
			}
		}
	}

	// final debug prints
	std::cout << "FINAL SOLUTION: ";
	for (SearchAction &action: solution) std::cout << action << ", " << std::endl;

	std::cout << "FINAL SOLUTION_DEPTH: ";
	for (action_depth_type &action: solution_with_depth) std::cout << std::get<0>(action) << " (depth: " << std::get<1>(action) << "), " << std::endl;

	std::cout << "FINAL STATE:\n" << working_state << std::endl;

	std::cout << "END OF IDS\n";

	return solution;
}


double StudentHeuristic::distanceLowerBound(const GameState &state) const {
		// todo distance to next card that can be in home_dest  
		// the further we are the bigger the number is
		
		// Bigger the number -> bigger the penalisation 
		double out = 0; 

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
