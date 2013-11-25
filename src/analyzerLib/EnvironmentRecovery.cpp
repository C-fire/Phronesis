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


#include "EnvironmentRecovery.hpp"

EnvironmentRecovery::EnvironmentRecovery() :
		AgentRecovery() {
	m_envAgent = NULL;
}

EnvironmentRecovery::EnvironmentRecovery(EnvironmentAgent * agent) :
		AgentRecovery(agent) {
	m_envAgent = agent;
	analyzeAgent();
}
EnvironmentRecovery::EnvironmentRecovery(EnvironmentAgent * agent,
		recoveryAction action) :
		AgentRecovery(agent, action) {
	m_envAgent = agent;
}

EnvironmentRecovery::EnvironmentRecovery(const EnvironmentRecovery & source) {
	m_envAgent = source.m_envAgent;
	m_fstabRecovery = source.m_fstabRecovery;
	m_pidsToBeKilled = source.m_pidsToBeKilled;
	m_processAgents = source.m_processAgents;
	m_processRecoveries = source.m_processRecoveries;
}

EnvironmentRecovery & EnvironmentRecovery::operator=(
		const EnvironmentRecovery & source) {
	*this = (AgentRecovery&) source;
	m_envAgent = source.m_envAgent;
	m_fstabRecovery = source.m_fstabRecovery;
	m_pidsToBeKilled = source.m_pidsToBeKilled;
	m_processAgents = source.m_processAgents;
	m_processRecoveries = source.m_processRecoveries;

	return *this;
}

EnvironmentRecovery::~EnvironmentRecovery() {
}

void EnvironmentRecovery::analyzeAgent() {
	stringstream ss;
	string pb, sol, adv, com;
	recoveryAction act;
	vector<recoveryAction> acts;

	string agentName = m_envAgent->getName();
	string server = m_envAgent->getServer()->getAddress();

	// 2013-02-26
	// if (m_envAgent)  is not a good test, I don't know why it's here...
	// probably old remains
	// replace it with if (m_envAgent->m_pingable)
	if (m_envAgent->m_pingable) { //The machine is in principle reachable

		//2013-06-25
		// Add a test on the state of the remote agent

		// If the Remote Agent is not running, we should restart it, eventhough there is the
		// possibility that the server is stuck/overloaded
		if (!m_envAgent->m_agentRunning) {

			pb = "The RemoteAgent on server \""
					+ m_agent->getServer()->getName() + "\" (address : "
					+ server + ") cannot be contacted.";
			sol = "Restart the Agent";
			act = RECOVERY_ENV_AGENT;
			adv =
					"It is also also possible that the server is so overloaded that the Agent cannot respond.";
			com = getCommand(act);
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			ru.setAdvancedAdvice(adv);
			m_recoveryUnits.push_back(ru);

		}

		// If there are full disk, we should do some cleaning
		if (m_envAgent->m_diskSpace_pb) {
			ss.str(std::string());
			ss << "Some of the disks are full at a critical level : " << endl;
			map<string, bool>::iterator diskPbIt;
			for (diskPbIt = m_envAgent->m_diskSpace_pb_detail.begin();
					diskPbIt != m_envAgent->m_diskSpace_pb_detail.end();
					diskPbIt++) {

				if (diskPbIt->second) {
					ss << "\t" << diskPbIt->first << " : "
							<< m_envAgent->m_true_diskSpace[diskPbIt->first]
							<< endl;
				}
			}
			pb = ss.str();
			sol = "Make some cleaning.";
			act = RECOVERY_ENV_DISK;
			com = getCommand(act);
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			m_recoveryUnits.push_back(ru);
		}

		// If we have inodes problems, we should do some cleaning
		if (m_envAgent->m_inode_pb) {
			ss.str(std::string());
			ss << "Some of the disks are running out of inodes : " << endl;
			map<string, bool>::iterator diskPbIt;
			for (diskPbIt = m_envAgent->m_inode_pb_detail.begin();
					diskPbIt != m_envAgent->m_inode_pb_detail.end();
					diskPbIt++) {

				if (diskPbIt->second) {
					ss << "\t" << diskPbIt->first << " : "
							<< m_envAgent->m_true_inodes[diskPbIt->first]
							<< endl;
				}
			}
			pb = ss.str();
			sol = "Make some cleaning.";
			act = RECOVERY_ENV_INODE;
			com = getCommand(act);
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			m_recoveryUnits.push_back(ru);
		}

		// If there is a load problem, we give the list of top cpu consumers
		// We also explain that a high load might come from having a big number
		// of programs in a io wait state.
		if (m_envAgent->m_load_pb) {

			// 2013-07-02
			// Since the environment is now tested in the last position
			// if the process consuming too much was under our control, we would have seen it
			// However, it is still possible that it shows up in the "top consumer list",
			// but eh, that's why we have a warning message
			// ----------------------------------------------------------------------------
			//Complicated case...
			//We should eventually kill the process taking so much cpu
			// EXCEPT if this process is under our control ...
			// hum..... So far let's just kill it. The aggregator will see it next round and solve it

			ss.str(std::string());
			ss
					<< "the machine is CPU overloaded. The details below list the top CPU consumers : "
					<< endl;
			vector<triplet<string, float, string> >::iterator consumer;

			// For each process in the top consumer list, we create a temporary
			// ProcessAgent for which we then create a ProcessRecovery with the instruction RECOVERY_PROCESS_STOP
			// ProcessAgents are stored in m_processAgents, and ProcessRecovery in m_processRecovery
			// The ProcessRecovery objects are then used when getting the command to solve the problem
			for (consumer = m_envAgent->m_cpuConsumers.begin();
					consumer != m_envAgent->m_cpuConsumers.end(); consumer++) {
				ss << "\t" << consumer->getThird() << " (PID "
						<< consumer->getFirst() << ") : "
						<< consumer->getSecond() << "%" << endl;

				if (m_pidsToBeKilled.find(consumer->getFirst())
						== m_pidsToBeKilled.end()) { //the pid is not already on the list to be killed, so we can add it without duplicate
					m_pidsToBeKilled.insert(consumer->getFirst());
					//push_back actually makes a copy of the agent
					m_processAgents.push_back(
							ProcessAgent(
									boost::lexical_cast<int>(
											consumer->getFirst())));
					m_processAgents.back().setServer(m_envAgent->getServer());
					m_processRecoveries.push_back(
							ProcessRecovery(&m_processAgents.back(),
									RECOVERY_PROCESS_STOP));
				}

			}
			pb = ss.str();
			sol = "you can kill the top CPU consumers.";
			ss.str(std::string());
			ss << "be aware that this is a snapshot of the CPU consumption."
					<< endl;
			ss
					<< "It is possible that some of the processes just use a lot of CPU for a very short time"
					<< endl;
			ss
					<< "so be careful in your killing choices. (This advice is always true, not only for processes)"
					<< endl;
			ss
					<< "Also, note that a high load might indicate that there are a lot of programs in an io wait state. "
					<< "So high load is not necessarily a real problem. "
					<< "It should really worry you only if the high load is coupled with all the CPU being used all the time.";
			adv = ss.str();
			act = RECOVERY_ENV_OVERLOAD;
			com = getCommand(act);
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			ru.setAdvancedAdvice(adv);
			m_recoveryUnits.push_back(ru);
		}

		// Same principle here as for the cpu overload, but we use the memory consumers instead of the cpu
		if (m_envAgent->m_mem_pb || m_envAgent->m_swap_pb) {

			ss.str(std::string());
			if (m_envAgent->m_mem_pb) {
				ss
						<< "the memory of the machine is overloaded. The details below list the top memory consumers : "
						<< endl;
			} else if (m_envAgent->m_swap_pb) {
				ss
						<< "the swap of the machine is overloaded. The details below list the top memory consumers : "
						<< endl;
			}
			vector<triplet<string, float, string> >::iterator consumer;
			for (consumer = m_envAgent->m_memConsumers.begin();
					consumer != m_envAgent->m_memConsumers.end(); consumer++) {
				ss << "\t" << consumer->getThird() << " (PID "
						<< consumer->getFirst() << ") : "
						<< consumer->getSecond() << "%" << endl;

				if (m_pidsToBeKilled.find(consumer->getFirst())
						== m_pidsToBeKilled.end()) { //the pid is not already on the list to be killed, so we can add it without duplicate
					m_pidsToBeKilled.insert(consumer->getFirst());
					//push_back actually makes a copy of the agent
					m_processAgents.push_back(
							ProcessAgent(
									boost::lexical_cast<int>(
											consumer->getFirst())));
					m_processAgents.back().setServer(m_envAgent->getServer());

					m_processRecoveries.push_back(
							ProcessRecovery(&m_processAgents.back(),
									RECOVERY_PROCESS_STOP));
				}

			}
			pb = ss.str();
			sol = "you can kill the top memory consumers.";
			ss.str(std::string());
			ss << "be aware that this is a snapshot of the memory consumption."
					<< endl;
			ss
					<< "It is possible that some of the processes just use a lot of memory for a very short time"
					<< endl;
			ss
					<< "so be careful in your killing choices. (This advice is always true, not only for processes)";
			adv = ss.str();
			act = RECOVERY_ENV_OVERLOAD;
			com = getCommand(act);
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			ru.setAdvancedAdvice(adv);
			m_recoveryUnits.push_back(ru);
		}

		// If there was a problem with the fstab, then we create
		// a FileRecovery for the FileAgent and apply its recommendation.
		if (m_envAgent->m_fstab_pb) {
			m_fstabRecovery = FileRecovery(m_envAgent->m_fstab);
			pb = "the fstab is not correct.";
			sol = "See files details.";
			act = RECOVERY_ENV_FSTAB;
			com = getCommand(act);
			RecoveryUnit ru(agentName, server, act);
			ru.setProblem(pb);
			ru.setSolution(sol);
			ru.setCommand(com);
			m_recoveryUnits.push_back(ru);
			vector<RecoveryUnit> fstabRu = m_fstabRecovery.getRecoveryUnits();
			for (vector<RecoveryUnit>::iterator fr = fstabRu.begin();
					fr != fstabRu.end(); fr++) {
				m_recoveryUnits.push_back(*fr);
			}
		}

	} else {
		// If we have a server problem, we suggest the ipmi solution, or the manual solution

		pb = "The server \"" + m_agent->getServer()->getName()
				+ "\" (address : " + server + ") seems to be down";
		sol = "Turn it on again.";
		act = RECOVERY_ENV_ON;
		com = getCommand(act);
		RecoveryUnit ru(agentName, server, act);
		ru.setProblem(pb);
		ru.setSolution(sol);
		ru.setCommand(com);
		m_recoveryUnits.push_back(ru);
	}
}

string EnvironmentRecovery::getCommand(recoveryAction action) const {
	stringstream ss;

	switch (action) {

	case RECOVERY_ENV_MOUNT:
		//This case is even not tested yet

		break;
	case RECOVERY_ENV_UMOUNT:
		//This case is even not tested yet
		break;

	case RECOVERY_ENV_DISK:
		ss << "rm -rf " << endl;
		break;

	case RECOVERY_ENV_INODE:
		ss << "rm -rf " << endl;
		break;

	case RECOVERY_ENV_CPU:
		//NOT USED, see RECOVERY_ENV_OVERLOAD
		break;

	case RECOVERY_ENV_MEM:
		//not used, see RECOVERY_ENV_OVERLOAD
		break;

	case RECOVERY_ENV_SWAP:
		// not used : see RECOVERY_ENV_OVERLOAD
		break;

	case RECOVERY_ENV_OVERLOAD: {
		vector<ProcessRecovery>::const_iterator pr;
		for (pr = m_processRecoveries.begin(); pr != m_processRecoveries.end();
				pr++) {
			vector<RecoveryUnit> prru = pr->getRecoveryUnits();
			vector<RecoveryUnit>::iterator ru;
			for (ru = prru.begin(); ru != prru.end(); ru++) {
				//ss << ru->getSolution();
				ss << ru->getCommand();
			}
		}

		break;
	}
	case RECOVERY_ENV_FSTAB:

		break;

	case RECOVERY_ENV_ON:
		ss
				<< "ipmitool <ipmiUser> -I lan -H <ipmiAddress> -P <ipmiPass> chassis power on"
				<< endl
				<< "(Alternatively, you could also go to press the button...)"
				<< endl;
		break;
	case RECOVERY_ENV_AGENT:
		ss << "service phronesisAgentd restart" << std::endl;
		break;
	default:
		ss << "The problem Id is unknown " << action << endl;
		break;
	}

	return ss.str();

}


