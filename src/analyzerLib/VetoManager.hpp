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


#ifndef VETO_MANAGER_HPP
#define VETO_MANAGER_HPP

/*!
 * \file VetoManager.hpp
 * \brief VetoManager class
 * \author Christophe Haen
 */

#include <string>
#include <map>

/*!
 * @class VetoManager
 *
 * This class contains the list of Veto and their values
 *
 */
class VetoManager {
protected:

	std::map<std::string, bool> m_vetoMap; /*!< Contains the values of the Veto */

public:

	/*!
	 * @brief destructor
	 */
	virtual ~VetoManager() {
	}
	;

	/*!
	 * @brief Set the value to a veto
	 * @param name name of the veto
	 * @param value value of the veto
	 */
	void setVeto(std::string name, bool value);

	/*!
	 * @brief Returns the value of a veto
	 *
	 * If the veto has no value defined, we return false
	 *
	 * @param name name of the Veto
	 * @return value of the veto
	 */
	bool getVeto(std::string name) const;

	/*!
	 * @brief returns all the Veto
	 * @return map containing the Veto names and values
	 */
	std::map<std::string, bool> getAllVeto() const;
};

#endif
