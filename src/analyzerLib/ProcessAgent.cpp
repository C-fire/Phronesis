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


#include "ProcessAgent.hpp"

void ProcessAgent::init() {
	m_process_not_running = true;
	m_user_pb = false;
	m_cpu_pb = false;
	m_mem_pb = false;
	m_limits_pb = false;
	m_limStatus_pb = false;
	m_env_pb = false;
	m_responsibleChild = this;

	m_true_cpu = 0;
	m_true_mem = 0;
	m_multiplicity = -1;
	m_env = NULL;
	m_lvl = 0;
	m_pid = 0;
	m_maxCpu = 100;
	m_maxMemory = 100;
}
ProcessAgent::ProcessAgent() {
	init();

}

ProcessAgent::ProcessAgent(int pid) {
	init();
	m_pid = pid;

}

ProcessAgent::ProcessAgent(int id, string name, Server * server,
		string procName, string user, map<string, unsigned long int> limits,
		FileAgent * env, int multiplicity, float maxCpu, float maxMemory) :
		Agent(id, name, server) {
	init();
	m_procName = procName;
	m_command = procName; //Default solution
	m_user = user;
	m_env = env;
	m_limits_map = limits;
	m_lvl = 0;
	m_multiplicity = multiplicity;
	m_maxCpu = maxCpu;
	m_maxMemory = maxMemory;

}

ProcessAgent::ProcessAgent(const ProcessAgent& source) :
		Agent(source) {
	m_procName = source.m_procName;
	m_pid = source.m_pid;
	m_env = source.m_env;
	m_maxCpu = source.m_maxCpu;
	m_maxMemory = source.m_maxMemory;
	m_service = source.m_service;
	m_command = source.m_command;
	m_user = source.m_user;
	m_limits_map = source.m_limits_map;
	m_true_user = source.m_true_user;
	m_true_cpu = source.m_true_cpu;
	m_true_mem = source.m_true_mem;
	m_process_not_running = source.m_process_not_running;
	m_user_pb = source.m_user_pb;
	m_cpu_pb = source.m_cpu_pb;
	m_mem_pb = source.m_mem_pb;
	m_limits_pb = source.m_limits_pb;
	m_limits_pb_detail_map = source.m_limits_pb_detail_map;
	m_limStatus_pb = source.m_limStatus_pb;
	m_limStatus_pb_detail_map = source.m_limStatus_pb_detail_map;
	m_env_pb = source.m_env_pb;
	m_pidList = source.m_pidList;
	m_pidTree = source.m_pidTree;
	m_lvl = source.m_lvl;
	m_sons = source.m_sons;
	m_multiplicity = source.m_multiplicity;

}
ProcessAgent & ProcessAgent::operator=(const ProcessAgent & source) {
	*this = (Agent&) source;
	m_procName = source.m_procName;
	m_pid = source.m_pid;
	m_env = source.m_env;
	m_maxCpu = source.m_maxCpu;
	m_maxMemory = source.m_maxMemory;
	m_service = source.m_service;
	m_command = source.m_command;
	m_user = source.m_user;
	m_limits_map = source.m_limits_map;
	m_true_user = source.m_true_user;
	m_true_cpu = source.m_true_cpu;
	m_true_mem = source.m_true_mem;
	m_process_not_running = source.m_process_not_running;
	m_user_pb = source.m_user_pb;
	m_cpu_pb = source.m_cpu_pb;
	m_mem_pb = source.m_mem_pb;
	m_limits_pb = source.m_limits_pb;
	m_limits_pb_detail_map = source.m_limits_pb_detail_map;
	m_limStatus_pb = source.m_limStatus_pb;
	m_limStatus_pb_detail_map = source.m_limStatus_pb_detail_map;
	m_env_pb = source.m_env_pb;
	m_pidList = source.m_pidList;
	m_pidTree = source.m_pidTree;
	m_lvl = source.m_lvl;
	m_sons = source.m_sons;
	m_multiplicity = source.m_multiplicity;

	return *this;

}
ProcessAgent::~ProcessAgent() {

}

void ProcessAgent::setPid(int pid) {
	m_pid = pid;
}

//Considere only one match
void ProcessAgent::setPidList(vector<int> pidList) {
	m_pidList = pidList;
}
void ProcessAgent::setPidTree(map<int, vector<int> > pidTree) {
	m_pidTree = pidTree;
}

void ProcessAgent::setLvl(int lvl) {
	m_lvl = lvl;
}

void ProcessAgent::addSon(ProcessAgent * son) {
	m_sons.push_back(son);
	son->setLvl(m_lvl + 1);
}

void ProcessAgent::setService(string service) {
	this->m_service = service;
}

void ProcessAgent::setCommand(string command) {
	this->m_command = command;
}

// 07-12-2012 In this version, I replace  head = V(a) + V(c) with head = a + c
vector<int> ProcessAgent::findProcessTreeHead() {

	map<int, vector<int> >::iterator head;

	//if there is only one element, the only possibility is the value
	if (m_pidTree.size() == 1) {
		vector<int> ret;
		ret.push_back(m_pidTree.begin()->first);
		return ret;
	}

	/*else, we have to find one which is a key, but not a value. Here is the method :
	 * Map content :
	 * a->b
	 * c->d
	 * b->e
	 * d->f
	 *
	 * keys = {a,c,b,d}
	 * values = {b,d,e,f}
	 *
	 * keys - values = {a,c}
	 *
	 * head = V(a) + V(c)
	 */

	vector<int> keys;
	vector<int> values;
	vector<int>::iterator it;
	for (head = m_pidTree.begin(); head != m_pidTree.end(); head++) {
		keys.push_back(head->first);
		for (it = (head->second).begin(); it != (head->second).end(); it++) {
			values.push_back(*it);
		}
	}

	vector<int> diff(values.size() + keys.size());
	sort(keys.begin(), keys.end());
	sort(values.begin(), values.end());

	it = set_difference(keys.begin(), keys.end(), values.begin(), values.end(),
			diff.begin()); //those in keys which are not in values

	//it points at the end of the constructed diff
	diff.erase(it, diff.end());

	vector<int> heads;
	vector<int>::iterator v;
	for (it = diff.begin(); it != diff.end(); it++) {
		heads.push_back(*it);
	}

	return heads;
}

/*
 * I finally understand what I wanted to do here...
 * m_sons contains processAgents that are defined in
 * the db as forks (like the first process of httpd ran by root, and all the forks, as Apache)
 * What I am (or should be) doing is to somehow match the pid tree I find, with this structure.
 * Now, if I have 2 children that are different, I have to be careful to assign the correct pid to the correct child
 * otherwise everything will be wrong. This can be done based on the searched process string for example.
 * For this, I would need to get more info from the remote host. Let's keep it in mind, and like this so far.
 */

bool ProcessAgent::analyze() {
	m_iManager->sendStatistic(m_name + "_analyze");

#ifdef SIMU
	MetaAgent::TMP_attempts++;

	return Agent::analyze();
#else
	Agent::analyze();

	string space = "";
	for (int i = 0; i < m_deep; i++) {
		space += "\t";
	}

	bool envPb = m_server->getEnvironmentAgent()->makeQuickEnvironmentCheck();
	if (envPb) {
		m_responsibleChild = m_server->getEnvironmentAgent();
		//m_cause = m_responsibleChild->getCause();
		setCause(m_responsibleChild->getCause());
		return true;
	}
	// From now on it should be safe to query the Agent, it should answer...
	// but still, I should protect all the query

	int ret = false;
	m_process_not_running = false;

	//If we are a root process, we build the PidTree
	if (m_lvl == 0) {
		m_iManager->addLog("ProcessAgent " + m_name + " : starting analysis");

		MetaAgent::TMP_attempts++;
		map<string, bool> require;
		Request tm(Request::PID_TREE, m_procName, require);
		string hostAddr = m_server->getAddress();
		pidTreeInfo pidTree;
		try {
			pidTree = queryRemoteAgent<pidTreeInfo>(hostAddr, tm);
		} catch (std::exception &e) {
			m_remoteAgentPb = true;
			m_responsibleChild = this;
			stringstream ss(stringstream::in | stringstream::out);
			ss << "Problem executing the Request on " << hostAddr << " : "
					<< e.what();
			setCause(ss.str());
			m_iManager->sendStatistic(m_name + "_requestProblem");

			return true;
		}
//		stringstream pidTreeStr(stringstream::in | stringstream::out);
//		pidTreeStr << pidTree;
//		m_iManager->addLog(
//				"ProcessAgent " + m_name + " : pid tree \n" + pidTreeStr.str());

		// If we cannot get a pidTree, it means that the process is not running
		if (pidTree.hasRuntimeError("pidTree")) {
			//if (!m_pidTree.size()) {
			setCause(
					"ProcessAgent " + m_name
							+ " : the process seems not to be running (details : "
							+ pidTree.m_runtimeErrors["pidTree"] + ")");

			m_process_not_running = true;
			m_user_pb = false;
			m_cpu_pb = false;
			m_mem_pb = false;

			m_limits_pb = false;

			m_limStatus_pb = false;
			m_env_pb = false;
			m_responsibleChild = this;
			m_iManager->sendStatistic(m_name + "_requestProblem");

			return true;
		}

		m_pidTree = pidTree.m_pidTree;

		// We are looking for the tree heads
		vector<int> temp_pidList = findProcessTreeHead();

		// Here we can use the multiplicity of the /!\ parent process /!\.
		// Normally we could do a smarter choice, but
		// for now, if we need n heads, we take n random...
		// It is a temporary solution to fight against the 400 nfs processes...

		if (m_multiplicity == -1)		// we take all the processes
			m_pidList = temp_pidList;
		else {
			for (int m = 0; m < m_multiplicity; m++) {
				if (temp_pidList.empty())
					break;
				int rnd = rand() % temp_pidList.size();
				int rndPid = temp_pidList[rnd];
				m_pidList.push_back(rndPid);
				temp_pidList.erase(temp_pidList.begin() + rnd);
			}
		}

	}

	// This section is now executed independently of the depth we are in the tree

	// m_pidList contains all the children at a given level that have the same parent :
	//  - if we are at level 0 (root process), then it is set by the findProcessTreeHead
	//  - Otherwise, the current processAgent fills in the list with its children, and call this method recursively
	//
	// For all the pid in m_pidList, instead of creating a new object, we simply replace
	// the value of the attributes pid, pidList and pidTree in the instances in m_sons.
	// So we analyze ourself in depth (user, cpu, etc) and then analyze every child instance.
	//
	// The instances in m_sons are created when reading the database. In fact,
	// this system is yet incomplete because it supports only one type of child
	// per Process. So in fact, m_sons's size is always 1. The problem with having more
	// is that you would need to know which "type" of fork the pid you are about to analyze is.
	// If for example a process A forks B and C, each of them having different characteristics, like user;
	// How do I know whether the pid I am about to analyze is of type B or C? This will probably need
	// quite a big reshuffeling of the ProcessAgent.
	//
	for (vector<int>::iterator it = m_pidList.begin(); it != m_pidList.end();
			it++) {
		if (ret)
			break;
		m_pid = *it;
		vector<int> pidSonList = m_pidTree[*it];

		bool selfRet = selfAnalyze();
		ret = ret || selfRet;
		if ((!ret) && (m_sons.size())) {
			m_sons[0]->setPidList(pidSonList);
			m_sons[0]->setPidTree(m_pidTree);
			int retSon = m_sons[0]->analyze();
			ret = ret || retSon;
			if (ret) {
				m_cause = m_sons[0]->m_cause;
				m_responsibleChild = m_sons[0];
			}

		}
	}

	return ret;
#endif
}

/* The limit check will have to be more precise (< or <= are not good for all the cases)*/
bool ProcessAgent::selfAnalyze() {
	string space = "";
	for (int i = 0; i < m_deep; i++) {
		space += "\t";
	}

	m_user_pb = false;
	m_cpu_pb = false;
	m_mem_pb = false;

	m_limits_pb = false;

	m_limStatus_pb = false;
	m_env_pb = false;

	std::map<std::string, bool> require;
	require["cmdLine"] = true;
	require["user"] = true;
	require["cpu"] = true;
	require["memory"] = true;
	require["limits"] = true;
	require["limitStatus"] = true;

	Request tm(Request::PROCESS_PID, boost::lexical_cast<std::string>(m_pid),
			require);
	std::string serverAddr = m_server->getAddress();
	procInfo procI;

	// First we get all the information, with a retry
	// We can get an error for the limit if it is kernel issue
	try {
		procI = queryRemoteAgent<procInfo>(serverAddr, tm);
		if (!procI.m_runtimeErrors.empty()) {
			if (!((procI.m_runtimeErrors.size() == 1)
					&& (procI.hasRuntimeError("limitStatus")))) {
				m_iManager->addLog("the request had exceptions, I try again");
				procI = queryRemoteAgent<procInfo>(serverAddr, tm);
			}
		}
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

//	stringstream procIStr(stringstream::in | stringstream::out);
//	procIStr << procI;
//
//	m_iManager->addLog(
//			"ProcessAgent " + m_name + "("
//					+ boost::lexical_cast<std::string>(m_pid)
//					+ ") : returned by the agent \n" + procIStr.str());

	// If we have a problem getting the user of a process, it means
	// that the process is not running anymore, but it was at the time
	// we queried the pidTree. So either the process crashed in between,
	// either it was a temporary process created on demand (httpd, gearmand, nfsd....)
	// The criteria we have decided is that if the process is not a root process
	// and if there can be an infinite number of it (multiplicity = -1), then we
	// just ignore it
	if (procI.hasRuntimeError("user")) {
		if (boost::starts_with(procI.m_runtimeErrors["user"],
				"Could not find a process")) {
			if ((m_lvl > 0) && (m_multiplicity == -1)) {
				//The process was just temporary, we ignore it
				return false;
			} else
				//If it is really the base, we should not be here since the pidTree would already be empty
				return true;

		}
	}

	// Kernel proc are special case, and for the time being,
	// the only way we have to recognoze them is because the user
	// has writen it with a '*' at the beginning.
	// If the command line is different than the one we expect, it normaly means
	// that the regex has matched another process. So we ignore it.
	// This is really not ideal, because then we cannot spot processes that are started
	// with wrong arguments. This should really be changed
	if (!procI.hasRuntimeError("cmdLine")) {
		if (!((!procI.m_cmdLine.size()) && (m_procName[0] == '*'))) {
			if (!boost::contains(procI.m_cmdLine, m_procName)) {
				m_iManager->addLog(
						"The cmd line " + procI.m_cmdLine
								+ " is probably not what I want (" + m_procName
								+ ")");
				return false;
			}
		} else {
			m_iManager->addLog(
					"This is a kernel process, special treatment...");

		}
	}

	// We signal to the user any runtime error
	if (!procI.m_runtimeErrors.empty()) {
		for (std::map<std::string, std::string>::iterator erIt =
				procI.m_runtimeErrors.begin();
				erIt != procI.m_runtimeErrors.end(); erIt++) {
			m_iManager->addLog(
					"Exception on the RemoteAgent : " + erIt->first + " : "
							+ erIt->second);
		}
	}

	// Check that the user is as expected
	if ((m_user.size()) || (!procI.hasRuntimeError("user"))) {

		m_true_user = procI.m_user;
		m_user_pb = m_user.compare(m_true_user);

		if (m_user_pb) {
			setCause(
					"ProcessAgent " + m_name + "("
							+ boost::lexical_cast<std::string>(m_pid)
							+ ") : the user of the process is " + m_true_user
							+ " instead of " + m_user);
			m_responsibleChild = this;
		}
	}

	// Check that the CPU consumption is beyond the threshold
	m_true_cpu = procI.m_cpu;
	m_cpu_pb = (m_true_cpu >= m_maxCpu);
	if (m_cpu_pb) {
		setCause(
				"ProcessAgent " + m_name + "("
						+ boost::lexical_cast<std::string>(m_pid)
						+ ") the process uses too much cpu (max "
						+ boost::lexical_cast<std::string>(m_maxCpu) + "%) : "
						+ boost::lexical_cast<std::string>(m_true_cpu));
		m_responsibleChild = this;
	}

	// Check that the memory consumption is beyond the threshold
	m_true_mem = procI.m_mem;
	m_mem_pb = (m_true_mem >= m_maxMemory);
	if (m_mem_pb) {
		setCause(
				"ProcessAgent " + m_name + "("
						+ boost::lexical_cast<std::string>(m_pid)
						+ ") the process uses too much memory (max "
						+ boost::lexical_cast<std::string>(m_maxMemory)
						+ "%) : "
						+ boost::lexical_cast<std::string>(m_true_mem));
		m_responsibleChild = this;
	}

	std::map<std::string, std::pair<unsigned long int, unsigned long int> > processLimits_map =
			procI.m_limits;

	std::map<std::string, unsigned long int> limitingMap;

	// We check that the limits are not lower than what required by the user
	for (std::map<std::string, std::pair<unsigned long int, unsigned long int> >::const_iterator it =
			processLimits_map.begin(); it != processLimits_map.end(); it++) {
		std::string limitName = it->first;
		std::pair<unsigned long int, unsigned long int> realLimits = it->second;
		unsigned long int limitMin = m_limits_map[limitName];
		unsigned long int limiting;

		limiting = min(realLimits.first, realLimits.second);
		if (limiting < limitMin) {
			setCause(
					"ProcessAgent " + m_name + "("
							+ boost::lexical_cast<std::string>(m_pid)
							+ ")Limit " + limitName
							+ " not fitting : required = "
							+ boost::lexical_cast<std::string>(limitMin)
							+ " soft = "
							+ boost::lexical_cast<std::string>(realLimits.first)
							+ " hard = "
							+ boost::lexical_cast<std::string>(
									realLimits.second));
			m_limits_pb_detail_map[limitName] = true;
			m_limits_pb = true;
			m_responsibleChild = this;
		}

		limitingMap[limitName] = limiting;

	}

	// Check the limit status
	// Some limit status are really strange, and that's why
	// we simply ignore them :  MAX_REALTIME_PRIORITY
	// Some limits MUST be greater, but others can be equal :
	// MAX_NICE_PRIORITY, MAX_CORE_FILE_SIZE
	std::map<std::string, unsigned long int> processLimitStatus =
			procI.m_limitStatus;
	std::vector<std::string> limitToIgnoreBecauseFuckedUp;
	limitToIgnoreBecauseFuckedUp.push_back("MAX_REALTIME_PRIORITY");

	std::vector<std::string> limitThatCanBeEquals;
	limitThatCanBeEquals.push_back("MAX_NICE_PRIORITY");
	limitThatCanBeEquals.push_back("MAX_CORE_FILE_SIZE");

	for (std::map<std::string, unsigned long int>::const_iterator it =
			processLimitStatus.begin(); it != processLimitStatus.end(); it++) {
		std::string limitName = it->first;

		//Skip some fucked up limits
		if (find(limitToIgnoreBecauseFuckedUp.begin(),
				limitToIgnoreBecauseFuckedUp.end(), limitName)
				!= limitToIgnoreBecauseFuckedUp.end())
			continue;

		unsigned long int curStatus = it->second;
		unsigned long int limiting = limitingMap[limitName];

		if (limiting != LIMIT_UNLIMITED) {
			if (curStatus == limiting) {

				//It is okay if some limits are reached
				if (find(limitThatCanBeEquals.begin(),
						limitThatCanBeEquals.end(), limitName)
						!= limitThatCanBeEquals.end())
					continue;

				setCause(
						"ProcessAgent " + m_name + "("
								+ boost::lexical_cast<std::string>(m_pid)
								+ ")Limit " + limitName
								+ " reached  : limiting = "
								+ boost::lexical_cast<std::string>(limiting)
								+ " status = "
								+ boost::lexical_cast<std::string>(curStatus));

				m_limStatus_pb_detail_map[limitName] = true;
				m_limStatus_pb = true;
				m_responsibleChild = this;
			} else if (curStatus > limiting) { //That should not be possible...
				setCause(
						"ProcessAgent " + m_name + "("
								+ boost::lexical_cast<std::string>(m_pid)
								+ ") Da Fuck??? Why is  " + limitName
								+ " ABOVE its limit?? limiting = "
								+ boost::lexical_cast<std::string>(limiting)
								+ " status = "
								+ boost::lexical_cast<std::string>(curStatus));
			}
		}

	}

	// We check the environment variable
	// For this, we need to define the paht of the environment file (/proc/pid/environ)
	// If there is a problem with the environment variables, we don't set the FileAgent responsible,
	// because it would not make any sense since the file is not writen by the user.
	if (m_env) {
		m_env->setPath(
				"/proc/" + boost::lexical_cast<string>(m_pid) + "/environ");
		m_env_pb = m_env->analyze();
		if (m_env_pb) {
			setCause(
					"Problem with the environment of the Process : "
							+ m_env->getCause());
			m_responsibleChild = this;

		}
	}

	bool hasProblem = (m_user_pb || m_cpu_pb || m_mem_pb || m_limits_pb
			|| m_limStatus_pb || m_env_pb);
	if (hasProblem)
		m_iManager->sendStatistic(m_name + "_responsible");

	return hasProblem;
}
