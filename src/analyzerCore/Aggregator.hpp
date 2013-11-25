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


#ifndef AGGREGATOR_HPP
#define AGGREGATOR_HPP

/*!
 * \file Aggregator.hpp
 * \brief Aggregator class
 * \author Christophe Haen
 */

#include <syslog.h>

#include "AnalyzerDBAPI.hpp"
#include "MetaAgentPool.hpp"
#include "Coordinator.hpp"
#include <boost/lexical_cast.hpp>
#include "Scheduler.hpp"
#include "RuleManager.hpp"
#include "InteractionManager.hpp"
#include "UserQuestion.hpp"
#include "RecoveryEngine.hpp"
#include "ProbabilityManagerDb.hpp"
#include <set>

typedef std::vector<Coordinator*> vec_coord; //!< typedef for std::vector<Coordinator*>

/*!
 * \class Aggregator
 * \brief The Aggregator class is responsible for maintaining the list of ongoing problems.
 *
 *  The Aggregator class is the first one called by the AnalyzerEngine when there is problems.
 *  It will keep an up to date problem list during the whole recovery process.
 *  It will find the top problems in the list, and send them to the Scheduler.
 *  It will also be responsible for calling the RecoveryEngine.
 *
 *  There should normally be only one instance of the Aggregator.
 */
class Aggregator {
protected:
	AnalyzerDBAPI * m_dbApi; /*!< pointer to the AnalyzerDBAPI */
	MetaAgentPool * m_maPool; /*!< pointer to the MetaAgentPool */
	Scheduler * m_scheduler; /*!< pointer to the Scheduler */
	InteractionManager * m_iManager; /*!< pointer to the InteractionManager */
	RecoveryEngine * m_recoveryEngine; /*!< pointer to the RecoveryEngine */
	RuleManager * m_ruleManager; /*!< pointer to the RuleManager */
	ProbabilityManagerDb * m_pm; /*!< pointer to the ProbabilityManagerDb */

	map<string, Coordinator *> m_problems; /*!< on going problems : <MetaAgent name, pointer to the Coordinator> */
	map<string, Coordinator *> m_undiagnosedProblems; /*!< entities from the ongoing problems that could not be solved */
	map<string, Coordinator *> m_diagnosedProblems; /*!< entities from the ongoing problems that have been solved*/

	MetaAgent * m_respChild; //!< Pointer to the child we consider responsible in the current diagnosis
	vector<Coordinator *> m_impactedCoordinators; //!< List of Coordinator that can be impacted by the responsible agent problem
	Coordinator * m_diagnosedCoordinator; //!< Coordinator that was diagnosed to have the faulty agent. It is used for the dependency rules inference

	bool m_tryingValidRules; /*!< This is set to true when we start using validated rules to diagnose undiagnosed problems */
	bool m_tryingUnvalidRules; /*!< This is set to true when we start using unvalidated rules to diagnose undiagnosed problems. Sounds like a last hope...*/

	bool m_recommendationApplied; /*!< Set by the recoveryEngine. True if the user applied the recommendation, False otherwise */

	bool m_environmentResponsible; /*!< Set to true if the environment (EnvAgent or Attached Coords) are faulty, false otherwise.*/
	bool m_serverDown; /*!< Set to true if the problem comes from a Server down */

	map<string, Coordinator *> m_tryingProblems; /*!< This map contains the Coordinator that are being tried without being reported as having problem.

	 We treat them a special way when updating the problem list; */

//	/*!
//	 *  \brief Finds the Top problems in the ongoing problem list
//	 *
//	 *  This method filters in the ongoing problem lists which one should be addressed.
//	 *	Either we keep the entities the highest in the entity trees, or we keep the lowest ones.
//	 *	I would say that keeping the lowest one makes more sense, but this depends how the monitoring is done.
//	 *
//	 *  \param[in] swap : defines which entities to keep :
//	 *  					* 1 : (default) the lowest in the tree
//	 *  					* -1 : the highest in the tree.
//	 *
//	 *  \return Top coordinators as a map <Coordinator name, pointer to the coordinator>
//	 */
//
//	map<string, Coordinator *> findTopCoordinator(int swap = 1);

	/*!
	 *  \brief Updates the problem list
	 *
	 *  This method requires a reaction from the user or the monitoring system.
	 *  It gets the new list of ongoing problems. It then compares it to the old list and the unsolved problem list:
	 *  	- problems that are in the New list but are not in the Old are ignored for the moment
	 *  	- problems that are in the Old list but are not in the New or Unsolved create new rules (or increment the occurrence if existing)
	 *
	 *	The lists m_problems and m_unsolvedProblems are updated.
	 */
	void updateProblemList();

	/*!
	 *  \brief Transform the string describing the problems in a map of <Coordinator name, pointer to the Coordinator>
	 *
	 *  The string given as input should be list of Coordinator name, separated by semi-column ";" (in principle, it is
	 *  insured by the ProblemQuestion::checkUserAction method, but one never knows, so we are checking it again).
	 *  It decomposes the list and returns a map containing the Coordinator name as key, and a pointer to that
	 *  Coordinator as value.
	 *
	 *	\param[in] problems : list of problems separated with semi-column
	 *
	 *	\return Map containing pointers to the Coordinator, referenced by their name
	 */
	map<string, Coordinator *> parseProblemString(string problems);

	/*!
	 * @brief Analyzes the server's environment
	 *
	 * It analyzes the EnvironmentAgent, as well as all the Coordinators attached to the Servers
	 *
	 * @param coordVec list of Coordinators to analyze
	 * @return true if there is a problem in the environments, false otherwise
	 */
	bool analyzeEnvironments(vector<Coordinator *> coordVec);

	vec_coord followCoordinatorPath(std::vector<int> prefix,
			vec_coord candidates);

	map<string, Coordinator *> findLowestCoordinators();
	map<string, Coordinator *> findRootCoordinators();

public:

	/*!
	 * \brief Constructor
	 *
	 * @param dbApi : pointer to the AnalyzerDBAPI
	 * @param maPool : pointer to the MetaAgentPool
	 * @param scheduler : pointer to the Scheduler
	 * @param ruleManager : pointer to the RuleManager
	 * @param iManager : pointer to the InteractionManager
	 * @param recoveryEngine : pointer to the RecoveryEngine
	 * @param pm : pointer to the ProbabilityManager
	 */
	Aggregator(AnalyzerDBAPI * dbApi, MetaAgentPool * maPool,
			Scheduler * scheduler, RuleManager * ruleManager,
			InteractionManager * iManager, RecoveryEngine * recoveryEngine,
			ProbabilityManagerDb * pm);

	/*!
	 * @fn bool analyze(string problems);
	 * @brief Analyze problems given as argument
	 *
	 * This function is called by the AnalyzerEngine.
	 * It performs the analysis of the problem list given as argument.
	 * It first finds the list of top Coordinator, gives them to the Scheduler,
	 * try to recover the Agent pointed by the Scheduler, updates the problem list, and so on.
	 *
	 * @param problems :  list of problems separated with semi-column
	 * @return	Always false so far
	 */
	bool analyze(string problems);

	/*!
	 * \brief Checks all the Agents
	 *
	 * it performs a complete check of all the Agents using the Analyze algorithm
	 * Thus it is very likely than some agents are not checked
	 *
	 * \return The map associating the name of the coordinator with its problem
	 */
	std::map<std::string, std::string> checkAll();

	/*!
	 * \brief Checks all the Low level Agents
	 *
	 * it performs a complete check of all the Agents
	 *
	 * \return The map associating the name of the coordinator with its problem
	 */
	std::map<std::string, std::string> fullCheck();

};
#endif
