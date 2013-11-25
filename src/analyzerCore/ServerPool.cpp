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


#include "ServerPool.hpp"

ServerPool::ServerPool(AnalyzerDBAPI * dbApi) {
	m_dbApi = dbApi;
	m_dbApi->retrieveAllServers(m_servers);
}

ServerPool::~ServerPool() {
	map<string, Server *>::iterator it;
	for (it = m_servers.begin(); it != m_servers.end(); it++) {
		delete it->second;
	}

}

Server * ServerPool::getServer(string serverName) const {
	map<std::string, Server*>::const_iterator it = m_servers.find(serverName);
	if ( it == m_servers.end())
		throw std::runtime_error("There is no server called " + serverName);
	return it->second;

}

map<string, Server *> ServerPool::getServersMap() const {
	return m_servers;
}

void ServerPool::reinitializeServersStatus(){
	for (map<string, Server *>::iterator it = m_servers.begin(); it != m_servers.end(); it++) {
		it->second->reinitializeStatus();
	}
}
