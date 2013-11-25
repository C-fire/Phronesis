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


#include "FileRecovery.hpp"

FileRecovery::FileRecovery() :
		AgentRecovery() {
	m_fileAgent = NULL;

}

FileRecovery::FileRecovery(FileAgent * agent) :
		AgentRecovery(agent) {
	m_fileAgent = agent;
	analyzeAgent();
}

FileRecovery::FileRecovery(FileAgent * agent, recoveryAction action) :
		AgentRecovery(agent, action) {
	m_fileAgent = agent;

}

FileRecovery::FileRecovery(const FileRecovery & source) :
		AgentRecovery(source) {
	m_fileAgent = source.m_fileAgent;
}

FileRecovery & FileRecovery::operator=(const FileRecovery & source) {
	*this = (AgentRecovery&) source;
	m_fileAgent = source.m_fileAgent;
	return *this;
}

FileRecovery::~FileRecovery() {

}

void FileRecovery::analyzeAgent() {
	AgentRecovery::analyzeAgent();

	string agentName = m_fileAgent->getName();
	string server = m_fileAgent->getServer()->getAddress();
	stringstream ss;

	string pb, sol, adv, com;
	recoveryAction act;
	vector<recoveryAction> acts;

	// If the file is not present :
	// - either the path is not mounted, in which case we suggest to do so
	// - either it is really not there, in which case we suggest to create it
	if (m_fileAgent->m_no_such_file) {

		if (m_fileAgent->m_notMounted) {
			pb = "The path where the file is supposed to be ("
					+ m_fileAgent->m_mountpoint.m_dir
					+ ") seems to be missing.";
			sol = "You should mount it it.";
			act = RECOVERY_FILE_MOUNT_MISSING;
			com = getCommand(act);
			ss.str(std::string());
			ss << "The mountpoint is defined as follow : " << std::endl;
			ss << "Device : " << m_fileAgent->m_mountpoint.m_device
					<< std::endl;
			ss << "Directory : " << m_fileAgent->m_mountpoint.m_dir
					<< std::endl;
			ss << "Type : " << m_fileAgent->m_mountpoint.m_type << std::endl;
			ss << "Options : " << m_fileAgent->m_mountpoint.m_options
					<< std::endl;
			adv = ss.str();
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			ru.setAdvancedAdvice(adv);
			m_recoveryUnits.push_back(ru);
		} else {

			pb = "The file does not seem to be present.";
			sol = "You should create it.";
			act = RECOVERY_FILE_CREATE;
			com = getCommand(act);
			ss.str(std::string());
			ss
					<< "It is also possible that you have a problem with the device serving the file : "
					<< m_fileAgent->m_mountpoint.m_device << std::endl;
			adv = ss.str();
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			ru.setAdvancedAdvice(adv);
			m_recoveryUnits.push_back(ru);
		}

	}

	if (m_fileAgent->m_owner_pb) {
		pb = "The file has the wrong owner ('" + m_fileAgent->m_true_owner
				+ "' instead of '" + m_fileAgent->m_owner + "').";
		sol = "You should change it.";
		act = RECOVERY_FILE_CHOWN;
		com = getCommand(act);
		RecoveryUnit ru(agentName, server, act);
		ru.setProblem(pb);
		ru.setSolution(sol);
		ru.setCommand(com);
		m_recoveryUnits.push_back(ru);

	}

	if (m_fileAgent->m_group_pb) {

		pb = "The file has the wrong group ('" + m_fileAgent->m_true_group
				+ "' instead of '" + m_fileAgent->m_group + "').";
		sol = "You should change it.";
		act = RECOVERY_FILE_CHGRP;

		com = getCommand(act);

		RecoveryUnit ru(agentName, server, act);
		ru.setProblem(pb);
		ru.setSolution(sol);
		ru.setCommand(com);
		m_recoveryUnits.push_back(ru);

	}

	if (m_fileAgent->m_perm_pb) {
		pb = "The file has the wrong permissions('" + m_fileAgent->m_true_perm
				+ "' instead of '" + m_fileAgent->m_perm + "').";
		sol = "You should change them.";
		act = RECOVERY_FILE_CHMOD;
		com = getCommand(act);
		RecoveryUnit ru(agentName, server, act);
		ru.setProblem(pb);
		ru.setSolution(sol);
		ru.setCommand(com);
		m_recoveryUnits.push_back(ru);
	}

	if (m_fileAgent->m_attr_pb) {
		pb = "The file has the wrong attributes('" + m_fileAgent->m_true_attr
				+ "' instead of '" + m_fileAgent->m_attr + "').";
		sol = "You should change them.";
		act = RECOVERY_FILE_CHATTR;
		com = getCommand(act);
		RecoveryUnit ru(agentName, server, act);
		ru.setProblem(pb);
		ru.setSolution(sol);
		ru.setCommand(com);
		m_recoveryUnits.push_back(ru);

	}

	// There is no point in suggesting an action for the patterns AND the md5
	// Since the pattern information is more precise, we give this one if we have it
	if (m_fileAgent->m_patterns_pb) {
		ss.str(std::string());
		ss
				<< "The file content is not as expected. The following expressions were not found "
				<< endl;
		for (unsigned int i = 0; i < m_fileAgent->m_patterns_pb_detail.size();
				i++) {
			if (m_fileAgent->m_patterns_pb_detail[i]) {
				ss << "\t" << m_fileAgent->m_patterns[i] << endl;
			}
		}
		pb = ss.str();
		sol = "Check the file content.";
		act = RECOVERY_FILE_PATTERN;
		com = getCommand(act);
		RecoveryUnit ru(agentName, server, act);
		ru.setProblem(pb);
		ru.setSolution(sol);
		ru.setCommand(com);
		m_recoveryUnits.push_back(ru);

	} else if (m_fileAgent->m_md5_pb) { //If we have pattern problem, it's obvious we have an checksum problem
		pb = "The file has the wrong checksum ('" + m_fileAgent->m_true_md5
				+ "' instead of '" + m_fileAgent->m_md5 + "').";
		sol = "Check the file content.";
		act = RECOVERY_FILE_MD5;
		com = getCommand(act);
		RecoveryUnit ru(agentName, server, act);
		ru.setProblem(pb);
		ru.setSolution(sol);
		ru.setCommand(com);
		m_recoveryUnits.push_back(ru);

	}

}

string FileRecovery::getCommand(recoveryAction action) const {
	stringstream ss;
	switch (action) {
	case RECOVERY_FILE_CREATE:

		break;
	case RECOVERY_FILE_CHGRP:

		ss << "chgrp " << m_fileAgent->m_group << " " << m_fileAgent->m_filename
				<< endl;
		break;

	case RECOVERY_FILE_CHOWN:

		ss << "chown " << m_fileAgent->m_owner << " " << m_fileAgent->m_filename
				<< endl;
		break;

	case RECOVERY_FILE_CHMOD:

		ss << "chmod " << m_fileAgent->m_perm << " " << m_fileAgent->m_filename
				<< endl;
		break;

	case RECOVERY_FILE_CHATTR:

		ss << "chattr " << m_fileAgent->m_attr << " " << m_fileAgent->m_filename
				<< endl;
		break;

	case RECOVERY_FILE_PATTERN:
		// no command to suggest
		break;

	case RECOVERY_FILE_MD5:
		// no command to suggest
		break;
	case RECOVERY_FILE_MOUNT_MISSING:
		ss << "mount " << m_fileAgent->m_mountpoint.m_dir << std::endl;
		break;
	default:
		ss << "The problem Id is unknown " << action << endl;

		break;
	}
	return ss.str();
}

