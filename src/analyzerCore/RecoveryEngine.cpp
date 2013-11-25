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


#include "RecoveryEngine.hpp"

RecoveryEngine::RecoveryEngine(AnalyzerDBAPI * dbApi,
		InteractionManager * iManager, MetaAgentPool * maPool, ServerPool * sm,
		RuleManager * rm) {
	m_dbAPI = dbApi;
	m_iManager = iManager;
	m_maPool = maPool;
	m_sm = sm;
	m_ruleManager = rm;
}

RecoveryEngine::~RecoveryEngine() {

}

bool RecoveryEngine::recover(AgentRecovery * ra) {
	m_lastProcedure.clear();

	m_iManager->addLog("RecoveryEngine : trying to recover");

	// Get the actions to take to recover the faulty agent
	vector<RecoveryUnit> ru = ra->getRecoveryUnits();

	m_iManager->addLog(
			"RecoveryEngine : the required recovery steps for the faulty agents are the following");
	stringstream ruStr(stringstream::in | stringstream::out);
	for (vector<RecoveryUnit>::iterator ap = ru.begin(); ap != ru.end(); ap++) {
		ruStr << *ap << std::endl;

	}
	m_iManager->addLog(ruStr.str());

	m_iManager->addLog("RecoveryEngine : checking the recovery dependencies");

	// Build the full procedure
	vector<RecoveryUnit> allProcedure = m_ruleManager->RecoveryRuleBFS(ru);

	std::vector<RecoveryHistory> history;

#ifndef SIMU
	history = m_dbAPI->getRecoveryHistory(
			ra->getAgent()->getName());
#endif

	stringstream solSS(stringstream::in | stringstream::out);

	solSS << "RecoveryEngine : full solution" << std::endl;
	for (vector<RecoveryUnit>::iterator ap = allProcedure.begin();
			ap != allProcedure.end(); ap++) {
		solSS << *ap << endl;
	}

	if (!history.empty()) {
		solSS << std::endl
				<< "==================== History ===================="
				<< std::endl;
		for (std::vector<RecoveryHistory>::const_iterator hist =
				history.begin(); hist != history.end(); hist++) {
			solSS << *hist << std::endl;
		}
	}

	m_iManager->addLog(solSS.str());

	// If the user applied our recommendation, we keep the procedure
	// to be able to fill in the history database
	YesNoQuestion * ynq = new YesNoQuestion(
			"Did you apply the following suggestions? \n\n" + solSS.str());
	m_iManager->ask(ynq);
	string answer = ynq->getAnswer();
	bool isYes = ynq->isYes();
	delete ynq;

	if (isYes) {
		m_lastProcedure = allProcedure;
	}

	return isYes;
}

bool RecoveryEngine::recover(ProcessAgent * faultyPa) {
	ProcessRecovery pr(faultyPa);
	bool recovered = recover(&pr);
	return recovered;
}

bool RecoveryEngine::recover(FileAgent * faultyFa) {
	FileRecovery fr(faultyFa);
	bool recovered = recover(&fr);
	return recovered;
}

bool RecoveryEngine::recover(EnvironmentAgent * faultyEa) {
	EnvironmentRecovery er(faultyEa);
	bool recovered = recover(&er);
	return recovered;
}

std::vector<RecoveryUnit> RecoveryEngine::getLastProcedure() const {
	return m_lastProcedure;
}

