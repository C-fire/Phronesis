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


#include "EnvironmentAgent.hpp"

void EnvironmentAgent::init() {
	m_fstab = NULL;
	m_maxLoad = 2;
	m_maxMemory = 100;
	m_maxSwap = 99;
	m_diskSpace_pb = false;
	m_diskSpace_pb_detail.clear();
	m_mem_pb = false;
	m_inode_pb = false;
	m_inode_pb_detail.clear();
	m_load_pb = false;
	m_swap_pb = false;
	m_fstab_pb = false;
	m_cpuConsumers.clear();
	m_agentRunning = false;
	m_pingTested = false;
	m_agentTested = false;
	m_environmentTested = false;

}

EnvironmentAgent::EnvironmentAgent() {
	init();
}
EnvironmentAgent::EnvironmentAgent(int id, string name, Server * server,
		FileAgent * fstab, float maxLoad, float maxMemory, float maxSwap) :
		Agent(id, name, server) {
	init();
	m_fstab = fstab;
	m_maxLoad = maxLoad;
	m_maxMemory = maxMemory;
	m_maxSwap = maxSwap;

}

EnvironmentAgent::EnvironmentAgent(const EnvironmentAgent & source) :
		Agent(source) {
	m_fstab = source.m_fstab;

	m_maxLoad = source.m_maxLoad;
	m_maxMemory = source.m_maxMemory;
	m_maxSwap = source.m_maxSwap;
	m_true_diskSpace = source.m_true_diskSpace;
	m_true_inodes = source.m_true_inodes;
	m_true_load = source.m_true_load;
	m_true_mem = source.m_true_mem;
	m_true_swap = source.m_true_swap;
	m_cpuConsumers = source.m_cpuConsumers;
	m_memConsumers = source.m_memConsumers;
	m_diskSpace_pb = source.m_diskSpace_pb;
	m_diskSpace_pb_detail = source.m_diskSpace_pb_detail;
	m_mem_pb = source.m_mem_pb;
	m_inode_pb = source.m_inode_pb;
	m_inode_pb_detail = source.m_inode_pb_detail;
	m_load_pb = source.m_load_pb;
	m_swap_pb = source.m_swap_pb;
	m_fstab_pb = source.m_fstab_pb;
	m_fileSystemToIgnore = source.m_fileSystemToIgnore;
	m_pingable = source.m_pingable;
	m_pingTested = source.m_pingTested;
	m_agentRunning = source.m_agentRunning;
	m_agentTested = source.m_agentTested;
	m_environment_pb = source.m_environment_pb;
	m_environmentTested = source.m_environmentTested;

}
EnvironmentAgent & EnvironmentAgent::operator=(
		const EnvironmentAgent & source) {
	*this = (Agent&) source;
	m_fstab = source.m_fstab;

	m_maxLoad = source.m_maxLoad;
	m_maxMemory = source.m_maxMemory;
	m_maxSwap = source.m_maxSwap;
	m_true_diskSpace = source.m_true_diskSpace;
	m_true_inodes = source.m_true_inodes;
	m_true_load = source.m_true_load;
	m_true_mem = source.m_true_mem;
	m_true_swap = source.m_true_swap;
	m_cpuConsumers = source.m_cpuConsumers;
	m_memConsumers = source.m_memConsumers;
	m_diskSpace_pb = source.m_diskSpace_pb;
	m_diskSpace_pb_detail = source.m_diskSpace_pb_detail;
	m_mem_pb = source.m_mem_pb;
	m_inode_pb = source.m_inode_pb;
	m_inode_pb_detail = source.m_inode_pb_detail;
	m_load_pb = source.m_load_pb;
	m_swap_pb = source.m_swap_pb;
	m_fstab_pb = source.m_fstab_pb;
	m_fileSystemToIgnore = source.m_fileSystemToIgnore;
	m_pingable = source.m_pingable;
	m_pingTested = source.m_pingTested;
	m_agentRunning = source.m_agentRunning;
	m_agentTested = source.m_agentTested;
	m_environment_pb = source.m_environment_pb;
	m_environmentTested = source.m_environmentTested;

	return *this;

}
EnvironmentAgent::~EnvironmentAgent() {

}

bool EnvironmentAgent::analyze() {
	MetaAgent::TMP_attempts++;
	m_iManager->sendStatistic(m_name + "_analyze");

#ifdef SIMU
	m_environment_pb = Agent::analyze();
	m_environmentTested = true;
	return m_environment_pb;
#else
	Agent::analyze();
	m_environmentTested = true;
	m_diskSpace_pb = false;
	m_diskSpace_pb_detail.clear();
	m_mem_pb = false;
	m_inode_pb = false;
	m_inode_pb_detail.clear();
	m_load_pb = false;
	m_swap_pb = false;
	m_fstab_pb = false;
	m_cpuConsumers.clear();

	m_iManager->addLog("EnvironmentAgent " + m_name + " : starting analysis");

	string serverAddr = m_server->getAddress();

	// Here I list all the information I want. I could replace it with a fetchAll...
	std::map<std::string, bool> require;
	require["cpu"] = true;
	require["memory"] = true;
	require["swap"] = true;
	require["diskSpace"] = true;
	require["inodes"] = true;
	require["cpuConsumers"] = true;
	require["memConsumers"] = true;
	Request tm(Request::ENVIRONMENT, "", require);
	envInfo env;

	// Make the query on the remote agent
	// In case of problem, we set the remoteAgentPb flag, and set ourselves as responsible
	try {
		env = queryRemoteAgent<envInfo>(serverAddr, tm);
	} catch (std::exception &e) {
		m_remoteAgentPb = true;
		m_responsibleChild = this;
		stringstream ss(stringstream::in | stringstream::out);
		ss << "Problem executing the Request on " << serverAddr << " : "
				<< e.what();
		setCause(ss.str());
		m_iManager->sendStatistic(m_name + "_requestProblem");

		return true;
	}
//
//	stringstream envStr(stringstream::in | stringstream::out);
//	envStr << env;

	map<string, float>::iterator it;

	// For each mountpoint, we check the disk space used and make sure it is bellow 99 %
	// Disks that are declared in m_fileSystemToIgnore are ignored
	if (!env.hasRuntimeError("diskSpace")) {
		m_true_diskSpace = env.m_diskSpace;
		for (it = m_true_diskSpace.begin(); it != m_true_diskSpace.end();
				it++) {
			if (find(m_fileSystemToIgnore.begin(), m_fileSystemToIgnore.end(),
					it->first) == m_fileSystemToIgnore.end()) {
				if (it->second > 99.0) {

					setCause(
							"EnvironmentAgent " + m_name + " : disk "
									+ it->first + " is full at "
									+ boost::lexical_cast<std::string>(
											it->second)
									+ "%, which is more than the limit (99)");
					m_diskSpace_pb = true;
					m_diskSpace_pb_detail[it->first] = true;
					m_responsibleChild = this;
				} else {
					m_diskSpace_pb_detail[it->first] = false;
				}
			} else {
				m_diskSpace_pb_detail[it->first] = false;
			}
		}
	} else {
		//We shall maybe have an "unknown state"
	}

	// For each mountpoint, we check the inodes used and make sure it is bellow 95 %
	// Disks that are declared in m_fileSystemToIgnore are ignored
	m_true_inodes = env.m_inodes;

	for (it = m_true_inodes.begin(); it != m_true_inodes.end(); it++) {
		if (find(m_fileSystemToIgnore.begin(), m_fileSystemToIgnore.end(),
				it->first) == m_fileSystemToIgnore.end()) {
			if (it->second > 95.0) {
				setCause(
						"EnvironmentAgent " + m_name + " : disk " + it->first
								+ " inode usage is "
								+ boost::lexical_cast<std::string>(it->second)
								+ "%, which is more than the limit (95)");
				m_inode_pb = true;
				m_inode_pb_detail[it->first] = true;
				m_responsibleChild = this;
			} else {
				m_inode_pb_detail[it->first] = false;
			}
		} else {
			m_diskSpace_pb_detail[it->first] = false;
		}
	}

	// Check that the load of the server is bellow the critical threshold.
	// If it is not the case, we keep the list of the biggest cpu consumers
	m_true_load = env.m_cpu;
	if (m_true_load > m_maxLoad) {
		setCause(
				"EnvironmentAgent " + m_name + " : cpu load is "
						+ boost::lexical_cast<std::string>(m_true_load)
						+ " which is above the limit ("
						+ boost::lexical_cast<std::string>(m_maxLoad) + ")");
		m_load_pb = true;
		m_responsibleChild = this;
		m_cpuConsumers = env.m_cpuConsumers;
	}

	// Check that the memory consumption of the server is bellow the critical threshold
	// If it is not the case, we keep the list of the biggest memory consumers
	m_true_mem = env.m_mem;
	if (m_true_mem > m_maxMemory) {
		setCause(
				"EnvironmentAgent " + m_name + " : memory load is "
						+ boost::lexical_cast<std::string>(m_true_mem)
						+ " which is above the limit (+ "
						+ boost::lexical_cast<std::string>(m_maxMemory) + "%)");
		m_responsibleChild = this;
		m_memConsumers = env.m_memConsumers;
		m_mem_pb = true;
	}

	// Check that the swap consumption of the server is bellow the critical threshold
	// If it is not the case, we keep the list of the biggest memory consumers
	m_true_swap = env.m_swap;
	if (m_true_swap > m_maxSwap) {
		setCause(
				"EnvironmentAgent " + m_name + " : swap load is "
						+ boost::lexical_cast<std::string>(m_true_swap)
						+ " which is above the limit ( + "
						+ boost::lexical_cast<std::string>(m_maxSwap) + "%)");
		m_responsibleChild = this;
		m_memConsumers = env.m_memConsumers;
		m_swap_pb = true;
	}

	// If we have an fstab agent defined, we analyze it
	if (m_fstab) {
		m_fstab_pb = m_fstab->analyze();
		if (m_fstab_pb) {
			// we don't delegate the probleme here...
			setCause("Problem with the fstab : " + m_fstab->getCause());
			m_responsibleChild = this;
		}
	}
	m_environment_pb = (m_diskSpace_pb || m_mem_pb || m_inode_pb || m_load_pb
			|| m_swap_pb || m_fstab_pb);

//	bool hasProblem = (m_diskSpace_pb || m_mem_pb || m_inode_pb || m_load_pb
//			|| m_swap_pb || m_fstab_pb);
	if (m_environment_pb)
		m_iManager->sendStatistic(m_name + "_responsible");

	return m_environment_pb;
#endif
}

bool EnvironmentAgent::isServerPingable() {
	// if not yet tested, try to do a ping on the server
	if (!m_pingTested) {
		string cmd = "ping -c 1 -W 2 " + m_server->getAddress() + " >/dev/null";
		int ret = system(cmd.c_str());
		m_pingable = !ret;
		m_pingTested = true;

		// 2013-02-26
		// try this to avoid the NULL problem in the aggregator m_respChild->getResponsibleChild()
		if (!m_pingable) {
			m_responsibleChild = this;
			setCause("Server not pingable");
		}
	}

	return m_pingable;
}

bool EnvironmentAgent::isAgentRunning() {
	std::map<std::string, bool> require;
	Request req(Request::ALIVE, "knock knock", require);

	// If we did not test the remote agent during this diagnosis phase
	if (!m_agentTested) {

		// We send an alive message. If no answer is received,
		// we consider the remote agent to have a problem

		try {
			AliveMsg alive = queryRemoteAgent<AliveMsg>(m_server->getAddress(),
					req);
			m_agentRunning = true;
		} catch (std::exception & e) {
			m_agentRunning = false;
			m_responsibleChild = this;
			//Here as well, we need to copy the exception message to keep it
			char * c_str = (char *) malloc(
					sizeof(char) * (strlen(e.what()) + 1));
			strcpy(c_str, e.what());
			setCause(std::string(c_str));
			free(c_str);
		}
		m_agentTested = true;

	}
	return m_agentRunning;
}

void EnvironmentAgent::reinitializeStatus() {
	MetaAgent::reinitializeStatus();
	m_pingTested = false;
	m_agentTested = false;
	m_environmentTested = false;
}

bool EnvironmentAgent::isEnvironmentResponsible() {
	return true;
}

bool EnvironmentAgent::wasEnviromentTested() {
	return m_environmentTested;
}

bool EnvironmentAgent::isEnvironmentSain() {
	return !m_environment_pb;
}

bool EnvironmentAgent::makeQuickEnvironmentCheck() {
	if (!isServerPingable())
		return true;

	if (!isAgentRunning())
		return true;

	return false;
}

void EnvironmentAgent::addFSToIgnore(std::string fsName) {
	m_fileSystemToIgnore.push_back(fsName);
}
