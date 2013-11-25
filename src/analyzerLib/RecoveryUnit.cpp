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


#include "RecoveryUnit.hpp"
RecoveryUnit::RecoveryUnit() {
	m_action = RECOVERY_ERROR;
}

RecoveryUnit::RecoveryUnit(string maName, string server,
		recoveryAction action) {
	m_action = action;
	m_actionStr = recoveryActionToString(action);
	m_server = server;
	m_maName = maName;
}

RecoveryUnit::RecoveryUnit(string maName, string server, string actionStr) {

	m_action = recoveryActionFromString(actionStr);
	m_actionStr = actionStr;
	m_server = server;
	m_maName = maName;

}

RecoveryUnit::RecoveryUnit(const RecoveryUnit& source) {
	m_action = source.m_action;
	m_actionStr = source.m_actionStr;
	m_server = source.m_server;
	m_maName = source.m_maName;
	m_problem = source.m_problem;
	m_command = source.m_command;
	m_solution = source.m_solution;
	m_advancedAdvice = source.m_advancedAdvice;
}

RecoveryUnit & RecoveryUnit::operator=(const RecoveryUnit & source) {
	m_action = source.m_action;
	m_actionStr = source.m_actionStr;
	m_server = source.m_server;
	m_maName = source.m_maName;
	m_problem = source.m_problem;
	m_command = source.m_command;
	m_solution = source.m_solution;
	m_advancedAdvice = source.m_advancedAdvice;

	return *this;
}

string RecoveryUnit::getAdvancedAdvice() const {
	return m_advancedAdvice;
}

string RecoveryUnit::getProblem() const {
	return m_problem;
}

string RecoveryUnit::getSolution() const {
	return m_solution;
}

string RecoveryUnit::getDescription() const {
	stringstream ss;
	ss << "Problem : " << m_problem << endl;
	ss << "Solution : " << m_solution << endl;
	ss << "Advanced advices : " << m_advancedAdvice << endl;
	return ss.str();
}

void RecoveryUnit::setAdvancedAdvice(string advancedAdvice) {
	m_advancedAdvice = advancedAdvice;
}

void RecoveryUnit::setProblem(string problem) {
	m_problem = problem;
}

void RecoveryUnit::setSolution(string solution) {
	m_solution = solution;
}

recoveryAction RecoveryUnit::getAction() const {
	return m_action;
}

string RecoveryUnit::getActionStr() const {
	return m_actionStr;
}

string RecoveryUnit::getCommand() const {
	return m_command;
}

string RecoveryUnit::getMaName() const {
	return m_maName;
}

void RecoveryUnit::setAction(recoveryAction action) {
	m_action = action;
}

void RecoveryUnit::setActionStr(string actionStr) {
	m_actionStr = actionStr;
}

void RecoveryUnit::setCommand(string command) {
	m_command = command;
}

void RecoveryUnit::setMaName(string maName) {
	m_maName = maName;
}

string RecoveryUnit::getServer() const {
	return m_server;
}

void RecoveryUnit::setServer(string server) {
	m_server = server;
}

recoveryAction recoveryActionFromString(string ra) {
	if (!ra.compare("RECOVERY_FILE_CREATE"))
		return RECOVERY_FILE_CREATE;
	if (!ra.compare("RECOVERY_FILE_CHGRP"))
		return RECOVERY_FILE_CHGRP;
	if (!ra.compare("RECOVERY_FILE_CHOWN"))
		return RECOVERY_FILE_CHOWN;
	if (!ra.compare("RECOVERY_FILE_CHMOD"))
		return RECOVERY_FILE_CHMOD;
	if (!ra.compare("RECOVERY_FILE_CHATTR"))
		return RECOVERY_FILE_CHATTR;
	if (!ra.compare("RECOVERY_FILE_PATTERN"))
		return RECOVERY_FILE_PATTERN;
	if (!ra.compare("RECOVERY_FILE_MD5"))
		return RECOVERY_FILE_MD5;
	if (!ra.compare("RECOVERY_PROCESS_START"))
		return RECOVERY_PROCESS_START;
	if (!ra.compare("RECOVERY_PROCESS_STOP"))
		return RECOVERY_PROCESS_STOP;
	if (!ra.compare("RECOVERY_PROCESS_RESTART"))
		return RECOVERY_PROCESS_RESTART;
	if (!ra.compare("RECOVERY_PROCESS_ULIMIT"))
		return RECOVERY_PROCESS_ULIMIT;
	if (!ra.compare("RECOVERY_PROCESS_CHUSER"))
		return RECOVERY_PROCESS_CHUSER;
	if (!ra.compare("RECOVERY_PROCESS_ENVIRONMENT"))
		return RECOVERY_PROCESS_ENVIRONMENT;
	if (!ra.compare("RECOVERY_SERVICE_START"))
		return RECOVERY_SERVICE_START;
	if (!ra.compare("RECOVERY_SERVICE_STOP"))
		return RECOVERY_SERVICE_STOP;
	if (!ra.compare("RECOVERY_SERVICE_RESTART"))
		return RECOVERY_SERVICE_RESTART;
	if (!ra.compare("RECOVERY_SERVICE_RELOAD"))
		return RECOVERY_SERVICE_RELOAD;
	if (!ra.compare("RECOVERY_SERVICE_CHUSER"))
		return RECOVERY_SERVICE_CHUSER;
	if (!ra.compare("RECOVERY_SERVICE_ENVIRONMENT"))
		return RECOVERY_SERVICE_ENVIRONMENT;
	if (!ra.compare("RECOVERY_ENV_MOUNT"))
		return RECOVERY_ENV_MOUNT;
	if (!ra.compare("RECOVERY_ENV_UMOUNT"))
		return RECOVERY_ENV_UMOUNT;
	if (!ra.compare("RECOVERY_ENV_DISK"))
		return RECOVERY_ENV_DISK;
	if (!ra.compare("RECOVERY_ENV_INODE"))
		return RECOVERY_ENV_INODE;
	if (!ra.compare("RECOVERY_ENV_FSTAB"))
		return RECOVERY_ENV_FSTAB;
	if (!ra.compare("RECOVERY_ENV_CPU"))
		return RECOVERY_ENV_CPU;
	if (!ra.compare("RECOVERY_ENV_OVERLOAD"))
		return RECOVERY_ENV_OVERLOAD;
	if (!ra.compare("RECOVERY_ENV_SWAP"))
		return RECOVERY_ENV_SWAP;
	if (!ra.compare("RECOVERY_ENV_MEM"))
		return RECOVERY_ENV_MEM;
	if (!ra.compare("RECOVERY_ENV_ON"))
		return RECOVERY_ENV_ON;
	if (!ra.compare("RECOVERY_ENV_OFF"))
		return RECOVERY_ENV_OFF;
	if (!ra.compare("RECOVERY_ENV_REBOOT"))
		return RECOVERY_ENV_REBOOT;
	if (!ra.compare("RECOVERY_ENV_AGENT"))
		return RECOVERY_ENV_AGENT;

	return RECOVERY_ERROR;

}

string recoveryActionToString(recoveryAction ra) {
	switch (ra) {
	case RECOVERY_FILE_CREATE:
		return "RECOVERY_FILE_CREATE";
	case RECOVERY_FILE_CHGRP:
		return "RECOVERY_FILE_CHGRP";
	case RECOVERY_FILE_CHOWN:
		return "RECOVERY_FILE_CHOWN";
	case RECOVERY_FILE_CHMOD:
		return "RECOVERY_FILE_CHMOD";
	case RECOVERY_FILE_CHATTR:
		return "RECOVERY_FILE_CHATTR";
	case RECOVERY_FILE_PATTERN:
		return "RECOVERY_FILE_PATTERN";
	case RECOVERY_FILE_MD5:
		return "RECOVERY_FILE_MD5";
	case RECOVERY_PROCESS_START:
		return "RECOVERY_PROCESS_START";
	case RECOVERY_PROCESS_STOP:
		return "RECOVERY_PROCESS_STOP";
	case RECOVERY_PROCESS_RESTART:
		return "RECOVERY_PROCESS_RESTART";
	case RECOVERY_PROCESS_ULIMIT:
		return "RECOVERY_PROCESS_ULIMIT";
	case RECOVERY_PROCESS_CHUSER:
		return "RECOVERY_PROCESS_CHUSER";
	case RECOVERY_PROCESS_ENVIRONMENT:
		return "RECOVERY_PROCESS_ENVIRONMENT";
	case RECOVERY_SERVICE_START:
		return "RECOVERY_SERVICE_START";
	case RECOVERY_SERVICE_STOP:
		return "RECOVERY_SERVICE_STOP";
	case RECOVERY_SERVICE_RESTART:
		return "RECOVERY_SERVICE_RESTART";
	case RECOVERY_SERVICE_RELOAD:
		return "RECOVERY_SERVICE_RELOAD";
	case RECOVERY_SERVICE_CHUSER:
		return "RECOVERY_SERVICE_CHUSER";
	case RECOVERY_SERVICE_ENVIRONMENT:
		return "RECOVERY_SERVICE_ENVIRONMENT";
	case RECOVERY_ENV_MOUNT:
		return "RECOVERY_ENV_MOUNT";
	case RECOVERY_ENV_UMOUNT:
		return "RECOVERY_ENV_UMOUNT";
	case RECOVERY_ENV_DISK:
		return "RECOVERY_ENV_DISK";
	case RECOVERY_ENV_INODE:
		return "RECOVERY_ENV_INODE";
	case RECOVERY_ENV_FSTAB:
		return "RECOVERY_ENV_FSTAB";
	case RECOVERY_ENV_CPU:
		return "RECOVERY_ENV_CPU";
	case RECOVERY_ENV_OVERLOAD:
		return "RECOVERY_ENV_OVERLOAD";
	case RECOVERY_ENV_SWAP:
		return "RECOVERY_ENV_SWAP";
	case RECOVERY_ENV_MEM:
		return "RECOVERY_ENV_MEM";
	case RECOVERY_ENV_ON:
		return "RECOVERY_ENV_ON";
	case RECOVERY_ENV_OFF:
		return "RECOVERY_ENV_OFF";
	case RECOVERY_ENV_REBOOT:
		return "RECOVERY_ENV_REBOOT";
	case RECOVERY_ENV_AGENT:
		return "RECOVERY_ENV_AGENT";
	default:
		return "RECOVERY_ERROR";
	}

	return "RECOVERY_ERROR";
}

std::ostream& operator <<(std::ostream& Stream, const RecoveryUnit & recUnit) {

	Stream << "MetaAgent name : " << recUnit.m_maName << endl;
	Stream << "Server : " << recUnit.m_server << endl;
	Stream << "RecoveryAction : " << recUnit.m_actionStr << " ("
			<< recUnit.m_action << ")" << endl;

	if (recUnit.m_problem.compare("")) {
		Stream << "Problem : " << recUnit.m_problem << endl;
	}
	if (recUnit.m_solution.compare("")) {
		Stream << "Solution : " << recUnit.m_solution << endl;
	}
	if (recUnit.m_advancedAdvice.compare("")) {
		Stream << "Advanced advice : " << recUnit.m_advancedAdvice << endl;
	}
	if (recUnit.m_command.compare("")) {
		Stream << "Command : " << recUnit.m_command << endl;
	}

	return Stream;
}

bool operator==(RecoveryUnit const &r1, RecoveryUnit const &r2) {
	string ma1 = r1.getMaName();
	string ma2 = r2.getMaName();
	string a1 = r1.getActionStr();
	string a2 = r2.getActionStr();

	return (!((ma1.compare(ma2)) || (a1.compare(a2))));

}

