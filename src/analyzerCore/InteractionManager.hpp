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


#ifndef INTERACTION_MANAGER_HPP
#define INTERACTION_MANAGER_HPP

/*!
 * \file InteractionManager.hpp
 * \brief InteractionManager class, and all the XMLRPC classes
 * \author Christophe Haen
 */

#include <string>
#include <iostream>
#include <queue>
#include <exception>
#include <stdexcept>
#include <set>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/list.hpp>

#include "InteractionManagerAbstract.hpp"
#include "SyncManager.hpp"
#include "UserInteraction.hpp"
#include "UserQuestion.hpp"
#include "UserMsg.hpp"
#include "Message.hpp"
#include <syslog.h>
#include "connection.hpp"
#include "VetoManager.hpp"

#define NQUESTIONS 100	/*!< maximum number of UserQuestion */
#define NMSG 100	/*!< maximum number of UserMsg */

using namespace std;

extern bool stopReceived; /*!< Boolean used as an exit condition */

/*!
 * \class InteractionManager
 * \brief The InteractionManager class is responsible for all the interaction between the user and the software.
 *
 *	The InteractionManager runs in a separate thread.
 *	It keeps arrays for the UserQuestions, UserMsg, and logs.
 *	It also manages all the mutex involved to wait for the user answers.
 *
 *  There should normally be only one instance of the AnalyzerEngine.
 */
class InteractionManager: public InteractionManagerAbstract {
private:

	SyncManager * syncManager; /*!< pointer to the  SyncManager */
	pthread_mutex_t m_questionMutex[NQUESTIONS]; /*!< Array of mutex for the UserQuestion. The mutex allows us to wait for the User answer  */
	UserQuestion * m_questions[NQUESTIONS]; /*!< Array of pointer to the UserQuestion */
	YesNoQuestion * m_yn_questions[NQUESTIONS]; /*!< Array of pointer to the YesNoQuestion */
	ProblemQuestion * m_pb_questions[NQUESTIONS]; /*!< Array of pointer to the problemQuestion */
	UserMsg * m_msg[NMSG]; /*!< Array of pointer to the UserMsg */
	queue<string> m_log; /*!< queue containing logs (size of 100)  */
	VetoManager * m_vManager; /*!< pointer to the VetoManager */
	int m_pbListId; /*!< This id contains the id of the question that requires the problem list. */
	std::string m_currentProblemList; /*!< This string contains the list of problems currently being investigated */

	/*!
	 *	\brief Add a UserQuestion
	 *
	 *	This method is called to add a UserQuestion in the array.
	 *
	 * @param question pointer to the UserQuestion to add
	 * @return the index where the UserQuestion is stored. If there is no place, it returns -1
	 */
	int addQuestion(UserQuestion * question);

	/*!
	 * \brief Ask a UserQuestion to the user
	 *
	 * This method adds the UserQuestion in the array, and lock the mutex to wait for the user answer.
	 * @param question	UserQuestion to ask
	 * @param qId the id of the question
	 * @param pbList set to true if the question concerns the problem list
	 *
	 */
	void ask(UserQuestion * question, int qId, bool pbList = false);

public:

	/*!
	 * \brief Constructor
	 *
	 * The constructor initializes the arrays and mutexes.
	 *
	 * @param syncManager pointer to the SyncManager
	 */
	InteractionManager(SyncManager * syncManager);

	/*!
	 * \brief Destructor
	 *
	 *	It frees only the array of messages.
	 *	The array of UserQuestion is freed by the one that asked.
	 */
	~InteractionManager();

	/*!
	 *	\brief Main method of the thread
	 *
	 *	This function is an infinite loop exiting when stopReceived is set to true.
	 *	Before exiting the thread, the method releases all the mutexes it has.
	 */
	void run();

	/*!
	 * \brief Ask a YesNoQuestion to the user
	 *
	 * This method adds the YesNoQuestion in the array,and call the ask(UserQuestion *, bool) method.
	 * @param question	YesNoQuestion to ask
	 */
	void ask(YesNoQuestion * question);

	/*!
	 * \brief Ask a problemQuestion to the user
	 *
	 * This method adds the ProblemQuestion in the array,and call the ask(UserQuestion *, bool) method.
	 * @param question	ProblemQuestion to ask
	 */
	void ask(ProblemQuestion * question);

	/*!
	 * \brief Send a message to the user
	 *
	 *	Add a UserMsg in the array.
	 *
	 * @param msg UserMsg to send.
	 */
	void addMsg(UserMsg * msg);

	/*!
	 * Add a log message
	 *
	 * Pushes a log message in the queue. If there is more than 100,
	 * it removes the oldest one.
	 *
	 * @param log Message to add in the log.
	 */
	void addLog(string log);

	/*!
	 * Set the answer to a UserQuestion
	 *
	 * \throw Exception if the UserQuestion::setAnswer throws a std::runtime_error
	 *
	 * @param i index of the question in the array
	 * @param answer answer to the question
	 */
	void setAnswer(int i, string answer);

	/*!
	 * Shortcut to give an answer concerning the pb list, without knowing its id
	 *
	 * \throw Exception if the UserQuestion::setAnswer throws a std::runtime_error
	 *
	 * @param pbList the list of problems;
	 */
	void giveProblemList(string pbList);

	/*!
	 * @brief Acknowledge a message
	 *
	 *	Removes an delete a message from the UserMsg array.
	 *
	 * @throw exception if there is no message at the index provided.
	 * @param id index of the UserMsg to acknowledge.
	 */
	void ackMsg(int id);

	/*!
	 * @brief Notify to observers the current list of problems being diagnosed
	 * @param currentProblemList list of problems being diagnosed
	 */
	void setCurrentProblemList(std::string currentProblemList);

	/*!
	 * @brief Return the current problem list
	 *
	 * It is used to notify new observers about the problems.
	 *
	 * @return Current problem list
	 */
	std::string getCurrentProblemList() const;

	/*!
	 * @brief Returns the list of Yes-or-No questions
	 * @return list of Y-N questions
	 */
	YesNoQuestion ** getYNQuestionList() {
		return m_yn_questions;
	}

	/*!
	 * @brief Returns the questions for problems
	 * @return List of problem questions
	 */
	ProblemQuestion ** getPbQuestionList() {
		return m_pb_questions;
	}

	/*!
	 * @brief Get the list of messages
	 * @return List of messages
	 */
	UserMsg ** getMsgList() {
		return m_msg;
	}

	/*!
	 * @brief return the current local time
	 * @return current local time
	 */
	std::string getCurrentTime() const;

	/*!
	 * @brief return the current local date
	 * @return current local date
	 */
	std::string getCurrentDate() const;

	boost::signals2::signal<void(const api_message& msg)> m_signal_notify; /*!< Boost signal used to notify the observers */

	/*!
	 * Set the value to a Veto
	 *
	 *
	 * @param name name of the Veto
	 * @param value value of the veto
	 */
	void setVeto(std::string name, bool value);

	/*!
	 * @brief set the VetoManager
	 * @param vm pointer to the VetoManager
	 */
	void setVetoManager(VetoManager * vm);

	/*!
	 * @brief returns the list of Veto
	 * @param vetoName name of the Veto. If not given, returns all the Veto
	 * @return map containing the Veto and values
	 */
	std::map<std::string, bool> getVeto(std::string vetoName = "");

	/*!
	 * @brief sends a statistic
	 *
	 * The value is by default 0. This means the statname is just a counter,
	 * up to the receiver to do whatever he wants with it.
	 *
	 * @param statName Name of the statistic
	 * @param value Value of the statistic
	 */
	void sendStatistic(std::string statName, int value = 0);

};

class api_client_pool;
class api_server;

typedef boost::shared_ptr<api_client_pool> api_client_pool_ptr; //!< boost::shared_ptr on the api_client_pool
typedef boost::weak_ptr<api_client_pool> api_client_pool_wptr; //!< boost::weak_ptr on the api_client_pool

class api_session;
typedef boost::shared_ptr<api_session> api_session_ptr; //!< boost::shared_ptr on the api_session

/*!
 * @class api_client_pool
 * Class that serves as a container for all the observers
 */
class api_client_pool {
public:
	/*!
	 * @brief Constructor
	 *
	 * @param server Reference to the api_server that accepts the connections
	 * @param im Reference to the InteractionManager
	 */
	api_client_pool(api_server& server, InteractionManager & im);

	/*!
	 * @brief Add a new participant to the pool
	 *
	 * When a new participant joins the pool, we give it the current
	 * status :
	 *  - all the y-n questions
	 *  - all the pb questions
	 *  - all the messages
	 *  - the current pb list
	 *  - the status of all the vetos
	 *
	 * All those messages are dated and timestamped "old"
	 *
	 * @param participant
	 */
	void join(api_session_ptr participant);

	/*!
	 * @brief Remove a participant from the list
	 *
	 * When an error occurs writing to a observer,
	 * we remove it from the list.
	 *
	 * @param participant api_session to remove
	 */
	void leave(api_session_ptr participant);

	/*!
	 * @brief Deliver a message to all the participants
	 *
	 * spam...
	 * @param msg Message to deliver
	 */
	void deliver(const api_message& msg);

	/*!
	 * @brief Callback method for the InteractionManager's signal
	 *
	 * This method is bind to the signal of the InteractionManager.
	 * When a method triggers this signal, OnNotice is called
	 * @param msg
	 */
	void OnNotice(const api_message& msg);

private:
	std::set<api_session_ptr> m_participants; //!< All the observers
	api_server& m_server;  //!< reference to the api_server
	InteractionManager & m_subject; //!< Reference to the InteractionManager
};

/*!
 * @class api_session
 *
 * This Class represents the connection session with an observer.
 * It works following the factory pattern to make sure that
 * we are only using api_session_ptr, which are boost::shared_ptr.
 */
class api_session: public boost::enable_shared_from_this<api_session> {
public:

	/*!
	 * @brief Destructor
	 */
	virtual ~api_session() {
	}

	/*!
	 * @brief Method to create a new api_session_ptr
	 * @param tcp_connection tcp_connection to use
	 * @param room room to join
	 * @param im reference to the interaction manager
	 * @return boost::shared_ptr to a new api_session
	 */
	static api_session_ptr create(connection_ptr tcp_connection,
			api_client_pool_ptr room, InteractionManager & im) {
		api_session_ptr session(new api_session(tcp_connection, room, im));

		session->wait_for_data();
		return session;
	}

	/*!
	 * @brief Delivier a message to the observer
	 * @param msg Message to be delivered
	 */
	void deliver(const api_message& msg);

private:
	/*!
	 * @brief Constructor
	 *
	 * It is called by the create method, and hidden from the user
	 *
	 * @param tcp_connection tcp_connection to use
	 * @param room room to join
	 * @param im reference to the interaction manager
	 */
	api_session(connection_ptr tcp_connection, api_client_pool_ptr room,
			InteractionManager & im);

	/*!
	 * @brief Waits for data to come from the observer
	 */
	void wait_for_data();

	/*!
	 * @brief Callback method when writing to a client
	 *
	 * If any problem happens while sending the data,
	 * we disconnect the observer and exit
	 *
	 * @param error Error code
	 */
	void handle_write(const boost::system::error_code& error);

	/*!
	 * @brief Callback method when getting data from the client
	 *
	 * We check the request of the observer, and take the appropriate
	 * actions (answer the request, or execute the task like giving problem list)
	 *
	 * @param error Error code
	 */
	void handle_read(const boost::system::error_code& error);

	connection_ptr m_tcp_connection; //!< shared pointer to a tcp_connection
	api_client_pool_wptr m_room; //!< weak pointer to the client pool
//	std::list<api_message> m_write_events;
	api_message m_message; //!< Message received from the client
	bool is_leaving; //!< flag set to true if the observer is logging out. Not really used in fact
	InteractionManager & m_subject; //!< reference to the InteractionManager
};

using boost::asio::ip::tcp;

/*!
 * @class api_server
 * Class to act as a server for all the API connections
 */
class api_server {
public:
	/*!
	 * @brief Constructor
	 * @param io_service boost::asio::io_service on which we are running
	 * @param endpoint Endpoint (port number) on which we listen
	 * @param im Reference to the InteractionManager
	 */
	api_server(boost::asio::io_service& io_service,
			const tcp::endpoint& endpoint, InteractionManager& im) :
			m_io_service(io_service), m_acceptor(io_service, endpoint), m_room(
					new api_client_pool(*this, im)), m_subject(im) {
		wait_for_connection();

	}

	/*!
	 * @brief Waits for connection
	 *
	 * When a new connection happens, handle_accept is called asynchronously
	 */
	void wait_for_connection() {
		connection_ptr new_connection(new tcp_connection(m_io_service));

		m_acceptor.async_accept(new_connection->socket(),
				boost::bind(&api_server::handle_accept, this,
						boost::asio::placeholders::error, new_connection));
	}

private:

	/*!
	 * @brief Callback method when a new connection is received
	 *
	 * If no error happened, we create a new api_session,
	 * add it to the api_client_pool and wait for other connections
	 *
	 * @param error Error number
	 * @param new_connection Shared pointer to a tcp_connection
	 */
	void handle_accept(const boost::system::error_code& error,
			connection_ptr new_connection) {
		if (!error) {
			api_session_ptr session = api_session::create(new_connection,
					m_room, m_subject);
			m_room->join(session);
			wait_for_connection();
		} else {
			// I SHOULD FIND A WAY TO NOTIFY THE LOGS
//			std::cerr << "Connection refusee" << std::endl;
		}
	}

	boost::asio::io_service& m_io_service; //!< boost::asio::io_service on which we run
	tcp::acceptor m_acceptor; //!< accepts connections
	api_client_pool_ptr m_room; //!< shared pointer to the api_client_room
	InteractionManager & m_subject; //!< reference to the interaction manager
};

/*!
 * @class LocalObserver
 *
 * Abstract class to represent observer that runs localy
 * (like the cout or the syslog)
 */
class LocalObserver {
public:
	/*!
	 * Constructor
	 * @param im Reference to the InteractionManager
	 */
	LocalObserver(InteractionManager & im) :
			m_subject(im) {
	}

	/*!
	 * @brief Connects to the InteractionManager
	 *
	 * Binds OnNotice method with the InteractionManager's signal
	 */
	void connect() {
		m_subject.m_signal_notify.connect(
				boost::bind(&LocalObserver::OnNotice, this, _1));
	}

	/*!
	 * @brief Callback method of the signal
	 *
	 * This method must be implemented by the classes that inherits from LocalObserver
	 * @param msg Message which is received
	 */
	virtual void OnNotice(const api_message& msg) = 0;

	/*!
	 * @brief Destructor
	 */
	virtual ~LocalObserver() {
	}

	/*!
	 * Infinite loop to run the thread.
	 * Exists when the stopReceived flag is set to true
	 */
	void run() {
		while (!stopReceived) {
			sleep(1);
		}
	}
private:
	InteractionManager & m_subject; //!< Reference to the InteractionManager
};

/*!
 * @class CoutObserver
 *
 * Class to output to the console the messages.
 * It is used when running in non-daemon mode
 */
class CoutObserver: public LocalObserver {
public:
	/*!
	 * @brief Constructor
	 * @param im Reference to the InteractionManager
	 */
	CoutObserver(InteractionManager & im) :
			LocalObserver(im) {
	}

	/*!
	 * Destructor
	 */
	virtual ~CoutObserver() {

	}

	/*!
	 * @brief Callback method of the signal
	 *
	 * Prints whatever is received on the screen
	 *
	 * @param msg Message received
	 */
	void OnNotice(const api_message& msg) {
		std::cout << msg.m_text << std::endl;
	}
};

/*!
 * @class SyslogObserver
 * Class used to dump the messages received into the syslog facility
 */
class SyslogObserver: public LocalObserver {
public:
	/*!
	 * @brief Constructor
	 * @param im Reference to the InteractionManager
	 */
	SyslogObserver(InteractionManager & im) :
			LocalObserver(im) {
	}

	/*!
	 * Destructor
	 */
	virtual ~SyslogObserver() {
		closelog();
	}

	/*!
	 * @brief Callback method of the signal
	 *
	 * Dumps the message received into the syslog facility
	 * (facility LOG_INFO so far. It should be changed to be
	 * more modular. On the todo list)
	 *
	 * @param msg Message to be dumped
	 */
	void OnNotice(const api_message& msg) {
		syslog(LOG_INFO, "%s", msg.m_text.c_str());

	}

};

#endif
