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


#include "RuleManager.hpp"

int RuleManager::dependency_validation_threshold;

RuleManager::RuleManager(AnalyzerDBAPI * dbApi, MetaAgentPool * maPool,
		InteractionManager * iManager) {
	m_dbApi = dbApi;
	m_maPool = maPool;
	m_iManager = iManager;
	m_recoveryRulesMap = dbApi->getRecoveryRulesMap();
	map<pair<string, string>, int>::iterator it;
	for (it = m_recoveryRulesMap.begin(); it != m_recoveryRulesMap.end();
			it++) {
		m_recoveryRulesMapRevert[it->second] = it->first;
	}
	m_recoveryRuleTree = dbApi->getRecoveryRulesTree(m_recoveryRulesMap);
	m_recoveryRuleAfterTree = dbApi->getRecoveryRulesTree(m_recoveryRulesMap,
			"after");
	m_recoveryRuleBeforeTree = dbApi->getRecoveryRulesTree(m_recoveryRulesMap,
			"before");

}

bool RuleManager::addSolveRule(Coordinator * a, Coordinator * b) {

	if (a == b)
		return false;

	bool wasNewRuleCreated = false;

	m_iManager->addLog(
			"New Rule : " + a->getName() + " solves " + b->getName());
	m_dbApi->incrementDependencyOccurence(a->getId(), b->getId());

	// Validate the rule if we reached the dependency_validation_threshold
	if (dependency_validation_threshold) {
		int ruleOccurrence = m_dbApi->getDependencyOccurence(a->getId(),
				b->getId());
		if (ruleOccurrence == dependency_validation_threshold) {
			m_iManager->addLog(
					"I valid the rule " + a->getName() + " -> " + b->getName());
			m_dbApi->validateDependency(a->getId(), b->getId());
			wasNewRuleCreated = true;
		}
	}

	return wasNewRuleCreated;

}

int RuleManager::getWeight(Coordinator * a) {
	int w = 0;
	if (m_weightMap.find(a) != m_weightMap.end()) {
		w = m_weightMap[a];
	} else {
		int id = a->getId();
		vector<string> afterNames(10);
		afterNames = m_dbApi->getAfters(id);
		vector<string>::iterator it;

		for (it = afterNames.begin(); it != afterNames.end(); it++) {
			w += 1 + getWeight(m_maPool->getCoordinator(*it));
		}
		m_weightMap[a] = w;
	}

	return w;
}

vector<Coordinator *> RuleManager::sortAccordingDependencies(
		vector<Coordinator *> coord) {
	if (!m_weightMap.empty())
		m_weightMap.clear();
	vector<Coordinator *> sortedCoord;
	vector<Coordinator *>::iterator it;

	for (it = coord.begin(); it != coord.end(); it++) {
		sortedCoord.push_back(*it);
	}

	for (it = sortedCoord.begin(); it != sortedCoord.end(); it++) {
		(*it)->setWeight(getWeight(*it));
	}

	sort(sortedCoord.begin(), sortedCoord.end(), compareCoordinatorWeight);

	return sortedCoord;
}

bool compareCoordinatorWeight(Coordinator * a, Coordinator * b) {
	return (a->getWeight() > b->getWeight());
}

bool RuleManager::isConnected(int start, int end,
		vector<pair<int, int> > &edges, int amountOfNodes,
		map<int, vector<int> > & cache) {

	map<int, vector<int> >::iterator el = cache.find(start);

// The DFS has not been done for the start element yet. So we do it...
	if (el == cache.end()) {
		cache[start] = BFS(start, edges, amountOfNodes);
	}

	vector<int> connectedToStart = cache[start];

//if end is in the list of nodes connected to start
	if ((find(connectedToStart.begin(), connectedToStart.end(), end))
			!= connectedToStart.end())
		return true;
	else
		return false;

}

vector<int> RuleManager::BFS(int start, vector<pair<int, int> > & edges,
		int amountOfNodes, bool reverseOrder) {

	queue<int> Q;
	vector<int> result;
	vector<bool> visited(amountOfNodes, false);
	int i;

	Q.push(start);
	visited[start] = true;
	vector<pair<int, int> >::iterator found;

	while (!Q.empty()) {
		int k = Q.front();
		Q.pop();
		result.push_back(k);
		for (i = 0; i < amountOfNodes; ++i) {
			//found will be edges.end() if there is no link k->i
			found = find(edges.begin(), edges.end(), pair<int, int>(k, i));
			if ((found != edges.end()) && !visited[i]) {
				Q.push(i);
				visited[i] = true;
			} else {
			}
		}
	}

	if (reverseOrder)
		reverse(result.begin(), result.end());

	return result;
}

// We define A < B if either
// * there is a path from A to B in the after tree
// * there is a path from B to A in the before tree
bool RuleManager::sortstruct::operator()(int A, int B) {
	bool AtoBafter = m_rm->isConnected(A, B, m_rm->m_recoveryRuleAfterTree,
			m_rm->m_recoveryRulesMap.size(), m_rm->m_connexAfter);
	if (AtoBafter)
		return true;
	bool BtoAbefore = m_rm->isConnected(B, A, m_rm->m_recoveryRuleBeforeTree,
			m_rm->m_recoveryRulesMap.size(), m_rm->m_connexBefore);

	return (AtoBafter || BtoAbefore);

}

vector<RecoveryUnit> RuleManager::RecoveryRuleBFS(
		vector<RecoveryUnit> startRules) {

	vector<RecoveryUnit> ret;

	vector<RecoveryUnit> lonelyRules; // It is possible that an action does not imply any other, so it does not have a place in the tree.
// In this case it does not have an id. So we just push it there, and at the end, we put them
// wherever we want.

	ret.clear();

	vector<int> startRulesId;
	map<int, RecoveryUnit> startRulesMap;
	int treeSize = m_recoveryRulesMap.size() + 1; // +1 for the artificial node

	for (vector<RecoveryUnit>::iterator it = startRules.begin();
			it != startRules.end(); it++) {
		pair<string, string> startPair(it->getMaName(), it->getActionStr());

		if (m_recoveryRulesMap.find(startPair) == m_recoveryRulesMap.end()) { //No rules concerning this action, so we put it into the lonelyRules
			lonelyRules.push_back(*it);
		} else {
//			cout << "initial id " << m_recoveryRulesMap[startPair] << endl;
			startRulesId.push_back(m_recoveryRulesMap[startPair]);
			startRulesMap[m_recoveryRulesMap[startPair]] = *it;
		}

	}

	vector<pair<int, int> > ruleTree = m_recoveryRuleTree;
//create artificial nodes
	for (vector<int>::iterator it = startRulesId.begin();
			it != startRulesId.end(); it++) {
		bool createLink = true;

		for (vector<int>::iterator it2 = startRulesId.begin();
				it2 != startRulesId.end(); it2++) {
			if (find(ruleTree.begin(), ruleTree.end(),
					pair<int, int>(*it2, *it)) != ruleTree.end()) { //if there is a link it2->it
				createLink = false;
			}
		}

		if (createLink)
			ruleTree.push_back(pair<int, int>(0, *it));

	}

	vector<int> connectedRules = BFS(0, ruleTree, treeSize);

	connectedRules.erase(find(connectedRules.begin(), connectedRules.end(), 0)); //We remove 0 from the list, since it is a fake entity
	sortstruct s(this);
	sort(connectedRules.begin(), connectedRules.end(), s);

	for (vector<int>::iterator it = connectedRules.begin();
			it != connectedRules.end(); it++) {

		map<int, RecoveryUnit>::iterator ruleInStartRules = startRulesMap.find(
				*it);
		if (ruleInStartRules != startRulesMap.end()) { //Then it is a rule that was give at the start.
			ret.push_back(ruleInStartRules->second);

		} else {
			pair<string, string> p = m_recoveryRulesMapRevert[*it];
			string agentName = p.first;
			string serverName =
					m_maPool->getMetaAgent(p.first)->getServer()->getName();
			string action = p.second;

			RecoveryUnit ru(agentName, serverName, action);

			ret.push_back(ru);
		}
	}

//Now we append all the lonely rules :
	for (vector<RecoveryUnit>::iterator it = lonelyRules.begin();
			it != lonelyRules.end(); it++) {
		ret.push_back(*it);
	}

	return ret;

}

std::map<std::string, Coordinator *> RuleManager::getPredecessors(
		std::map<std::string, Coordinator *> & unsolvedPb) const {
	std::map<std::string, Coordinator *> predecessors;
	std::set<std::string> predecessorsName;

	for (std::map<std::string, Coordinator *>::const_iterator it =
			unsolvedPb.begin(); it != unsolvedPb.end(); it++) {
		std::vector<std::string> before = m_dbApi->getBefores(
				it->second->getId());
		for (std::vector<std::string>::const_iterator st = before.begin();
				st != before.end(); st++)
			predecessorsName.insert(*st);
	}

	for (std::set<std::string>::const_iterator it = predecessorsName.begin();
			it != predecessorsName.end(); it++) {
		Coordinator * coord = m_maPool->getCoordinator(*it);
		predecessors[*it] = coord;
	}

	return predecessors;
}
