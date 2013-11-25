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


#include "ProcessRecovery.hpp"

ProcessRecovery::ProcessRecovery() :
		AgentRecovery() {
	m_processAgent = NULL;

}

ProcessRecovery::ProcessRecovery(ProcessAgent * agent) :
		AgentRecovery(agent) {
	m_processAgent = agent;
	analyzeAgent();
}

ProcessRecovery::ProcessRecovery(ProcessAgent * agent, recoveryAction action) :
		AgentRecovery(agent, action) {
	m_processAgent = agent;
}

ProcessRecovery::ProcessRecovery(const ProcessRecovery & source) :
		AgentRecovery(source) {
	m_processAgent = source.m_processAgent;
}

ProcessRecovery & ProcessRecovery::operator=(const ProcessRecovery & source) {
	*this = (AgentRecovery &) source;
	m_processAgent = source.m_processAgent;

	return *this;
}

ProcessRecovery::~ProcessRecovery() {
}


// This method is very similar to all the other RecoveryAgent,
// but there are 2 specificities :
// - first, when we have problem with the environment variable,
//   even if it is managed by a FileAgent, we don't use a FileRecovery
//   because its recovery solution would be meaningless
// - we have to make the difference in the recovery suggestions depending
//   on whether the user provided us with a command or a service.
void ProcessRecovery::analyzeAgent() {
	AgentRecovery::analyzeAgent();

	string agentName = m_processAgent->getName();
	string server = m_processAgent->getServer()->getAddress();

	stringstream ss;
	string pb, sol, adv, com;
	recoveryAction act;
	vector<recoveryAction> acts;
	if (m_processAgent->m_service.empty()) { //No service is specified, so we work on the process directly
		if (m_processAgent->m_process_not_running) {
			ss.str(std::string());
			ss << "the process" << m_processAgent->m_procName
					<< "is not running.";
			pb = ss.str();
			sol = "restart the process.";
			act = RECOVERY_PROCESS_START;
			com = getCommand(act);
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			m_recoveryUnits.push_back(ru);
			return;

		}
		if (m_processAgent->m_cpu_pb) {
			ss.str(std::string());
			ss << "the cpu consumption is too high ("
					<< m_processAgent->m_true_cpu << ").";
			pb = ss.str();
			sol = "Restart the process.";
			act = RECOVERY_PROCESS_RESTART;
			com = getCommand(act);
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			m_recoveryUnits.push_back(ru);
		}

		if (m_processAgent->m_mem_pb) {
			ss.str(std::string());
			ss << "the memory consumption is too high ("
					<< m_processAgent->m_true_mem << ").";
			pb = ss.str();
			sol = "Restart the process.";
			act = RECOVERY_PROCESS_RESTART;
			com = getCommand(act);

			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);

			m_recoveryUnits.push_back(ru);
		}

		if (m_processAgent->m_env_pb) {
			ss.str(std::string());
			ss
					<< "the environment of the process is not correct. See details : "
					<< endl;
			for (unsigned int i = 0;
					i < m_processAgent->m_env->m_patterns_pb_detail.size();
					i++) {
				if (m_processAgent->m_env->m_patterns_pb_detail[i]) {
					ss << "\t" << m_processAgent->m_env->m_patterns[i] << endl;
				}
			}
			pb = ss.str();

			sol = "Restart the process";
			ss.str(std::string());
			ss
					<< "Solution 1 : restart the process with the environment variable in front."
					<< endl;
			ss
					<< "Solution 2 : check the configuration files that are sourced at the beginning.";
			adv = ss.str();
			act = RECOVERY_PROCESS_ENVIRONMENT;
			com = getCommand(act);

			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			ru.setAdvancedAdvice(adv);
			m_recoveryUnits.push_back(ru);
		}

		if (m_processAgent->m_user_pb) {
			pb = "the process is not running with the proper user ('"
					+ m_processAgent->m_true_user + "' instead of '"
					+ m_processAgent->m_user + "').";
			sol = "restart the process with the correct user.";
			adv =
					"A incorrect owner or wrongly set SUID bit on the executable can be the origin of the problem.";
			//		acts.clear();
			//		acts.push_back(RECOVERY_PROCESS_STOP);
			//		acts.push_back(RECOVERY_PROCESS_START);
			act = RECOVERY_PROCESS_CHUSER;
			com = getCommand(act);

			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			ru.setAdvancedAdvice(adv);
			m_recoveryUnits.push_back(ru);
		}

	} else {

		if (m_processAgent->m_process_not_running) {

			pb = "the process" + m_processAgent->m_procName + "is not running.";
			sol = "restart the service.";
			act = RECOVERY_SERVICE_RESTART;
			com = getCommand(act);

			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);

			m_recoveryUnits.push_back(ru);
		}
		if (m_processAgent->m_cpu_pb) {
			ss.str(std::string());
			ss << "the cpu consumption is too high ("
					<< m_processAgent->m_true_cpu << ").";
			pb = ss.str();
			sol = "restart the service.";
			act = RECOVERY_SERVICE_RESTART;
			com = getCommand(act);

			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);

			m_recoveryUnits.push_back(ru);
		}

		if (m_processAgent->m_mem_pb) {
			ss.str(std::string());
			ss << "the memory consumption is too high ("
					<< m_processAgent->m_true_mem << ").";
			pb = ss.str();
			sol = "restart the service.";
			act = RECOVERY_SERVICE_RESTART;
			com = getCommand(act);

			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);

			m_recoveryUnits.push_back(ru);
		}

		if (m_processAgent->m_env_pb) {
			stringstream ss;
			ss
					<< "the environment of the process is not correct. See details : "
					<< endl;
			for (unsigned int i = 0;
					i < m_processAgent->m_env->m_patterns_pb_detail.size();
					i++) {
				if (m_processAgent->m_env->m_patterns_pb_detail[i]) {
					ss << "\t" << m_processAgent->m_env->m_patterns[i] << endl;
				}
			}
			pb = ss.str();

			sol = "Restart the process";
			ss.str(std::string());
			ss
					<< "Solution 1 : restart the service with the environment variable in front."
					<< endl;
			ss
					<< "Solution 2 : check the configuration files that are sourced at the beginning.";
			adv = ss.str();
			act = RECOVERY_SERVICE_ENVIRONMENT;
			com = getCommand(act);

			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setAdvancedAdvice(adv);
			ru.setCommand(com);

			m_recoveryUnits.push_back(ru);
		}

		if (m_processAgent->m_user_pb) {
			pb = "the process is not running with the proper user ('"
					+ m_processAgent->m_true_user + "' instead of '"
					+ m_processAgent->m_user + "').";
			sol = "restart the service.";
			adv =
					"A incorrect owner or wrongly set SUID bit on the executable can be the origin of the problem.";
			act = RECOVERY_SERVICE_CHUSER;
			com = getCommand(act);

			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setAdvancedAdvice(adv);
			ru.setCommand(com);

			m_recoveryUnits.push_back(ru);
		}

	}

	if (m_processAgent->m_limits_pb) {
		pb = "one of the limit is not set properly.";
		sol = "kneel down and pray. Negotiations with God in progress.";
		adv = "So far no automatic recovery offered.\n";
		adv +=
				"\tThe problem usually comes from a ulimit command call somewhere in a script/code.\n";
		adv += "\tThe problem can also be system wide.";
		act = RECOVERY_PROCESS_ULIMIT;
		com = getCommand(act);

		RecoveryUnit ru(agentName, server, act);
		ru.setProblem(pb);
		ru.setSolution(sol);
		ru.setAdvancedAdvice(adv);
		ru.setCommand(com);

		m_recoveryUnits.push_back(ru);
	}

	if (m_processAgent->m_limStatus_pb) {

		pb = "one of the limit has reached its maximum.";
		sol =
				"kneel down and pray. Negotiations with God in progress. (Try restarting first...)";
		adv = "So far no automatic recovery offered.\n";
		act = RECOVERY_PROCESS_ULIMIT;
		com = getCommand(act);

		RecoveryUnit ru(agentName, server, act);
		ru.setProblem(pb);
		ru.setSolution(sol);
		ru.setAdvancedAdvice(adv);
		ru.setCommand(com);

		m_recoveryUnits.push_back(ru);
	}
}

string ProcessRecovery::getCommand(recoveryAction action) const {

	stringstream ss;

	switch (action) {

	case RECOVERY_PROCESS_START:
		ss << m_processAgent->m_command << endl;

		break;

	case RECOVERY_PROCESS_STOP:
		ss << "kill -9 " << m_processAgent->m_pid << endl;
		break;

	case RECOVERY_PROCESS_RESTART:
		ss << "kill -9 " << m_processAgent->m_pid << "; "
				<< m_processAgent->m_command << endl;

		break;

	case RECOVERY_PROCESS_ULIMIT:
		//Complicated... might miss some info (limit name for example)... for later
		ss << "Ulimit to be seen later" << endl;

		break;

	case RECOVERY_PROCESS_CHUSER: {
		ss << "kill -9 " << m_processAgent->m_pid << "; sudo -u "
				<< m_processAgent->m_user << " " << m_processAgent->m_command
				<< endl;

		break;
	}

	case RECOVERY_PROCESS_ENVIRONMENT:
		ss << "unknown" << endl;
		break;

	case RECOVERY_SERVICE_START:
		ss << "service " << m_processAgent->m_service << " start" << endl;
		break;

	case RECOVERY_SERVICE_STOP:
		ss << "service " << m_processAgent->m_service << " stop" << endl;
		break;

	case RECOVERY_SERVICE_RESTART:
		ss << "service " << m_processAgent->m_service << " restart" << endl;
		break;

	case RECOVERY_SERVICE_RELOAD:
		ss << "service " << m_processAgent->m_service << " reload" << endl;
		break;
	case RECOVERY_SERVICE_CHUSER:

		break;
	case RECOVERY_SERVICE_ENVIRONMENT:
		ss << "unknown." << endl;
		break;

	default:
		ss << "The problem Id is unknown " << action << endl;
		break;
	}

	return ss.str();

}

