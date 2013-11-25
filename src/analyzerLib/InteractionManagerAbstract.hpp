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


#ifndef INTERACTION_MANAGER_ABSTRACT_HPP
#define INTERACTION_MANAGER_ABSTRACT_HPP

/*!
 * \file InteractionManagerAbstract.hpp
 * \brief InteractionManagerAbstract class
 * \author Christophe Haen
 */
#include <string>

#include "UserMsg.hpp"
#include "UserQuestion.hpp"

/*!
 * \class InteractionManagerAbstract
 * \brief The InteractionManagerAbstract class
 */
class InteractionManagerAbstract {
public:

	/*!
	 * \brief Destructor
	 *
	 *	It frees only the array of messages.
	 *	The array of UserQuestion is freed by the one that asked.
	 */
	virtual ~InteractionManagerAbstract() {
	}
	;

	/*!
	 *	\brief Main method of the thread
	 *
	 *	This function is an infinite loop exiting when stopReceived is set to true.
	 *	Before exiting the thread, the method releases all the mutexes it has.
	 */
	virtual void run() = 0;


	/*!
	 * \brief Ask a YesNoQuestion to the user
	 *
	 * This method adds the YesNoQuestion in the array,and call the ask(UserQuestion *, bool) method.
	 * @param question	YesNoQuestion to ask
	 */
	virtual void ask(YesNoQuestion * question) = 0;

	/*!
	 * \brief Ask a problemQuestion to the user
	 *
	 * This method adds the ProblemQuestion in the array,and call the ask(UserQuestion *, bool) method.
	 * @param question	ProblemQuestion to ask
	 */
	virtual void ask(ProblemQuestion * question) = 0;

	/*!
	 * \brief Send a message to the user
	 *
	 *	Add a UserMsg in the array.
	 *
	 * @param msg UserMsg to send.
	 */
	virtual void addMsg(UserMsg * msg) = 0;

	/*!
	 * Add a log message
	 *
	 * Pushes a log message in the queue. If there is more than 100,
	 * it removes the oldest one.
	 *
	 * @param log Message to add in the log.
	 */
	virtual void addLog(std::string log) = 0;

	/*!
	 * Set the answer to a UserQuestion
	 *
	 * \throw Exception if the UserQuestion::setAnswer throws a UserQuestionException
	 *
	 * @param i index of the question in the array
	 * @param answer answer to the question
	 */
	virtual void setAnswer(int i, std::string answer) = 0;

	/*!
	 * Shortcut to give an answer concerning the pb list, without knowing its id
	 *
	 * \throw Exception if the UserQuestion::setAnswer throws a UserQuestionException
	 *
	 * @param pbList the list of problems;
	 */
	virtual void giveProblemList(std::string pbList) = 0;


	/*!
	 * @brief Acknowledge a message
	 *
	 *	Removes an delete a message from the UserMsg array.
	 *
	 * @throw exception if there is no message at the index provided.
	 * @param id index of the UserMsg to acknowledge.
	 */
	virtual void ackMsg(int id) = 0;

	/*!
	 * @brief Sets the current problem list
	 * @param currentProblemList current problem list
	 */
	virtual void setCurrentProblemList(std::string currentProblemList) = 0;

	/*!
	 * @brief sends a statistic
	 *
	 * The value is by default 0. This means the statname is just a counter,
	 * up to the receiver to do whatever he wants with it.
	 *
	 * @param statName Name of the statistic
	 * @param value Value of the statistic
	 */
	virtual void sendStatistic(std::string statName, int value = 0) = 0;
};
#endif
