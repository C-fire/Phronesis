/*
Phronesis, a diagnosis and recovery tool for system administrators.
Copyright (C) 2013 Christophe Haen

This file is part of Phronesis.

Phronesis is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
any later version.

Phronesis is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Phronesis. If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef RULE_MANAGER_HPP
#define RULE_MANAGER_HPP

/*!
 * \file RuleManager.hpp
 * \brief RuleManager class
 * \author Christophe Haen
 */

#include "AnalyzerDBAPI.hpp"
#include "MetaAgentPool.hpp"
#include "Coordinator.hpp"
#include "RecoveryUnit.hpp"
#include "InteractionManager.hpp"
//#include "RecoveryRuleInfo.hpp"

/*!
 * @class RuleManager
 * @brief This class is responsible for all the dependencies between MetaAgent
 *
 * This class takes care of the diagnostic dependencies between Coordinator,
 * but also the recovery dependencies.
 *
 */
class RuleManager {

protected:
	AnalyzerDBAPI * m_dbApi; /*!< pointer to the AnalyzerDBAPI */
	MetaAgentPool * m_maPool; /*!< pointer to the MetaAgentPool */
	InteractionManager * m_iManager; /*!< pointer to the InteractionManager  */


	map<pair<string, string>, int> m_recoveryRulesMap; /*!< map which associates every pair (MetaAgent name, recovery action name) with a unique id */
	map<int, pair<string, string> > m_recoveryRulesMapRevert; /*!< map which associates a unique id with a pair (MetaAgent name, recovery action name)  */

	vector<pair<int, int> > m_recoveryRuleAfterTree; /*!< map which represents the tree of 'after' rules : if I do A, I need to do B after */
	vector<pair<int, int> > m_recoveryRuleBeforeTree; /*!< map which represents the tree of 'before' rules : if I do A, I need to do B before */
	vector<pair<int, int> > m_recoveryRuleTree; /*!< map which represents the tree of all the rules, no matter the direction */

	// Those 2 elements are temporary
	// They are reinitialized at every call to RecoveryRuleBFS -> no...
	// and are used as a cache.
	// the value is the DFS starting from the key
	map<int, vector<int> > m_connexBefore; /*!< Cache for the BFS on the Before tree */
	map<int, vector<int> > m_connexAfter; /*!< Cache for the BFS on the After tree */

	map<Coordinator *, int> m_weightMap; /*!< map matching a Coordinator with its weight dependency wise (CAUTION : only to be used internally by sortAccordingRules as a cache) */


	/*!
	 * @brief Returns the weight of a Coordinator
	 *
	 * If the Coordinator is already in the cache (m_weightMap) it is directly returned.
	 * Otherwise, it is computed (according to the Coordinator's dependencies) and added in the cache.
	 *
	 * The weight of a Coordinator is 1 + the weights of all the Coordinators
	 * that depend on it.
	 *
	 * @param a pointer to the Coordinator
	 * @return the weight of the Coordinator
	 */
	int getWeight(Coordinator * a);

	/*!
	 * @brief breadth-first search
	 *
	 * Make a search in the Tree going for breadth first.
	 *
	 * @param start starting node
	 * @param edges the list of edges
	 * @param amountOfNodes how many nodes are in the graph
	 * @param reverseOrder if we should return the result in reverse order (default : false)
	 * @return List of nodes organized in breadth-first order
	 */
	vector<int> BFS(int start, vector<pair<int, int> > & edges,
			int amountOfNodes, bool reverseOrder = false);

	/*!
	 *	@brief Checks if 2 nodes are connected
	 *
	 *	The method is based on the RuleManager::BFS method.
	 *	The cache given as parameter contains previous results.
	 *	If the cache already has the result, then this one is returned.
	 *	Otherwise the BFS is executed, the result stored in the cache and returned
	 *
	 * @param start starting node
	 * @param end ending node
	 * @param edges the list of edges
	 * @param amountOfNodes how many nodes are in the graph
	 * @param cache a reference to the cache to store results for future calls
	 *
	 * @return true if start and end are connected, false otherwise
	 */
	bool isConnected(int start, int end, vector<pair<int, int> > &edges,
			int amountOfNodes, map<int, vector<int> > & cache);

	/*!
	 * @class sortstruct
	 * @brief Internal class for sorting two nodes given as index
	 *
	 * The class is used to sort the vector of RecoveryUnit represented by unique integers
	 *
	 */
	class sortstruct {
	public:
		// sortstruct needs to know its containing object
		RuleManager* m_rm; /*!< pointer to the RuleManager. sortstruct needs to know its containing object */

		/*!
		 * @brief Constructor
		 * @param rm pointer to the RuleManager
		 */
		sortstruct(RuleManager* rm) :
				m_rm(rm) {
		}
		;

		/*!
		 * @brief Method actually doing the sorting.
		 *
		 * We define A < B if either
		 * 	- there is a path from A to B in the after tree
		 * 	- there is a path from B to A in the before tree
		 *
		 * @param i first node
		 * @param j second node
		 * @return true if i < j, false otherwise
		 */
		bool operator()(int i, int j);
	};

public:

	/*!
	 * @brief Constructor
	 *
	 * The constructor builds all the trees and fetch information about them in the database.
	 *
	 * @param dbApi	pointer to the AnalyzerDBAPI
	 * @param maPool pointer to the MetaAgentPool
	 * @param iManager pointer to the InteractionManager
	 */
	RuleManager(AnalyzerDBAPI * dbApi, MetaAgentPool * maPool, InteractionManager * iManager);

	/*!
	 * @brief Add a rule "solving A solved B"
	 *
	 * If the rule is not present in the database, it creates it.
	 * Otherwise it just increment the occurrence counter of this rule.
	 * It does not set it as valid!
	 *
	 * @param a pointer to the first coordinator
	 * @param b pointer to the second coordinator
	 * @return true if the rule was validated, false otherwise
	 */
	bool addSolveRule(Coordinator * a, Coordinator * b); //solving a solved b

	/*!
	 * @brief Sorts a vector of pointers of Coordinator according to their dependency rules
	 *
	 *	The stl::sort algorithm is used with compareCoordinatorWeight as a comparison function
	 *
	 * @param coord list of pointers of Coordinator
	 * @return the sorted vector
	 */
	vector<Coordinator *> sortAccordingDependencies(vector<Coordinator *> coord);


	/*!
	 * @brief from a list of RecoveryUnit, returns a list with all the dependencies, and sorted according to them.
	 *
	 * @param startRules the original rules
	 *
	 * @return sorted vector with all the RecoveryUnit needed to execute the original ones.
	 */
	vector<RecoveryUnit> RecoveryRuleBFS(vector<RecoveryUnit> startRules);

	friend class sortstruct;

	/*!
	 * \brief Returns the predecessors of the Coordinators passed in parameters
	 * This method returns a map of Coordinator indexed by their name. It contains
	 * the predecessors of the Coordinator passed as parameter. The 'predecessor' term
	 * is with respect to the dependency rules.
	 *
	 * @param unsolvedPb List of Coordinators we are interested in
	 * @return List of Coordinators preceding those passed as parameters.
	 */
	std::map<std::string, Coordinator *> getPredecessors(
			std::map<std::string, Coordinator *> & unsolvedPb) const;

	static int dependency_validation_threshold; //!< Threshold after which a rule is automaticaly validated


};

/*!
 * @brief Compares the weight of 2 coordinators
 *
 * This function is used as a comparison function by RuleManager::sortAccordingRules.
 * We are sure that the weight obtained with Coordinator::getWeight are up to date there,
 * because they were just refreshed in RuleManager::sortAccordingRules
 *
 * @param a first Coordinator
 * @param b second Coordinator
 * @return true if the weight of a is smaller than the weight of b. False otherwise.
 */
bool compareCoordinatorWeight(Coordinator * a, Coordinator * b);
#endif
