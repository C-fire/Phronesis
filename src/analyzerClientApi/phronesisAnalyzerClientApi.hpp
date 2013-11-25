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


#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/tuple/tuple.hpp>

//////////////////////////////////////////////////////////////////////////////////////
//  This is a copy of what is in the phronesisAnalyzerLib.
//  It is kind of ugly, but so much easier.
//
//////////////////////////////////////////////////////////////////////////////////////

class api_message {
public:

	api_message() {
		reset();
	}

	api_message(int type, int id, std::string text) :
			m_type(type), m_id(id), m_text(text) {
	}
	;

	void reset() {
		m_text.clear();
		m_date.clear();
		m_time.clear();
		m_id = -1;
	}

	int m_type; // Type of the event : NEW_MSG, etc.
	//Msg/Question related data
	int m_id;
	std::string m_text;
	std::string m_date;
	std::string m_time;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_type & m_id & m_text & m_date & m_time;
	}

	enum {
		NEW_MSG,
		NEW_LOG,
		NEW_QUESTION,
		NEW_YN_QUESTION,
		NEW_PB_QUESTION,
		ACK_MSG,
		ANSWER_QUESTION,
		CURRENT_PB,
		GIVE_PB,
		SET_VETO, //The trick is to use the id as boolean
		GET_VETO,
		NEW_VETO,
		STATISTIC,
		ERROR_TEXT
	};

	friend std::ostream& operator <<(std::ostream & out, api_message & msg) {
		out << "m_type " << msg.m_type << " m_id " << msg.m_id << " m_text "
				<< msg.m_text << "m_date " << msg.m_date << " m_time "
				<< msg.m_time;
		return out;
	}
};

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
		std::vector<boost::asio::const_buffer> buffers;
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
		boost::asio::async_read(m_socket, boost::asio::buffer(m_inbound_header),
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
			boost::get<0>(handler)(e);
		} else {
			// Determine the length of the serialized data.
			std::istringstream is(std::string(m_inbound_header, header_length));
			std::size_t m_inbound_datasize = 0;
			if (!(is >> std::hex >> m_inbound_datasize)) {
				// Header doesn't seem to be valid. Inform the caller.
				boost::system::error_code error(
						boost::asio::error::invalid_argument);
				boost::get<0>(handler)(error);
				return;
			}

			// Start an asynchronous call to receive the data.
			m_inbound_data.resize(m_inbound_datasize);
			void (tcp_connection::*f)(const boost::system::error_code&, T&,
					boost::tuple<Handler>)
					= &tcp_connection::handle_read_data<T, Handler>;
			boost::asio::async_read(m_socket,
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
			boost::get<0>(handler)(e);
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
				boost::get<0>(handler)(error);
				return;
			}

			// Inform caller that data has been received ok.
			boost::get<0>(handler)(e);
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

	boost::asio::ip::tcp::socket m_socket; /*! The underlying socket. */

	/// The size of a fixed length header.
	enum {
		header_length = 8
	};

	std::string m_outbound_header; /*! Holds an outbound header. */

	std::string m_outbound_data; /*! Holds the outbound data. */

	char m_inbound_header[header_length]; /*! Holds an inbound header. */

	std::vector<char> m_inbound_data; /*! Holds the inbound data. */
};

/*! \typedef boost::shared_ptr<tcp_connection> connection_ptr;
 *  a boost::shared_ptr on a tcp_connection
 */
typedef boost::shared_ptr<tcp_connection> connection_ptr;

//////////////////////////////////////////////////////////////////////////////////////
//
//  END OF THE COPY
//
//////////////////////////////////////////////////////////////////////////////////////

using boost::asio::ip::tcp;

/*!
 * @class Observer
 * @brief Observer interface
 */
class Observer {
public:
	virtual ~Observer() {
	}
	;
	virtual void update(api_message data) = 0;
};

/*!
 * @class Observable
 * @brief Observable interface
 */
class Observable {
public:

	void notify(api_message data) {
		for (std::list<Observer*>::iterator it = list_observers.begin();
				it != list_observers.end(); ++it) {
			(*it)->update(data);
		}
	}
	void add_observer(Observer* observer) {
		this->list_observers.push_back(observer);
	}
	void del_observer(Observer* observer) {
		this->list_observers.remove(observer);
	}

private:
	std::list<Observer*> list_observers;
};

/*!
 * @class api_client
 * @brief Connects to the phrCore and takes care of the data transmission
 *
 * It sends data to the Core, and gets messages that come from it when
 * the InteractionManager signal is triggered.
 * It is an Observable, so it notifies its observers when it receives data
 */
class api_client: public Observable {

public:

	/*!
	 * @brief Constructor
	 * @param io_service boost::asio::io_service to run on
	 * @param endpoint End point to connect to
	 */
	api_client(boost::asio::io_service& io_service, tcp::endpoint& endpoint) :
			m_io_service(io_service), m_tcp_connection(
					connection_ptr(new tcp_connection(io_service))), m_endpoint(
					endpoint) {
		m_isConnected = false;
		m_quit = false;
		// Connection to server
		tcp::socket& sock = m_tcp_connection->socket();
		sock.async_connect(endpoint,
				boost::bind(&api_client::handle_connect, this,
						boost::asio::placeholders::error));

	}

	/*!
	 * @brief Send data
	 * @param msg message to send
	 */
	void write(api_message& msg) {
		m_tcp_connection->async_write(msg,
				boost::bind(&api_client::handle_write, this,
						boost::asio::placeholders::error));
	}

	/*!
	 * @brief Close the connection
	 *
	 * Manually close the connection
	 */
	void close() {
		m_quit = true;

		m_io_service.post(boost::bind(&api_client::do_close, this));

		m_isConnected = false;

	}

	/*!
	 * @brief Returns whether we are connected or not to the phrCore
	 * @return true if connected, false otherwise
	 */
	bool isConnected() {
		return m_isConnected;
	}

private:

	/*!
	 * @brief Callback method when connection established
	 *
	 *  If the connection fails, we wait for a second, and try
	 *  again.
	 *
	 * @param error Boost error code
	 */
	void handle_connect(const boost::system::error_code& error) {
		if (!error) {
			m_isConnected = true;
			m_tcp_connection->async_read(m_message_read,
					boost::bind(&api_client::handle_read, this,
							boost::asio::placeholders::error));
		} else {
			if (!m_quit) {
				m_isConnected = false;
				sleep(1);
				tcp::socket& sock = m_tcp_connection->socket();
				sock.async_connect(m_endpoint,
						boost::bind(&api_client::handle_connect, this,
								boost::asio::placeholders::error));
			}
		}
	}

	/*!
	 * @brief Callback method when data is received
	 *
	 * If no error, we notify all or observers (only 1 in principle..)
	 * Otherwise, we try to reconnect
	 *
	 * @param error Boost error code
	 */
	void handle_read(const boost::system::error_code& error) {
		if (!error) {
			notify(m_message_read);

			m_message_read.reset();
			m_tcp_connection->async_read(m_message_read,
					boost::bind(&api_client::handle_read, this,
							boost::asio::placeholders::error));
		} else {
			do_close();
			// Try to reconnect
			if (m_isConnected == true) {
				m_isConnected = false;
				tcp::socket& sock = m_tcp_connection->socket();
				sock.async_connect(m_endpoint,
						boost::bind(&api_client::handle_connect, this,
								boost::asio::placeholders::error));
			}

		}
	}

	/*!
	 * @brief Callback method when writing data
	 *
	 * If an error occurs, we disconnect
	 *
	 * @param error Boost error code
	 */
	void handle_write(const boost::system::error_code& error) {
		if (error) {
			do_close();
		}
	}

	/*!
	 * Close the tcp connection
	 */
	void do_close() {
		m_tcp_connection->socket().close();
	}

private:
	boost::asio::io_service& m_io_service; //!< Reference to the boost::asio::io_service we run on
//	boost::asio::io_service io_service;
	connection_ptr m_tcp_connection; //!< shared pointer to the tcp connection

	tcp::endpoint& m_endpoint; //!< endpoint we are trying to connect to

	api_message m_message_read; //!< Message we receive
//	std::string m_login;
	bool m_isConnected; //!< Flag for the status of the connection
	bool m_quit; //!< Flag set to true when we want to disconnect

};

/*!
 * @class AbstractPhronesisClient
 *
 * This class is a "template" for all the classes that
 * want to implement the API. They will have to implement
 * the update method, which is the callback method
 * every time something changes in the phrCore
 */
class AbstractPhronesisClient: public Observer {
protected:
	api_client * m_c; //!< Pointer to the api_client that manages the connection
	bool m_quit; //!< Flag set to true when we want to quit
	std::string m_serverAddr; //!< Address of the server that runs the phrCore
	std::string m_port; //!< Port at which to connect the phrCore

	void sendOneMessage(api_message msg) {
		boost::asio::io_service io_service;

		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(tcp::v4(), m_serverAddr,
				m_port);
		boost::asio::ip::tcp::resolver::iterator iter;
		try {
			iter = resolver.resolve(query);
		} catch (boost::system::system_error & e) {
			throw std::runtime_error(e.what());
		}
		boost::asio::ip::tcp::resolver::iterator end; // End marker.
		boost::asio::ip::tcp::endpoint endpoint = *iter;
		while (iter != end) {
			endpoint = *iter++;
		}

		api_client c(io_service, endpoint);
		c.add_observer(this);

		boost::thread t(
				boost::bind(&boost::asio::io_service::run, &io_service));
		sleep(1);
		c.write(msg);
		c.close(); // Ferme la connection
		t.join(); // On rejoint le thread
	}

public:

	/*!
	 * @brief Constructor
	 *
	 */
	AbstractPhronesisClient() {
		m_c = NULL;
		m_quit = false;
	}

	/*!
	 * @brief Constructor
	 *
	 * This constructor sets the address and the port of the phrCore
	 * It is then possible to use the method defaultRun
	 *
	 * @param serverAddr Address of the server
	 * @param port Port to contact the phrCore
	 */
	AbstractPhronesisClient(std::string serverAddr, std::string port) {
		m_c = NULL;
		m_quit = false;
		m_serverAddr = serverAddr;
		m_port = port;
	}

	/*!
	 * @brief Checks whether we are connected to the phrCore
	 * @return True if we are connected, false otherwise
	 */
	bool isConnected() {
		if (m_c)
			return m_c->isConnected();
		else
			return false;
	}

	/*!
	 * @brief Callback method when the phrCore changes
	 *
	 * When the phrCore changes, the api_client receives a message
	 * and then call this method.
	 *
	 * It is pure virtual, all the class that will use the api
	 * will have to implement it to deal with the comming messages
	 *
	 * @param msg message received
	 */
	virtual void update(api_message msg) {
		throw std::runtime_error("Not implemented. Implement me!");
	}

	/*!
	 * @brief Answer a question
	 * @param id Index of the question
	 * @param answer Answer to give
	 */
	void answerQuestion(int id, std::string answer) {
		if (m_c) {
			api_message msg(api_message::ANSWER_QUESTION, id, answer);
			m_c->write(msg);
		}
	}

	/*!
	 * @brief Acknowledge a message
	 * @param id Index of the message
	 */
	void ackMessage(int id) {
		if (m_c) {
			api_message msg(api_message::ACK_MSG, id, "");
			m_c->write(msg);
		}
	}

	/*!
	 * @brief Give a problem list
	 * @param pb List of problems (separated by space or semi-column)
	 */
	void giveProblemList(std::string pb) {
		if (m_c) {
			api_message msg(api_message::GIVE_PB, -1, pb);
			m_c->write(msg);
		}
	}

	/*!
	 * @brief Set the value of a veto
	 * @param vetoName name of the veto
	 * @param vetoValue value of the veto
	 */
	void setVeto(std::string vetoName, bool vetoValue) {
		api_message msg(api_message::SET_VETO, vetoValue, vetoName);
		if (m_c) {
			m_c->write(msg);
		} else {
			sendOneMessage(msg);
		}
	}

	/*!
	 * @brief Infinite loop for the thread
	 *
	 * This method is called after all the connection happened by run()
	 * It exits only when the "quit flag" is set to true.
	 *
	 * @param c Reference to the client
	 */
	virtual void loop(api_client & c) {
		while (!m_quit) {
			sleep(1);
		}
	}

	/*!
	 * Disconnect and stop the threads
	 */
	void quit() {
		m_quit = true;
	}

	/*!
	 * @brief Run with the arguments given to the constructor
	 *
	 * For technical reasons, it was not possible to give
	 * default arguments to the run method (because of the boost::thread)
	 * So this method just call run() with the arguments given to the constructor
	 */
	virtual void defaultRun() {
		run(m_serverAddr, m_port);
	}

	/*!
	 * @brief Run the api
	 *
	 * Create the api_client, add ourselve to the observer list,
	 * start the thread of the io_service and enter loop().
	 * This method should be started in a thread in order not to be blocking.
	 *
	 * @param serverAddr Address of the server
	 * @param port Port to contact the phrCore
	 */
	virtual void run(std::string serverAddr, std::string port) {

		boost::asio::io_service io_service;

		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(tcp::v4(), serverAddr,
				port);
		boost::asio::ip::tcp::resolver::iterator iter;
		try {
			iter = resolver.resolve(query);
		} catch (boost::system::system_error & e) {
			throw std::runtime_error(e.what());
		}
		boost::asio::ip::tcp::resolver::iterator end; // End marker.
		boost::asio::ip::tcp::endpoint endpoint = *iter;
		while (iter != end) {
			endpoint = *iter++;
		}

		api_client c(io_service, endpoint);
		c.add_observer(this);
		m_c = &c;

		boost::thread t(
				boost::bind(&boost::asio::io_service::run, &io_service));

		loop(c);
		std::cout << "out of loop" << std::endl;
		c.close();
		t.join();

	}

};
