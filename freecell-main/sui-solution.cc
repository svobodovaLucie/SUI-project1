#include "search-strategies.h"
#include <queue>

#include "mem_watch.h"
#include "memusage.h"


std::vector<SearchAction> BreadthFirstSearch::solve(const SearchState &init_state) {

	// queue with next states 
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

	for (size_t path= 0; ; ++path) {
    

		
//		std::cout << "\n--------------\n";
		// ... GET CURRENT STATE
		// if que is not empty go to the state otherwise it is first try 
		// This is to way to get to currently procesed node 
		SearchState workingState(init_state);
		SearchAction currentAction = currentActions[0];

		if (!actionsQueue.empty()){
			currentActions = actionsQueue.front();
			actionsQueue.pop();
//			for (const SearchAction& action : currentActions) {
//				std::cout << "akce: " << action << "\n";
//			}
			for (const SearchAction& action : currentActions) {
//				std::cout << "ws: " << workingState << "\n";
				workingState = action.execute(workingState);
				currentAction = action;
				if (workingState.isFinal()){
//					std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
//					std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n";
					return currentActions;
				}
				
			}
		}
		
		// ... GENERATE NEXT STATES 
		std::vector<SearchAction> actions = workingState.actions();		
//		printf("pocet akcii %d/n", actions.size());
		for (const SearchAction& action: actions) {
			// story whole new path to the queue 
			//  current actions (S,A,B) + actuall == (S,A,B,C)
			std::vector<SearchAction> potentionalSolution = currentActions; 
			potentionalSolution.push_back(action);

			// store action vector to queue 
			actionsQueue.push(potentionalSolution);
			auto mem = getCurrentRSS();
			if (mem > mem_limit_ - 100000) {
				return {};
		}
		}
	}

	/*
	actionsQueue.push();
	actionsQueue.size();
	actionsQueue.front();
	actionsQueue.back();
	*/
	
	// actionsQueue.pop();

	// TODO 
	// mem check 
	// memusage.h:getCurrentRSS()

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
