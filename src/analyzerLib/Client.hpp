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


#ifndef CLIENT_H
#define CLIENT_H

/*!
 * \file Client.hpp
 * \brief Client class
 *
 * This class is the one which is used to talk between the Core and the Agents
 *
 * \author Christophe Haen
 */

#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

#include "Message.hpp"
#include "connection.hpp"

using boost::asio::ip::tcp;

/*!
 * \class client
 * \brief Client to manage the communication between the Core and the Agents
 *
 * It establish the communication between the Core and Agents, waits for input,
 * and allows to send data. Because it is running in a multi threaded context,
 * all the exceptions remain internal to the class, and it makes use of boolean
 * to signal errors to the caller.
 */
template<class T>
class client {
public:
	/*!
	 * @brief Constructor
	 *
	 *
	 * @param io_service A reference to the boost::asio::io_service to be used
	 * @param endpoint A reference to the boost::asio::ip::tcp::endpoint to connect to
	 * @param timeout Number of seconds after which we consider the remote connection unresponsive (default 20)
	 */
	client(boost::asio::io_service& io_service, tcp::endpoint& endpoint,
			int timeout = 20) :
			m_io_service(io_service), m_tcp_connection(
					connection_ptr(new tcp_connection(io_service))), timer(
					io_service, boost::posix_time::seconds(1)) {

		tcp::socket& sock = m_tcp_connection->socket();

		timer.expires_from_now(boost::posix_time::seconds(timeout));

		m_receivedAnswer = false;
		m_timeout = false;
		m_exception = false;

		// If the timeout is reached, the callback method client::timeout is called
		timer.async_wait(boost::bind(&client<T>::timeout, this));

		// Set the callback method of the connection to client::handle_connect
		sock.async_connect(endpoint,
				boost::bind(&client<T>::handle_connect, this,
						boost::asio::placeholders::error));

	}

	/*!
	 *  @brief Method to be used by the caller to send a Request
	 *  @param e Reference to the Request instance to send
	 */
	void write(Request& e) {
		m_tcp_connection->async_write(e,
				boost::bind(&client::handle_write, this,
						boost::asio::placeholders::error));

	}

	/*!
	 * @brief Trigger the closing of the connection
	 */
	void close() {
		//std::cout << "Close" << std::endl;
		m_io_service.post(boost::bind(&client::do_close, this));
	}

	bool m_receivedAnswer; /*!< True if an answer was received, false otherwise */
	bool m_timeout; /*!< True if the timeout was hit, false otherwise */
	bool m_exception; /*!< True if an exception was thrown, false otherwise */

	std::string m_exception_msg; /*!< Exception message if an excpetion was thrown */

	/*!
	 * @brief Return the response
	 * @return Response of the Remote Agent
	 */
	T getAnswer() const {
		return m_dataReceived;
	}

private:

	/*!
	 * @brief Callback method for the connection
	 *
	 * When the connection is established, this method is called.
	 * If there is no error at the connection, it starts an asynchronous wait for input.
	 * Otherwise, it keeps the error message and sets the exception flag
	 *
	 * @param error Error code if any
	 */
	void handle_connect(const boost::system::error_code& error) {
		if (!error) {
			//std::cout << "Handle connect attach handle_read" << std::endl;
			m_tcp_connection->async_read(m_dataReceived,
					boost::bind(&client::handle_read, this,
							boost::asio::placeholders::error));

		} else {

			m_exception_msg = "Error in handle_connect : "
					+ std::string(error.category().message(error.value()))
					+ " (" + boost::lexical_cast<std::string>(error.value())
					+ ")";
			m_exception = true;

		}
	}

	/*!
	 * @brief Callback method for read
	 *
	 * When data are received, this method is called.
	 * The data are stored in the member m_info.
	 * If there was no error at the reception, it sets the receive flag to true,
	 * and cancels the timeout.
	 * Otherwise, it keeps the error message, sets the exception flag, and close the connection
	 *
	 * @param error Error code if any
	 */
	void handle_read(const boost::system::error_code& error) {
		if (!error) {

			m_receivedAnswer = true;
			timer.cancel();
		} else {

			m_exception_msg = "Error in handle_read : "
					+ std::string(error.category().message(error.value()))
					+ " (" + boost::lexical_cast<std::string>(error.value())
					+ ")";
			m_exception = true;
			do_close();
		}
	}

	/*!
	 * @brief Callback method for write
	 *
	 * When data are sent, this method is called.
	 * If there was no error when sending data, nothing is done.
	 * Otherwise, it keeps the error message, sets the exception flag, and close the connection
	 *
	 * @param error Error code if any
	 */
	void handle_write(const boost::system::error_code& error) {
		if (error) {
			m_exception_msg = "Error in handle_write : "
					+ std::string(error.category().message(error.value()))
					+ " (" + boost::lexical_cast<std::string>(error.value())
					+ ")";
			m_exception = true;
			do_close();
		}
	}

	/*!
	 * @brief Closes the connection
	 */
	void do_close() {
		if (!m_exception)
			m_tcp_connection->socket().close();
	}

	/*!
	 * @brief Callback method for timeout
	 * Sets the timeout flag to true.
	 */
	void timeout() {
		//timeout is also called when we cancel, so we have to protect against false timeout
		if (!m_receivedAnswer)
			m_timeout = true;
	}

private:
	boost::asio::io_service& m_io_service; /*!< reference to the boost::asio::io_service used for the connection */
	connection_ptr m_tcp_connection; /*!< shared pointer to a tcp_connection instance */
	T m_dataReceived; /*!< Data received */
	boost::asio::deadline_timer timer; /*!< timer used to manage the timeout */
};

#endif
