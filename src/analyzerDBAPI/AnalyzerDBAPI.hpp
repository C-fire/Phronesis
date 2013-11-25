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


#ifndef ANALYZER_DB_API_H
#define ANALYZER_DB_API_H

/*!
 * \file AnalyzerDBAPI.hpp
 * \brief AnalyzerDBAPI class
 * This file defines the AnalyzerDBAPI class
 * It ensures all the communication with the db.
 * \author Christophe Haen
 */

/*!
 * Set to 1 if we use mysql db. This should be replaced by a compiler option
 */
#define USE_MYSQL 1

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <queue>
#include <exception>
#include <stdexcept>

#include <stdarg.h>

#include <mysql/mysql.h>

#include <stdlib.h>
#include <stdio.h>

#include "MetaAgent.hpp"
#include "Coordinator.hpp"
#include "Agent.hpp"
#include "FileAgent.hpp"
#include "FolderAgent.hpp"
#include "ProcessAgent.hpp"
#include "EnvironmentAgent.hpp"

#include "Server.hpp"
#include "AgentRecovery.hpp"

/*! Macro to go through all lines of the query result */
#define FOR_EACH_LINE   for (unsigned int i = 0; i < this->m_queryResult.size(); i++){ \
                           vector<string> line = this->m_queryResult[i];

/*! Macro to go through all columns of the query result */
#define FOR_EACH_COL    for (unsigned int j= 0; j < line.size(); j++){\
                           string col = line[j];

/*! Macro to go through all lines and columns of the query result */
#define FOR_EACH        FOR_EACH_LINE FOR_EACH_COL

/*! Macro that ends the FOR_EACH_COL macro */
#define DONE_COL        }

/*! Macro that ends the FOR_EACH_LINE macro */
#define DONE_LINE       }

/*! Macro that ends the FOR_EACH_LINE and FOR_EACH_COL macros */
#define DONE            DONE_COL DONE_LINE

using namespace std;

/*!
 * \class AnalyzerDBAPI
 * \brief Class to manage the connection to the database
 *
 *  This class acts as a layer between the AnalyzerCore or the AnalyzerCLI and the database.
 *  It contains all the methods that are required to retrieve the needed information.
 */
class AnalyzerDBAPI {

private:

	/**
	 * \enum DbType
	 * \brief Supported db type
	 *
	 * Those constants define which db types are supported.
	 */
	enum DbType {
		MySQL, Oracle
	};

	DbType dbType; /*!< dbType being used*/

	vector<vector<string> > m_queryResult; /*!< Whenever a query is done, the result is stored there */

	/*Specific MySQL attributes */

	MYSQL m_connection; /*!< attribute to store the connection to the Mysql db */
	MYSQL_RES *m_result; /*!< pointer to the mysql query result */

	/*!
	 *  \brief Connection function to the database
	 *
	 *	\param[in] server : address of the server to connect
	 *	\param[in] user : username for the db
	 *	\param[in] passwd : password for the db
	 *	\param[in] port : port number on which to connect
	 *	\param[in] name : name of the database
	 *
	 * \throw std::runtime_error If initialization of the db library fails
	 * \throw std::runtime_error If the connection to the db fails
	 */
	void connect(string server, string user, string passwd, int port,
			string name); //return error Code

	/*!
	 *  \brief Execute an sql query on a connected database.
	 *
	 *	This method should be used only for query to extract information from the db.
	 *	See insertQuery for tables modifications.
	 *
	 *	The results are stored in m_queryResult
	 *
	 *	\throw std::runtime_error In case of query error (there is one retry if the error is lost connection)
	 *
	 *	\param[in] Query : query to execute
	 *	\param[in] printReq : debug boolean to know whether to print the query or not
	 *	\param[in] retry : false if it is the first attempt to execute the query, true otherwise.
	 *				We set it to true when the query failed because of a connection problem, in which case
	 *				we retry once.
	 *	\return the amount of raws retrieved
	 */
	int executeQuery(string Query, bool printReq = false, bool retry = false);

	/*!
	 *  \brief Perform an SQL query that modifies the tables (insert, update)
	 *
	 *	\throw AnalyzerDBAPIExceptions in case of any problem... more exceptions soon
	 *
	 *	\param[in] query : query to execute
	 *	\param[in] retry : false if it is the first attempt to execute the query, true otherwise.
	 *				We set it to true when the query failed because of a connection problem, in which case
	 *				we retry once.
	 */
	void insertQuery(string query, bool retry = false);

	/*!
	 *  \brief Escape the query string
	 *
	 *	\param[in] pStr : query to escape
	 *	\return The escaped string
	 */
	string escapeString(string pStr);

	/*!
	 * \brief returns the max id of a table
	 *
	 * @param table the table we are interested in
	 * @return the maximum id
	 */
	std::string getMaxId(std::string table);

public:

	/*!
	 *  \brief Constructor
	 *
	 *	The Constructor makes a connection attempt.
	 *
	 *	\param[in] type : database type wanted (see DbType)
	 *	\param[in] server : address of the server to connect
	 *	\param[in] user : username for the db
	 *	\param[in] passwd : password for the db
	 *	\param[in] port : port number on which to connect
	 *	\param[in] name : name of the database
	 *
	 */
	AnalyzerDBAPI(string type, string server, string user, string passwd,
			int port, string name);

	/*!
	 * \brief Destructor
	 *
	 *	Closes the connection and unload the libraries.
	 */
	virtual ~AnalyzerDBAPI();

	/*!
	 * \brief Retrieve all the Agent from the database
	 *
	 * @param[in,out] inV : reference to the vector of pointer of Agent to be filled in
	 * @param[in] servers : map of Servers (key : Server name, value : pointer to the Server)
	 */
	void retrieveAllAgents(vector<Agent *> & inV,
			map<string, Server *> servers);

	/*!
	 * \brief Retrieve all the Coordinator from the database
	 *
	 * @param[in,out] inV : reference to the vector of pointer of Coordinator to be filled in
	 */
	void retrieveAllCoordinators(vector<Coordinator *> & inV);

	/*!
	 * \brief Retrieve the MetaAgent tree
	 *
	 * Retrieve the MetaAgent tree from the database and returns it as a list of edge (link)
	 *
	 * \return MetaAgent tree
	 */
	vector<vector<string> > getTree();

	/*!
	 * \brief Retrieve the list of Coordinators to attach to servers
	 *
	 * Retrieve the Attached Coordinator tree from the database and returns it as a list of edge (link)
	 *
	 * \return Coordinators attached to Servers tree
	 */
	vector<vector<string> > getServerAttachedCoordinators();

	/*!
	 * \brief Getter for the occurrence of an edge
	 *
	 * This returns how many time an edge was responsible in a problem
	 *
	 *	\param[in] id_parent : id of the parent MetaAgent
	 *	\param[in] id_daugther : id of the daughter MetaAgent
	 * \return Occurrence of a given edge
	 */
	int getOccurrence(int id_parent, int id_daugther);

	/*!
	 * \brief Getter of how many time a MetaAgent had a problem.
	 *
	 * @param id_parent	id of the MetaAgent
	 * @return	Total number of problem on the MetaAgent
	 */
	int getTotal(int id_parent);

	/*!
	 *	\brief Increments the counter of the unsolvable situation
	 * 	@param id_agent	: id of the agent unsolvable
	 */
	void addUnsolved(int id_agent);

	/*!
	 * \brief Increments the occurrence of an edge
	 *
	 *	\param[in] pId : id of the parent MetaAgent
	 *	\param[in] dId : id of the daughter MetaAgent
	 */
	void incrementOccurrence(int pId, int dId);

	/*!
	 * \brief Increment how many time a Coordinator had a problem.
	 *
	 * This actually increments the local and the global total
	 *
	 * @param pId	id of the MetaAgent
	 */
	void incrementTotal(int pId);

	/*!
	 * \brief Increment just the local total of how many times a MetaAgent had a problem
	 *
	 * @param pId id of the MetaAgent
	 */
	void incrementLocalTotal(int pId);

	/*!
	 * \brief Increment a dependency occurrence.
	 *
	 *	This increases the counter that represents the dependency
	 *	"id_prior solved id_after"
	 *
	 * @param id_prior	beginning of the edge
	 * @param id_after	end of the edge
	 */
	void incrementDependencyOccurence(int id_prior, int id_after);

	/*!
	 * \brief Getter for the Dependency occurrence
	 *
	 * @param id_prior	beginning of the edge
	 * @param id_after	end of the edge
	 *
	 * @return	The occurrence of the given dependency
	 */
	int getDependencyOccurence(int id_prior, int id_after);

	/*!
	 * \brief Validate the given dependency
	 *
	 *	The dependencies are valid only once approved. This sets the dependency as approved in the database.
	 *
	 * @param id_prior	beginning of the edge
	 * @param id_after	end of the edge
	 */
	void validateDependency(int id_prior, int id_after);

	/*!
	 * \brief Getter for all the MetaAgent name solved by the metaAgent id_prior
	 *
	 *	It returns the name of all the MetaAgent that are an end vertex of
	 *	a dependency edge that has id_prior as start
	 *
	 * @param id_prior	beginning of the edge
	 * \return List of MetaAgent names
	 */
	vector<string> getAfters(int id_prior);

	/*!
	 * \brief Getter for all the MetaAgent name that solve the metaAgent id_after
	 *
	 *	It returns the name of all the MetaAgent that are an start vertex of
	 *	a dependency edge that has id_after as end
	 *
	 * @param id_after	beginning of the edge
	 * \return List of MetaAgent names
	 */
	std::vector<std::string> getBefores(int id_after);

	/*!
	 * \brief Retrieve all the Servers from the database
	 *
	 *	This method fills in the map given as argument.
	 *	The key of the map is the Server name, and the value is a pointer to the Server
	 *
	 * @param[in,out] servers : reference to the map of Server
	 */
	void retrieveAllServers(map<string, Server *> & servers);

	/*!
	 * \brief Increments the counter of how many time a server failed.
	 *
	 *	The methods increments how  many time a dead server is found dead.
	 *
	 * @param[in] sId : id of the Server
	 */
	void incrementServerFailure(int sId);

	/*!
	 *	\brief Returns a map containing a unique identifier for every pair (MetaAgent name, recoveryAction name)
	 *
	 *	Every pair (MetaAgent name, recoveryAction) will be assigned a unique identifier.
	 *	The pairs come from the recovery rules tree, that represents what action requires another one.
	 *	The id is generated.
	 *
	 *	The map is the following : < <MetaAgent name, action name> , id >
	 *
	 * @return Map of unique identifier for every (MetaAgent name, recoveryAction name ) pair
	 */
	map<pair<string, string>, int> getRecoveryRulesMap();

	/*!
	 *	\brief Returns a vector containing all the pair of rules that go in the direction argument.
	 *
	 *	The method will return a vector containing pairs of ids (id1, id2). Each id is associated to a pair (MetaAgent name, recoveryAction name).
	 *	The pairs that are returned are all the rules that go in the direction given as the argument.
	 *
	 *	\param[in]	rulesMap : the map matching a pair (MetaAgent name, recoveryAction name) with a unique identifier.
	 *	\param[in]	direction : the direction of rule we are interested in.
	 *		- "after" : for the rules that needs id2 to be executed after id1
	 *		- "before" : for the rules that needs id2 to be executed before id1
	 *		- "" : (default) both directions
	 *
	 * @return Vector containing the rules going in the required direction
	 */
	vector<pair<int, int> > getRecoveryRulesTree(
			map<pair<string, string>, int> rulesMap, string direction = "");

	/*!
	 * \brief Returns the history of recovery attempts made on a responsible
	 *
	 * @param responsibleName Name of the entity we suspect to be responsible from the problem
	 * @return Vector of RecoveryHistory
	 */
	std::vector<RecoveryHistory> getRecoveryHistory(
			std::string responsibleName);

	/*!
	 * \brief Add a RecoveryHistory event into the database
	 * @param event history event
	 */
	void addRecoveryHistoryEvent(RecoveryHistory event);

};

#endif
