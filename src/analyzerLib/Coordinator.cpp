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


#include "Coordinator.hpp"

Coordinator::algoType Coordinator::sortAlgo = Coordinator::ALWAYS_BEST;

Coordinator::Coordinator(int id, string name, std::vector<int> classif,
		int tolerate) :
		MetaAgent(id, name) {
	m_classif = classif;
	m_tolerate = tolerate;
	m_vManager = NULL;

	// We choose the Algorithm to sort the children when performing a diagnosis
	switch (sortAlgo) {
	case ALWAYS_BEST:
		ptr_sortAlgo = &Coordinator::alwaysBestSort;
		m_useRawOccurrenceForProbability = false;
		break;
	case WEIGHTED_RND:
		ptr_sortAlgo = &Coordinator::weightedRndSort;
		m_useRawOccurrenceForProbability = true;
		break;
	}
}
Coordinator::Coordinator() :
		MetaAgent(0, "") {
	//m_classif = "";
	m_tolerate = 0;
	m_vManager = NULL;

	switch (sortAlgo) {
	case ALWAYS_BEST:
		ptr_sortAlgo = &Coordinator::alwaysBestSort;
		m_useRawOccurrenceForProbability = false;
		break;
	case WEIGHTED_RND:
		ptr_sortAlgo = &Coordinator::weightedRndSort;
		m_useRawOccurrenceForProbability = true;
		break;
	}
}

Coordinator::Coordinator(const Coordinator & src) :
		MetaAgent(src) {
	m_children = src.m_children;
	m_classif = src.m_classif;
	m_tolerate = src.m_tolerate;
	m_vManager = src.m_vManager;
	ptr_sortAlgo = src.ptr_sortAlgo;
	m_veto = src.m_veto;
	m_useRawOccurrenceForProbability = src.m_useRawOccurrenceForProbability;
}

Coordinator & Coordinator::operator=(const Coordinator & source) {
	*this = (MetaAgent&) source;
	m_children = source.m_children;
	m_classif = source.m_classif;
	m_tolerate = source.m_tolerate;
	m_vManager = source.m_vManager;
	ptr_sortAlgo = source.ptr_sortAlgo;
	m_veto = source.m_veto;
	m_useRawOccurrenceForProbability = source.m_useRawOccurrenceForProbability;
	return *this;
}

Coordinator::~Coordinator() {

}

/*SHOULD I ADD RULES IN THERE BEFORE PRIORITY????*/

bool Coordinator::analyze() {
	MetaAgent::analyze();

	string space = "";
	for (int i = 0; i < m_deep; i++) {
		space += "\t";
	}
	m_deep++;

	int curBadChildren = 0; // Counter of bad children, to be compared with m_tolerate

	m_iManager->addLog("Coordinator " + m_name + " : entering analysis");

	// If a Veto is active, it is not needed to perform diagnosis on this Coordinator
	if (areVetoActivated()) {
		m_iManager->addLog(
				"A veto is activated, I don't check this Coordinator");
		return false;
	}

	bool problemFound = false; // Flag if the problem was found

	vector<MetaAgent *>::iterator it;

	// Assign the probabilities to each child.
	// If m_useRawOccurrenceForProbability is set, we get the occurrence as such, and not the
	// occurrence divided by the total
	for (it = m_children.begin(); it != m_children.end(); it++) {
		try {
			double p = m_pm->getProbability(this, *it,
					m_useRawOccurrenceForProbability);
			(*it)->setProba(p);
		} catch (std::exception & e) {
			(*it)->setProba(0);
		}
	}

	// The children are sorted using the algorithm chosen at the beginning.
	std::vector<MetaAgent *> sortedChildren = (this->*ptr_sortAlgo)(m_children);

	// Start from the beginning of the sorted children vector
	it = sortedChildren.begin();

	// We analyze all the children until we found a problem or reached the end of the children
	while ((!problemFound) && (it != sortedChildren.end())) {

		bool childProblem = (*it)->analyze(); // Analyze the children and get the result

		// if we have a problem on the children
		if (childProblem) {

			// Increase the amount of bad children
			curBadChildren++;
			m_iManager->addLog(
					"Coordinator " + m_name + " : the child " + (*it)->getName()
							+ " seems to have a problem.");

			// If we are bellow or at the limit, we just continue
			if (curBadChildren <= m_tolerate) {
				m_iManager->addLog(
						"we can still tolerate it ("
								+ boost::lexical_cast<std::string>(
										curBadChildren) + " <= "
								+ boost::lexical_cast<std::string>(m_tolerate)
								+ ")");
			} else { //Otherwise, we set the problemFound flag, and the children as responsible

				m_iManager->addLog(
						"Setting it as responsible (Cause : "
								+ (*it)->getCause() + ")");

				problemFound = true;

				m_responsibleChild = *it;

				m_cause = m_name + " -> " + (*it)->getCause();
			}
		}
		it++;
	}
	m_deep--;

	if (!problemFound) {
		m_cause = "";
	}

	return problemFound;
}

void Coordinator::addChild(MetaAgent * child) {
	this->m_children.push_back(child);
}

std::vector<int> Coordinator::getClassification() const {
	return m_classif;
}

ostream& Coordinator::printOperator(ostream& out) const {
	out << "[Coordinator] " << (*this) << " classif ";
	for (std::vector<int>::const_iterator it = m_classif.begin();
			it != m_classif.end(); it++) {
		out << *it << ",";
	}
	out << std::endl;
	out << "\tDaughters:";

	vector<MetaAgent *>::const_iterator it;

	for (it = m_children.begin(); it != m_children.end(); it++) {
		out << (*it)->getName() << ",";
	}
	return out;
}

set<Server*> Coordinator::getInvolvedServers() const {
	set<Server*> servers;
	vector<MetaAgent *>::const_iterator it;
	for (it = m_children.begin(); it != m_children.end(); it++) {
		set<Server*> childSrv = (*it)->getInvolvedServers();
		servers.insert(childSrv.begin(), childSrv.end());
	}

	return servers;
}

void Coordinator::updateProbability() {
	// We update the occurrence of the link, and recursively call the method on the responsible child
	m_pm->updateProbability(this, m_responsibleChild);
	m_iManager->addLog(
			"I update " + this->getName() + " -> "
					+ m_responsibleChild->getName());
	m_responsibleChild->updateProbability();
}

int Coordinator::getMaxRecoveryAttemptsAllowed() {
	return 2;
}

bool Coordinator::areVetoActivated() {
	for (std::set<std::string>::const_iterator veto = m_veto.begin();
			veto != m_veto.end(); veto++) {
		if (m_vManager->getVeto(*veto))
			return true;
	}
	return false;
}

void Coordinator::setVetoManager(VetoManager * vManager) {
	m_vManager = vManager;
}

void Coordinator::addVeto(std::string veto) {
	m_veto.insert(veto);
}

std::vector<MetaAgent *> Coordinator::alwaysBestSort(
		std::vector<MetaAgent*> & input) {
	std::vector<MetaAgent*> inputCpy = input;
	sort(inputCpy.begin(), inputCpy.end(), sortMetaAgentByProba);

	return inputCpy;

}

std::vector<MetaAgent *> Coordinator::weightedRndSort(
		std::vector<MetaAgent*> & input) {

	std::vector<MetaAgent*> children = input;
	std::vector<MetaAgent *> sortedNodes;

	// For all the children
	while (!children.empty()) {

		double proba = 0; // incremental value of the occurrences
		MetaAgent * chosenNode = NULL;
		std::vector<double> probaVec; // vector that contains the progressive sum of all the occurrences, used to randomly choose a child
		probaVec.clear();

		// First we construct the probaVec from all the MetaAgent left in the children vector
		for (vector<MetaAgent *>::iterator it = children.begin();
				it != children.end(); it++) {
			double childOcc = (*it)->getProba();
			proba += childOcc;
			probaVec.push_back(proba);
		}

		// If all the occurrences are null, we choose a random one
		if (proba == 0) {
			chosenNode = children[rand() % children.size()];
		} else {
			// Otherwise, we take a random number between 0 and proba, and see to which index it corresponds in the probaVec.
			// We take the child at the same index in the children vector.
			int rnd = rand() % (int) (proba);
			unsigned int id = 0;
			while ((id < probaVec.size()) && (rnd > (probaVec[id] - 1))) {
				id++;
			}

			chosenNode = children[id];
		}
		sortedNodes.push_back(chosenNode);
		// We remove chosenNode from children not to take it into account again
		children.erase(std::find(children.begin(), children.end(), chosenNode));
	}
	return sortedNodes;

}

bool operator!=(const Coordinator &a, const Coordinator &b) {
	return (!(a == b));
}

