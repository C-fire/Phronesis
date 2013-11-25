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


#ifndef RECOVERY_ENGINE_ABSTRACT_HPP
#define RECOVERY_ENGINE_ABSTRACT_HPP

/*!
 * \file RecoveryEngineAbstract.hpp
 * \brief Abstract class to expose interface to the Agents, and implement the Inversion Of Control pattern
 * \author Christophe Haen
 */

#include "MetaAgent.hpp"

class Agent;
class ProcessAgent;
class FileAgent;
class EnvironmentAgent;

/*!
 * @class RecoveryEngineAbstract
 * @brief Abstract Class to expose the RecoveryEngine functionalities to the Agent
 *
 */
class RecoveryEngineAbstract {

public:
	/*!
	 * @brief Constructor
	 */
	virtual ~RecoveryEngineAbstract() {
	}
	;

	/*!
	 * @brief Recover faulty ProcessAgent (see RecoveryEngine)
	 * @param faultyPa Faulty ProcessAgent
	 * @return True if the user applied the suggestions, false otherwise
	 */
	virtual bool recover(ProcessAgent * faultyPa) = 0;

	/*!
	 * @brief Recover faulty FileAgent (see RecoveryEngine)
	 * @param faultyFa Faulty FileAgent
	 * @return True if the user applied the suggestions, false otherwise
	 */
	virtual bool recover(FileAgent * faultyFa) = 0;

	/*!
	 * @brief Recover faulty EnvironmentAgent (see RecoveryEngine)
	 * @param faultyEa Faulty EnvironmentAgent
	 * @return True if the user applied the suggestions, false otherwise
	 */
	virtual bool recover(EnvironmentAgent * faultyEa) = 0;
};

#endif
