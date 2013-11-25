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


#ifndef USER_MSG_HPP
#define USER_MSG_HPP

/*!
 * \file UserMsg.hpp
 * \brief UserMsg classes.
 *
 * One can question the interest of having it still...
 * However it might be used in the future (data mining warning?).
 *
 * \author Christophe Haen
 */

#include <iostream>
#include <algorithm>
#include "UserInteraction.hpp"

using namespace std;

/**
 * \enum msgLevels
 * \brief Messages levels
 *
 * Those constants define how important or critical a messaeg is
 */
typedef enum {
	INFO, /*!< Just an information message */
	WARNING, /*!< Warning message */
	ERROR, /*!< Report about an error */
	CRITICAL
/*!< Reports a critical problem */
} msgLevels;
//
/*!
 * \class UserMsg
 * \brief Abstract class for all the messages send to the user
 *
 *  The class is the abstract class for all the messages. It does not expect any reaction from the user
 */
class UserMsg: public UserInteraction {
protected:

	string m_msg; /*!< string that contains the message for the user */
	msgLevels m_lvl; /*!< severity of the message */

public:

	/*!
	 *  \brief Constructor
	 *
	 *	\param[in] msg : message to deliver to the user
	 *	\param[in] lvl : severity of the message
	 */
	UserMsg(string msg, msgLevels lvl);

	/*!
	 * \brief Copy constructor
	 * @param source source UserMsg
	 */
	UserMsg(const UserMsg & source);

	/*!
	 * \brief Assignment operator
	 * @param source source UserMsg
	 * @return *this
	 */
	UserMsg & operator=(const UserMsg & source);

	/*!
	 *  \brief Destructor
	 *
	 */
	virtual ~UserMsg();

	/*!
	 *  \brief Getter for the message
	 *
	 *  Returns m_msg
	 *
	 *  \return The message for the user
	 */
	virtual string getMsg();

	/*!
	 *  \brief Getter for the message severity
	 *
	 *  Returns m_lvl
	 *
	 *  \return The severity of the message
	 */
	virtual msgLevels getLevel();

	/*!
	 *  \brief Output stream operator
	 *
	 *	\param[in] m : reference to the UserMsg to display
	 *	\param[in,out] out : the output stream
	 *  \return The output stream
	 */
	friend ostream& operator<<(ostream& out, UserMsg& m);
};

#endif
