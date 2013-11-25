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


#include "FolderAgent.hpp"

void FolderAgent::init() {
	m_no_such_folder = false;
	m_notMounted = false;
	m_filePtr = NULL;
	m_maxDepth = 0;

}

FolderAgent::FolderAgent(int id, string name, Server * server, string dirname,
		string owner, string group, string perm, string attr,
		vector<string> patterns, int maxDepth,
		std::vector<std::string> filenameFilters) :
		Agent(id, name, server) {
	init();
	m_dirname = dirname;
	m_owner = owner;
	m_group = group;
	m_perm = perm;
	m_attr = attr;
	m_patterns = patterns;
	m_maxDepth = maxDepth;
	m_filenameFilters = filenameFilters;
}

FolderAgent::FolderAgent() :
		Agent() {
	init();
}

FolderAgent::FolderAgent(const FolderAgent& source) :
		Agent(source) {
	m_no_such_folder = source.m_no_such_folder;
	m_notMounted = source.m_notMounted;
	m_filePtr = NULL;
	m_maxDepth = source.m_maxDepth;
	m_dirname = source.m_dirname;
	m_owner = source.m_owner;
	m_group = source.m_group;
	m_perm = source.m_perm;
	m_attr = source.m_attr;
	m_patterns = source.m_patterns;
	m_filenameFilters = source.m_filenameFilters;

}

FolderAgent & FolderAgent::operator=(const FolderAgent& source) {
	*this = (Agent&) source;
	m_no_such_folder = source.m_no_such_folder;
	m_notMounted = source.m_notMounted;
	m_filePtr = NULL;
	m_maxDepth = source.m_maxDepth;
	m_dirname = source.m_dirname;
	m_owner = source.m_owner;
	m_group = source.m_group;
	m_perm = source.m_perm;
	m_attr = source.m_attr;
	m_patterns = source.m_patterns;
	m_filenameFilters = source.m_filenameFilters;
	return *this;
}

FolderAgent::~FolderAgent() {
	if (m_filePtr)
		delete m_filePtr;
}

void FolderAgent::setPath(string path) {
	m_dirname = path;
}

bool FolderAgent::analyze() {
	MetaAgent::TMP_attempts++;
	m_iManager->sendStatistic(m_name + "_analyze");

#ifdef SIMU
	bool ret = Agent::analyze();
	return ret;
#else
	m_no_such_folder = false;
	m_notMounted = false;
	if (m_filePtr) {
		delete m_filePtr;
		m_filePtr = NULL;
	}

	// First we make a quick environment check on the server
	bool envPb = m_server->getEnvironmentAgent()->makeQuickEnvironmentCheck();
	if (envPb) {
		m_responsibleChild = m_server->getEnvironmentAgent();
		setCause(m_responsibleChild->getCause());
		return true;
	}

	m_iManager->addLog("FolderAgent " + m_name + " : starting analysis");

	Agent::analyze();
	string space;
	string serverAddr = m_server->getAddress();

	std::map<std::string, bool> empty;
	for (int i = 0; i < m_deep; i++) {
		space += "\t";
	}

	m_cause = "";

	//first we analyze ourself as a file
	m_filePtr = new FileAgent(m_id, "content_" + m_name, m_server, m_dirname,
			m_owner, m_group, m_perm, m_attr, "", m_patterns);
	m_filePtr->setInteractionManager(m_iManager);
	m_filePtr->setPManager(m_pm);

	//not to count us several times
	MetaAgent::TMP_attempts--;

	if (m_filePtr->analyze()) {
		m_responsibleChild = m_filePtr;
		setCause(m_responsibleChild->getCause());

		m_iManager->sendStatistic(m_name + "_responsible");
		return true;
	}

	//From here we know that the folder exists

	Request tm(Request::FOLDER_CONTENT, m_dirname, empty);
	std::map<std::string, std::string> extraArgs;
	if (m_maxDepth)
		extraArgs["maxDepth"] = boost::lexical_cast<std::string>(m_maxDepth);
	tm.setExtraArguments(extraArgs);

	// We get the list of files in the folder
	// that are at a maximum depth of m_maxDepth
	folderInfo folderI;
	try {
		folderI = queryRemoteAgent<folderInfo>(serverAddr, tm);
	} catch (std::exception &e) {
		m_remoteAgentPb = true;
		m_responsibleChild = m_filePtr;
		stringstream ss(stringstream::in | stringstream::out);
		ss << "Problem executing the Request on " << serverAddr << " : "
				<< e.what();
		setCause(ss.str());

		m_iManager->sendStatistic(m_name + "_requestProblem");

		return true;
	}

	delete m_filePtr;
	m_filePtr = NULL;
	stringstream folderIStr(stringstream::in | stringstream::out);
	folderIStr << folderI;

	m_iManager->addLog(
			"FolderAgent " + m_name + " : returned by the agent \n"
					+ folderIStr.str());

	// For each file in the list, we check whether it matches
	// the name filter (if any).
	// If yes, we create a temporary FileAgent whose expected
	// attributes are those we expect from the folder.
	// This FileAgent is then checked.
	// If a FileAgent happens to be faulty, it is not deleted,
	// and set as responsible.

	for (std::vector<std::string>::const_iterator fileIt =
			folderI.m_fileList.begin(); fileIt != folderI.m_fileList.end();
			fileIt++) {

		boost::regex re;
		std::vector<std::string> fnSplit;
		boost::split(fnSplit, *fileIt, boost::is_any_of("/"));
		std::string tmpFilename = fnSplit.back();
		bool willAnalyzeThisFile = false;

		for (std::vector<std::string>::iterator it = m_filenameFilters.begin();
				it != m_filenameFilters.end(); it++) {

			// Match the string against the current regular expression
			try {
				re.assign(*it, boost::regex_constants::extended);
				if (regex_search(tmpFilename, re)) {
					willAnalyzeThisFile = true;
					break;
				}

			} catch (boost::regex_error& e) {
				stringstream ss(stringstream::in | stringstream::out);
				ss << "\"" << *it << "\" is not a valid regular expression: ("
						<< e.what()
						<< ") This file will not be checked, but I don't return an error."
						<< std::endl;
				m_iManager->addLog(ss.str());
			}
		}

		if (!willAnalyzeThisFile)
			continue;

		m_filePtr = new FileAgent(m_id, "content_" + m_name, m_server, *fileIt,
				m_owner, m_group, m_perm, m_attr, "", m_patterns);
		m_filePtr->setInteractionManager(m_iManager);
		m_filePtr->setPManager(m_pm);

		//not to count us several times
		MetaAgent::TMP_attempts--;
		if (m_filePtr->analyze()) {
			m_responsibleChild = m_filePtr;
			setCause(m_filePtr->getCause());
			m_iManager->sendStatistic(m_name + "_responsible");
			return true;
		}
		delete m_filePtr;
		m_filePtr = NULL;
	}

	return false;
#endif
}
