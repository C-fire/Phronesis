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


#ifndef USER_INTERACTION_HPP
#define USER_INTERACTION_HPP

/*!
 * \file UserInteraction.hpp
 * \brief UserInteraction class
 * \author Christophe Haen
 */

#include <string>
using namespace std;

/*!
 * \class UserInteraction
 * \brief Abstract class for all the interactions between the user and the software
 *
 *  The class is the abstract class for all the interaction : Messages or Questions.
 */
class UserInteraction {
public:
	/*!
	 * \brief Constructor
	 */
	UserInteraction() {
	}
	;

	/*!
	 * \brief Destructor
	 */
	virtual ~UserInteraction() {
	}
	;

};

#endif
