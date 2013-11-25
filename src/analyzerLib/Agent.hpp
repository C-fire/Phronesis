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


#ifndef AGENT_HPP
#define AGENT_HPP

/*!
 * \file Agent.hpp
 * \brief Agent base class
 * \author Christophe Haen
 *
 */

#include <iostream>
#include <fstream>
#include "MetaAgent.hpp"
#include "RecoveryEngineAbstract.hpp"

using namespace std;

/*! \class Agent
 * \brief Base class for all the agent
 *
 *  The class is the base class for all the real agents (File, Process...).
 *  It inherits from the MetaAgent class.
 *
 *  All the Agent classes have 3 sets of attributes : the expected ones, the real ones, and the booleans set to true
 *  if the expected and real values don't match.
 */
class Agent: public MetaAgent {

protected:

	ostream& printOperator(ostream& out) const;

	Server * m_server; /*!< Pointer to the Server on which the agent is running */

public:
	/*!
	 *  \brief Constructor
	 *
	 *  Base constructor of the Agent class
	 */
	Agent();

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the Agent class
	 *  \param[in] id : integer unique identifier for the agent
	 *  \param[in] name : name of the agent
	 *  \param[in] server : pointer on the server on which the agent is running
	 */
	Agent(int id, string name, Server * server);

	/*!
	 *  \brief Copy constructor
	 *
	 *   Copy constructor of the Agent class
	 *  \param[in] src : source agent that we will copy
	 */
	Agent(const Agent & src);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the Agent class
	 */
	virtual ~Agent();

	/*!
	 *  \brief Analysis of the Agent state
	 *
	 *  This methods will have the agent analyze its state.
	 *  It will compare its expected state with the actual one.
	 *
	 *  \return true if there is a problem, false otherwise
	 */
	virtual bool analyze();

	/*!
	 *  \brief Returns the list of Server related to the Agent
	 *
	 *  The method returns all the Servers that are related one way or another
	 *  to the Agent.
	 *
	 *  \return set of Server pointer
	 */
	virtual set<Server *> getInvolvedServers() const;

	/*!
	 *  \brief Updates the occurrence probabilities
	 *
	 *  The method updates the occurrence probabilities of the children.
	 *  In the Agent class, this method does nothing but has been implemented
	 *  for compilation reasons
	 *
	 */
	virtual void updateProbability();

	/*!
	 * \brief Getter for the Server
	 *
	 * @return The Server on which the Agent runs
	 */
	virtual Server * getServer() const;

	/*!
	 * \brief Setter for the Server
	 *
	 * \param srv Pointer to the Server to assign
	 */
	virtual void setServer(Server * srv);

	using MetaAgent::operator=;
	/*!
	 * @brief assignment operator
	 * This operator uses the MetaAgent assignment operator
	 * @param source the source Agent
	 * @return *this
	 */
	Agent & operator=(const Agent& source);


};

#endif
