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


#ifndef PROBABILITY_MANAGER_DB_HPP
#define PROBABILITY_MANAGER_DB_HPP

/*!
 * \file ProbabilityManagerDb.hpp
 * \brief ProbabilityManagerDb.hpp class
 * \author Christophe Haen
 */

#include "AnalyzerDBAPI.hpp"
#include "ProbabilityManager.hpp"

/*!
 * \class ProbabilityManagerDb
 * \brief This class manages all the probabilities and their storage in the DB
 *
 *	It inherits from the abstract class ProbabilityManager
 *
 *  There should normally be only one instance of the ProbabilityManagerDb.
 */
class ProbabilityManagerDb: public ProbabilityManager {
protected:
	AnalyzerDBAPI * m_dbApi; /*!< pointer to the AnalyzerDBAPI */

public:

	/*!
	 * @brief Constructor
	 *
	 * @param dbApi pointer to the AnalyzerDBAPI
	 */
	ProbabilityManagerDb(AnalyzerDBAPI * dbApi);

	/*!
	 * @brief Destructor
	 */
	virtual ~ProbabilityManagerDb();

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
	 * \returns The probability of the daughter being responsible for the problem of the parent
	 *
	 */
	double getProbability(MetaAgent * parent, MetaAgent * daughter, bool rawOccurrence = false);

	/*!
	 * @brief Increments the counter for the unsolved case for a given MetaAgent
	 *
	 * SEEMS TO BE DEPRECATED
	 *
	 * @param agent MetaAgent that we could not diagnosed
	 */
	void addUnsolved(MetaAgent * agent);

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
	void updateProbability(MetaAgent * parent, MetaAgent * daughter);


	/*!
	 * @brief This method increments only the local total of a given MetaAgent
	 *
	 * In principle, this method should be call only by Agent (not Coordinator),
	 * and it should be at the end of the updateProbabiliy chain that runs from
	 * the top Coordinator to the Agent.
	 * @param ma The metaAgent whose local_total needs to be incremented
	 */
	void incrementLocalTotal(MetaAgent * ma);


};

#endif
