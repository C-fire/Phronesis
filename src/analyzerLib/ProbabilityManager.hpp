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


#ifndef PROBABILITY_MANAGER_HPP
#define PROBABILITY_MANAGER_HPP

/*!
 * \file ProbabilityManager.hpp
 * \brief Contains the ProbabilityManager abstract class
 * \author Christophe Haen
 */

#include "MetaAgent.hpp"

class MetaAgent;

/*!
 * \class ProbabilityManager
 * \brief Abstract class for the ProbabilityManager
 *
 *  The ProbabilityManager is in charge of getting and updating probabilities.
 */
class ProbabilityManager {

public:

	/*!
	 * \brief Constructor
	 */
	ProbabilityManager() {
	}
	;

	/*!
	 * \brief Destructor
	 */
	virtual ~ProbabilityManager() {
	}
	;

	/*!
	 * \brief Getter for the probability of occurrence for a given link
	 *
	 * This returns the real probability than a problem in the parent is
	 * due to the daughter
	 *
	 * \param[in] parent : pointer to the parent MetaAgent
	 * \param[in] daughter : parent to the daughter MetaAgent
 	 * \param[in] rawOccurrence : if set to true, return the occurrence without dividing by the total
	 *
	 *
	 * \returns The probability of the daughter being responsible for the problem of the parent
	 *
	 */
	virtual double getProbability(MetaAgent * parent, MetaAgent * daughter, bool rawOccurrence = false) = 0;

	/*!
	 * \brief Updates the probability of occurrence for a given link
	 *
	 * This updates the probability than a problem in the parent is
	 * due to the daughter.
	 *
	 * \param[in] parent : pointer to the parent MetaAgent
	 * \param[in] daughter : parent to the daughter MetaAgent
	 *
	 */
	virtual void updateProbability(MetaAgent * parent,
			MetaAgent * daughter) = 0;

	/*!
	 * @brief This method increments only the local total of a given MetaAgent
	 * @param ma The metaAgent whose local_total needs to be incremented
	 */
	virtual void incrementLocalTotal(MetaAgent * ma) = 0;
};

#endif
