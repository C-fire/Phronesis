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


#ifndef META_AGENT_HPP
#define META_AGENT_HPP

/*!
 * \file MetaAgent.hpp
 * \brief MetaAgent class
 * This file defines the MetaAgent class, 2 operators for it (== and <)
 *  and a comparison function for their probability (sortMetaAgentByProba)
 * \author Christophe Haen
 */

#include<iostream>
#include <cstdlib>
#include <set>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "ProbabilityManager.hpp"
#include "Server.hpp"
#include "RecoveryEngineAbstract.hpp"
#include "InteractionManagerAbstract.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "connection.hpp"

using namespace std;

class ProbabilityManager;
class RecoveryEngineAbstract;

/*!
 * \class MetaAgent
 * \brief Abstract class for all the agent
 *
 *  The class is the abstract class for all the agents : the real ones (which inherit from Agent), and the coordinator.
 */
class MetaAgent {

protected:

	int m_id; /*!< Unique integer identifier */

	string m_name; /*!< Unique string identifier */

	ProbabilityManager * m_pm; /*!< Pointer to the probability manager */

	double m_proba; /*!< This is temporary variable just used for sorting.
	 It changes all the time, depending on the context.
	 It should never be used to get a probability */

	int m_weight; /*!<The same than m_proba. The weight is calculated from the dependency rules, and is used to sort MetaAgent according to rules.*/

	string m_cause; /*!< The string refining the cause of the problem on the agent */

	MetaAgent * m_responsibleChild; /*!< Pointer to the MetaAgent which is responsible for the problem being analyzed */

	int m_analyzeIterations; /*!< how many time we analyzed this entity in one aggregator::analyze process. It is used as a counter to avoid infinite loop*/

	InteractionManagerAbstract * m_iManager; //!< Pointer to the InteractionManager

	/*!
	 *  \brief printOperator
	 *
	 *  Output in a stream the content of the MetaAgent
	 *  \param[out] out : the stream
	 *  \return the stream
	 */
	virtual ostream& printOperator(ostream& out) const;

	static int m_deep; /*!< static variable used to check how deep in the tree we went when analyzing. Could be added to debug mode...*/

	/*!
	 * @brief Query the remote agent
	 *
	 * This method will send the given Request to a remote Agent
	 * using a Client class.
	 *
	 * @throw std::runtime_error If a timeout has occurred
	 * @throw std::runtime_error If an exception has occurred in the client
	 * @throw std::runtime_error If we cannot find on the network the address given as parameter
	 *
	 * @param address Address of the server
	 * @param tm Request
	 * @return The answer of the Agent
	 */
	template<class T>
	T queryRemoteAgent(std::string address, Request tm) {

		T answer;

		boost::asio::io_service io_service;

		// First we resolve the address, because it might be a DNS name
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(tcp::v4(), address,
				MetaAgent::agent_port);
		boost::asio::ip::tcp::resolver::iterator iter;
		try {
			iter = resolver.resolve(query);
		} catch (boost::system::system_error & e) {
			throw std::runtime_error(e.what());
		}
		boost::asio::ip::tcp::resolver::iterator end; // End marker.
		boost::asio::ip::tcp::endpoint endpoint = *iter;

		// If there are several address, we take the last one. Just so...
		while (iter != end) {
			endpoint = *iter++;
		}

		int timeout = MetaAgent::agent_timeout;

		// Create the client with the io_service and endpoint freshly created
		client<T> c(io_service, endpoint, timeout);

		// Run the io_service in a thread (needed!)
		boost::thread t(
				boost::bind(&boost::asio::io_service::run, &io_service));

		// Send the request to the client
		c.write(tm);

		// We wait until we have either received an answer, hit the timeout or an exception
		while (!c.m_receivedAnswer && !c.m_timeout && !c.m_exception)
			;

		// If we had an exception in the client, we copy it, and throw it
		if (c.m_exception) {
			//We have to force the copy of the string exception otherwise it does not work (copy on write mechanism of the std::string...)
			char * c_str = (char *) malloc(
					sizeof(char) * (c.m_exception_msg.size() + 1));
			strcpy(c_str, c.m_exception_msg.c_str());
			std::string exc(c_str);
			free(c_str);
			c.close();
			io_service.stop();
			t.join();
			throw std::runtime_error(exc);

		}

		// If we have a timeout and no answer received, we throw an error
		if (c.m_timeout) {
			if (!c.m_receivedAnswer) {
				stringstream ss;
				ss << "Timeout for request \"" << tm << "\" on " << address;
				c.close();
				io_service.stop();
				t.join();

				throw std::runtime_error(ss.str());
			}
		}

		// Get the answer
		answer = c.getAnswer();

		// Close the connection and end the thread.
		c.close();
		t.join();

		return answer;

	}

	bool m_remoteAgentPb; //!< Flag set to true when we have a communication problem with the remote agent

	/*!
	 * @brief Sets the cause of a problem
	 *
	 * Fills in the m_cause attribute and notify the InteractinManager
	 *
	 * @param cause cause of the problem
	 */
	void setCause(std::string cause);

public:
	/*!
	 *  \brief Constructor
	 *
	 *  Base constructor of the MetaAgent class
	 */
	MetaAgent();

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the MetaAgent class
	 *  \param[in] id : integer unique identifier for the agent
	 *  \param[in] name : name of the agent
	 */
	MetaAgent(int id, string name);

	/*!
	 *  \brief Copy constructor
	 *
	 *  Copy constructor of the MetaAgent class
	 *  \param[in] source : reference to the MetaAgent that we will copy
	 */
	MetaAgent(const MetaAgent & source);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the MetaAgent class
	 */
	virtual ~MetaAgent();

	/*!
	 *  \brief Analysis of the MetaAgent state
	 *
	 *  This methods will have the MetaAgent analyze its state.
	 *
	 *  \return true if there is a problem, false otherwise
	 */
	virtual bool analyze();

	/*!
	 *  \brief Set the pointer to the ProbabilityManager
	 *
	 *  \param[in] pm : pointer to the ProbabilityManager
	 */
	virtual void setPManager(ProbabilityManager * pm);

	/*!
	 *  \brief Add a child to the children tree
	 *
	 *  This is only valid for the Coordinator
	 *
	 *  \param[in] child : pointer to the child MetaAgent we are adding to the tree
	 */
	virtual void addChild(MetaAgent * child);

	/*!
	 *  \brief Getter for the name of the MetaAgent
	 *
	 *  Returns the name of the MetaAgent
	 *
	 *  \return the name of the MetaAgent
	 */
	virtual string getName() const;

	/*!
	 *  \brief Getter for the id of the MetaAgent
	 *
	 *  Returns the id of the MetaAgent
	 *
	 *  \return the id of the MetaAgent
	 */
	virtual int getId() const;

	/*!
	 *  \brief Getter for the temporary probability value of the MetaAgent
	 *
	 *	This method returns the m_proba attribute, which is only valid for one analysis.
	 *  It should never be used to get a probability of occurrence.
	 *  It is used only by the sortMetaAgentByProba function.
	 *
	 *  \return  the temporary probability of occurrence
	 */
	virtual double getProba() const;

	/*!
	 *  \brief Setter for the temporary probability value of the MetaAgent
	 *
	 *	This method sets the m_proba attribute
	 *
	 *  \param p : the current probability value
	 */
	virtual void setProba(double p);

	/*!
	 *  \brief Getter for the temporary weight value of the MetaAgent
	 *
	 *	This method returns the m_weight attribute, which is only valid for one analysis.
	 *  It should never be used to get the weight.
	 *  It is used only by the sortAccordingRUles function.
	 *
	 *  \return the temporary weight of the MetaAgent
	 */
	virtual int getWeight() const;

	/*!
	 *  \brief Setter for the temporary weight of the MetaAgent
	 *
	 *	This method sets the m_weight attribute
	 *
	 *  \param w : the current weight value
	 */
	virtual void setWeight(int w);

	/*!
	 *  \brief Getter for the cause of the problem
	 *
	 *  Returns the cause of the problem on the MetaAgent
	 *
	 *  \return the cause of the problem
	 */
	virtual string getCause() const;

	/*!
	 *  \brief Output stream operator
	 *
	 *  The output stream operator. It calls the printOperator method
	 *
	 *	\param[in] ma : reference on the MetaAgent you want to write in the stream
	 *	\param[in,out] out : the output stream
	 *  \return The output stream
	 */
	friend ostream& operator<<(ostream& out, const MetaAgent& ma) {
		return ma.printOperator(out);
	}

	/*!
	 *  \brief Getter for the list of Server related to the MetaAgent
	 *
	 *  The method returns all the Servers that are related one way or another
	 *  to the MetaAgent.
	 *
	 *  \return set of Server pointer
	 */
	virtual set<Server *> getInvolvedServers() const;

	/*!
	 *  \brief Recursively updates the occurrence probabilities of the children of this MetaAgent
	 *
	 *  Once the faulty MetaAgent has been found, one needs to update the occurrence probability.
	 *  This method does this by calling the updateProbability method of the ProbabilityManager
	 *  that will update the probability of going from this MetaAgent to the MetaAgent child that leads
	 *  to the faulty MetaAgent.
	 */
	virtual void updateProbability();

	/*!
	 *  \brief Getter for the iterations counter
	 *
	 *  Returns m_analyzeIterations, which represents how many time we analyzed this entity in one aggregator::analyze process
	 *
	 *  \return the amount of iterations
	 */
	virtual int getAnalyzeIterations() const;

	/*!
	 *  \brief Returns the server on which the Agent is running
	 *
	 *  The method returns the Server on which the Agent is running
	 *
	 * \return Pointer to the Server
	 */
	virtual Server * getServer() const;

	/*!
	 *  \brief Returns the MetaAgent responsible for the problem
	 *
	 *  The method returns m_responsibleChild
	 *
	 * \return Pointer to the MetaAgent responsible for the problem
	 */
	virtual MetaAgent * getResponsibleChild() const;

	static int TMP_attempts; /*!< static variable used to check how many retry we needed to find the problem. Could be added to debug mode...*/

	/*!
	 * \brief Trigger the recovery process for the MetaAgent
	 *
	 *	This follows the Inversion of Control pattern.
	 *	This method just calls the appropriate RecoveryEngine::recover method
	 *
	 * @param re pointer to the RecoveryEngine
	 * @return result of the recover
	 */
	virtual bool triggerRecover(RecoveryEngineAbstract * re) = 0;

	/*!
	 * @brief Setter for the InteractionManager
	 * @param iManager pointer to the InteractionManager
	 */
	void setInteractionManager(InteractionManagerAbstract * iManager);

	/*!
	 * @brief Set the responsible for the problem
	 * @param responsible The responsible child
	 */
	void setResponsibleChild(MetaAgent * responsible);

	/*!
	 * @brief Checks whether the server is pingable
	 *
	 * This method is supposed to be called in the Aggregator
	 * only when we know that we have an environment problem.
	 * It is overloaded only by the EnvironmentAgent that actually
	 * made the check and stored it. The behavior of this method
	 * is to return false if we are the responsible (should never happen), or
	 * to call isServerPingable on the responsibleChild
	 *
	 * @return True if the Server is pingable, false otherwise
	 */
	virtual bool isServerPingable();

	/*!
	 * @brief Reinitialize the values that are valid only during one diagnosis
	 *
	 * It is the case for the responsibleChild and the analyzeIteration.
	 */
	virtual void reinitializeStatus();

	/*!
	 * @brief Returns whether the environment is responsible (always false)
	 *
	 * The idea is that when we roll down the chain of responsible agents,
	 * we receive true only if the final responsible is a EnvironmentAgent.
	 * So this method always return false, but it is overloaded in EnvironmentAgent.
	 * @return false
	 */
	virtual bool isEnvironmentResponsible();

	/*!
	 * @brief Assignment operator
	 * @param source the MetaAgent we copy from
	 * @return *this
	 */
	MetaAgent & operator=(const MetaAgent& source);

	static std::string agent_port; /*!< static variable used to indicate on which port to connect to the remote agent. Default is 7171 */
	static int agent_timeout; /*!< Value in second after which we consider that the agent request has timed out */

};

/*!
 *  \brief Compare the probability of 2 MetaAgents that are in the same tree level
 *
 *	\param[in] a : pointer to the first MetaAgent
 *	\param[in] b : pointer to the second MetaAgent
 *
 *	\return true if probability(a) > probability(b), false otherwise
 */
bool sortMetaAgentByProba(MetaAgent * a, MetaAgent * b);

/*!
 *  \brief MetaAgent == operator
 *
 *	Compares if the names are the same
 *
 *	\param[in] a : reference to the first MetaAgent
 *	\param[in] b : reference to the second MetaAgent
 *
 *	\return true if a and b have the same name, false otherwise
 */
bool operator==(const MetaAgent &a, const MetaAgent &b);

/*!
 * 	\fn operator<(const MetaAgent &a, const MetaAgent &b);
 *  \brief MetaAgent < operator
 *
 *	Compares if the name of a is smaller (!!!) than the name of b
 *
 *	\param[in] a : reference to the first MetaAgent
 *	\param[in] b : reference to the second MetaAgent
 *
 *	\return true if name(a)< name(b), false otherwise
 */
bool operator<(const MetaAgent &a, const MetaAgent &b);

#endif
