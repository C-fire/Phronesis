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


#ifndef METAAGENT_POOL_HPP
#define METAAGENT_POOL_HPP

/*!
 * \file MetaAgentPool.hpp
 * \brief MetaAgentPool class
 * \author Christophe Haen
 */

#include <iostream>
#include <map>
#include <vector>

#include "MetaAgent.hpp"
#include "Coordinator.hpp"
#include "Agent.hpp"
#include "ServerPool.hpp"
#include "AnalyzerDBAPI.hpp"
#include "InteractionManager.hpp"
#include "VetoManager.hpp"

using namespace std;

/*!
 * \class MetaAgentPool
 * \brief Keeps all the MetaAgent/Agent/Coordinator in a pool and offer access to them
 *
 *  The MetaAgentPool will retrieve from the database all the MetaAgent, Agent, Coordinator
 *  and store them in a fashionable way.
 *
 *  There should normally be only one instance of the MetaAgentPool.
 */
class MetaAgentPool {

protected:

	AnalyzerDBAPI * m_dbApi; /*!< pointer to the AnalyzerDBAPI */
	ServerPool * m_sm; /*!< pointer to the ServerManager */

	map<string, MetaAgent *> m_metaAgents; /*!< map where the key is the MetaAgent name and the value a pointer to the MetaAgent */
	vector<Agent *> m_agents; /*!< vector of pointer to the Agent */
	vector<Coordinator *> m_coordinators; /*!< vector of pointer to the Coordinator */
	VetoManager * m_vManager; /*!< pointer to the VetoManager */

	/*!
	 * @brief Constructs the MetaAgent tree
	 *
	 * The method gets the tree from the AnalyzerDBAPI::getTree method,
	 * and then calls the MetaAgent::addChild method on the parents.
	 *
	 */
	void buildTree();

	/*!
	 * @brief Attach to the Servers their Attached Coordinators
	 *
	 * Queries the db to ask which Coordinator is attached to which
	 * Server, and then makes the association
	 *
	 */
	void attachCoordinatorsToServers();

public:

	/*!
	 * @brief Constructor
	 *
	 *	The constructor retrieves from the database all the MetaAgent, Agent and Coordinator.
	 *	It also fills in the Coordinator's first level server list with Coordinator::addFirstLvlServer
	 *
	 * @param dbApi	pointer to the AnalyzerDBAPI
	 * @param iManager pointer to the InteractionManager
	 * @param pm pointer to the ProbabilityManager
	 * @param sm pointer to the ServerManager
	 * @param vManager pointer to the VetoManager
	 */
	MetaAgentPool(AnalyzerDBAPI * dbApi, InteractionManager * iManager,
			ProbabilityManager * pm, ServerPool * sm, VetoManager * vManager);

	/*!
	 * @brief Destructor
	 *
	 * Deletes all Agent and Coordinator
	 */
	~MetaAgentPool();

	/*!
	 * @brief Getter for a MetaAgent from it's name
	 *
	 * Returns a pointer to a MetaAgent from it's name
	 *
	 * @param name name of the MetaAgent
	 * @return pointer to the associated MetaAgent
	 */
	MetaAgent * getMetaAgent(string name) const;

	/*!
	 * @brief Getter for an Agent from it's name
	 *
	 * Returns a pointer to an Agent from it's name
	 * @throw string if there is no MetaAgent called that way
	 * @throw string if it is not a Agent
	 *
	 * @param name name of the Agent
	 * @return pointer to the associated Agent
	 */
	Agent * getAgent(string name) const;

	/*!
	 * @brief Getter for an Coordinator from it's name
	 *
	 * Returns a pointer to an Coordinator from it's name
	 * @throw string if there is no MetaAgent called that way
	 * @throw string if it is not a Coordinator
	 *
	 * @param name name of the Coordinator
	 * @return pointer to the associated Coordinator
	 */
	Coordinator * getCoordinator(string name) const;

	/*!
	 *	Returns a vector containing all the Coordinator
	 *
	 * @return Vector of pointer to the Coordinators
	 */
	vector<Coordinator *> getAllCoordinators() const;

	/*!
	 *	Returns a vector containing all the Agents
	 *
	 * @return Vector of pointer to the Agents
	 */
	vector<Agent *> getAllAgents() const;



	/*!
	 * \brief Reinitializes all the MetaAgent
	 *
	 * Goes through all the MetaAgent map and call :
	 * 	- MetaAgent::reinitializeAnalyzeIterations
	 *	- MetaAgent::reinitializeResponsibleChild
	 *	The method is called at the end of an analysis.
	 */
	void reinitializeMetaAgents();

};

#endif
