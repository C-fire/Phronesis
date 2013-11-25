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


#include "Server.hpp"

Server::Server() {
	m_id = 0;
	m_envAgent = NULL;
}

Server::Server(int id, string name, string address) {
	m_id = id;
	m_name = name;
	m_address = address;
	m_envAgent = NULL;

}

Server::Server(const Server & source) {
	m_id = source.m_id;
	m_name = source.m_name;
	m_address = source.m_address;
	m_envAgent = source.m_envAgent;
	m_envCoords = source.m_envCoords;
}

Server & Server::operator=(const Server & source) {
	m_id = source.m_id;
	m_name = source.m_name;
	m_address = source.m_address;
	m_envAgent = source.m_envAgent;
	m_envCoords = source.m_envCoords;

	return *this;
}

string Server::getName() const {
	return m_name;
}

string Server::getAddress() const {
	return m_address;
}

int Server::getId() const {
	return m_id;
}

bool Server::isAlive() const {
	string cmd = "ping -c 1 " + m_address + " >/dev/null";
	int ret = system(cmd.c_str());
	return !ret;

}

void Server::setEnvironmentAgent(EnvironmentAgent * ea) {
	m_envAgent = ea;
}

EnvironmentAgent * Server::getEnvironmentAgent() {
	return m_envAgent;
}

void Server::reinitializeStatus() {
}

void Server::addAttachedCoordinator(Coordinator * coord) {
	m_envCoords.push_back(coord);
}

std::vector<Coordinator *> Server::getAttachedCoordinators() {
	return m_envCoords;
}

