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


#include "FileAgent.hpp"

void FileAgent::init() {
	m_no_such_file = false;
	m_notMounted = false;
	m_isStale = false;
	m_owner_pb = false;
	m_group_pb = false;
	m_perm_pb = false;
	m_attr_pb = false;
	m_md5_pb = false;
	m_patterns_pb = false;

}
FileAgent::FileAgent(int id, string name, Server * server, string filename,
		string owner, string group, string perm, string attr, string md5,
		vector<string> patterns) :
		Agent(id, name, server) {
	init();
	m_filename = filename;
	m_owner = owner;
	m_group = group;
	m_perm = perm;
	m_attr = attr;
	m_md5 = md5;
	m_patterns = patterns;

	m_owner_pb = false;
	m_group_pb = false;
	m_perm_pb = false;
	m_attr_pb = false;
	m_md5_pb = false;
	m_patterns_pb = false;
	m_no_such_file = false;
	m_notMounted = false;
	m_isStale = false;

}

FileAgent::FileAgent() :
		Agent() {
	init();
}

FileAgent::FileAgent(const FileAgent& source) :
		Agent(source) {

	m_filename = source.m_filename;
	m_owner = source.m_owner;
	m_group = source.m_group;
	m_perm = source.m_perm;
	m_attr = source.m_attr;
	m_md5 = source.m_md5;
	m_patterns = source.m_patterns;

	m_true_owner = source.m_true_owner;
	m_true_group = source.m_true_group;
	m_true_perm = source.m_true_perm;
	m_true_attr = source.m_true_attr;
	m_true_md5 = source.m_true_md5;
	m_mountpoint = source.m_mountpoint;

	m_no_such_file = source.m_no_such_file;
	m_notMounted = source.m_notMounted;
	m_isStale = source.m_isStale;
	m_owner_pb = source.m_owner_pb;
	m_group_pb = source.m_group_pb;
	m_perm_pb = source.m_perm_pb;
	m_attr_pb = source.m_attr_pb;
	m_md5_pb = source.m_md5_pb;
	m_patterns_pb = source.m_patterns_pb;
	m_patterns_pb_detail = source.m_patterns_pb_detail;
}

FileAgent & FileAgent::operator=(const FileAgent& source) {
	*this = (Agent&) source;

	m_filename = source.m_filename;
	m_owner = source.m_owner;
	m_group = source.m_group;
	m_perm = source.m_perm;
	m_attr = source.m_attr;
	m_md5 = source.m_md5;
	m_patterns = source.m_patterns;

	m_true_owner = source.m_true_owner;
	m_true_group = source.m_true_group;
	m_true_perm = source.m_true_perm;
	m_true_attr = source.m_true_attr;
	m_true_md5 = source.m_true_md5;
	m_mountpoint = source.m_mountpoint;

	m_no_such_file = source.m_no_such_file;
	m_notMounted = source.m_notMounted;
	m_isStale = source.m_isStale;
	m_owner_pb = source.m_owner_pb;
	m_group_pb = source.m_group_pb;
	m_perm_pb = source.m_perm_pb;
	m_attr_pb = source.m_attr_pb;
	m_md5_pb = source.m_md5_pb;
	m_patterns_pb = source.m_patterns_pb;
	vector<bool> m_patterns_pb_detail = source.m_patterns_pb_detail;
	return *this;
}

FileAgent::~FileAgent() {

}

void FileAgent::setPath(string path) {
	m_filename = path;
}

bool FileAgent::analyze() {
	// This is just for the statistic, not to take into account fstab agents
	if ((m_filename.compare("/etc/fstab"))
			&& (!((boost::starts_with(m_filename, "/proc/"))
					&& (boost::starts_with(m_filename, "/proc/"))))) { //if it is not already inside another agent
		MetaAgent::TMP_attempts++;
		m_iManager->sendStatistic(m_name + "_analyze");
	}
#ifdef SIMU
	bool ret = Agent::analyze();
	return ret;
#else

	// We do a quick environment check. If it is faulty, the Environment Agent is set responsible
	bool envPb = m_server->getEnvironmentAgent()->makeQuickEnvironmentCheck();
	if (envPb) {
		m_responsibleChild = m_server->getEnvironmentAgent();
		setCause(m_responsibleChild->getCause());
		return true;
	}

	m_iManager->addLog(
			"FileAgent " + m_name + " : starting analysis (" + m_filename
					+ ")");

	Agent::analyze();
	string space;
	string serverAddr = m_server->getAddress();

	std::map<std::string, bool> require;

	for (int i = 0; i < m_deep; i++) {
		space += "\t";
	}

	m_cause = "";
	m_owner_pb = false;
	m_group_pb = false;
	m_perm_pb = false;
	m_attr_pb = false;
	m_md5_pb = false;
	m_patterns_pb = false;
	m_no_such_file = false;
	m_notMounted = false;
	m_isStale = false;
	m_patterns_pb_detail = vector<bool>(m_patterns.size(), false);

	if (!m_owner.empty())
		require["owner"] = true;

	if (!m_group.empty())
		require["group"] = true;

	if (!m_perm.empty())
		require["permissions"] = true;

	if (!m_attr.empty())
		require["attributes"] = true;

	if (!m_md5.empty())
		require["md5"] = true;

	if (!m_patterns.empty()) {
		require["regex"] = true;
	}
	Request tm(Request::FILE, m_filename, require);
	if (m_patterns.size()) {
		tm.setRegex(m_patterns);
	}
	fileInfo fileI;

	// Make the query on the remote agent
	// In case of problem, we set the remoteAgentPb flag, and set ourselves as responsible
	try {
		fileI = queryRemoteAgent<fileInfo>(serverAddr, tm);
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

	stringstream fileIStr(stringstream::in | stringstream::out);
	fileIStr << fileI;

	m_iManager->addLog(
			"FileAgent " + m_name + " : returned by the agent \n"
					+ fileIStr.str());

	// If the file we are looking for does not exist, we try to get more
	// information to know why it is not there.
	// A possibility would be that the file were it should be is not mounted
	if (!fileI.m_fileExists) {

		setCause(
				"FileAgent " + m_name + " : the file '" + m_filename
						+ "' does not exist");
		m_no_such_file = true;
		m_responsibleChild = this;

		// Try to know a bit more why the file is not there
		std::map<std::string, bool> empty;
		Request mountPointReq(Request::MOUNTPOINT_PATH, m_filename, empty);

		// Get the mountpoint information for the file we are looking for
		try {
			m_mountpoint = queryRemoteAgent<MountpointInfo>(serverAddr,
					mountPointReq);
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

		if (!m_mountpoint.m_runtimeErrors.size()) { //if we had no problem looking at the mountpoint

			if (!m_mountpoint.m_type.compare("nfs")) { //So far I take only care of nfs, without automout

				if (m_mountpoint.m_mounted) { //If the file system is mounted

					// IF stale
					//  CURE STALE
					// ELSE
					// ADD ON THE TODO list the nfs server
					setCause("No such file");
					m_no_such_file = true;

				} else { // the FS is not mounted
					setCause("FileSystem not mounted");
					m_notMounted = true;
				}
			}

		}
		m_iManager->sendStatistic(m_name + "_responsible");
		return true;
	}

	// Check whether the owner is what it is supposed to be
	if (m_owner.size()) {
		m_true_owner = fileI.m_owner;

		m_owner_pb = m_true_owner.compare(m_owner);

		if (m_owner_pb) {
			m_responsibleChild = this;
			setCause(
					"File " + m_filename + " has owner " + m_true_owner
							+ " instead of " + m_owner);

		}
	}

	// Check whether the group is what it is supposed to be
	if (m_group.size()) {
		m_true_group = fileI.m_group;
		m_group_pb = m_true_group.compare(m_group);

		if (m_group_pb) {
			m_responsibleChild = this;
			setCause(
					"File " + m_filename + " has group " + m_true_group
							+ " instead of " + m_group);
		}
	}

	// Check whether the permissions are what they are supposed to be
	if (m_perm.size()) {

		m_true_perm = fileI.m_permissions;
		m_perm_pb = m_true_perm.compare(m_perm);

		if (m_perm_pb) {
			m_responsibleChild = this;
			setCause(
					"File " + m_filename + " has perm " + m_true_perm
							+ " instead of " + m_perm);
		}
	}

	// Check whether the attributes are what they are supposed to be
	if (m_attr.size()) {
		m_true_attr = fileI.m_attributes;
		m_attr_pb = m_true_attr.compare(m_attr);

		if (m_attr_pb) {
			m_responsibleChild = this;
			setCause(
					"File " + m_filename + " has attr " + m_true_attr
							+ " instead of " + m_attr);

		}
	}

	// Check whether the md5 is what it is supposed to be
	if (m_md5.size()) {
		m_true_md5 = fileI.m_md5;
		m_md5_pb = m_true_md5.compare(m_md5);

		if (m_md5_pb) {
			m_responsibleChild = this;
			setCause(
					"File " + m_filename + " has md5 " + m_true_md5
							+ " instead of " + m_md5);

		}
	}

	// Check whether all the paterns were matched
	if (m_patterns.size()) {
		for (unsigned int i = 0; i < m_patterns.size(); i++) {
			bool regStat = fileI.m_regex[m_patterns[i]];
			m_patterns_pb_detail[i] = regStat;
			m_patterns_pb = m_patterns_pb && regStat;
		}

		if (m_patterns_pb) {
			m_responsibleChild = this;
			std::string tmp_cause = "File " + m_filename
					+ " has does not have the following patterns : ";
			for (unsigned int i = 0; i < m_patterns_pb_detail.size(); i++) {
				if (m_patterns_pb_detail[i]) {
					tmp_cause += "\"" + m_patterns[i] + "\" , ";
				}
			}
			setCause("FileAgent " + m_name + " :  " + tmp_cause);

		}
	}

	bool final = m_owner_pb || m_group_pb || m_perm_pb || m_attr_pb || m_md5_pb
			|| m_patterns_pb;

	if (final)
		m_iManager->sendStatistic(m_name + "_responsible");
	return final;
#endif
}
