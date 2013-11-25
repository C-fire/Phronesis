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


#ifndef PHRONESIS_AGENT_HPP
#define PHRONESIS_AGENT_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/program_options.hpp>

#include "Configuration.hpp"
#include "connection.hpp"
#include "Treatment.hpp"

namespace po = boost::program_options;


/*!
 * \file PhronesisAgent.hpp
 * \brief Agent that will run on the monitored node and answer requests from the server
 * \author Christophe Haen
 *
 */

using boost::asio::ip::tcp;

/*!
 * \class PhronesisAgent
 * \brief This class is the monitoring agent running on every node
 *
 * This class is the "entry point" of the agent. There is only one instance of it,
 * whose role is to accept connection and create new Treatment instance
 *
 */
class PhronesisAgent {

public:
	/*!
	 * \brief Constructor
	 *
	 * Constructor of the PhronesisAgent class
	 *
	 * @param io_service The io service to manage to connection
	 * @param endpoint The endpoint to accept connection on port 7171
	 */
	PhronesisAgent(boost::asio::io_service & io_service,
			const tcp::endpoint& endpoint);

	/*!
	 * \brief Waits for connection
	 *
	 * Create a new connection and waits for someone to connect
	 */
	void wait_for_connection();

private:

	/*!
	 * \brief Handler when someone connects
	 *
	 * If there is no error, it starts and new Treatment instance, and goes back in wait_for_connection
	 *
	 * @param error The error that can be raised
	 * @param new_connection Boost shared pointer to the tcp_connection object
	 */
	void handle_accept(const boost::system::error_code& error,
			connection_ptr new_connection);

	boost::asio::io_service& m_io_service; /*!< The main Boost::asio io service on which relies the communication */
	tcp::acceptor m_acceptor; /*!< The Boost::asio acceptor use to accept connections */

};

#endif
