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


#ifndef PROCESS_RECOVERY_HPP
#define PROCESS_RECOVERY_HPP

/*!
 * \file ProcessRecovery.hpp
 * \brief ProcessRecovery class
 * \author Christophe Haen
 */

#include <iostream>

#include "AgentRecovery.hpp"
#include "ProcessAgent.hpp"
#include "FileRecovery.hpp"
#include "FileAgent.hpp"

using namespace std;

/*!
 * \class ProcessRecovery
 * \brief Class for the recoveries of the ProcessAgent
 *
 *  It includes all what is necessary to solve all the problems on one particular ProcessAgent.
 *  It inherits from AgentRecovery
 */
class ProcessRecovery: public AgentRecovery {
protected:
	void analyzeAgent();
	ProcessAgent * m_processAgent; /*!< pointer to the ProcessAgent on which we want to work */
	string getCommand(recoveryAction action) const;

public:
	/*!
	 * \brief Base constructor
	 */
	ProcessRecovery();

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the ProcessRecovery class
	 *
	 *  \param[in] agent : pointer to the ProcessAgent we want to work on
	 */
	ProcessRecovery(ProcessAgent * agent);

	/*!
	 *  \brief Constructor
	 *
	 * Constructor of the ProcessRecovery class
	 *
	 * \param[in] agent : pointer to the ProcessAgent we want to work on
	 * \param[in] action : recoveryAction that we want to apply to the ProcessAgent
	 */
	ProcessRecovery(ProcessAgent * agent, recoveryAction action);

	/*!
	 * @brief Copy constructor
	 * @param source source ProcessRecovery
	 */
	ProcessRecovery(const ProcessRecovery & source);

	using AgentRecovery::operator=;

	/*!
	 * @brief Assignment operator
	 * This operator uses the AgentRecovery::operator=
	 * @param source source ProcessRecovery
	 * @return *this
	 */
	ProcessRecovery & operator=(const ProcessRecovery & source);

	/*!
	 * @brief Destructor
	 */
	virtual ~ProcessRecovery();

};

#endif
