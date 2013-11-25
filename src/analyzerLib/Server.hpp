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


#ifndef SERVER_HPP
#define SERVER_HPP

/*!
 * \file Server.hpp
 * \brief Server class
 * \author Christophe Haen
 */

#include <iostream>
#include <stdlib.h>
#include "Client.hpp"
#include "Message.hpp"
#include "connection.hpp"

class EnvironmentAgent;
class Coordinator;

using namespace std;

/*!
 * \class Server
 * \brief Class that represents a server
 */
class Server {
protected:
	int m_id; /*!< unique integer identifier of the Server */
	string m_name; /*!< unique string identifier of the Server */
	string m_address; /*!< Address of the server (IP or dns name) */
	EnvironmentAgent * m_envAgent; /*!< EnvironmentAgent associated to the Server*/

	std::vector<Coordinator *> m_envCoords; /*!< List of coordinators that needs to be okay on an environment (e.g. ldap client config)*/

	 public:

	 /*!
	 *  \brief Base Constructor
	 *
	 *  Constructor of the Server class
	 */
	Server();

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the Server class
	 *
	 *  \param[in] id : id of the Server
	 *  \param[in] name : name of the Server
	 *  \param[in] address : ip address or dns-name of the Server
	 */
	Server(int id, string name, string address);

	/*!
	 * @brief Copy constructor
	 * @param source source Server
	 */
	Server(const Server & source);

	/*!
	 * @brief Assignment operator
	 * @param source source Server
	 * @return *this
	 */
	Server & operator=(const Server & source);

	/*!
	 *  \brief Getter for the identifier
	 *
	 *  Returns m_id
	 *
	 *  \return unique Server identifier
	 */
	int getId() const;

	/*!
	 *  \brief Getter for the name
	 *
	 *  Returns m_name
	 *
	 *  \return unique Server name
	 */
	string getName() const;

	/*!
	 *  \brief Getter for the address
	 *
	 *  Returns m_address
	 *
	 *  \return Server address
	 */
	string getAddress() const;

	/*!
	 *  \brief Server < operator
	 *
	 *	Compares if the name of the left server is smaller (!!!) than the name of x
	 *
	 *	\param[in] x : reference to the Server to which we are comparend
	 *
	 *	\return true if name(this)< name(x), false otherwise
	 */
	bool operator<(const Server& x) const {
		return m_name < x.m_name;
	}
	;

	/*!
	 *  \brief Setter for the EnvironmentAgent
	 *
	 *  Sets the EnvironmentAgent (m_envAgent)
	 *
	 *  \param[in] ea : pointer to the EnvironmentAgent
	 */
	void setEnvironmentAgent(EnvironmentAgent * ea);

	/*!
	 *  \brief Getter for the EnvironmentAgent
	 *
	 *  Returns m_envAgent
	 *
	 *  \return Server EnvironmentAgent
	 */
	EnvironmentAgent * getEnvironmentAgent();

	/*!
	 *  \brief Checks whether the server is alive
	 *
	 *  It pings the Server to see if it answers.
	 *
	 *  \return true if the Server answers the ping request
	 */
	bool isAlive() const;

	/*!
	 * @brief Reinitialize the status of the server
	 *
	 * Set m_pingChecked and m_agentTested to false
	 */
	void reinitializeStatus();

	/*!
	 * @brief Add a coordinator to the list of enviCoordinator
	 * @param coord coordinator to be attached
	 */
	void addAttachedCoordinator(Coordinator * coord);


	/*!
	 * @brief Return list of attached Coordinators
	 * @return Attached Coordinators
	 */
	std::vector<Coordinator * > getAttachedCoordinators();


};

#endif
