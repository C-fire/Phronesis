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


#ifndef COORDINATOR_HPP
#define COORDINATOR_HPP

/*!
 * \file Coordinator.hpp
 * \brief Coordinator class
 * \author Christophe Haen
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

#include "MetaAgent.hpp"
#include "EnvironmentAgent.hpp"

#include "VetoManager.hpp"

using namespace std;

/*!
 * \class Coordinator
 * \brief Abstract class for all the agent
 *
 *  The class is the class that aggregates other MetaAgents. It is based on the Composite design pattern
 *  It inherits from the MetaAgent class.
 */
class Coordinator: public MetaAgent {

protected:

	vector<MetaAgent *> m_children; /*!< All the MetaAgent children of the Coordinator */

	/*!
	 * @brief Function called by the stream operator
	 * @param out stream to write in
	 * @return the stream passed in parameter
	 */
	ostream& printOperator(ostream& out) const;

	std::vector<int> m_classif; /*!< Classification of the Coordinator. It represents the path to access the element starting from the root. Each tree has a different root
	 It is used to find more easily the root/lowest coordinator in a tree*/

	int m_tolerate; /*!< Tells how many child can be in bad state without returning itself a bad state */

	VetoManager * m_vManager; /*!< Pointer to the veto manager */
	std::set<std::string> m_veto; /*!< List of Veto that concern this Coordinator*/

	std::vector<MetaAgent *> (Coordinator::*ptr_sortAlgo)(
			std::vector<MetaAgent*> & input); /*!< This method pointer points either to the alwaysBestSort or weightedRndSort */
	bool m_useRawOccurrenceForProbability; /*!< Whether we prefer raw occurrences rather than probability. Useful for the sorting algorithm */

	/*!
	 * @brief Sorting algorithm "always best" for the children
	 *
	 * This is the algorithm used to choose the order in which we analyze
	 * the children when we choose "ALWAYS_BEST".
	 * It simply sorts the children in the decreasing order of probability
	 *
	 * @param input Vector of children to sort
	 * @return sorted list of children
	 */
	std::vector<MetaAgent *> alwaysBestSort(std::vector<MetaAgent*> & input);

	/*!
	 * @brief Sorting algorithm "weighted random" for the children
	 *
	 * This is the algorithm used to choose the order in which we analyze
	 * the children when we choose "WEIGHTED_RND".
	 * The children are chosen randomly, but their probability is weighted
	 * based on their occurrence (e.g. a agent that was two times more faulty than another one
	 * has two times more chanced to be picked first)
	 *
	 * @param input Vector of children to sort
	 * @return sorted list of children
	 */
	std::vector<MetaAgent *> weightedRndSort(std::vector<MetaAgent*> & input);

public:

	/*!
	 *  \brief Constructor
	 *
	 *  Base constructor of the Coordinator class
	 */
	Coordinator();

	/*!
	 *  \brief Constructor
	 *
	 *  Constructor of the Coordinator class
	 *  \param[in] id : integer unique identifier for the Coordinator
	 *  \param[in] name : name of the Coordinator
	 *  \param[in] classif : classification vector (see m_classif)
	 *  \param[in] tolerate : tolerate threshold of bad child
	 */
	Coordinator(int id, string name, std::vector<int> classif, int tolerate);

	/*!
	 *  \brief Copy constructor
	 *
	 *   Copy constructor of the Coordinator class
	 *  \param[in] src : reference to the Coordinator that we will copy
	 */
	Coordinator(const Coordinator & src);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the Coordinator class
	 */
	virtual ~Coordinator();

	using MetaAgent::operator=;
	/*!
	 * @brief Assignment operator
	 * This operator uses the MetaAgent::operator=
	 * @param source source Coordinator
	 * @return *this
	 */
	Coordinator & operator=(const Coordinator & source);

	/*!
	 *  \brief Analysis of the Coordinator state
	 *
	 *  CAUTION : bugs. see source code
	 *
	 *  This methods will recursively analyze the state of all its children.
	 *  The coordinator is considered to be in a good state if all the children
	 *  are in a good state.
	 *
	 *  It will also (or should...) check the state of the EnvironmentAgent of the involved server.
	 *  Here might be the bug (see source).
	 *
	 *  The first step is to sort all the children according to their probabilities.
	 *  For every child, in the order of the list :
	 *  - If we haven't analyze the EnvironmentAgent
	 *  - of the Server on which the current child is running, we analyze it.
	 *  - If the EnvironmentAgent is not healthy, we consider this to be the source of the problem.
	 *  - Otherwise, we analyze the child.
	 *
	 *
	 *  \return true if there is a problem, false otherwise
	 */
	bool analyze();

	/*!
	 * @brief Add a child
	 * @param child Pointer to the children to add
	 */
	void addChild(MetaAgent * child);

	/*!
	 *  \brief Getter for the classification string
	 *
	 *  returns m_classif
	 *
	 *  \return classification vector
	 */
	std::vector<int> getClassification() const;

	/*!
	 *  \brief Returns the list of Server related to the Coordinator
	 *
	 *  The method returns all the Servers of the children Agent,
	 *   plus recursively those from the children Coordinator
	 *
	 *  \return set of Server pointer
	 */
	set<Server*> getInvolvedServers() const;

	/*!
	 *  \brief Recursively updates the occurrence probabilities of the children of this Coordinator
	 *
	 *  Once the faulty MetaAgent has been found, one needs to update the occurrence probability.
	 *  This method does this by calling the updateProbability method of the ProbabilityManager
	 *  that will update the probability of going from this Coordinator to the MetaAgent child that leads
	 *  to the faulty MetaAgent.
	 */
	void updateProbability();

	/*!
	 * @brief Useless here but inherited from MetaAgent. Just throws.
	 * @param re Pointer to the RecoveryEngine
	 * @return Nothing, always throws
	 */
	bool triggerRecover(RecoveryEngineAbstract * re) {
		throw std::runtime_error(
				"Coordinator::triggerRecover Not implemented!");
	}

	/*!
	 * @brief Returns how many time we can attempt to recover this Coordinator in one diagnosis procedure
	 *
	 * For the time being, this method always returns 2. This is not good, it should be a bit smarter and depends
	 * on the amount of MetaAgent bellow. On the todo list...
	 * @return Maximum amount of attempts
	 */
	int getMaxRecoveryAttemptsAllowed();

	/*!
	 * @brief Check if any of our veto is on. If yes, we just quit
	 *
	 * If a Veto is on, then there is no point in checking this Coordinator.
	 * We just exit saying that it's all fine.
	 *
	 * @return true if we can exit, false otherwise
	 */
	bool areVetoActivated();

	/*!
	 * @brief Set the VetoManager
	 * @param vManager pointer to the VetoManager
	 */
	void setVetoManager(VetoManager * vManager);

	/*!
	 * @brief Add a Veto to the list of Veto this Coordinator depends
	 * @param veto veto to add to the list
	 */
	void addVeto(std::string veto);

	/*!
	 * Enum containing the possible sorting algorithms for the children
	 */
	enum algoType {
		ALWAYS_BEST, WEIGHTED_RND
	};
	static algoType sortAlgo; /*!< The type of algo used to explore the tree */

};

/*!
 *  \brief Coordinator != operator
 *
 *	Compares if the names are different
 *
 *	\param[in] a : reference to the first Coordinator
 *	\param[in] b : reference to the second Coordinator
 *
 *	\return true if a and b have different name, false otherwise
 */
bool operator!=(const Coordinator &a, const Coordinator &b);

#endif
