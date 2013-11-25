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


#ifndef ENVIRONMENT_RECOVERY_HPP
#define ENVIRONMENT_RECOVERY_HPP

/*!
 * \file EnvironmentRecovery.hpp
 * \brief EnvironmentRecovery class
 * \author Christophe Haen
 */

#include <iostream>
#include <sstream>

#include "RecoveryUnit.hpp"
#include "AgentRecovery.hpp"
#include "EnvironmentAgent.hpp"
#include "ProcessAgent.hpp"
#include "FileRecovery.hpp"
#include "ProcessRecovery.hpp"
#include <boost/lexical_cast.hpp>

using namespace std;

/*!
 * \class EnvironmentRecovery
 * \brief Class for the recoveries of the EnvironmentRecovery
 *
 *  It includes all what is necessary to solve all the problems on one particular EnvironmentAgent.
 *  It inherits from AgentRecovery
 */
class EnvironmentRecovery: public AgentRecovery {
protected:
	void analyzeAgent();
	EnvironmentAgent * m_envAgent; /*!< pointer to the EnvironmentAgent we want to work on */
	FileRecovery m_fstabRecovery; /*!< pointer to the FileRecovery taking care of the fstab file */

	//Those 2 vectors and the set are needed to clean the server in case of overload
	set<string> m_pidsToBeKilled; /*!< set of pids to be killed because they provoke an overload of the system*/
	vector<ProcessAgent> m_processAgents; /*!< vector of ProcessAgent that represent processes overloading the system */
	vector<ProcessRecovery> m_processRecoveries; /*!< vector of ProcessRecovery taking care of the processes overloading the system */

	/*!
	 * @brief Returns a string containing the command given a recoveryAction
	 *
	 * @param action recoveryAction to execute
	 * @return The actual command to execute
	 */
	string getCommand(recoveryAction action) const;

public:

	/*!
	 * \brief Base constructor
	 */
	EnvironmentRecovery();

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the EnvironmentRecovery class
	 *
	 *  \param[in] agent : pointer to the EnvironmentAgent we want to work on
	 */
	EnvironmentRecovery(EnvironmentAgent * agent);

	/*!
	 *  \brief Constructor
	 *
	 * Constructor of the EnvironmentRecovery class
	 *
	 * \param[in] agent : pointer to the EnvironmentAgent we want to work on
	 * \param[in] action : recoveryAction that we want to apply to the EnvironmentAgent
	 */
	EnvironmentRecovery(EnvironmentAgent * agent, recoveryAction action);

	/*!
	 * @brief Copy constructor
	 * @param source EnvironmentRecovery to copy
	 */
	EnvironmentRecovery(const EnvironmentRecovery & source);

	using AgentRecovery::operator=;
	/*!
	 * @brief Assignment operator
	 * @param source EnvironmentRecovery to copy
	 * @return Reference to this
	 */
	EnvironmentRecovery & operator=(const EnvironmentRecovery & source);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the FileRecovery class
	 */
	virtual ~EnvironmentRecovery();

};

#endif
