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


#ifndef PROCESS_AGENT_HPP
#define PROCESS_AGENT_HPP

/*!
 * \file ProcessAgent.hpp
 * \brief ProcessAgent class
 * \author Christophe Haen
 */
#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#include "Agent.hpp"
#include "FileAgent.hpp"
#include "EnvironmentAgent.hpp"
#include "Server.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

using namespace std;

/*!
 * \class ProcessAgent
 * \brief Class for all the process concerns
 *
 *  The class is responsible for checking the proper state of a given process.
 *  It inherits from the Agent class.
 */
class ProcessAgent: public Agent {
protected:

	string m_procName; /*!< Name of the process (with the options). If the process is a system process, it should start with a star ("*") */
	int m_pid; /*!< Measured pid of the process */
	FileAgent * m_env; /*!< FileAgent to analyze the environment file (/proc/pid/environ) of the agent */
	float m_maxCpu; /*!< the limit we put on cpu usage */
	float m_maxMemory; /*!< the limit we put on memory usage */

	string m_service; /*!< Name of the service the process is associated with */
	string m_command; /*!< Command to run to start the process */

	//expected values
	string m_user; /*!< Expected user running the process */
	std::map<std::string, unsigned long int> m_limits_map; /*!< Expected values of the limits. Unlimited is represent with LIMIT_UNLIMITED. */

	//true values
	string m_true_user; /*!< Actual user running the process */
	float m_true_cpu; /*!< Measured CPU load */
	float m_true_mem; /*!< Measured memory consumption */

	bool m_process_not_running; /*!< Boolean set to true if the process is not running */
	bool m_user_pb; /*!< Boolean set to true if the expected and the actual users don't match */
	bool m_cpu_pb; /*!< Boolean set to true if the cpu load is above a certain threshold */
	bool m_mem_pb; /*!< Boolean set to true if the memory consumption is above a certain threshold */
	bool m_limits_pb; /*!< Boolean set to true if one or more limit don't have the expected value */
	std::map<std::string, bool> m_limits_pb_detail_map; /*!< Vector of booleans set to true if the given limit does not match the expected value */

	bool m_limStatus_pb; /*!< Boolean set to true if one or more limit have reached the threshold */
	std::map<std::string, bool> m_limStatus_pb_detail_map; /*!< Vector of booleans set to true if the given limit has reached the threshold */
	bool m_env_pb; /*!< Boolean set to true if there is a problem with the process environment */

	vector<int> m_pidList; /*!< List of pids that are the runing process(es) we are responsible for, and at the same depth in the pid tree */
	map<int, vector<int> > m_pidTree; /*!< Map (pid, list of pids) used for processes that have spawned children */
	int m_lvl; /*!< Depth in the process children tree */
	vector<ProcessAgent *> m_sons; /*!< Vector of pointers to the ProcessAgent responsible for the direct child of the current process */

	/*!
	 *  \brief Analysis of the current pid only
	 *
	 *  This method analyze the current pid only, and does not take into account
	 *  the status of the children processes
	 *
	 *  \return true if there is a problem with the current pid
	 */
	bool selfAnalyze();

	/*!
	 *  \brief Find the top process in the processes tree
	 *
	 *  If the process has 1 as parent pid (which is the case for services for example)
	 *  we take the pid directly child of 1 as tree head.
	 *
	 *  \return List of pids that are head of trees
	 */
	vector<int> findProcessTreeHead();

	int m_multiplicity; /*!< Maximum amount of concurrent processes at the same level. -1 if unlimited */

	/*!
	 * @brief Initialize the values to default
	 * In particular
	 *
	 *	- m_multiplicity = -1;
	 *	- m_maxCpu = 100;
	 *	- m_maxMemory = 100;
	 */
	void init();

public:
	/*!
	 *  \brief Base constructor
	 *
	 *  Base constructor of the ProcessAgent class
	 */
	ProcessAgent();

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the ProcessAgent class.
	 *  It is meant to be used for process not fully controlled by us,
	 *	but that needs to be killed for server overload reasons for example.
	 *
	 *	\param[in] pid pid of the process we get attached to
	 */
	ProcessAgent(int pid); //

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the ProcessAgent class.
	 *
	 *	\param[in] id : unique integer identifier of the MetaAgent
	 *	\param[in] name : unique string identifier of the MetaAgent
	 *	\param[in] server : pointer to the server on which the process runs
	 *	\param[in] procName : name of the process, with arguments and full path of binary. if it is a system process, it should start with "*"
	 *	\param[in] user : user running the process
	 *	\param[in] limits : list of environment limits
	 *	\param[in] env : pointer to a FileAgent responsible for checking the environment file of the process (/proc/pid/environ)
	 *	\param[in] multiplicity : amount of concurrent processes at the same level. -1 if unlimited
	 *	\param[in] maxCpu Maximum cpu percentage the process can consume
	 *	\param[in] maxMemory Maximum memory percentage the process can consume
	 */
	ProcessAgent(int id, string name, Server * server, string procName,
			string user, map<string, unsigned long int> limits, FileAgent * env,
			int multiplicity, float maxCpu, float maxMemory);

	/*!
	 * @brief Copy constructor
	 * @param source source ProcessAgent
	 */
	ProcessAgent(const ProcessAgent& source);

	using Agent::operator=;
	/*!
	 * @brief Assignment operator
	 * This operator uses the Agent::operator=
	 * @param source source ProcessAgent
	 * @return *this
	 */
	ProcessAgent & operator=(const ProcessAgent & source);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the ProcessAgent class
	 */
	virtual ~ProcessAgent();

	/*!
	 * \brief Setter for the pid of the process
	 *
	 *	Sets the m_pid value
	 *
	 * \param pid : pid of the process
	 */
	void setPid(int pid);

	/*!
	 * \brief Setter for the pid list
	 *
	 * sets m_pidList
	 *
	 * \param pidList : vector of pid
	 */
	void setPidList(vector<int> pidList);

	/*!
	 * \brief Setter for the pid tree
	 *
	 * sets m_pidTree
	 *
	 * \param pidTree : map (pid, children pid) representing the pid tree
	 */
	void setPidTree(map<int, vector<int> > pidTree);

	/*!
	 * \brief Setter for the depth in the pid tree
	 *
	 * sets m_lvl
	 *
	 * \param lvl : depth in the pid tree
	 */
	void setLvl(int lvl);

	/*!
	 * \brief Add a ProcessAgent representing a child process
	 *
	 * Adds a ProcessAgent in m_sons
	 *
	 * \param son : ProcessAgent pointer for the child process
	 */
	void addSon(ProcessAgent * son);

	/*!
	 * \brief Analyzes the state of the processAgent
	 *
	 * Analyzes all the processes in the tree
	 *
	 *	\return true if there is a problem with the process
	 */
	bool analyze();

	/*!
	 * \brief Setter for the service associated to the process
	 *
	 * sets m_service
	 *
	 * \param service : name of the service
	 */
	void setService(string service);

	/*!
	 * \brief Setter for the command used to start the process
	 *
	 * sets m_command
	 *
	 * \param command : command line
	 */
	void setCommand(string command);

	bool triggerRecover(RecoveryEngineAbstract * re) {
		return re->recover(this);
	}

	friend class ProcessRecovery;

};

#endif
