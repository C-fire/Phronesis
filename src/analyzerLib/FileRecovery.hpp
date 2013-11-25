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


#ifndef FILE_RECOVERY_HPP
#define FILE_RECOVERY_HPP

/*!
 * \file FileRecovery.hpp
 * \brief FileRecovery class
 * \author Christophe Haen
 */

#include <iostream>

#include "RecoveryUnit.hpp"
#include "AgentRecovery.hpp"
#include "FileAgent.hpp"

using namespace std;

/*!
 * \class FileRecovery
 * \brief Class for the recoveries of the FileAgent
 *
 *  It includes all what is necessary to solve all the problems on one particular FileAgent.
 *  It inherits from AgentRecovery
 */
class FileRecovery: public AgentRecovery {
protected:
	void analyzeAgent();

	FileAgent * m_fileAgent; /*!< pointer to the FileAgent on which we want to work */
	string getCommand(recoveryAction action) const;

public:
	/*!
	 * \brief Base constructor
	 */
	FileRecovery();

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the FileRecovery class
	 *
	 *  \param[in] agent : pointer to the FileAgent we want to work on
	 */
	FileRecovery(FileAgent * agent);

	/*!
	 *  \brief Constructor
	 *
	 * Constructor of the FileRecovery class
	 *
	 * \param[in] agent : pointer to the FileAgent we want to work on
	 * \param[in] action : recoveryAction that we want to apply to the FileAgent
	 */
	FileRecovery(FileAgent * agent, recoveryAction action);

	/*!
	 * @brief Copy constructor
	 * @param source source FileRecovery
	 */
	FileRecovery(const FileRecovery & source);

	using AgentRecovery::operator=;
	/*!
	 * @brief Assignment operator
	 * This operator uses the AgentRecovery::operator=
	 * @param source source FileRecovery
	 * @return *this
	 */
	FileRecovery & operator=(const FileRecovery & source);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the FileRecovery class
	 */
	virtual ~FileRecovery();

};

#endif
