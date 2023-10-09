#include "search-strategies.h"
#include <queue>

#include "mem_watch.h"
#include "memusage.h"


std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {

	// OPEN: queue with next states 
	//            S 
	//       A   B  C  D
	// [(S,D),(S,C),(S,B),(S,A)] 
	std::queue<std::vector<SearchAction>> actionsQueue;

	// Finall solution vector  
	std::vector<SearchAction> solution;

	// init first state 
	SearchState workingState(init_state);
	
	// vector with currently processed actions 	
	std::vector<SearchAction> currentActions;

	// CLOSED 
	std::vector<SearchState> closed;

	for (size_t path= 0; ; ++path) {
		
		SearchState workingState(init_state);  // TODO maybe there is something more optimal
		
		// ... GET CURRENT STATE
		// if que is not empty go to the state otherwise it is first try 
		// This is to way to get to currently procesed node 
		if (!actionsQueue.empty()){
			currentActions = actionsQueue.front();
			actionsQueue.pop();
			for (const SearchAction& action : currentActions) {
				workingState = action.execute(workingState);
			}
		}
		
		// ... GENERATE NEXT STATES 
		std::vector<SearchAction> actions = workingState.actions();		
		for (const SearchAction& action: actions) {

			// story whole new path to the queue 
			//  current actions (S,A,B) + actuall == (S,A,B,C)
			std::vector<SearchAction> potentionalSolution = currentActions; 
			potentionalSolution.push_back(action);
			
			SearchState tempState(init_state); // TODO maybe there is something more optimal 
			for (const SearchAction& action : potentionalSolution) {
				tempState = action.execute(tempState);
			}

			// Check if currently expanded node is already a solution	
			if (tempState.isFinal()){
				printf("Finnal mem usage %ld\n", getCurrentRSS());
				return potentionalSolution;
			}

//			// CLOSED 
//    	if (std::find(closed.begin(), closed.end(), tempState) != v.end()) {
//        std::cout << "Element found";
//    	}
//			tempState = action.execute(tempState);
			
			// store action vector to queue 
			actionsQueue.push(potentionalSolution);

			// mem test 
			if ((size_t)getCurrentRSS() > mem_limit_ - 10000) {
				return {};
			}
		}
	}

	return {};
}

std::vector<SearchAction> DepthFirstSearch::solve(const SearchState &init_state) {
	return {};
}

double StudentHeuristic::distanceLowerBound(const GameState &state) const {
    return 0;
}

std::vector<SearchAction> AStarSearch::solve(const SearchState &init_state) {
	return {};
}
