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


#include "MetaAgentPool.hpp"

MetaAgentPool::MetaAgentPool(AnalyzerDBAPI * dbApi,
		InteractionManager * iManager, ProbabilityManager * pm, ServerPool * sm,
		VetoManager * vManager) {
	this->m_dbApi = dbApi;
	this->m_vManager = vManager;
	this->m_sm = sm;
	this->m_dbApi->retrieveAllAgents(this->m_agents, m_sm->getServersMap());
	this->m_dbApi->retrieveAllCoordinators(m_coordinators);

	vector<Agent *>::iterator it;
	for (it = m_agents.begin(); it != m_agents.end(); it++) {
		(*it)->setPManager(pm);
		(*it)->setInteractionManager(iManager);

		m_metaAgents[(*it)->getName()] = *it;
	}

	vector<Coordinator *>::iterator it2;
	for (it2 = m_coordinators.begin(); it2 != m_coordinators.end(); it2++) {
		(*it2)->setPManager(pm);
		(*it2)->setInteractionManager(iManager);
		(*it2)->setVetoManager(m_vManager);
		m_metaAgents[(*it2)->getName()] = *it2;


	}

	try {
		buildTree();
	} catch (std::exception &e) {
		iManager->addLog(
				"Cannot build the MetaAgent Tree\nDetails : "
						+ std::string(e.what()));
		throw;
	}

	try {
		attachCoordinatorsToServers();
	} catch (std::exception &e) {
		iManager->addLog(
				"Cannot attach Coordinators to Servers\nDetails : "
						+ std::string(e.what()));
		throw;
	}

}

MetaAgentPool::~MetaAgentPool() {
	vector<Agent *>::iterator it2;
	for (it2 = m_agents.begin(); it2 != m_agents.end(); it2++) {
		delete *it2;
	}

	vector<Coordinator *>::iterator it3;
	for (it3 = m_coordinators.begin(); it3 != m_coordinators.end(); it3++) {
		delete *it3;
	}
}

void MetaAgentPool::buildTree() {
	vector<vector<string> > tree = m_dbApi->getTree();
	vector<vector<string> >::iterator it;

	for (it = tree.begin(); it != tree.end(); it++) {
		string parent_name = (*it)[0];
		string daughter_name = (*it)[1];


		MetaAgent * parent = getMetaAgent(parent_name);
		MetaAgent * daughter = getMetaAgent(daughter_name);

		parent->addChild(daughter);

	}
}

void MetaAgentPool::attachCoordinatorsToServers() {

	vector<vector<string> > tree = m_dbApi->getServerAttachedCoordinators();
	vector<vector<string> >::iterator it;

	for (it = tree.begin(); it != tree.end(); it++) {
		string server_name = (*it)[0];
		string coord_name = (*it)[1];

		Server * srv = m_sm->getServer(server_name);
		Coordinator * coord = getCoordinator(coord_name);

		srv->addAttachedCoordinator(coord);

	}

}

MetaAgent * MetaAgentPool::getMetaAgent(string name) const {
	map<string, MetaAgent *>::const_iterator it;
	it = m_metaAgents.find(name);

	if (it == m_metaAgents.end()) {
		throw std::runtime_error("No MetaAgent called " + name);
	}
	return it->second;

}

Agent * MetaAgentPool::getAgent(string name) const {
	MetaAgent * ma;

	ma = getMetaAgent(name);

	vector<Agent *>::const_iterator pA;
	pA = find(m_agents.begin(), m_agents.end(), ma);

	if (pA == m_agents.end()) {
		throw std::runtime_error("MetaAgent " + name + " is not of type Agent");
	}
	return *pA;
}

Coordinator * MetaAgentPool::getCoordinator(string name) const {
	MetaAgent * ma;
	ma = getMetaAgent(name);

	vector<Coordinator *>::const_iterator pC;
	pC = find(m_coordinators.begin(), m_coordinators.end(), ma);

	if (pC == m_coordinators.end()) {
		throw std::runtime_error(
				"MetaAgent " + name + " is not of type Coordinator");
	}
	return *pC;
}



vector<Coordinator *> MetaAgentPool::getAllCoordinators() const {
	return this->m_coordinators;
}

vector<Agent *> MetaAgentPool::getAllAgents() const {
	return this->m_agents;
}

void MetaAgentPool::reinitializeMetaAgents() {
	map<string, MetaAgent*>::iterator it;

	for (it = m_metaAgents.begin(); it != m_metaAgents.end(); it++) {
		it->second->reinitializeStatus();
	}
}

