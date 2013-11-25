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


#ifndef CONNECTION_H
#define CONNECTION_H

/*!
 * \file connection.hpp
 * \brief tcp_connection class
 * \author Christophe Haen
 */

#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>


/*!
 * \class tcp_connection
 * \brief Class to manage the TCP connection between two hosts
 */
class tcp_connection {
public:

	/*!
	 * \brief Constructor.
	 *
	 * @param io_service boost::asio::ioservice on which the connection is based
	 */
	tcp_connection(boost::asio::io_service& io_service) :
			m_socket(io_service) {
	}

	/*!
	 * @brief Getter for the socket
	 * @return The socket object
	 */
	boost::asio::ip::tcp::socket& socket() {
		return m_socket;
	}

	/*!
	 * \brief Asynchronously write a data structure to the socket.
	 * @param t The data to write
	 * @param handler the method to call when the writing is done
	 */
	template<typename T, typename Handler>
	void async_write(const T& t, Handler handler) {
		// Serialize the data first so we know how large it is.
		std::ostringstream archive_stream;
		boost::archive::text_oarchive archive(archive_stream);
		archive << t;
		m_outbound_data = archive_stream.str();

		// Format the header.
		std::ostringstream header_stream;
		header_stream << std::setw(header_length) << std::hex
				<< m_outbound_data.size();
		if (!header_stream || header_stream.str().size() != header_length) {
			// Something went wrong, inform the caller.
			boost::system::error_code error(
					boost::asio::error::invalid_argument);
			m_socket.get_io_service().post(boost::bind(handler, error));
			return;
		}
		m_outbound_header = header_stream.str();

		// Write the serialized data to the socket. We use "gather-write" to send
		// both the header and the data in a single write operation.
		std::vector < boost::asio::const_buffer > buffers;
		buffers.push_back(boost::asio::buffer(m_outbound_header));
		buffers.push_back(boost::asio::buffer(m_outbound_data));
		boost::asio::async_write(m_socket, buffers, handler);
	}

	/*!
	 * \brief Asynchronously read a data structure from the socket.
	 * @param t Where to put the data we read
	 * @param handler the method to call when the reading is done
	 */
	template<typename T, typename Handler>
	void async_read(T& t, Handler handler) {
		// Issue a read operation to read exactly the number of bytes in a header.
		void (tcp_connection::*f)(const boost::system::error_code&, T&,
				boost::tuple<Handler>)
				= &tcp_connection::handle_read_header<T, Handler>;
		boost::asio::async_read(
				m_socket,
				boost::asio::buffer(m_inbound_header),
				boost::bind(f, this, boost::asio::placeholders::error,
						boost::ref(t), boost::make_tuple(handler)));
	}

	/*!
	 * \brief read the header which contains the size
	 *
	 * Handle a completed read of a message header. The handler is passed using
	 * a tuple since boost::bind seems to have trouble binding a function object
	 * created using boost::bind as a parameter.
	 *
	 * @param e boost::system::error_code
	 * @param t the object where to read
	 * @param handler the method to call when the reading is done
	 */
	template<typename T, typename Handler>
	void handle_read_header(const boost::system::error_code& e, T& t,
			boost::tuple<Handler> handler) {
		if (e) {
			boost::get < 0 > (handler)(e);
		} else {
			// Determine the length of the serialized data.
			std::istringstream is(std::string(m_inbound_header, header_length));
			std::size_t m_inbound_datasize = 0;
			if (!(is >> std::hex >> m_inbound_datasize)) {
				// Header doesn't seem to be valid. Inform the caller.
				boost::system::error_code error(
						boost::asio::error::invalid_argument);
				boost::get < 0 > (handler)(error);
				return;
			}

			// Start an asynchronous call to receive the data.
			m_inbound_data.resize(m_inbound_datasize);
			void (tcp_connection::*f)(const boost::system::error_code&, T&,
					boost::tuple<Handler>)
					= &tcp_connection::handle_read_data<T, Handler>;
			boost::asio::async_read(
					m_socket,
					boost::asio::buffer(m_inbound_data),
					boost::bind(f, this, boost::asio::placeholders::error,
							boost::ref(t), handler));
		}
	}

	/*!
	 * \brief Handle a completed read of message data.
	 *
	 * @param e boost::system::error_code
	 * @param t the object where to read
	 * @param handler the method to call when the reading is done
	 */
	template<typename T, typename Handler>
	void handle_read_data(const boost::system::error_code& e, T& t,
			boost::tuple<Handler> handler) {
		if (e) {
			boost::get < 0 > (handler)(e);
		} else {
			// Extract the data structure from the data just received.
			try {
				std::string archive_data(&m_inbound_data[0],
						m_inbound_data.size());
				std::istringstream archive_stream(archive_data);
				boost::archive::text_iarchive archive(archive_stream);
				archive >> t;
			} catch (std::exception& e) {
				// Unable to decode data.
				boost::system::error_code error(
						boost::asio::error::invalid_argument);
				boost::get < 0 > (handler)(error);
				return;
			}

			// Inform caller that data has been received ok.
			boost::get < 0 > (handler)(e);
		}
	}

	/*!
	 * \brief close the socket
	 */
	void close() // (7)
	{
		m_socket.close();
	}

private:

	boost::asio::ip::tcp::socket m_socket; /*!< The underlying socket. */

	/// The size of a fixed length header.
	enum {
		header_length = 8
	};

	std::string m_outbound_header; /*!< Holds an outbound header. */

	std::string m_outbound_data; /*!< Holds the outbound data. */

	char m_inbound_header[header_length]; /*!< Holds an inbound header. */

	std::vector<char> m_inbound_data; /*!< Holds the inbound data. */
};

/*! \typedef boost::shared_ptr<tcp_connection> connection_ptr;
 *  a boost::shared_ptr on a tcp_connection
 */
typedef boost::shared_ptr<tcp_connection> connection_ptr;

#endif
