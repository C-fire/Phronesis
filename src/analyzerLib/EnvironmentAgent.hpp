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


#ifndef ENVIRONMENT_AGENT_HPP
#define ENVIRONMENT_AGENT_HPP

/*!
 * \file EnvironmentAgent.hpp
 * \brief EnvironmentAgent class
 * \author Christophe Haen
 */

#include <iostream>
#include <boost/lexical_cast.hpp>

#include "Agent.hpp"
#include "FileAgent.hpp"
#include "Server.hpp"

class Server;

using namespace std;

/*!
 * \class EnvironmentAgent
 * \brief Class for all the environment concerns
 *
 *  The class is responsible for checking the health of an environment for a given Server (CPU, MEM, Disk space...)
 *  It inherits from the Agent class.
 */
class EnvironmentAgent: public Agent {
protected:

	FileAgent * m_fstab; /*!< Pointer to the FileAgent responsible for checking the fstab file (/etc/fstab) */

	float m_maxLoad; /*!< Maximum load allowed */
	float m_maxMemory; /*!< Maximum memory usage allowed */
	float m_maxSwap; /*!< Maximum swap usage allowed */

	map<string, float> m_true_diskSpace; /*!< Measured free disk space (in %) of the different partitions */
	map<string, float> m_true_inodes; /*!< Measured free inodes (in %) of the different partitions */
	float m_true_load; /*!< measured CPU load */
	float m_true_mem; /*!< Measured free memory (in %) */
	float m_true_swap; /*!< Measured free swap (in %) */

	vector<triplet<string, float, string> > m_cpuConsumers; /*!< Vector of triplets(pid, cpu consumption, process name) which represents the top CPU consumers*/
	vector<triplet<string, float, string> > m_memConsumers; /*!< Vector of triplets(pid,  memory consumption, process name) which represents the top memory consumers*/

	bool m_diskSpace_pb; /*!< Boolean true if there is a problem with a disk space on one or more partitions */
	map<string, bool> m_diskSpace_pb_detail; /*!< map(partition name, boolean) set to true if the concerned partition has disk space problem */
	bool m_mem_pb; /*!< Boolean true if there is a problem with the memory consumption */
	bool m_inode_pb; /*!< Boolean true if there is a problem with a inodes on one or more partitions */
	map<string, bool> m_inode_pb_detail; /*!< map(partition name, boolean) set to true if the concerned partition has inodes problem */
	bool m_load_pb; /*!< Boolean true if there is a problem with the cpu load*/
	bool m_swap_pb; /*!< Boolean true if there is a problem with the swap space */
	bool m_fstab_pb; /*!< Boolean true if there is a problem with the fstab file */

	vector<string> m_fileSystemToIgnore; /*!< List of FS to ignore when checking them (space, inodes..) */

	bool m_pingable; /*!< true if the server is pingable */
	bool m_pingTested; /*!< true if it was tested during this round of analysis */
	bool m_agentRunning; /*!< true if the phronesisAgent is running on it */
	bool m_agentTested; /*!< true if it was tested during this round of analysis */
	bool m_environment_pb; /*!< true if there is an environment problem (not speaking of the server down or the agent not running)*/
	bool m_environmentTested; /*!< true if the environment was analyzed during the current analisis phase*/

	/*!
	 * @brief Initialize the default values
	 *
	 * This method is called by the constructors. Default values are
	 * m_maxLoad = 2;
	 * m_maxMemory = 100;
	 * m_maxSwap = 99;
	 */
	void init();

public:

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor for the EnvironmentAgent class
	 *
	 *	\param[in] id : unique integer identifier of the MetaAgent
	 *	\param[in] name : unique string identifier of the MetaAgent
	 *	\param[in] server : pointer to the server on which the process runs
	 *	\param[in] fstab : pointer to the FileAgent responsible for checking the fstab file of the server
	 *	\param[in] maxLoad maximum load allowed
	 *	\param[in] maxMemory maximum memory usage allowed
	 *	\param[in] maxSwap maximum swap usage allowed
	 */
	EnvironmentAgent(int id, string name, Server * server, FileAgent * fstab,
			float maxLoad, float maxMemory, float maxSwap);

	/*!
	 * @brief Default constructor
	 */
	EnvironmentAgent();

	/*!
	 * @brief Copy constructor
	 * @param source source EnvironmentAgent
	 */
	EnvironmentAgent(const EnvironmentAgent & source);

	using Agent::operator=;
	/*!
	 * @brief Assignment operator
	 * @param source source EnvironmentAgent
	 * @return *this
	 */
	EnvironmentAgent & operator=(const EnvironmentAgent & source);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the EnvironmentAgent class
	 */
	virtual ~EnvironmentAgent();

	bool analyze();

	bool triggerRecover(RecoveryEngineAbstract * re) {
		return re->recover(this);
	}

	/*!
	 * @brief Checks whether the server is pingable
	 * @return True if the server is pingable, false otherwise
	 */
	bool isServerPingable();

	/*!
	 * @brief Checks whether the Remote Agent is running
	 * @return True if the Remote Agent is running, false otherwise
	 */
	bool isAgentRunning();

	/*!
	 * @brief Reinitialize the agent
	 *
	 * Basically reset the flags that are valid only during one diagnosis
	 * like m_pingTested, m_agentTested and m_environmentTested
	 */
	void reinitializeStatus();

	/*!
	 * @brief Returns whether the environment is responsible (always true)
	 *
	 * This method is inherited from MetaAgent. The idea is that when we roll down
	 * the chain of responsible agents, we receive true only if the final responsible
	 * is a EnvironmentAgent. So this method always return true.
	 * @return true
	 */
	bool isEnvironmentResponsible();

	/*!
	 * @brief Checks whether the environment was already tested
	 *
	 * This method returns true only if analyze was called. Checking
	 * the status of the server and the Remote Agent is not testing the environment
	 * @return True if the environment was analyzed, false otherwise
	 */
	bool wasEnviromentTested();

	/*!
	 * @brief Checks if the environment is sain
	 *
	 * The "sain" refers to real environment problem (cpu, disk...) and not
	 * the server being down or the agent unreachable.
	 *
	 * @return True if the analyze method showed a problem, false otherwise
	 */
	bool isEnvironmentSain();

	/*!
	 * \brief Checks that we can actually diagnose a remote machine
	 *
	 * This makes a very quick environment check, namely it checks
	 * if the server is up, and if the agent is running.
	 * If it is not the case, the environment is set as responsible.
	 *
	 * @return true if there is a problem
	 */
	bool makeQuickEnvironmentCheck();

	/*!
	 * @brief Add a FileSystem to ignore when doing the checks
	 *
	 * Some FS (random choice : quantum) always shows 99% usage of inodes
	 * For this reason, we can ignore a given mountpoint when doing the check
	 *
	 * @param fsName mountpoint to ignore
	 */
	void addFSToIgnore(std::string fsName);

	friend class EnvironmentRecovery;

};

#endif
