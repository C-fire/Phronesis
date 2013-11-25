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


#include "Agent.hpp"

Agent::Agent() :
		MetaAgent() {
	m_server = NULL;
}

Agent::Agent(int id, string name, Server * server) :
		MetaAgent(id, name) {
	m_server = server;

}

Agent::Agent(const Agent & src) :
		MetaAgent(src) {
	m_server = src.m_server;
}

Agent::~Agent() {

}

Agent & Agent::operator=(const Agent& source) {
	*this = (MetaAgent&) source;
	m_server = source.m_server;
	return *this;
}

bool Agent::analyze() {
	MetaAgent::analyze();


#ifdef SIMU
	bool problem = false;
	string line;
	ifstream myfile ("/tmp/phrSimuFile");
	if (myfile.is_open())
	{
		while ( myfile.good() )
		{
			getline (myfile,line);
			if (boost::starts_with(line, m_name)) {
				std::vector<std::string> strs;
//				std::cout << "Read in file : " << line << std::endl;
				boost::split(strs, line, boost::is_any_of(":"));
				problem = boost::lexical_cast<int>(strs[1]);
				myfile.close();
				if (problem){
					m_responsibleChild = this;
					m_iManager->sendStatistic(m_name + "_responsible");

				}


				return problem;
			}

		}
		myfile.close();
		m_iManager->addLog(m_name + " : unable to find my name");
	}
	else {
		m_iManager->addLog(m_name + " : unable to open file");

	}

	return problem;

#endif
	return false;
}

ostream& Agent::printOperator(ostream& out) const {
	out << "	[Agent] " << (*this) << endl;
	return out;
}

set<Server*> Agent::getInvolvedServers() const {
	set<Server*> involved;
	involved.insert(m_server);
	return involved;
}

void Agent::updateProbability() {
	//Not implemented
	//throw;
	m_pm->incrementLocalTotal(this);

}

Server * Agent::getServer() const {
	return m_server;
}

void Agent::setServer(Server * srv){
	m_server = srv;
}

