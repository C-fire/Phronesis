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


#include "AgentRecovery.hpp"

AgentRecovery::AgentRecovery() {
	m_agent = NULL;
}

AgentRecovery::AgentRecovery(Agent * agent) {
	this->m_agent = agent;
}

AgentRecovery::AgentRecovery(Agent * agent, recoveryAction action) {
	this->m_agent = agent;
	m_recoveryUnits.push_back(
			RecoveryUnit(agent->getName(), agent->getServer()->getAddress(),
					action));
}

AgentRecovery::AgentRecovery(const AgentRecovery & source) {
	m_agent = source.m_agent;
	m_recoveryUnits = source.m_recoveryUnits;

}

AgentRecovery & AgentRecovery::operator=(const AgentRecovery & source) {
	m_agent = source.m_agent;
	m_recoveryUnits = source.m_recoveryUnits;
	return *this;
}

AgentRecovery::~AgentRecovery() {

}
void AgentRecovery::analyzeAgent() {
	m_recoveryUnits.clear();
}

vector<RecoveryUnit> AgentRecovery::getRecoveryUnits() const {
	return m_recoveryUnits;
}

Agent * AgentRecovery::getAgent() const {
	return m_agent;
}

