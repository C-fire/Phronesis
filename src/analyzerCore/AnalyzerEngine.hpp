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


#ifndef ANALYZER_ENGINE_H
#define ANALYZER_ENGINE_H

/*!
 * \file AnalyzerEngine.hpp
 * \brief AnalyzerEngine class
 * \author Christophe Haen
 */

#include <iostream>
#include <map>
#include <time.h>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>


#include "SyncManager.hpp"
#include "AnalyzerDBAPI.hpp"
#include "MetaAgentPool.hpp"
#include "ProbabilityManagerDb.hpp"
#include "Aggregator.hpp"
#include "Scheduler.hpp"
#include "RuleManager.hpp"
#include "InteractionManager.hpp"
#include "UserQuestion.hpp"
#include "ServerPool.hpp"
#include "RecoveryEngine.hpp"
#include "VetoManager.hpp"

using namespace std;
extern bool stopReceived; /*!< Boolean used as an exit condition */

/*!
 * \class AnalyzerEngine
 * \brief The AnalyzerEngine class is the entry point for all the analysis purposes.
 *
 *  The AnalyzerEngine has two running mode :
 *  	- The normal one is to run in a separate thread as an infinite loop.
 *  	  It then waits to get informed about new problems. It then starts
 *  	  the Aggregator.
 *  	- The second option is to run only the method checkAll, which checks
 *  	  all the agents, and exit.
 *
 *	The AnalyzerEngine also creates and destroy all the Pools and Managers,
 *	except the AnalyzerDBAPI and the InteractionManager.
 *
 *  There should normally be only one instance of the AnalyzerEngine.
 */
class AnalyzerEngine {

private:

	AnalyzerDBAPI * m_dbAPI; /*!< pointer to the AnalyzerDBAPI */
	MetaAgentPool * m_maPool; /*!< pointer to the MetaAgentPool */
	ProbabilityManagerDb * m_pm; /*!< pointer to the ProbabilityManagerDb */
	Aggregator * m_agg; /*!< pointer to the Aggregator */
	Scheduler * m_scheduler; /*!< pointer to the Scheduler */
	RuleManager * m_ruleManager; /*!< pointer to the RuleManager  */
	InteractionManager * m_iManager; /*!< pointer to the InteractionManager  */
	ServerPool * m_sp; /*!< pointer to the ServerPool */
	RecoveryEngine * m_recoveryEngine; /*!< pointer to the RecoveryEngine*/
	VetoManager * m_vManager; /*!< pointer to the veto Manager */

	int m_replication; /*!< For the statistics */
public:

	/*!
	 *	\brief Constructor
	 *
	 *	The constructor will initialize all the Managers and the Pools,
	 *	except the AnalyzerDBAPI and the InteractionManager which are
	 *	given as attributes.
	 *
	 * @param dbApi pointer to the AnalyzerDBAPI
	 * @param iManager	pointer to the InteractionManager
	 */
	AnalyzerEngine(AnalyzerDBAPI * dbApi, InteractionManager * iManager);

	/*!
	 *	\brief Destructor
	 *
	 *	This will free all the Managers and the Pools,
	 *	except the AnalyzerDBAPI and the InteractionManager.
	 */
	~AnalyzerEngine();


	/*!
	 *	\brief Main method of the thread
	 *
	 *	This function is an infinite loop waiting
	 *	for user's inputs and then triggering the analyze method.
	 *	It exits when the boolean stopReceived is set to true.
	 */
	void run();

	/*!
	 *	\brief Starts the analysis of a problem.
	 *
	 *	This method will first call the Aggregator::analyze method,
	 *	and then call the MetaAgentPool::reinitializeMetaAgents method
	 * @param problem	list of problems separated with semi-column
	 * @return	Always false
	 */
	bool analyze(string problem);

	/*!
	 * \brief Check all the Agents using the analyze method
	 *
	 *	Calls Aggregator::checkAll and then exits the threads.
	 *	It sends to the InterfaceManager a report on the output
	 *	of Aggregator::checkAll
	 *
	 */
	void checkAll();


	/*!
	 * \brief Make a full check of all the agents
	 *
	 *	Calls Aggregator::fullCheck and then exits the threads.
	 *	It sends to the InterfaceManager a report on the output
	 *	of Aggregator::checkAll	 *
	 */
	void fullCheck();
};

#endif
