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
	SearchState workingState(init_state);
	
	// vector with currently processed actions 	
	std::vector<SearchAction> currentActions;

	// CLOSED 
	std::set<SearchState> closed;

	// look ahead buffer 	
	std::vector<SearchAction> potentionalSolution; 

	for (size_t path= 0; ; path++) {

		SearchState workingState(init_state);  
		
		// ... GET CURRENT STATE
		// if que is not empty go to the state otherwise it is first try 
		// This is to way to get to currently procesed node 
		if (!open.empty()){
			currentActions = open.front();
			open.pop();
			for (const SearchAction& action : currentActions) {
				workingState = action.execute(workingState);
			}
		}
		// There is no solution
		else if (open.empty() && path > 0){ 
			return {};
		}
		
		// ... GENERATE NEXT STATES and expand them 
		std::vector<SearchAction> actions = workingState.actions();		
		for (const SearchAction& action: actions) {

			// story whole new path to the queue 
			//  current actions (S,A,B) + actuall == (S,A,B,C)
			potentionalSolution = currentActions; 
			potentionalSolution.push_back(action);
			
			SearchState tempState(init_state); 
			for (const SearchAction& action : potentionalSolution) {
				tempState = action.execute(tempState);
			}
			
			// IF state is already in closed dont expand 
			if (closed.find(tempState) == closed.end()) {
				
				// Check if currently expanded node is already a solution	
				if (tempState.isFinal()){
					printf("Closed: %ld Open: %ld\n",closed.size(), open.size() );
					printf("Finnal mem usage %ld KB\n", (getCurrentRSS()/1000)); //TODO REMOVE 
					return potentionalSolution;
				}
				
				// store action vector to queue
				closed.insert(tempState);
				open.push(potentionalSolution);
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

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	
	return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
		// todo distance to next card that can be in home_dest  
		// the further we are the bigger the number is
		
		// Bigger the number -> bigger the penalisation 
		double out = 0; 

		// Get current homeStack
		// based on that find out what card should follow 
    std::array<HomeDestination, nb_homes> homeStacks;
		for (HomeDestination& home: homeStacks){
			if (home.topCard().has_value()){
      	std::cout << "home" << home.topCard().value();
			}
		}
    
		std::array<WorkStack, nb_stacks> workStacks = state.stacks;
		// Get throughout home_dest and count the penalisation 
		for (WorkStack& stack : workStacks) {
			
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
	SearchState workingState(init_state);
	std::vector<SearchAction> currentActions;
	std::set<SearchState> closed;
	std::vector<SearchAction> potentionalSolution; 
	

	compute_heuristic(workingState, *heuristic_);
	

	for (size_t path= 0; ; path++) {

		//printf("heuristic %f ", compute_heuristic(workingState, heuristic));
		
		if (!open.empty()){
			currentActions = open.front();
			open.pop();
			for (const SearchAction& action : currentActions) {
				workingState = action.execute(workingState);
			}
		}
		else if (open.empty() && path > 0){ 
			return {};
		}
		
		std::vector<SearchAction> actions = workingState.actions();		
		for (const SearchAction& action: actions) {

			potentionalSolution = currentActions; 
			potentionalSolution.push_back(action);
			
			SearchState tempState(init_state); 
			for (const SearchAction& action : potentionalSolution) {
				tempState = action.execute(tempState);
			}
			
			if (closed.find(tempState) == closed.end()) {
				
				if (tempState.isFinal()){
					printf("Closed: %ld Open: %ld\n",closed.size(), open.size() );
					printf("Finnal mem usage %ld KB\n", (getCurrentRSS()/1000)); //TODO REMOVE 
					return potentionalSolution;
				}
				closed.insert(tempState);
				open.push(potentionalSolution);
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
