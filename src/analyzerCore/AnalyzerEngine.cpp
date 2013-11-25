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


#include "AnalyzerEngine.hpp"

AnalyzerEngine::AnalyzerEngine(AnalyzerDBAPI * dbApi,
		InteractionManager * iManager) {
	this->m_dbAPI = dbApi;
	this->m_iManager = iManager;
	m_vManager = new VetoManager();
	m_iManager->setVetoManager(m_vManager);
	this->m_pm = new ProbabilityManagerDb(dbApi);
	this->m_sp = new ServerPool(dbApi);
	this->m_maPool = new MetaAgentPool(m_dbAPI, iManager, m_pm, m_sp, m_vManager);
	this->m_ruleManager = new RuleManager(m_dbAPI, m_maPool, m_iManager);
	this->m_scheduler = new Scheduler(m_dbAPI, m_iManager, m_maPool,
			m_ruleManager);
	this->m_recoveryEngine = new RecoveryEngine(m_dbAPI, iManager, m_maPool,
			m_sp, m_ruleManager);
	this->m_agg = new Aggregator(m_dbAPI, m_maPool, m_scheduler, m_ruleManager,
			m_iManager, m_recoveryEngine, m_pm);


	m_replication = 0;

}

AnalyzerEngine::~AnalyzerEngine() {
	delete this->m_pm;
	delete this->m_maPool;
	delete this->m_scheduler;
	delete this->m_agg;
	delete this->m_ruleManager;
	delete this->m_sp;
	delete this->m_recoveryEngine;
	delete this->m_vManager;
}


bool AnalyzerEngine::analyze(string problem) {
#ifdef SIMU
	m_iManager->addLog(
			"****************** REPETITION "
					+ boost::lexical_cast<std::string>(m_replication++)
					+ " ******************");
#endif
	bool out = false;
	try {
		out = m_agg->analyze(problem);
		m_maPool->reinitializeMetaAgents();
	} catch (std::exception &e) {
		std::stringstream ss(stringstream::in | stringstream::out);
		ss << e.what();
		m_iManager->addLog(ss.str());
	}

	return out;
}

void AnalyzerEngine::checkAll() {

	m_iManager->addLog("Checking all coordinators ... ");
	std::map<std::string, std::string> report = m_agg->checkAll();
	m_iManager->addLog("Done");
	m_iManager->addLog("=============== REPORT ================");
	for (std::map<std::string, std::string>::const_iterator it = report.begin();
			it != report.end(); it++)
		m_iManager->addLog(it->first + " : " + it->second);

}

void AnalyzerEngine::fullCheck() {

	m_iManager->addLog("Checking all agents ... ");
	std::map<std::string, std::string> report = m_agg->fullCheck();
	m_iManager->addLog("Done");
	m_iManager->addLog("=============== REPORT ================");
	for (std::map<std::string, std::string>::const_iterator it = report.begin();
			it != report.end(); it++)
		m_iManager->addLog(it->first + " : " + it->second);

}

void AnalyzerEngine::run() {
	m_iManager->addLog("The analyzer Engine has started");
	sleep(2); //Allow time for client connection, so they don't miss the beginning of the show
	while (!stopReceived) {
		m_iManager->setCurrentProblemList("No problem being treated now");

		ProblemQuestion * pq = new ProblemQuestion(
				"Enter the problems list : ");
		m_iManager->ask(pq);
		string pb = pq->getAnswer();
		delete pq;
		string pbLog = "Received problem list : " + pb;
		m_iManager->addLog(pbLog);
		if (!stopReceived) {
			analyze(pb);
		}
	}
	m_iManager->addLog("Exiting analyzer engine");
}
