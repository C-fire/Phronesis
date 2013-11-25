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


#ifndef USER_QUESTION_HPP
#define USER_QUESTION_HPP

/*!
 * \file UserQuestion.hpp
 * \brief UserQuestion classes
 * \author Christophe Haen
 */

#include <iostream>
#include <algorithm>
#include <exception>
#include <stdexcept>
#include <assert.h>

#include "UserInteraction.hpp"

using namespace std;

/*!
 * \class UserQuestion
 * \brief Abstract class for all the questions send to the user
 *
 *  The class is the abstract class for all the questions. They all expect a reaction from the user
 */
class UserQuestion: public UserInteraction {

protected:
	string m_question; /*!< the question to ask to the user */
	string m_answer; /*!< the answer given by the user */

	/*!
	 *  \brief Check if the answer given by the user meets the expectation
	 */
	virtual void checkUserAction() = 0;

public:

	/*!
	 *  \brief Constructor
	 *
	 *	\param[in] question : question to ask to the user
	 *
	 */
	UserQuestion(string question);

	/*!
	 * \brief Copy constructor
	 * @param source source UserQuestion
	 */
	UserQuestion(const UserQuestion & source);

	/*!
	 * @brief Assignment operator
	 * @param source source UserQuestion
	 * @return *this
	 */
	UserQuestion & operator=(const UserQuestion & source);

	/*!
	 *  \brief Destructor
	 */
	virtual ~UserQuestion();

	/*!
	 *  \brief Getter for the question
	 *
	 *	Returns m_question
	 *
	 *	\return Question to the user
	 */
	virtual string getQuestion() const;

	/*!
	 *  \brief Setter for the answer
	 *
	 *	set m_answer
	 *
	 *	\param[in] answer : Answer of the user
	 */
	virtual void setAnswer(string answer);

	/*!
	 *  \brief Getter for the answer
	 *
	 *	Returns m_answer
	 *
	 *	\return Answer of the user
	 */
	virtual string getAnswer() const;

    /*!
     * @brief Stream operator
     * @param out Stream to write in
     * @param q UserQuestion to write into the stream
     * @return Stream given as input
     */
	friend ostream& operator<<(ostream& out, UserQuestion& q);

};

/*!
 * \class YesNoQuestion
 * \brief Class the represents Yes or No questions
 *
 *  The class represents questions that can be answered only with yes or no.
 *  It inherits from UserQuestion
 */
class YesNoQuestion: public UserQuestion {
protected:

	/*!
	 *  \brief Check if the answer is either "y" or "n"
	 *
	 *  \throw std::runtime_error if it is not the case
	 *
	 */
	void checkUserAction();
public:

	/*!
	 *  \brief Constructor
	 *
	 *	\param[in] question : question to ask to the user
	 *
	 */
	YesNoQuestion(string question);

	/*!
	 *  \brief Destructor
	 */
	~YesNoQuestion();

	/*!
	 * @brief Returns true if the answer is yes
	 *
	 * @return true if the answer is yes, false otherwise
	 */
	bool isYes() const;

	/*!
	 * @brief Returns true if the answer is no
	 *
	 * @return true if the answer is no, false otherwise
	 */
	bool isNo() const;
};

/*!
 * \class ProblemQuestion
 * \brief Class the represents the question asking for a list of ongoing problems
 *
 *  The class represents questions that asks the user to give a list of ongoing problems.
 *  The problem list should be given without space, and the problems should be separated with semi-column.
 */
class ProblemQuestion: public UserQuestion {
protected:

	/*!
	 *  \brief Check if the answer is a list of problem separated with semi-column
	 *
	 *  \throw std::runtime_error if it is not the case
	 *
	 */
	void checkUserAction() ;
public:

	/*!
	 *  \brief Constructor
	 *
	 *	\param[in] question : question to ask to the user
	 *
	 */
	ProblemQuestion(string question);

	/*!
	 *  \brief Destructor
	 */
	~ProblemQuestion();

};

#endif
