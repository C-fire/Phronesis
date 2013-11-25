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


#ifndef RECOVERY_ENGINE_H
#define RECOVERY_ENGINE_H

/*!
 * \file RecoveryEngine.hpp
 * \brief RecoveryEngine class
 * \author Christophe Haen
 */

#include <iostream>
#include <map>
#include <time.h>
#include <syslog.h>

#include "RecoveryEngineAbstract.hpp"
#include "AnalyzerDBAPI.hpp"
#include "MetaAgentPool.hpp"
#include "RuleManager.hpp"
#include "InteractionManager.hpp"
#include "UserQuestion.hpp"
#include "ServerPool.hpp"
#include "AgentRecovery.hpp"
#include "ProcessRecovery.hpp"
#include "FileRecovery.hpp"
#include "EnvironmentRecovery.hpp"

using namespace std;

/*!
 * @class RecoveryEngine
 * @brief Class in charge of recovering a spotted problem
 *
 *	This class is called by the Aggregator, once a MetaAgent has been found faulty.
 *	The MetaAgent is then passed on to the RecoveryEngine which tries to cure it.
 */
class RecoveryEngine: public RecoveryEngineAbstract {

private:

	AnalyzerDBAPI * m_dbAPI; /*!< pointer to the AnalyzerDBAPI*/
	MetaAgentPool * m_maPool;/*!< pointer to the MetaAgentPool*/
	RuleManager * m_ruleManager;/*!< pointer to the RuleManager*/
	InteractionManager * m_iManager;/*!< pointer to the InteractionManager*/
	ServerPool * m_sm;/*!< pointer to the ServerManager*/
	vector<RecoveryUnit> m_lastProcedure; /*!< the complete last procedure that was suggested. It is used to fill in the history database */

	/*!
	 * @brief Recovers the faulty MetaAgent and its dependencies.
	 *
	 * This method gets all the dependencies that has the RecoveryAgent
	 * thanks to the RuleManager::RecoveryRuleBFS. It then displays the solution..
	 *
	 *
	 * @param ra AgentRecovery of the faulty MetaAgent
	 * @return return always true so far
	 */
	bool recover(AgentRecovery * ra);

public:

	/*!
	 * @brief Constructor
	 *
	 * @param dbApi pointer to theAnalyzerDBAPI
	 * @param iManager pointer to the InteractionManager
	 * @param maPool pointer to the MetaAgentPool
	 * @param sm pointer to the ServerManager
	 * @param rm pointer to the RuleManager
	 */
	RecoveryEngine(AnalyzerDBAPI * dbApi, InteractionManager * iManager,
			MetaAgentPool * maPool, ServerPool * sm, RuleManager * rm);

	/*!
	 * @brief Destructor
	 */
	~RecoveryEngine();

	/*!
	 * @brief Recover a faulty ProcessAgent
	 *
	 *	The method creates a ProcessRecovery object associated
	 *	with the ProcessAgent and calls the RecoveryEngine::recover(AgentRecovery *) method
	 * @param faultyPa the faulty ProcessAgent
	 * @return True if the user applied the suggestions, false otherwise
	 */
	bool recover(ProcessAgent * faultyPa);

	/*!
	 * @brief Recover a faulty FileAgent
	 *
	 *	The method creates a FileRecovery object associated
	 *	with the FileAgent and calls the RecoveryEngine::recover(AgentRecovery *) method
	 * @param faultyFa the faulty FileAgent
	 * @return True if the user applied the suggestions, false otherwise
	 */
	bool recover(FileAgent * faultyFa);

	/*!
	 * @brief Recover a faulty EnvironmentAgent
	 *
	 *	The method creates a EnvironmentRecovery object associated
	 *	with the EnvironmentAgent and calls the RecoveryEngine::recover(AgentRecovery *) method
	 * @param faultyEa the faulty EnvironmentAgent
	 * @return True if the user applied the suggestions, false otherwise
	 */
	bool recover(EnvironmentAgent * faultyEa);

	/*!
	 * @brief Getter for the last procedure
	 * @return Last procedure suggested to the user
	 */
	std::vector<RecoveryUnit> getLastProcedure() const;

};

#endif
