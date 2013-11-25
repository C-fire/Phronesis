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


#include "Aggregator.hpp"

Aggregator::Aggregator(AnalyzerDBAPI * dbApi, MetaAgentPool * maPool,
		Scheduler * scheduler, RuleManager * ruleManager,
		InteractionManager * iManager, RecoveryEngine * recoveryEngine,
		ProbabilityManagerDb * pm) {
	m_dbApi = dbApi;
	m_maPool = maPool;
	m_scheduler = scheduler;
	m_ruleManager = ruleManager;
	m_iManager = iManager;
	m_recoveryEngine = recoveryEngine;
	m_respChild = NULL;
	m_impactedCoordinators.clear();
	m_diagnosedCoordinator = NULL;
	m_pm = pm;
	m_tryingValidRules = false;
	m_tryingUnvalidRules = false;
	m_recommendationApplied = false;
	m_environmentResponsible = false;
	m_serverDown = false;
}

// analyzeEnvironments:
// For each Coordinator, we get all the Servers that are bellow.
// We go through each Server, and analyze the EnvironmentAgent
// until we find a problematic one.
//
// When it is the case, we focus on this particular agent,
// we don't diagnose several environment problem at the time.
// We then check all the Coordinators that have a child running
// on this Server, and add them to the impacted Coordinator.
//
// For each server, after checking the EnvironmentAgent,
// we look at the Attached Coordinators. We actually
// use the scheduler to perform a normal full diagnosis
// of the Attached Coordinators.

bool Aggregator::analyzeEnvironments(vector<Coordinator *> coordVec) {
	m_iManager->sendStatistic("aggregator_analyze_environment");
	vector<Coordinator *> sortedVec = m_ruleManager->sortAccordingDependencies(
			coordVec);

	std::set<Server *> testedServer;
	Server * respServer = NULL;

	std::string logMsg = "Impacted by the environment problem : ";
	for (std::vector<Coordinator *>::iterator co = sortedVec.begin();
			co != sortedVec.end(); co++) {

		set<Server*> servers = (*co)->getInvolvedServers();
		for (set<Server*>::iterator srv = servers.begin(); srv != servers.end();
				srv++) {

			Server * curSrv = *srv;

			// We take care of one server at the time
			if (respServer != NULL) {
				if (curSrv != respServer) {
					continue;
				} else {
					m_impactedCoordinators.push_back(*co);
					logMsg += " " + (*co)->getName();
					continue;
				}
			} else {
				if (testedServer.find(curSrv) != testedServer.end()) {
					continue;
				}
			}

			testedServer.insert(curSrv);

			EnvironmentAgent * curEnv = curSrv->getEnvironmentAgent();

			if (!curEnv->wasEnviromentTested()) {
				curEnv->analyze();
			} else {
				m_iManager->addLog(
						"We already tested the EnvironmentAgent on "
								+ curSrv->getName());
				assert(curEnv->wasEnviromentTested());
			}

			if (!curEnv->isEnvironmentSain()) {
				m_respChild = curEnv;
				respServer = curSrv;

				m_iManager->addLog(
						"Bad environment for server " + curSrv->getName());

			} else {

				std::vector<Coordinator *> attachedCoords =
						curSrv->getAttachedCoordinators();

				if (!attachedCoords.empty()) {
					map<string, Coordinator *> attachedCoordsMap;
					for (std::vector<Coordinator *>::const_iterator att =
							attachedCoords.begin(); att != attachedCoords.end();
							att++) {
						attachedCoordsMap[(*att)->getName()] = *att;
					}

					bool problemDiagnosed = m_scheduler->analyze(
							attachedCoordsMap);

					logMsg = "";

					if ((problemDiagnosed)) {
						Coordinator * faultyCoord =
								m_scheduler->getDiagnosedProblem();

						m_respChild = faultyCoord;

						//Danger of infinite loop if not properly reinitialized
						// BUG if something goes wrong, m_respChild can be null. Put protection
						while (m_respChild
								!= (m_respChild->getResponsibleChild())) {
							m_respChild = m_respChild->getResponsibleChild();
						}

						m_impactedCoordinators.push_back(*co);
						respServer = curSrv;
						m_iManager->addLog(
								"We have diagnosed an Environment problem on the server "
										+ curSrv->getName()
										+ ". The problem comes from "
										+ faultyCoord->getName() + "("
										+ m_respChild->getName() + ")");
					}
				}

			}
		}

	}

	if (!m_impactedCoordinators.empty())
		m_iManager->addLog(logMsg);

	if (respServer)
		return true;
	else
		return false;
}

/* 10-12-2012  This version is a bit more elegant.
 * First of all it does not stop all what it is doing if one server is down
 * Also, it checks the environment at the end, not the beginning.
 *
 * Principle :
 *  We loop until we have emptied the problem list,
 *  or that we give up.
 *  First of all, we need to find the so called "top coordinators",
 *  so in fact, those the lowest in the tree (genius...).
 *  Those Coordinators are passed to the Scheduler for a diagnosis.
 *  We get back a flag whether we could find a faulty Agent,
 *  and we can retrieve from the Scheduler all the Coordinators
 *  that it analyzed without finding anything wrong. From this list
 *  we fill in our own "undiagnosed" list (I should change name...)
 *  and we remove them from the problem list.
 *
 *  - If a problem was diagnosed :
 *   We get the diagnosed Coordinator and the responsible Child
 *   (by "rolling down" the chain of responsibleChild).
 *   We check whether it is an Environment Problem (at this stage,
 *   it can only be a server down or unresponsive Remote Agent)
 *   If it is an environment problem, we just build the impacted
 *   Coordinators list from all the Coordinators in the problem and
 *   undiagnosed lists.
 *  - If no problem was found :
 *   we do real EnvironmentAgent check using analyzeEnvironments
 *
 *  Now, we suppose that a problem was found, either by the scheduler
 *  or in the environment checks.
 *  If the problem comes from a dead server, we incremente its
 *  failure counter in the database.
 *  All the impacted Coordinators are added to the "diagnosed" list.
 *  Then, we try to recover the responsible Agent using the RecoveryEngine.
 *
 *  If we still have problems in the problem list and if we are not currently
 *  trying to use dependency rules, and we could find a problem, then we
 *  update the problem list by asking the user what the situation is,
 *  and we start all the process again.
 *
 *  Otherwise, if we have still undiagnosed problem, we can use the
 *  dependency rules to find Coordinators that were not reported
 *  as faulty, but from which the reported ones depend.
 *  In this case, we start all the process again with those new Coordinators
 *
 *  We stop when we have successfully diagnosed everything, or when
 *  we have tried all the dependency rules without success.
 *  If we could not diagnose some Coordinators, we signal it to the user
 *  and increment the "counter of the shame"
 *
 */
bool Aggregator::analyze(string problems) {
	m_iManager->sendStatistic("aggregator_analyze");
	string pbs = problems;
	string pb;
	m_undiagnosedProblems.clear();
	m_diagnosedProblems.clear();
	m_problems.clear();
	m_tryingProblems.clear();
	string logMsg;
	m_tryingValidRules = false;
	m_tryingUnvalidRules = false;
	m_recommendationApplied = false;
	m_environmentResponsible = false;
	m_serverDown = false;

	MetaAgent::TMP_attempts = 0;

	m_problems = parseProblemString(problems);

	if (m_problems.empty()) {
		m_iManager->addLog("No problem to treat");
		throw std::runtime_error("No problem to treat");
	}
	do {
		m_iManager->sendStatistic("aggregator_analyze_loop");
		m_respChild = NULL;
		m_impactedCoordinators.clear();
		m_diagnosedCoordinator = NULL;

		/*!
		 * 2013-07-10
		 * Replace findTopCoordinator with findLowestCoordinators
		 * This require the new type of Classification (path, and not letter-depth)
		 */
//		map<string, Coordinator *> tp = findTopCoordinator();
		map<string, Coordinator *> tp = findLowestCoordinators();

		logMsg = "The top problems are : ";
		for (map<string, Coordinator *>::iterator it3 = tp.begin();
				it3 != tp.end(); it3++) {
			logMsg += it3->first + ",";

		}
		logMsg += "\n";
		m_iManager->addLog(logMsg);
		logMsg = "";

		bool problemDiagnosed = m_scheduler->analyze(tp);
		map<string, Coordinator *> ups = m_scheduler->getUndiagnosedProblems();

		if (!ups.empty()) {

			logMsg = "We could not diagnose : ";
			for (map<string, Coordinator *>::iterator u = ups.begin();
					u != ups.end(); u++) {
				logMsg += u->first + ",";
				m_undiagnosedProblems[u->first] = u->second;
				m_problems.erase(u->first);
			}
			m_iManager->addLog(logMsg);
			logMsg = "";

		}

		if ((problemDiagnosed)) {

			m_diagnosedCoordinator = m_scheduler->getDiagnosedProblem();
			m_respChild = m_diagnosedCoordinator;

			//Danger of infinite loop if not properly reinitialized
			// BUG if something goes wrong, m_respChild can be null. Put protection
			while (m_respChild != (m_respChild->getResponsibleChild())) {
				m_respChild = m_respChild->getResponsibleChild();
			}

			// 2013-04-08
			// Here I need to set the variable m_environmentResponsible
			// because we might detect an environment problem without the
			// need to go through the analyzeEnvironments method
			// (like a server down or so)
			m_environmentResponsible = m_respChild->isEnvironmentResponsible();

			// 2013-06-01
			// If it is an environment problem, I need to add all the
			// coordinator that uses this environment into the impactedCoordinators
			if (m_environmentResponsible) {

				Server * respServer = m_respChild->getServer();
				std::vector<Coordinator *> allCoords;

				for (map<string, Coordinator *>::iterator co =
						m_problems.begin(); co != m_problems.end(); co++) {
					allCoords.push_back(co->second);
				}
				for (map<string, Coordinator *>::iterator co =
						m_undiagnosedProblems.begin();
						co != m_undiagnosedProblems.end(); co++) {
					allCoords.push_back(co->second);
				}
				for (std::vector<Coordinator *>::iterator co =
						allCoords.begin(); co != allCoords.end(); co++) {

					set<Server*> servers = (*co)->getInvolvedServers();
					for (set<Server*>::iterator srv = servers.begin();
							srv != servers.end(); srv++) {

						if (*srv == respServer) {
							m_impactedCoordinators.push_back(*co);
						}
					}
				}
			} else {
				m_impactedCoordinators.push_back(m_diagnosedCoordinator);
			}

			m_iManager->addLog(
					"We have diagnosed a problem on "
							+ m_diagnosedCoordinator->getName()
							+ ". The problem comes from "
							+ m_respChild->getName());

		} else {

			m_iManager->addLog(
					"We could not diagnose any problem. We will have a look at the environments");

			// In principle, ALL the coordinators that were in the problem list
			// should now be in the undiagnosed list
			vector<Coordinator *> vec;
			for (map<string, Coordinator *>::iterator it = ups.begin();
					it != ups.end(); it++) {
				vec.push_back(it->second);
			}

			// 2013-02-26
			// Put all this in a function
			m_environmentResponsible = analyzeEnvironments(vec);

		}

		if (!m_impactedCoordinators.empty()) {
			std::string ackMsg = "DIAGNOSED:";
			for (vector<Coordinator *>::const_iterator imp =
					m_impactedCoordinators.begin();
					imp != m_impactedCoordinators.end(); imp++)
				(ackMsg) += (*imp)->getName() + ",";
			UserMsg * ackMsgMsg = new UserMsg(ackMsg, INFO);

			m_iManager->addMsg(ackMsgMsg);
		}

		if (m_respChild) {

			// 2013-04-05
			// replace test on m_respChild by the bool returned by analyzeEnvironment
//			if (m_respChild->isEnvironmentResponsible()) {
			if (m_environmentResponsible) {

				// Check all the services impacted by this server;
				logMsg = "We have diagnosed an environment problem on "
						+ m_respChild->getServer()->getName();
				m_iManager->addLog(logMsg);
				logMsg = "The possibly impacted Coordinators are : ";
				for (vector<Coordinator *>::const_iterator itc =
						m_impactedCoordinators.begin();
						itc != m_impactedCoordinators.end(); itc++) {
					m_diagnosedProblems[(*itc)->getName()] = (*itc);
					logMsg += " " + (*itc)->getName();
				}
				m_iManager->addLog(logMsg);

				if (!m_respChild->isServerPingable()) {
					m_serverDown = true;
					try {
						m_dbApi->incrementServerFailure(
								m_respChild->getServer()->getId());
					} catch (exception &e) {
						m_iManager->addLog(e.what());
					}

				}

			} else {
				//do the normal stuff

				logMsg = "We have diagnosed "
						+ m_diagnosedCoordinator->getName();
				m_iManager->addLog(logMsg);

				logMsg = "The cause was " + m_diagnosedCoordinator->getCause();
				m_iManager->addLog(logMsg);

				logMsg = "Responsible : " + m_respChild->getName();
				m_iManager->addLog(logMsg);
				logMsg = "";
				m_diagnosedProblems[m_diagnosedCoordinator->getName()] =
						m_diagnosedCoordinator;
			}
			m_recommendationApplied = m_respChild->triggerRecover(
					m_recoveryEngine);
		}

//		if ((!m_problems.empty()) && (!m_tryingValidRules)
//				&& (!m_tryingUnvalidRules)) { //Can I really do this test?? Not sure... to be checked

		if ((!(m_problems.empty() && m_tryingValidRules)) && (m_respChild)) { //Can I really do this test?? Not sure... to be checked
			updateProblemList();
		} else {
			// Here, if there are undiagnosed problems, we can check the entities upon which they depend
			// We can first have a look at the first degree
			if (!m_tryingValidRules) {
				m_tryingValidRules = true;
				m_problems = m_ruleManager->getPredecessors(
						m_undiagnosedProblems);
				m_tryingProblems = m_problems;
				m_iManager->addLog(
						"I could not find anything. Trying the valid rules.");
			}

		}

	} while (!m_problems.empty());

	if (!m_undiagnosedProblems.empty()) {
		m_iManager->sendStatistic("aggregator_unsolved");
		logMsg =
				"There is still some undiagnosed problem that I am giving up on the way :";
		for (map<string, Coordinator *>::iterator it =
				m_undiagnosedProblems.begin();
				it != m_undiagnosedProblems.end(); it++) {
			if (m_tryingProblems.find(it->first) != m_tryingProblems.end())
				continue;
			logMsg += " " + it->first;
			m_pm->addUnsolved(it->second);
			m_iManager->sendStatistic("aggregator_unsolved_counter");
			m_iManager->sendStatistic(it->first + "_unsolved");
		}

		m_iManager->addLog(logMsg);
	}

	std::string attemptsStr = "VISITED:"
			+ boost::lexical_cast<std::string>(MetaAgent::TMP_attempts);
	m_iManager->sendStatistic("visited", MetaAgent::TMP_attempts);

	UserMsg * attemptsMsg = new UserMsg(attemptsStr, INFO);
	m_iManager->addMsg(attemptsMsg);

	return m_undiagnosedProblems.empty();
}

////TEST : we take the lowest entity instead of the higher...
//map<string, Coordinator *> Aggregator::findTopCoordinator(int swap) {
//	map<string, vector<Coordinator *> > topPb;
//	map<string, int> topLvl;
//	map<string, Coordinator *>::iterator it;
//	map<string, int>::iterator it2;
//	vector<Coordinator *>::iterator it3;
//	map<string, vector<Coordinator *> >::iterator it4;
//	int minLvl;
//	map<string, Coordinator *> tp;
//
//	for (it = m_problems.begin(); it != m_problems.end(); it++) {
//		string classif = (it->second)->getClassification();
//		int dash = classif.find("-");
//		string tree = classif.substr(0, dash);
//		string lvlStr = classif.substr(dash + 1);
//		int level = boost::lexical_cast<int>(lvlStr);
//
//		it2 = topLvl.find(tree);
//
//		if (it2 != topLvl.end())
//			minLvl = topLvl[tree];
//
//		if ((it2 == topLvl.end()) || (swap * level > swap * minLvl)) {
//			topLvl[tree] = level;
//			topPb.erase(tree);
//			vector<Coordinator *> tmp;
//			tmp.push_back(it->second);
//			topPb[tree] = tmp;
//		} else if (level == minLvl) {
//			topPb[tree].push_back(it->second);
//		}
//	}
//
//	for (it4 = topPb.begin(); it4 != topPb.end(); it4++) {
//		vector<Coordinator *> tmp = it4->second;
//		for (it3 = tmp.begin(); it3 != tmp.end(); it3++) {
//			tp[(*it3)->getName()] = *it3;
//		}
//	}
//
//	return tp;
//}

/* 10-12-2012  This version is a bit more elegant.
 * it makes the difference between environment problems and the others
 * for probability update.
 * It also take into account the new problems, even if it does not do much with it..
 *
 * Principle :
 *  The first step is to ask the user for the new problems.
 *  We then build several vectors:
 *  newProblems : contains all the Coordinator that the user will have given
 *  allCurrentProblems : if we are not trying rules, we add the elements of m_problems,
 *                       as well as the elements of m_undiagnosedProblems that are not in m_tryingProblems
 *  currentUndiagnosed : contains the elements of m_undiagnosedProblems that are not in m_tryingProblems
 *  currentProblem : if we are not trying rules, contains all the elements of m_problems
 *  allButImpacted : if we are not trying rules, contains all the elements of m_problems that are not in m_impactedCoordinators,
 *  				 as well as the elements of m_undiagnosedProblems that are not in m_tryingProblems and m_impactedCoordinators
 *
 *  We then construct goneProblems, which is the set different (those in the first not in the second)
 *  between allCurrentProblems and newProblems.
 *  If the recommendation of the RecoveryEngine were applied, we add the history in the database.
 *
 *  For each Coordinator in goneProblems :
 *    - we remove it either from m_problems or m_undiagnosedProblems.
 *    - if it was not an environment problem, we infer a rule between the m_diagnosedCoordinator and the element
 *
 *  We then have a look at m_impactedCoordinators, so the problems we think should be resolved :
 *  For each Coordinator in m_impactedCoordinators, we look whether the problem is still reported
 *  by the user.
 *   - if the problem is still there and if we are not trying rules :
 *       if we did not reach the maximum amount of trials (Coordinator::getMaxRecoveryAttemptsAllowed), we keep trying
 *       if we did reach it, we remove the Coordinators from the m_problems and add it to the m_undiagnosedProblems
 *   - if the problem is not there anymore and was not an environment Problem, we increment the occurrences
 *
 *  If we are not trying rules, I create now a new vector appearedProblems,
 *  which is the set difference of newProblems and allCurrentProblems. I add those
 *  elements to m_problems.
 *
 *
 */
void Aggregator::updateProblemList() {

	//For the moment, we ignore new problems
	std::string logMsg;

	m_iManager->addLog("Waiting to update problem list...");
	string newPbStr;

	ProblemQuestion * p = new ProblemQuestion(
			"Please update the problem list : ");
	m_iManager->ask(p);
	newPbStr = p->getAnswer();
	delete p;

	map<string, Coordinator *> newPb = parseProblemString(newPbStr);
	vector<Coordinator *> newProblems;
	for (map<string, Coordinator *>::iterator it = newPb.begin();
			it != newPb.end(); it++) {
		newProblems.push_back(it->second);
	}

	vector<Coordinator *> allCurrentProblems, currentUndiagnosed,
			currentProblem, allButImpacted;

	if ((!m_tryingValidRules) && (!m_tryingUnvalidRules)) {
		for (std::map<std::string, Coordinator*>::const_iterator it =
				m_problems.begin(); it != m_problems.end(); it++) {
			allCurrentProblems.push_back(it->second);
			currentProblem.push_back(it->second);
			if (std::find(m_impactedCoordinators.begin(),
					m_impactedCoordinators.end(), it->second)
					== m_impactedCoordinators.end())
				allButImpacted.push_back(it->second);
		}
	}

	for (std::map<std::string, Coordinator*>::const_iterator it =
			m_undiagnosedProblems.begin(); it != m_undiagnosedProblems.end();
			it++) {

		// We don't want to infer rules on Coordinator we are just trying as last hope to diagnose
		if (m_tryingProblems.find(it->first) != m_tryingProblems.end())
			continue;

		allCurrentProblems.push_back((it->second));
		currentUndiagnosed.push_back(it->second);
		if (std::find(m_impactedCoordinators.begin(),
				m_impactedCoordinators.end(), it->second)
				== m_impactedCoordinators.end())
			allButImpacted.push_back(it->second);
	}

	vector<Coordinator *> goneProblems(
			allCurrentProblems.size() + newProblems.size()), appearedProblems(
			allCurrentProblems.size() + newProblems.size());

	std::sort(newProblems.begin(), newProblems.end());
	std::sort(allButImpacted.begin(), allButImpacted.end());
	std::sort(allCurrentProblems.begin(), allCurrentProblems.end());

	// 2013-04-05
	// I don't remember why I work with allButImpacted... it does not make sense now
	// I try with allCurrentProblems
	// I think I was working with allButImpacted to avoid making rules A->A
	// I have to check this. But why did I use allCurrentProblems in the end??

//	vector<Coordinator *>::iterator goneEnd = set_difference(
//			allButImpacted.begin(), allButImpacted.end(), newProblems.begin(),
//			newProblems.end(), goneProblems.begin());
	vector<Coordinator *>::iterator goneEnd = set_difference(
			allCurrentProblems.begin(), allCurrentProblems.end(),
			newProblems.begin(), newProblems.end(), goneProblems.begin());

	// If we applied the recommendation, then lets put it in history
	if (m_recommendationApplied) {
		std::string date = m_iManager->getCurrentDate();
		std::string time = m_iManager->getCurrentTime();
		std::string responsibleName = m_respChild->getName();
		std::string userEntry = "No user input supported yet";

		RecoveryHistory historyEvent(date, time, responsibleName, userEntry);
		std::vector<std::pair<std::string, std::string> > historyActions;
		vector<RecoveryUnit> lastProcedure =
				m_recoveryEngine->getLastProcedure();
		for (vector<RecoveryUnit>::const_iterator act = lastProcedure.begin();
				act != lastProcedure.end(); act++) {
			historyActions.push_back(
					std::make_pair((*act).getMaName(), (*act).getActionStr()));
		}
		historyEvent.setActions(historyActions);
		std::vector<std::string> problemsBefore, problemsAfter;

		// 2013-04-05
		// Here it would be better to use allCurrentProblems rather than just currentProblem
		// to also take into account what is in the unsolved list
		// 2013-04-23
		// I found why : to avoid add a rule "A solves A".
		// now, by working on currentProble, there are other stuff I miss, so I rather added a protection
		// in RuleManager::addRule
//		for (vector<Coordinator *>::const_iterator pbIt =
//				currentProblem.begin(); pbIt != currentProblem.end(); pbIt++) {
//			problemsBefore.push_back((*pbIt)->getName());
//		}
		for (vector<Coordinator *>::const_iterator pbIt =
				allCurrentProblems.begin(); pbIt != allCurrentProblems.end();
				pbIt++) {
			problemsBefore.push_back((*pbIt)->getName());
		}
		historyEvent.setProblemsBefore(problemsBefore);

		for (vector<Coordinator *>::const_iterator pbIt = newProblems.begin();
				pbIt != newProblems.end(); pbIt++) {
			problemsAfter.push_back((*pbIt)->getName());
		}

		historyEvent.setProblemsAfter(problemsAfter);

		m_dbApi->addRecoveryHistoryEvent(historyEvent);
	}

	// First we take care of problems that are not reported anymore
	for (vector<Coordinator *>::iterator it = goneProblems.begin();
			it != goneEnd; it++) {
		logMsg = "The problem on " + (*it)->getName() + " is gone";
		m_iManager->addLog(logMsg);

		std::map<std::string, Coordinator *>::iterator coord;
		if ((coord = m_undiagnosedProblems.find((*it)->getName()))
				!= m_undiagnosedProblems.end()) {
			m_undiagnosedProblems.erase(coord);
		} else if ((coord = m_problems.find((*it)->getName()))
				!= m_problems.end()) {
			m_problems.erase(coord);
		} else {
			logMsg = "Strange. I don't have any track of " + (*it)->getName();
			m_iManager->addLog(logMsg);
		}
		// 2013-04-05
		// replace test on m_respChild by the bool returned by analyzeEnvironment
		//if (m_respChild->isEnvironmentResponsible()) {
		if (m_environmentResponsible) {
			logMsg =
					"The problem is an environment problem, so I won't infer any rule";
			m_iManager->addLog(logMsg);
		} else {
			if (m_ruleManager->addSolveRule(m_diagnosedCoordinator, *it)) {
				m_iManager->addLog(
						"We just validated a new rule : "
								+ m_diagnosedCoordinator->getName() + " -> "
								+ (*it)->getName());
			}

		}
	}

//Let's have a look at those we think we have solved
	for (vector<Coordinator *>::iterator it = m_impactedCoordinators.begin();
			it != m_impactedCoordinators.end(); it++) {
		vector<Coordinator *>::iterator coord = find(newProblems.begin(),
				newProblems.end(), *it);

		if ((coord != newProblems.end()) && (!m_tryingUnvalidRules)
				&& (!m_tryingValidRules)) { //the problem we thought to have solved is still there, and it is not a trial Coordinator
			logMsg = "Obviously, repairing " + (*it)->getName()
					+ " did not change anything ...";
			m_iManager->addLog(logMsg);
			if ((*it)->getAnalyzeIterations()
					>= (*it)->getMaxRecoveryAttemptsAllowed()) {
				logMsg = " I already tried a lot, I give up with this one";
				m_iManager->addLog(logMsg);

				m_problems.erase((*it)->getName());
				m_undiagnosedProblems[(*it)->getName()] = (*it);
			} else {
				logMsg = " but I will still give a chance ... ";
				m_iManager->addLog(logMsg);
			}
		} else { //The problem is gone, as we expected. or it is a trial Coordinator
			// 2013-04-05
			// replace test on m_respChild by the bool returned by analyzeEnvironment
			//if (!m_respChild->isEnvironmentResponsible())
			if (!m_environmentResponsible)
				(*it)->updateProbability();
			m_problems.erase((*it)->getName());
		}
	}

// We don't take care of new problems in cases of guess yet
	if ((!m_tryingValidRules) && (!m_tryingUnvalidRules)) {

//		//And now I take care of brand new problems
//		vector<Coordinator *>::iterator appearedEnd = set_difference(
//				newProblems.begin(), newProblems.end(), allButImpacted.begin(),
//				allButImpacted.end(), appearedProblems.begin());
		//And now I take care of brand new problems
		vector<Coordinator *>::iterator appearedEnd = set_difference(
				newProblems.begin(), newProblems.end(),
				allCurrentProblems.begin(), allCurrentProblems.end(),
				appearedProblems.begin());

		for (vector<Coordinator *>::iterator it = appearedProblems.begin();
				it != appearedEnd; it++) {
			logMsg = "A new problem appears : " + (*it)->getName();
//			std::cout << logMsg << std::endl;
			m_iManager->addLog(logMsg);
			m_problems[(*it)->getName()] = (*it);
		}
	}

}

// New version using boost
map<string, Coordinator *> Aggregator::parseProblemString(string problems) {

	m_iManager->setCurrentProblemList(problems);

	std::vector<std::string> strs;
	map<string, Coordinator *> coordMap;

	// We split the problems either using spaces or semi-column
	boost::split(strs, problems, boost::is_any_of("; "));

	for (std::vector<std::string>::const_iterator it = strs.begin();
			it != strs.end(); it++) {
		try {
			Coordinator * t = m_maPool->getCoordinator(*it);
			coordMap[*it] = t;
		} catch (std::exception &s) {
			std::stringstream ss(stringstream::in | stringstream::out);
			ss << s.what();
			m_iManager->addLog(ss.str());
		}
	}

	return coordMap;
}

std::map<std::string, std::string> Aggregator::checkAll() {
	std::map<std::string, std::string> reportedProblems;
	vector<Coordinator *> coordinators = m_maPool->getAllCoordinators();
	vector<Coordinator *>::iterator it;
	string allCoordinator;
	for (it = coordinators.begin(); it != coordinators.end(); it++) {
		allCoordinator += (*it)->getName() + ";";
	}
	m_problems = parseProblemString(allCoordinator);
	/*!
	 * 2013-07-10
	 * Replace findTopCoordinator(-1) with
	 * findRootCoordinators
	 */
//	map<string, Coordinator *> topCoord = findTopCoordinator(-1);
	map<string, Coordinator *> topCoord = findRootCoordinators();
	map<string, Coordinator *>::iterator coord;
	for (coord = topCoord.begin(); coord != topCoord.end(); coord++) {
		if (coord->second->analyze()) {
			reportedProblems[coord->first] = coord->second->getCause();
		}
	}

	return reportedProblems;

}

std::map<std::string, std::string> Aggregator::fullCheck() {
	std::map<std::string, std::string> reportedProblems;
	vector<Agent *> agents = m_maPool->getAllAgents();
	vector<Agent *>::iterator it;

	for (it = agents.begin(); it != agents.end(); it++) {
		if ((*it)->analyze()) {
			reportedProblems[(*it)->getName()] = (*it)->getCause();
		}
	}

	return reportedProblems;

}

std::vector<Coordinator *> Aggregator::followCoordinatorPath(
		std::vector<int> prefix, std::vector<Coordinator *> candidates) {
	std::vector<Coordinator *> returnPath;
	int depth = prefix.size();
	std::map<int, std::vector<Coordinator *> > pathMap;

	for (std::vector<Coordinator *>::iterator c = candidates.begin();
			c != candidates.end(); c++) {
		int head = ((*c)->getClassification())[depth];
		if (pathMap.find(head) == pathMap.end()) {
			std::vector<Coordinator *> empty;
			pathMap[head] = empty;
		}
		pathMap[head].push_back(*c);
	}

	for (std::map<int, std::vector<Coordinator *> >::iterator pathHead =
			pathMap.begin(); pathHead != pathMap.end(); pathHead++) {
		std::vector<int> fp = prefix;
		if ((pathHead->second).size() == 1) {
			returnPath.push_back((pathHead->second).front());
		} else {
			fp.push_back(pathHead->first);
			std::vector<Coordinator *> pathes = pathHead->second;

			std::vector<Coordinator *>::iterator fpIt;
			for (fpIt = pathes.begin(); fpIt != pathes.end(); fpIt++) {
				if ((*fpIt)->getClassification() == fp) {
					break;
				}
			}
			pathes.erase(fpIt);
			std::vector<Coordinator *> nextRet = followCoordinatorPath(fp,
					pathes);
			for (std::vector<Coordinator *>::iterator el = nextRet.begin();
					el != nextRet.end(); el++) {
				returnPath.push_back(*el);
			}

		}
	}

	return returnPath;

}

map<string, Coordinator *> Aggregator::findLowestCoordinators() {
	std::vector<Coordinator *> problemVec;
	std::vector<int> empty;
	std::vector<Coordinator *> lowestVec;
	std::map<std::string, Coordinator *> lowestMap;
	for (std::map<std::string, Coordinator *>::iterator it = m_problems.begin();
			it != m_problems.end(); it++) {
		problemVec.push_back(it->second);
	}

	lowestVec = followCoordinatorPath(empty, problemVec);

	for (std::vector<Coordinator *>::iterator it = lowestVec.begin();
			it != lowestVec.end(); it++) {
		lowestMap[(*it)->getName()] = *it;
	}

	return lowestMap;

}

map<string, Coordinator *> Aggregator::findRootCoordinators() {

	std::map<std::string, Coordinator *> rootCoords;
	for (std::map<std::string, Coordinator *>::iterator it = m_problems.begin();
			it != m_problems.end(); it++) {
		if (it->second->getClassification().size() == 1)
			rootCoords[it->first] = it->second;

	}

	return rootCoords;

}
