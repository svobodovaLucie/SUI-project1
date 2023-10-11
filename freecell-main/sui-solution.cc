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
					printf("Closed: %ld Open: %ld",closed.size(), open.size() );
					printf(" Finnal mem usage %ld KB", (getCurrentRSS()/1000)); //TODO REMOVE
					printf(" SOLUTION DEPTH %ld\n", potentional_solution.size()); //TODO REMOVE

					return potentional_solution;
				}
				
				// store action vector to queue
				closed.insert(temp_state);
				open.push(potentional_solution);
			}

		}

		// mem test // 50MB
		if ((size_t)getCurrentRSS() > mem_limit_ - BFS_MEM_LIMIT_BYTES) {
			printf("Closed: %ld Open: %ld ",closed.size(), open.size() );
			printf("MEM crash %ld KB\n", (getCurrentRSS()/1000)); //TODO REMOVE 
			return {};
		}
	}

	return {};
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {

	std::vector<std::vector<SearchAction>> open;
	std::vector<SearchAction> current_actions;
	std::vector<SearchAction> actions;
	std::set<SearchState> closed;
	
	// init open 
	SearchState working_state(init_state);
	actions = working_state.actions();
	if (working_state.isFinal()){
		return {};
	}
	std::vector<SearchAction> init_actions;
	for (const SearchAction& act : actions) {
		init_actions = current_actions;
		init_actions.push_back(act);
	}
	
	size_t depth = 1;
	printf("==================== BEGIN =================\n");
	for (;depth <= DepthFirstSearch::depth_limit_; depth++) {
		printf("Depth: %ld open size: %ld closed size: %ld mem: %ld KB\n",depth, open.size(),closed.size(),getCurrentRSS()/1000);
		std::set<SearchState> closed = {};
		open.push_back(init_actions);
		
		while(!open.empty()){
			SearchState working_state(init_state);
			std::vector<SearchAction> current_actions = open.back(); // stack approach 
			open.pop_back();

			// this approach si slower but much more memory efficent 	
			for (const SearchAction& action : current_actions) {
				working_state = action.execute(working_state);
			}
			if (working_state.isFinal()){
				printf("=========  solution find DEPTH: %ld ============= \n\n", current_actions.size());
				return current_actions;
			}
			if (current_actions.size() < depth){
				actions = working_state.actions();
				for (const SearchAction& act : actions) {
					std::vector<SearchAction> new_actions = current_actions;
					new_actions.push_back(act);
					open.push_back(new_actions);
				}
			}
			// mem test // 50MB
			if ((size_t)getCurrentRSS() > mem_limit_ - BFS_MEM_LIMIT_BYTES) {
				printf("closed: %ld Open: %ld\n",closed.size(), open.size() );
				printf("MEM crash %ld KB\n", (getCurrentRSS()/1000)); //TODO REMOVE 
				return {};
			}
		}
		// clean and iterate 
		std::vector<SearchAction>().swap(current_actions);
	
	}
	printf("==================== Depth limit %ld ====================\n\n", depth-1);
	return {};
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
