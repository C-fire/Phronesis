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


#include "Scheduler.hpp"

Scheduler::Scheduler(AnalyzerDBAPI * dbApi, InteractionManager * iManager,
		MetaAgentPool * maPool, RuleManager * ruleManager) {
	m_dbApi = dbApi;
	m_iManager = iManager;
	m_maPool = maPool;
	m_ruleManager = ruleManager;
	m_diagnosed = NULL;
}


//returns 1 if problem found, 0 if not
bool Scheduler::analyze(map<string, Coordinator *> problems) {

	//Here we have to choose in the proper order. For the moment...random!
	//we should also take in account problem solved but not fixed... it should change the order of solving
	m_problems = problems;
	m_undiagnosedProblems.clear();
	m_diagnosed = NULL;
	Coordinator * next;

	bool problemDiagnosed = false;

	/* 2012-11-29 :
	 * I add the try catch block. The exception should be raised by getNext when
	 * m_problem is empty. But I think this will never be called in that case,
	 * because of the do while condition
	 *
	 */
	try {
		do {
			next = getNext();
			m_iManager->addLog("Scheduler : I will analyze " + next->getName());
			problemDiagnosed = next->analyze();
			if (!problemDiagnosed) {
				m_undiagnosedProblems[next->getName()] = next;
				m_problems.erase(next->getName());
			} else {
				m_diagnosed = next;
			}

		} while ((!problemDiagnosed) && (!m_problems.empty()));

		if ((problemDiagnosed) && (next)) {
			return true;
		}


	} catch (std::exception & e) { //In principle, this is only raised when the list is empty
		stringstream ss(stringstream::in | stringstream::out);
		ss
				<< "Exception in the Scheduler::analyze. This should not happen according to the algorithm logic. ("
				<< e.what() << ")";
		m_iManager->addLog(ss.str());
	}

	return false;
}
map<string, Coordinator *> Scheduler::getUndiagnosedProblems() const {
	return m_undiagnosedProblems;
}

Coordinator * Scheduler::getNext() {

	if (m_problems.empty()) {
		assert(!m_problems.empty());
		throw std::runtime_error("The problem list is empty.");
	}
	vector<Coordinator *> vec = getProblemsVector();
	vector<Coordinator *> sortedVec = m_ruleManager->sortAccordingDependencies(vec);
	vector<Coordinator *> candidates;

	//for sure sortedVec is not empty otherwise we would already exit before
	int weight = sortedVec[0]->getWeight();
	vector<Coordinator *>::iterator it = sortedVec.begin();
	while ((it != sortedVec.end()) && ((*it)->getWeight() == weight)) {
		candidates.push_back(*it);
		it++;
	}

	string logMsg = "Candidates : ";
	for (it = candidates.begin(); it != candidates.end(); it++) {
		logMsg += (*it)->getName() + ",";
	}
	m_iManager->addLog(logMsg);

	int s = candidates.size();
	int rd = rand();
	int rnd = rd % s;

	m_iManager->addLog(" I choose " + candidates[rnd]->getName());
	return candidates[rnd];

}

vector<Coordinator *> Scheduler::getProblemsVector() const {
	vector<Coordinator *> vec;
	map<string, Coordinator *>::const_iterator it;
	for (it = m_problems.begin(); it != m_problems.end(); it++) {
		vec.push_back(it->second);
	}

	return vec;
}

Coordinator * Scheduler::getDiagnosedProblem() const {
	return m_diagnosed;
}
