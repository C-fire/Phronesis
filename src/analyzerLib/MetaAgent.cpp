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


#include "MetaAgent.hpp"

int MetaAgent::m_deep = 0;
int MetaAgent::TMP_attempts;
std::string MetaAgent::agent_port;
int MetaAgent::agent_timeout;

MetaAgent::MetaAgent() {
	m_id = -1;
	m_pm = NULL;
	m_proba = 0;
	m_weight = 0;
	m_responsibleChild = this;
	m_analyzeIterations = 0;
	m_iManager = NULL;
	m_remoteAgentPb = false;
}

MetaAgent::MetaAgent(int id, string name) {
	this->m_id = id;
	this->m_name = name;
	m_proba = 0;
	m_weight = 0;
	m_pm = NULL;
	m_responsibleChild = this;
	m_iManager = NULL;
	m_analyzeIterations = 0;
	m_remoteAgentPb = false;
}
MetaAgent::MetaAgent(const MetaAgent & source) {
	m_id = source.m_id;
	m_name = source.m_name;
	m_pm = source.m_pm;
	m_proba = source.m_proba;
	m_weight = source.m_weight; //the same than m_proba. The weight is calculated from the rules
	m_cause = source.m_cause;
	m_responsibleChild = source.m_responsibleChild;
	m_analyzeIterations = source.m_analyzeIterations;
	m_iManager = source.m_iManager;
	m_remoteAgentPb = source.m_remoteAgentPb;
}

MetaAgent::~MetaAgent() {
}

MetaAgent & MetaAgent::operator=(const MetaAgent& source) {
	m_id = source.m_id;
	m_name = source.m_name;
	m_pm = source.m_pm;
	m_proba = source.m_proba;
	m_weight = source.m_weight; //the same than m_proba. The weight is calculated from the rules
	m_cause = source.m_cause;
	m_responsibleChild = source.m_responsibleChild;
	m_analyzeIterations = source.m_analyzeIterations;
	m_iManager = source.m_iManager;
	m_remoteAgentPb = source.m_remoteAgentPb;

	return *this;
}
bool MetaAgent::analyze() {
	m_analyzeIterations++;
	return 0;
}

void MetaAgent::addChild(MetaAgent * child) {

}

void MetaAgent::setPManager(ProbabilityManager * pm) {
	this->m_pm = pm;
}

string MetaAgent::getName() const {
	return m_name;
}

int MetaAgent::getId() const {
	return m_id;
}

ostream& MetaAgent::printOperator(ostream& out) const {
	out << "ID : " << this->m_id << " name " << this->m_name;
	return out;
}

double MetaAgent::getProba() const {
	return m_proba;
}

void MetaAgent::setProba(double p) {
	m_proba = p;
}

int MetaAgent::getWeight() const {
	return m_weight;
}

void MetaAgent::setWeight(int w) {
	m_weight = w;
}

string MetaAgent::getCause() const {
	return m_cause;
}

set<Server*> MetaAgent::getInvolvedServers() const {
	set<Server*> empt;
	return empt;
}

void MetaAgent::updateProbability() {
}

int MetaAgent::getAnalyzeIterations() const {
	return m_analyzeIterations;
}

Server * MetaAgent::getServer() const {
	throw 0; //Only the Agent send back one server
}

MetaAgent * MetaAgent::getResponsibleChild() const {
	return m_responsibleChild;
}

void MetaAgent::setInteractionManager(InteractionManagerAbstract * iManager) {
	m_iManager = iManager;
}

void MetaAgent::setResponsibleChild(MetaAgent * responsible) {
	m_responsibleChild = responsible;
}

bool MetaAgent::isServerPingable() {
	if (this == m_responsibleChild)
		return false;
	return m_responsibleChild->isServerPingable();
}

void MetaAgent::reinitializeStatus() {
	m_responsibleChild = NULL;
	m_analyzeIterations = 0;

}

bool MetaAgent::isEnvironmentResponsible() {
	return false;
}

void MetaAgent::setCause(std::string cause) {
	m_cause = cause;
	m_iManager->addLog(m_cause);
}

bool sortMetaAgentByProba(MetaAgent * a, MetaAgent * b) {
	double a_occ = a->getProba();
	double b_occ = b->getProba();

	return (a_occ > b_occ);
}

bool operator==(const MetaAgent &a, const MetaAgent &b) {
	int eq = (a.getName()).compare(b.getName());
	return (eq == 0);
}

bool operator<(const MetaAgent &a, const MetaAgent &b) {
	if (a.getName() < b.getName())
		return true;
	else if (b.getName() < a.getName())
		return false;

	return false;
}

