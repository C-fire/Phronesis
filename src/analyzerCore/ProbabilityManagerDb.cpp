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


#include "ProbabilityManagerDb.hpp"

ProbabilityManagerDb::ProbabilityManagerDb(AnalyzerDBAPI * dbApi) {
	this->m_dbApi = dbApi;
}

ProbabilityManagerDb::~ProbabilityManagerDb() {

}

double ProbabilityManagerDb::getProbability(MetaAgent * parent,
		MetaAgent * daughter, bool rawOccurrence) {
	double occurrence = 0;
	double total = 0;
	try {
		total = m_dbApi->getTotal(parent->getId());
		occurrence = m_dbApi->getOccurrence(parent->getId(), daughter->getId());
	} catch (std::exception &e) {
		throw;
	}

	if (rawOccurrence) {
		return occurrence;
	}

	if (total != 0)
		return occurrence / total;
	else
		return 1;
}

void ProbabilityManagerDb::addUnsolved(MetaAgent * agent) {
	m_dbApi->addUnsolved(agent->getId());
}

//Daughter is the faulty daughter that creates problem in parent
void ProbabilityManagerDb::updateProbability(MetaAgent * parent,
		MetaAgent * daughter) {
	m_dbApi->incrementTotal(parent->getId());
	m_dbApi->incrementOccurrence(parent->getId(), daughter->getId());
}


void ProbabilityManagerDb::incrementLocalTotal(MetaAgent * ma){
	m_dbApi->incrementLocalTotal(ma->getId());
}
