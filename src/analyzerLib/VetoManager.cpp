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


#include "VetoManager.hpp"

void VetoManager::setVeto(std::string name, bool value) {
	m_vetoMap[name] = value;
}

bool VetoManager::getVeto(std::string name) const {
	std::map<std::string, bool>::const_iterator elt;
	elt = m_vetoMap.find(name);

	if (elt == m_vetoMap.end())
		return false;

	return elt->second;
}

std::map<std::string, bool> VetoManager::getAllVeto() const{
	return m_vetoMap;
}
