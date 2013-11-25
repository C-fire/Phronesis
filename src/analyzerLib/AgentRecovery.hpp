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


#ifndef AGENT_RECOVERY_HPP
#define AGENT_RECOVERY_HPP

/*!
 * \file AgentRecovery.hpp
 * \brief AgentRecovery and RecoveryHistory classes
 * \author Christophe Haen
 */

#include <iostream>
#include <vector>
#include <sstream>

#include "Agent.hpp"
#include "RecoveryUnit.hpp"

using namespace std;

/*!
 * \class AgentRecovery
 * \brief Abstract class for the recoveries of the different Agent
 *
 *  It includes all what is necessary to solve all the problems on one particular Agent
 */
class AgentRecovery {
protected:

	Agent * m_agent; /*!< pointer to the Agent that we want to recover */

	vector<RecoveryUnit> m_recoveryUnits; /*!< the list of RecoveryUnit that needs to be applied to this Agent */

	/*!
	 *  \brief Analyze the Agent to find the actions required on this particular Agent
	 *
	 *	This method checks what are the problems found on the Agent, and associate every problem
	 *	to a solution. The solution is only local to the problem, and does not take into account what the
	 *	modifications imply for the others
	 */
	virtual void analyzeAgent() = 0;

	/*!
	 *  \brief Returns the command to execute
	 *
	 *  Depending on the action to be taken,
	 *  this method 'formalizes" the command to execute
	 *
	 *  \return The command to execute
	 *
	 */
	virtual string getCommand(recoveryAction action) const = 0;

public:

	/*!
	 *  \brief Constructor
	 *
	 *  Base constructor of the AgentRecovery class
	 */
	AgentRecovery();

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the AgentRecovery class
	 *
	 *  \param[in] agent : pointer to the Agent we want to work on
	 */
	AgentRecovery(Agent * agent);

	/*!
	 *  \brief Constructor
	 *
	 * Constructor of the AgentRecovery class
	 *
	 * \param[in] agent : pointer to the Agent we want to work on
	 * \param[in] action : recoveryAction that we want to apply to the Agent
	 */
	AgentRecovery(Agent * agent, recoveryAction action);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the AgentRecovery class
	 */
	virtual ~AgentRecovery();

	/*!
	 * @brief Copy constructor
	 * @param source source AgentRecovery
	 */
	AgentRecovery(const AgentRecovery & source);

	/*!
	 * @brief Assignment operator
	 * @param source source AgentRecovery
	 * @return *this
	 */
	AgentRecovery & operator=(const AgentRecovery & source);

	/*!
	 *  \brief Getter for the RecoveryUnit list  needed to solve the problems on the Agent
	 *
	 *  \return vector of RecoveryUnit
	 */
	vector<RecoveryUnit> getRecoveryUnits() const;

	/*!
	 * @brief Getter for the Agent
	 * @return The Agent we are trying to recover
	 */
	Agent * getAgent() const;

};

/*!
 * @class RecoveryHistory
 * @brief Reminder of previous consequences when attempting an action
 */
class RecoveryHistory {

private:

	std::string m_date; /*!< Date at which the recovery took place */
	std::string m_time; /*!< Time at which the recovery took place */
	std::string m_responsible; /*!< Name of the responsible agent that was targeted by the recovery actions */
	std::vector<std::string> m_problemsBefore; /*!< List of Coordinators that were faulty before taking the action */
	std::vector<std::string> m_problemsAfter; /*!< List of Coordinators that were faulty after taking the action */
	std::vector<std::pair<std::string, std::string> > m_actions; /*!< List of actions taken, and on which MetaAgent (MetaAgent name, action) */
	std::string m_userEntry; /*!< Comment left by the User */

public:
	/*!
	 * @brief Default constructor
	 */
	RecoveryHistory() {

	}



	/*!
	 * @brief Constructor
	 * @param date Date at which the recovery took place
	 * @param time Time at which the recovery took place
	 * @param responsible Name of the responsible agent that was targeted by the recovery actions
	 * @param userEntry Comment entered by the user
	 */
	RecoveryHistory(std::string date, std::string time, std::string responsible,
			std::string userEntry) {
		m_date = date;
		m_time = time;
		m_responsible = responsible;
		m_userEntry = userEntry;

	}

	/*!
	 * @brief Copy constructor
	 * @param src RecoveryHistory instance to copy
	 */
	RecoveryHistory(const RecoveryHistory & src) {
		m_date = src.m_date;
		m_time = src.m_time;
		m_responsible = src.m_responsible;
		m_problemsBefore = src.m_problemsBefore;
		m_problemsAfter = src.m_problemsAfter;
		m_actions = src.m_actions;
		m_userEntry = src.m_userEntry;
	}

	/*!
	 * @brief Assignment operator
	 *
	 * @param src RecoveryHistory instance to copy
	 * @return Reference on this
	 */
	RecoveryHistory & operator=(const RecoveryHistory & src) {
		m_date = src.m_date;
		m_time = src.m_time;
		m_responsible = src.m_responsible;
		m_problemsBefore = src.m_problemsBefore;
		m_problemsAfter = src.m_problemsAfter;
		m_actions = src.m_actions;
		m_userEntry = src.m_userEntry;

		return (*this);
	}

	/*!
	 * @brief Destructor
	 */
	virtual ~RecoveryHistory() {
	}
	;

	/*!
	 * @brief Getter for the date
	 * @return Date at which the recovery took place
	 */
	const std::string& getDate() const {
		return m_date;
	}

	/*!
	 * @brief Getter for the list of problems before the recovery
	 * @return List of faulty Coordinators before the recovery
	 */
	const std::vector<std::string>& getProblemsBefore() const {
		return m_problemsBefore;
	}

	/*!
	 * @brief Getter for the time
	 * @return Time at which the recovery took place
	 */
	const std::string& getTime() const {
		return m_time;
	}

	/*!
	 * @brief Getter for the user comment
	 * @return User comment
	 */
	const std::string& getUserEntry() const {
		return m_userEntry;
	}

	/*!
	 * @brief Getter for the action taken
	 *
	 * The return is a vector of pair, where the first element is the MetaAgent name,
	 * and the second is the action taken
	 *
	 * @return List of actions taken
	 */
	const std::vector<std::pair<std::string, std::string> >& getActions() const {
		return m_actions;
	}

	/*!
	 * @brief Setter for the actions taken
	 * @param actions List of actions taken
	 */
	void setActions(
			const std::vector<std::pair<std::string, std::string> >& actions) {
		m_actions = actions;
	}

//	void setDate(const std::string& date) {
//		m_date = date;
//	}

	/*!
	 * @brief Getter for the list of problems after the recovery
	 * @return List of faulty Coordinators after the recovery
	 */
	const std::vector<std::string>& getProblemsAfter() const {
		return m_problemsAfter;
	}

	/*!
	 * @brief Setter for the list of problems after the recovery
	 * @param problemsAfter List of faulty Coordinators after the recovery
	 */
	void setProblemsAfter(const std::vector<std::string>& problemsAfter) {
		m_problemsAfter = problemsAfter;
	}

	/*!
	 * @brief Setter for the list of problems before the recovery
	 * @param problemsBefore List of faulty Coordinators before the recovery
	 */
	void setProblemsBefore(const std::vector<std::string>& problemsBefore) {
		m_problemsBefore = problemsBefore;
	}

//	void setTime(const std::string& time) {
//		m_time = time;
//	}

//	void setUserEntry(const std::string& userEntry) {
//		m_userEntry = userEntry;
//	}

	/*!
	 * @brief Getter for the responsible MetaAgent
	 * @return Name of the MetaAgent that we want to recover
	 */
	const std::string& getResponsible() const {
		return m_responsible;
	}

//	void setResponsible(const std::string& responsible) {
//		m_responsible = responsible;
//	}

	/*!
	 * @brief Stream operator
	 * @param out Stream in which to write
	 * @param hist RecoveryHistory to stream into out
	 * @return The stream "out" passed as parameter
	 */
	friend ostream& operator<<(ostream& out, const RecoveryHistory& hist) {
		out << "Date : " << hist.m_date << std::endl;
		out << "Time : " << hist.m_time << std::endl;
		out << "Actions : " << std::endl;
		for (std::vector<std::pair<std::string, std::string> >::const_iterator it =
				hist.m_actions.begin(); it != hist.m_actions.end(); it++) {
			out << "\t" << (*it).first << " : " << (*it).second << std::endl;
		}
		out << "Problems before : " << std::endl;
		for (std::vector<std::string>::const_iterator it =
				hist.m_problemsBefore.begin();
				it != hist.m_problemsBefore.end(); it++) {
			out << "\t" << *it << std::endl;
		}
		out << "Problems after : " << std::endl;
		for (std::vector<std::string>::const_iterator it =
				hist.m_problemsAfter.begin(); it != hist.m_problemsAfter.end();
				it++) {
			out << "\t" << *it << std::endl;
		}
		out << "User entry : " << std::endl;
		out << hist.m_userEntry << std::endl;
		return out;
	}
};

#endif
