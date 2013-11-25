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


#include "UserMsg.hpp"

UserMsg::UserMsg(string msg, msgLevels lvl) {
	m_msg = msg.c_str() ;
	m_lvl = lvl;
}

UserMsg::UserMsg(const UserMsg & source) {
	m_msg = source.m_msg;
	m_lvl = source.m_lvl;
}

UserMsg & UserMsg::operator=(const UserMsg & source) {
	m_msg = source.m_msg;
	m_lvl = source.m_lvl;

	return *this;
}

UserMsg::~UserMsg() {

}

string UserMsg::getMsg() {
	return m_msg;
}

msgLevels UserMsg::getLevel() {
	return m_lvl;
}


ostream& operator<<(ostream& out, UserMsg& m) {
	switch (m.getLevel()) {
	case INFO:
		out << "[INFO] ";
		break;
	case WARNING:
		out << "[WARNING] ";
		break;
	case ERROR:
		out << "[ERROR] ";
		break;
	case CRITICAL:
		out << "[CRITICAL] ";
		break;
	}
	out << m.getMsg() << endl;
	return out;
}
