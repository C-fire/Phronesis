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


#ifndef SERVER_MANAGER_HPP
#define SERVER_MANAGER_HPP

/*!
 * \file ServerPool.hpp
 * \brief ServerPool class
 * \author Christophe Haen
 */


#include <exception>
#include <stdexcept>
#include "Server.hpp"
#include "AnalyzerDBAPI.hpp"

using namespace std;

/*!
 * @class ServerPool
 * @brief Keeps all the Server in a pool and offers easy access to it
 */
class ServerPool {
protected:

	AnalyzerDBAPI * m_dbApi; /*!< pointer to the AnalyzerDBAPI */
	map<string, Server *> m_servers; /*!< map which associated a Server name with a pointer to it */

public:
	/*!
	 * @brief Constructor
	 *
	 * The constructor retrieves all the Server from the database
	 *
	 * @param dbApi pointer to the AnalyzerDBAPI
	 */
	ServerPool(AnalyzerDBAPI * dbApi);

	/*!
	 * @brief Destructor
	 *
	 * Deletes all the Servers.
	 */
	~ServerPool();

	/*!
	 * @brief Returns a Server according to its name
	 *
	 * @param serverName name of the Server
	 * @return pointer to the searched Server
	 */
	Server * getServer(string serverName) const;

	/*!
	 * @brief Returns the map matching a Server name to a pointer to it
	 * @return the map of Server
	 */
	map<string, Server *> getServersMap() const;

	/*!
	 * @brief Reinitialize the status of all the servers
	 *
	 * By status, we mean up/down, phronesisAgent running or not and so on
	 */
	void reinitializeServersStatus();

};

#endif
