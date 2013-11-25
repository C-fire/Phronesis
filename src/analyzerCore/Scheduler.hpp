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


#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

/*!
 * \file Scheduler.hpp
 * \brief Scheduler class
 * \author Christophe Haen
 */

#include <assert.h>

#include "Coordinator.hpp"
#include "AnalyzerDBAPI.hpp"
#include "MetaAgentPool.hpp"
#include "RuleManager.hpp"
#include "InteractionManager.hpp"
/*!
 * \class Scheduler
 * \brief The Scheduler class is responsible for analyzing a list of top problem in the correct order and find the faulty one
 *
 *  The Scheduler is called by the Aggregator. It is given a list of top problems,
 *  that it will sort according to dependency rules.
 *  It then calls the MetaAgent::analyze method until it finds a faulty MetaAgent.
 *	When one is found, it gives back the hand to the Aggregator.
 *
 *  There should normally be only one instance of the Scheduler.
 */
class Scheduler {

protected:
	AnalyzerDBAPI * m_dbApi; /*!< pointer to the AnalyzerDBAPI*/
	InteractionManager * m_iManager; /*!< pointer to the InteractionManager */
	MetaAgentPool * m_maPool; /*!< pointer to the MetaAgentPool */
	RuleManager * m_ruleManager; /*!< pointer to the RuleManager */

	map<string, Coordinator *> m_problems; /*!< map of problems. The key is the name of the Coordinator, and the value is pointer to it */
	map<string, Coordinator *> m_undiagnosedProblems; /*!< map of unsolved problems. The key is the name of the Coordinator,
	 and the value is pointer to it. Those Coordinator are those that were analyzed without showing a problems */
	Coordinator * m_diagnosed; /*!< pointer to the Coordinator found faulty */

	/*!
	 * @brief Get the next Coordinator to diagnose
	 *
	 *	The method sorts the problems with RuleManager::sortAccordingRules.
	 *	If several Coordinator have the same minimum weight, we choose a random one.
	 *
	 * @return pointer to the next Coordinator to be analyzed
	 */
	Coordinator * getNext();

	/*!
	 * @brief returns the vector of pointers of Coordinator that are in the problem list
	 *
	 *	This transforms the map m_problems in a vector. It is used by Scheduler::getNext to sort.
	 *
	 * @return vector of pointers of Coordinator
	 */
	vector<Coordinator *> getProblemsVector() const;

public:

	/*!
	 * @brief Coordinator
	 *
	 * @param dbApi pointer to the AnalyzerDBAPI
	 * @param iManager pointer to the InteractionManager
	 * @param maPool pointer to the MetaAgentPool
	 * @param ruleManager pointer to the RuleManager
	 */
	Scheduler(AnalyzerDBAPI * dbApi, InteractionManager * iManager,
			MetaAgentPool * maPool, RuleManager * ruleManager);

	/*!
	 * @brief Analyzes the problems given as parameter
	 *
	 *	Until one faulty MetaAgent is found, the method
	 *	call the Coordinator::analyze method on the
	 *	Coordinator returned by the Scheduler::getNext method.
	 *	If a Coordinator seems healthy, it is added in the
	 *	list of unsolvedProblem, and removed from the problem map.
	 *
	 * @param problems maps of top problems
	 * @return true if a problem was found, false otherwise
	 */
	bool analyze(map<string, Coordinator *> problems);

	/*!
	 * @brief Returns the list of unsolved problems
	 *
	 * The map returned has as key the Coordinator name, and as a value a pointer to it.
	 *
	 * @return map of unsolvedProblems
	 */
	map<string, Coordinator *> getUndiagnosedProblems() const;

	/*!
	 * @brief Returns the faulty Coordinator
	 *
	 * @return a pointer to the Coordinator found as faulty
	 */
	Coordinator * getDiagnosedProblem() const;

};

#endif
