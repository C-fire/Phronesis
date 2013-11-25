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


#include "AnalyzerDBAPI.hpp"

AnalyzerDBAPI::AnalyzerDBAPI(string dbType, string server, string user,
		string passwd, int port, string name) {
	m_result = NULL;

	if (dbType.compare("oracle") == 0) {
		this->dbType = Oracle;
	} else if (dbType.compare("mysql") == 0) {
		this->dbType = MySQL;
	} else {
		throw std::runtime_error("This database type is not supported");
	}

	this->connect(server, user, passwd, port, name);

}

AnalyzerDBAPI::~AnalyzerDBAPI() {
#ifdef USE_MYSQL
	mysql_close(&m_connection);
	mysql_library_end();
#endif

}

string AnalyzerDBAPI::escapeString(string pStr) {
#ifdef USE_MYSQL
	char *tStr = new char[pStr.length() * 2 + 1];
	mysql_real_escape_string(&m_connection, tStr, pStr.c_str(), pStr.length());
	string retStr(tStr);
	delete[] tStr;
	return retStr;
#else
	return pStr;
#endif
}

void AnalyzerDBAPI::connect(string server, string user, string passwd, int port,
		string name) {

// If we use mysql db
#ifdef USE_MYSQL
	// We initialize le library, and throw an error if it fails
	mysql_library_init(0, NULL, NULL);
	if (mysql_init(&m_connection) == NULL) {

		throw std::runtime_error(
				"Error : "
						+ boost::lexical_cast<std::string>(
								mysql_errno(&m_connection)) + " "
						+ std::string(mysql_error(&m_connection)));
	}

	// Connect to the db, and throw if it fails

	my_bool my_true = true; //brrrrr, ugly!!
	mysql_options(&m_connection, MYSQL_OPT_RECONNECT, &my_true);
	if (mysql_real_connect(&m_connection, server.c_str(), user.c_str(),
			passwd.c_str(), name.c_str(), port, NULL, 0) == NULL) {
		throw std::runtime_error(
				"Error : "
						+ boost::lexical_cast<std::string>(
								mysql_errno(&m_connection)) + " "
						+ std::string(mysql_error(&m_connection)));
	}

#endif
}

int AnalyzerDBAPI::executeQuery(string Query, bool printReq, bool retry) {

	if (printReq)
		cout << Query << endl;
#ifdef USE_MYSQL
	int rowCount = 0;
	MYSQL_ROW row;

	int queryStatus = mysql_query(&m_connection, Query.c_str());

	// queryStatus is 0 if everything went fine
	// If the problem is that we were disconnected, we just retry it one
	// Otherwise we throw
	if (queryStatus) {
		//In case of loss of connection (errno = 2006) , we retry the query once
		if ((mysql_errno(&m_connection) == 2006) && (!retry)) {
			cout << "We have lost connection. Try to reconnect" << endl;
			return executeQuery(Query, printReq, true);
		} else
			throw std::runtime_error(
					"Error in the query : \"" + Query + "\" "
							+ boost::lexical_cast<std::string>(
									mysql_errno(&m_connection)) + " "
							+ std::string(mysql_error(&m_connection)));
	}

	// mysql_store_result returns null in case of error
	m_result = mysql_store_result(&m_connection);
	if (m_result == NULL) {
		mysql_free_result(m_result);
		throw std::runtime_error("Result null (query : " + Query + ")");
	}

	// Build the vector of result

	int num_fields = mysql_num_fields(m_result);
	vector<string> line;

	this->m_queryResult.clear();
	while ((row = mysql_fetch_row(m_result))) {
		for (int i = 0; i < num_fields; i++) {
			string r;
			if (row[i])
				r = row[i];
			else {
				mysql_free_result(m_result);
				throw std::runtime_error(
						"NULL column at index "
								+ boost::lexical_cast<std::string>(i)
								+ ". (query : " + Query + ")");
			}
			line.push_back(r);
		}
		this->m_queryResult.push_back(line);
		line.clear();
	}

	rowCount = m_queryResult.size();

	mysql_free_result(m_result);

	return rowCount;
#endif
}

void AnalyzerDBAPI::insertQuery(string query, bool retry) {

	int queryStatus = mysql_query(&m_connection, query.c_str());

	if (queryStatus != 0) {
		if ((mysql_errno(&m_connection) == 2006) && (!retry)) {
			cout << "We have lost connexion. Try to reconnect" << endl;
			insertQuery(query, true);
		} else
			throw std::runtime_error(
					"Error in the query : \"" + query + "\" "
							+ boost::lexical_cast<std::string>(
									mysql_errno(&m_connection)) + " "
							+ std::string(mysql_error(&m_connection)));
	}
}

//select t.id_parent, m.name, s.name from MetaAgentTree t, MetaAgent m, Server s where t.id_daughter = m.id and m.id_server = s.id and t.id_parent = 4 group by s.name;
// The order to retrieve is :
// - FileAgents
// - FolderAgents
// - Standard limits
// - ProcessAgents (the envAgent should already have been loaded)
// - ProcessAgent tree
// - EnvironmentAgents
void AnalyzerDBAPI::retrieveAllAgents(vector<Agent *> & inV,
		map<string, Server *> servers) {

	map<int, FileAgent *> fileAgentMap;
	map<int, ProcessAgent *> processAgentMap;
	int rowCount = 0;

	//First the file agents, because they might be used by processAgents;

	string req =
			"Select m.id, s.id, s.name, s.address, m.name, f.id, f.filename, f.owner, f.grp, f.permissions, f.attributes, f.md5 from MetaAgent m, FileAgent f, Server s where m.type like 'FileAgent' and f.id_metaAgent = m.id and m.id_server = s.id and m.valid is true;";
	try {
		rowCount = this->executeQuery(req);
	} catch (std::exception &e) {
		throw std::runtime_error(
				"Problem retrieving FileAgents.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	vector<vector<string> > fileAgents = m_queryResult;
	vector<vector<string> >::iterator line;

	for (line = fileAgents.begin(); line != fileAgents.end(); line++) {
		int idMeta = atoi((*line)[0].c_str());
		Server * faServer = servers[(*line)[2]];
		string idFile = (*line)[5];
		std::string maName = (*line)[4];
		string faFilename = (*line)[6];
		string faOwner = (*line)[7];
		string faGroup = (*line)[8];
		string faPerm = (*line)[9];
		string faAttr = (*line)[10];
		string faMd5 = (*line)[11];
		vector<string> faPatterns;

		string req = "select rule from FileContentRules where id_file = "
				+ idFile + ";";
		try {
			rowCount = this->executeQuery(req);
			vector<vector<string> > patternResult = m_queryResult;
			vector<vector<string> >::iterator pat;
			for (pat = patternResult.begin(); pat != patternResult.end();
					pat++) {
				faPatterns.push_back((*pat)[0]);
			}

		} catch (std::exception &e) {
			throw std::runtime_error(
					"Problem retrieving FileContent of the FileAgent " + maName
							+ ".\n(Details : " + std::string(e.what()) + ").");
		}

		FileAgent * fa = new FileAgent(idMeta, maName, faServer, faFilename,
				faOwner, faGroup, faPerm, faAttr, faMd5, faPatterns);

		fileAgentMap[atoi(idFile.c_str())] = fa;
		inV.push_back(fa);

	}

	//Now the Folder agents, because they might be used by processAgents;

	req =
			"Select m.id, s.id, s.name, s.address, m.name, f.id, f.filename, f.owner, f.grp, f.permissions, f.attributes, f.maxDepth from MetaAgent m, FileAgent f, Server s where m.type like 'FolderAgent' and f.id_metaAgent = m.id and m.id_server = s.id  and m.valid is true;";
	try {
		rowCount = this->executeQuery(req);
	} catch (std::exception &e) {
		throw std::runtime_error(
				"Problem retrieving FolderAgent.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	vector<vector<string> > folderAgents = m_queryResult;

	for (line = folderAgents.begin(); line != folderAgents.end(); line++) {
		int idMeta = atoi((*line)[0].c_str());
		Server * foServer = servers[(*line)[2]];
		string idFolder = (*line)[5];
		vector<string> patterns;
		std::vector<std::string> filenameFilters;

		std::string maName = (*line)[4];
		std::string foDirname = (*line)[6];
		std::string foOwner = (*line)[7];
		std::string foGroup = (*line)[8];
		std::string foPerm = (*line)[9];
		std::string foAttr = (*line)[10];
		int foMaxDepth = boost::lexical_cast<int>((*line)[11]);

		// We retrieve the patterns the files have to match
		string req = "select rule from FileContentRules where id_file = "
				+ idFolder + ";";
		try {
			rowCount = this->executeQuery(req);
			vector<vector<string> > patternResult = m_queryResult;
			vector<vector<string> >::iterator pat;
			for (pat = patternResult.begin(); pat != patternResult.end();
					pat++) {
				patterns.push_back((*pat)[0]);
			}

		} catch (std::exception &e) {
			throw std::runtime_error(
					"Problem retrieving FileContentRules for FolderAgent.\n(Details : "
							+ std::string(e.what()) + ").");
		}

		// We retrieve the filter for the names of the content
		req = "select filter from FilenameFilter where id_folder = " + idFolder
				+ ";";
		try {
			rowCount = this->executeQuery(req);
			vector<vector<string> > filterResult = m_queryResult;
			vector<vector<string> >::iterator filter;
			for (filter = filterResult.begin(); filter != filterResult.end();
					filter++) {
				filenameFilters.push_back((*filter)[0]);
			}

		} catch (std::exception &e) {
			throw std::runtime_error(
					"Problem retrieving FilenameFilter for FolderAgent.\n(Details : "
							+ std::string(e.what()) + ").");
		}

		FolderAgent * fa = new FolderAgent(idMeta, maName, foServer, foDirname,
				foOwner, foGroup, foPerm, foAttr, patterns, foMaxDepth,
				filenameFilters);

		inV.push_back(fa);

	}

	// Then retrieve the ProcessAgent

	req =
			"select m.id, s.id, s.name, s.address, m.name, p.id, p.procName, p.user, p.multiplicity, p.service, p.command, p.maxCpu, p.maxMemory from MetaAgent m, ProcessAgent p, Server s where p.id_metaAgent = m.id and m.id_server = s.id  and m.valid is true;";

	try {
		rowCount = this->executeQuery(req);
	} catch (std::exception &e) {
		throw std::runtime_error(
				"Problem retrieving ProcessAgents.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	vector<vector<string> > processAgents = m_queryResult;

	map<string, unsigned long int> defaultLimits;

	//First we take the default limit
	req = "select name, defValue from Limits order by id;";

	try {
		rowCount = this->executeQuery(req);
	} catch (std::exception &e) {
		throw std::runtime_error(
				"Problem retrieving limits.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	if (!rowCount) {
		throw std::runtime_error(
				"There is no default limit stored in the database. The database might be corrupted, not initialized, or you are not running the good schema version.");
	}

	vector<vector<string> > defaultLimitResult = m_queryResult;

	for (vector<vector<string> >::iterator lim = defaultLimitResult.begin();
			lim != defaultLimitResult.end(); lim++) {
		defaultLimits[(*lim)[0]] = (strtoul((*lim)[1].c_str(), NULL, 10));
	}

	for (line = processAgents.begin(); line != processAgents.end(); line++) {
		int idMeta = atoi((*line)[0].c_str());
		Server * paServer = servers[(*line)[2]];
		std::string idProc = (*line)[5];
		std::string maName = (*line)[4];
		std::string paProcName = (*line)[6];
		std::string paUser = (*line)[7];
		int paMultiplicity = boost::lexical_cast<int>((*line)[8]);
		float paMaxCpu = boost::lexical_cast<float>((*line)[11]);
		float paMaxMemory = boost::lexical_cast<float>((*line)[12]);
		std::string paService = (*line)[9];
		std::string paCommand = (*line)[10];

		// First we consider that all the limits are the standard one
		map<string, unsigned long int> limits = defaultLimits;

		//Then we modify the customized one
		req =
				"select l.name, p.value from ProcessLimits p, Limits l where p.id_limit = l.id and id_proc = "
						+ idProc + ";";

		try {
			rowCount = this->executeQuery(req);
			vector<vector<string> > culstomLim = m_queryResult;
			vector<vector<string> >::iterator lim;

			for (lim = culstomLim.begin(); lim != culstomLim.end(); lim++) {
				limits[(*lim)[0]] = (strtoul((*lim)[1].c_str(), NULL, 10));
			}
		} catch (exception &e) {
			throw std::runtime_error(
					"Problem retrieving custom limits.\n(Details : "
							+ std::string(e.what()) + ").");
		}

		//We check if there is a FileAgent for the environment of the process
		FileAgent * envAgent = NULL;

		req = "select id_procEnvFile from ProcessAgent where id = " + idProc
				+ " and id_procEnvFile is not NULL;";

		try {
			rowCount = this->executeQuery(req);
			if (rowCount) {
				int idEnvAgent = atoi(m_queryResult[0][0].c_str());
				map<int, FileAgent *>::iterator fileIt = fileAgentMap.find(
						idEnvAgent);
				if (fileIt == fileAgentMap.end()) { //The file agent has not been retrieved, which is bad!
					throw std::runtime_error(
							"The fileAgent with id "
									+ boost::lexical_cast<std::string>(
											idEnvAgent)
									+ "has not been retrieved. It is needed by the process agent "
									+ boost::lexical_cast<std::string>(idProc));
				} else {
					envAgent = fileIt->second;
				}
			}
		} catch (exception &e) {
			throw std::runtime_error(
					"Problem retrieving the environment file agent for the process.\n(Details : "
							+ std::string(e.what()) + ").");

		}

		ProcessAgent * procAgent = new ProcessAgent(idMeta, maName, paServer,
				paProcName, paUser, limits, envAgent, paMultiplicity, paMaxCpu,
				paMaxMemory);

		if (paService.length()) { //name of a service is provided
			procAgent->setService(paService);
		} else if (paCommand.length()) { //command is provided
			procAgent->setCommand(paCommand);
		} else { // If no service nor command is provided, I just use the proc name
			procAgent->setCommand(paProcName);

		}

		processAgentMap[atoi(idProc.c_str())] = procAgent;
		inV.push_back(procAgent);

	}

	//Now we build the tree for the processes
	req = "select id, id_parent from ProcessAgent where id_parent is not NULL;";

	try {
		rowCount = this->executeQuery(req);

		vector<vector<string> > procTree = m_queryResult;

		for (line = procTree.begin(); line != procTree.end(); line++) {
			int idChild = boost::lexical_cast<int>((*line)[0]);
			int idParent = boost::lexical_cast<int>((*line)[1]);
			processAgentMap[idParent]->addSon(processAgentMap[idChild]);
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving the parent process tree.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	// Finaly the EnvironmentAgent
	req =
			"select m.id, s.id, s.name, s.address, m.name, e.id, e.maxLoad, e.maxMemory, e.maxSwap from MetaAgent m, EnvironmentAgent e, Server s where e.id_metaAgent = m.id and m.id_server = s.id  and m.valid is true;";

	try {
		rowCount = this->executeQuery(req);
		if (!rowCount) {
			throw std::runtime_error(
					"No EnvironmentAgents, this should not happen. (req = \""
							+ req + "\"");
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving EnvironmentAgents.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	vector<vector<string> > envAgents = m_queryResult;

	for (line = envAgents.begin(); line != envAgents.end(); line++) {
		int idMeta = boost::lexical_cast<int>((*line)[0]);
		Server * eaServer = servers[(*line)[2]];
		string idEnv = (*line)[5];
		vector<float> limits;
		std::string maName = (*line)[4];
		float eaMaxLoad = boost::lexical_cast<float>((*line)[6]);
		float eaMaxMemory = boost::lexical_cast<float>((*line)[7]);
		float eaMaxSwap = boost::lexical_cast<float>((*line)[8]);

		//We check if there is a FileAgent for the fstab
		FileAgent * fstabFile = NULL;

		req = "select id_fstab from EnvironmentAgent where id = " + idEnv
				+ " and  id_fstab is not NULL;";

		try {
			rowCount = this->executeQuery(req);
			if (rowCount) {
				int idFstabAgent = boost::lexical_cast<int>(
						m_queryResult[0][0]);

				map<int, FileAgent *>::iterator fileIt = fileAgentMap.find(
						idFstabAgent);
				if (fileIt == fileAgentMap.end()) { //The file agent has not been retrieved, which is bad!
					throw std::runtime_error(
							"The fileAgent with id "
									+ boost::lexical_cast<std::string>(
											idFstabAgent)
									+ " has not been retrieved!!");
				} else {
					fstabFile = fileIt->second;
				}
			}
		} catch (exception &e) {
			throw std::runtime_error(
					"Problem retrieving the fstab file id.\n(Details : "
							+ std::string(e.what()) + ").");
		}

		// Retrieve the list of File system to ignore
		req =
				"select f.fsName from EnvironmentAgent e, IgnoredFS f where e.id = f.id_envAgent and e.id = "
						+ idEnv + ";";

		std::vector<std::string> ignoredFS;
		try {
			rowCount = this->executeQuery(req);
			vector<vector<string> > ignoredFSResult = m_queryResult;

			for (vector<vector<string> >::iterator fs = ignoredFSResult.begin();
					fs != ignoredFSResult.end(); fs++) {
				ignoredFS.push_back((*fs)[0]);
			}

		} catch (exception &e) {
			throw std::runtime_error(
					"Problem retrieving the ignored FS.\n(Details : "
							+ std::string(e.what()) + ").");
		}

		EnvironmentAgent * envAgent = new EnvironmentAgent(idMeta, maName,
				eaServer, fstabFile, eaMaxLoad, eaMaxMemory, eaMaxSwap);

		eaServer->setEnvironmentAgent(envAgent);

		for (vector<string>::iterator fs = ignoredFS.begin();
				fs != ignoredFS.end(); fs++) {
			envAgent->addFSToIgnore(*fs);
		}

		inV.push_back(envAgent);

	}

}

void AnalyzerDBAPI::retrieveAllCoordinators(vector<Coordinator *> & inV) {
	int rowCount = 0;

	// We get the list of Coordinators
	string req =
			"select id, name, classification, tolerate from MetaAgent where type like 'Coordinator' and valid is true;";
	try {
		rowCount = this->executeQuery(req);
		if (!rowCount) {
			throw std::runtime_error(
					"The database does not contain any Coordinator. This is not normal.");
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving Coordinators.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	vector<vector<string> > result = this->m_queryResult;
	vector<vector<string> >::iterator line;

	for (line = result.begin(); line != result.end(); line++) {
		std::string maIdStr = (*line)[0];
		int maId = boost::lexical_cast<int>((*line)[0]);
		std::string coName = (*line)[1];
		std::string coClassif = (*line)[2];
		std::vector<std::string> coClassifVecStr;
		boost::split(coClassifVecStr, coClassif, boost::is_any_of("-"));
		std::vector<int> coClassifVec;
		for (std::vector<std::string>::iterator it = coClassifVecStr.begin();
				it != coClassifVecStr.end(); it++) {
			coClassifVec.push_back(boost::lexical_cast<int>(*it));
		}
		int coTolerate = boost::lexical_cast<int>((*line)[3]);

		Coordinator * newCoord = new Coordinator(maId, coName, coClassifVec,
				coTolerate);
		inV.push_back(newCoord);

		// Check if there are Veto for this Coordinator
		req = "select name from Veto where idCoord = " + maIdStr + ";";
		try {
			rowCount = this->executeQuery(req);
		} catch (exception &e) {
			throw std::runtime_error(
					"Problem retrieving Veto for Coordinator.\n(Details : "
							+ std::string(e.what()) + ") (query : " + req
							+ ").");

		}
		vector<vector<string> > vetoResult = this->m_queryResult;
		vector<vector<string> >::iterator vetoLine;
		for (vetoLine = vetoResult.begin(); vetoLine != vetoResult.end();
				vetoLine++) {
			newCoord->addVeto((*vetoLine)[0]);
		}
	}

}

void AnalyzerDBAPI::retrieveAllServers(map<string, Server *> & servers) {
	int rowcount = 0;
	string req = "select id, name, address from Server";
	try {
		rowcount = this->executeQuery(req);
		if (!rowcount) {
			throw std::runtime_error(
					"The database does not contain any Server. This is not normal.");
		}
		vector<vector<string> > result = this->m_queryResult;
		vector<vector<string> >::iterator line;

		for (line = result.begin(); line != result.end(); line++) {
			int seId = boost::lexical_cast<int>((*line)[0]);
			std::string seName = (*line)[1];
			std::string seAddress = (*line)[2];
			servers[(*line)[1]] = new Server(seId, seName, seAddress);
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving Servers.\n(Details : "
						+ std::string(e.what()) + ").");
	}

}

void AnalyzerDBAPI::incrementServerFailure(int sId) {
	string req = "Update Server set occurrence = occurrence + 1 where id = "
			+ boost::lexical_cast<std::string>(sId) + ";";
	try {
		insertQuery(req);
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem incrementing failure occurrence of Server.\n(Details : "
						+ std::string(e.what()) + ").");
	}
}

int AnalyzerDBAPI::getOccurrence(int id_parent, int id_daugther) {
	int rowCount = 0;
	int occurrence = 0;
	string req =
			"select occurrence from Occurrence o, MetaAgentTree t where o.id = t.id_occurrence and t.id_parent = "
					+ boost::lexical_cast<std::string>(id_parent)
					+ " and t.id_daughter = "
					+ boost::lexical_cast<std::string>(id_daugther) + ";";
	try {
		rowCount = executeQuery(req);
		if (rowCount) {
			string occStr = m_queryResult[0][0];

			occurrence = boost::lexical_cast<int>(occStr);
		} else {
			throw std::runtime_error(
					"There is no link in the database going from id "
							+ boost::lexical_cast<std::string>(id_parent)
							+ " to "
							+ boost::lexical_cast<std::string>(id_daugther));
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving occurrence.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	return occurrence;
}

int AnalyzerDBAPI::getTotal(int id_parent) {
	int rowCount = 0;
	int total = 0;
	string req =
			"select total from Total, MetaAgent where Total.id = MetaAgent.id_total and MetaAgent.id = "
					+ boost::lexical_cast<std::string>(id_parent) + ";";
	try {
		rowCount = executeQuery(req);
		if (rowCount) {
			string totalStr = m_queryResult[0][0];
			total = boost::lexical_cast<int>(totalStr);
		} else {
			throw std::runtime_error(
					"There is no total in the database for the id "
							+ boost::lexical_cast<std::string>(id_parent));
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving occurrence.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	return total;
}

vector<vector<string> > AnalyzerDBAPI::getTree() {
	int rowCount = 0;
	string req = "select id_parent, id_daughter from MetaAgentTree";
	try {
		rowCount = executeQuery(req);
		if (!rowCount) {
			throw std::runtime_error(
					"The database does not contain any link between MetaAgents. This is not normal.");
		}
	} catch (exception &e) {

		throw std::runtime_error(
				"Problem retrieving MetaAgent tree.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	vector<vector<string> > tree;
	vector<vector<string> > treeId = m_queryResult;
	vector<vector<string> >::iterator it;

	for (it = treeId.begin(); it != treeId.end(); it++) {

		string id_parent = (*it)[0];
		string id_daughter = (*it)[1];

		req = "select name from MetaAgent where id = " + id_parent
				+ " and valid is true;";
		try {
			rowCount = executeQuery(req);
			if (!rowCount) { // should not happen because of key constraint -> happens for agent not fully defined
				continue;
			}
		} catch (exception &e) {
			throw std::runtime_error(
					"Problem retrieving a MetaAgent.\n(Details : "
							+ std::string(e.what()) + ").");
		}
		string parent_name = m_queryResult[0][0];

		req = "select name from MetaAgent where id = " + id_daughter + ";";
		try {
			rowCount = executeQuery(req);
			if (!rowCount) { // should not happen because of key constraint -> happens for agent not fully defined
				continue;
			}
		} catch (exception &e) {
			throw std::runtime_error(
					"Problem retrieving a MetaAgent.\n(Details : "
							+ std::string(e.what()) + ").");
		}
		string daughter_name = m_queryResult[0][0];

		vector<string> tmp;
		tmp.push_back(parent_name);
		tmp.push_back(daughter_name);
		tree.push_back(tmp);
	}
	return tree;
}

vector<vector<string> > AnalyzerDBAPI::getServerAttachedCoordinators() {
	string req =
			"select s.name, m.name from Server s, MetaAgent m where s.id = m.id_server and m.type like 'Coordinator'";
	try {
		executeQuery(req);

	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving Coordinators attached to Servers tree.\n(Details : "
						+ std::string(e.what()) + ").");
	}

	vector<vector<string> > tree;
	vector<vector<string> > treeId = m_queryResult;
	vector<vector<string> >::iterator it;

	for (it = treeId.begin(); it != treeId.end(); it++) {

		string serverName = (*it)[0];
		string coordName = (*it)[1];

		std::vector<std::string> tmp;

		tmp.push_back(serverName);
		tmp.push_back(coordName);
		tree.push_back(tmp);
	}
	return tree;
}

void AnalyzerDBAPI::addUnsolved(int id_agent) {
	string req =
			"Update Total t,MetaAgent a set t.unsolved = t.unsolved +1 where t.id = a.id_total and a.id = "
					+ boost::lexical_cast<std::string>(id_agent) + ";";
	try {
		insertQuery(req);
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem creating adding Unsolved.\n(Details : "
						+ std::string(e.what()) + ")");
	}

}

void AnalyzerDBAPI::incrementTotal(int pId) {
	string req =
			"Update Total t,MetaAgent a set t.total = t.total +1, a.local_total = a.local_total +1 where t.id = a.id_total and a.id = "
					+ boost::lexical_cast<std::string>(pId) + ";";
	try {
		insertQuery(req);
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem incrementing total.\n(Details : "
						+ std::string(e.what()) + ")");
	}

}

void AnalyzerDBAPI::incrementLocalTotal(int pId) {
	string req =
			"Update MetaAgent a set a.local_total = a.local_total +1 where a.id = "
					+ boost::lexical_cast<std::string>(pId) + ";";
	try {
		insertQuery(req);
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem incrementing localTotal.\n(Details : "
						+ std::string(e.what()) + ")");
	}

}

void AnalyzerDBAPI::incrementOccurrence(int pId, int dId) {
	string req =
			"Update Occurrence o,MetaAgentTree t set o.occurrence = o.occurrence +1, t.local_occurrence = t.local_occurrence +1 where o.id = t.id_occurrence and t.id_parent = "
					+ boost::lexical_cast<std::string>(pId)
					+ " and t.id_daughter = "
					+ boost::lexical_cast<std::string>(dId) + ";";
	try {
		insertQuery(req);
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem incrementing Occurrence.\n(Details : "
						+ std::string(e.what()) + ")");
	}

}

void AnalyzerDBAPI::incrementDependencyOccurence(int id_prior, int id_after) {
	int rowCount = 0;
	string strPrior = boost::lexical_cast<std::string>(id_prior);
	string strAfter = boost::lexical_cast<std::string>(id_after);
	string req = "Select id from Dependency where id_prior = " + strPrior
			+ " and id_after = " + strAfter + ";";
	try {
		rowCount = executeQuery(req);
		if (!rowCount) {
			// No such rule for the moment, so we create one
//			req = "Insert into Rule (id_prior, id_after) values (" + strPrior
//					+ "," + strAfter + ");";
			/*!
			 * 19-12-2012 : the Rule are not in auto Increment (shall I put it back?)
			 * in any case, this request makes the job
			 */
			req =
					"insert into Dependency (id, id_prior, id_after) select max(id) +1, "
							+ strPrior + ", " + strAfter + " from Dependency";
			try {
				insertQuery(req);
			} catch (exception &e) {
				throw std::runtime_error(
						"Problem creating new rule.\n(Details : "
								+ std::string(e.what()) + ")");
			}
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving dependency.\n(Details : "
						+ std::string(e.what()) + ")");
	}

	req = "Update Dependency  set occurrence = occurrence +1 where id_prior = "
			+ strPrior + " and id_after = " + strAfter + ";";
	try {
		insertQuery(req);
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem updating dependency.\n(Details : "
						+ std::string(e.what()) + ")");
	}
}

int AnalyzerDBAPI::getDependencyOccurence(int id_prior, int id_after) {
	string strPrior = boost::lexical_cast<std::string>(id_prior);
	string strAfter = boost::lexical_cast<std::string>(id_after);
	string req = "Select occurrence from Dependency where id_prior = "
			+ strPrior + " and id_after = " + strAfter + ";";
	int occ = 0;
	int rowCount = 0;
	try {
		rowCount = executeQuery(req);
		if (!rowCount) { //There is no such rules, and in principle there should be if we are here
			throw std::runtime_error(
					"No dependency matching the criteria.\n(Details : " + req
							+ ")");
		}
		occ = boost::lexical_cast<int>(m_queryResult[0][0]);
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving dependency.\n(Details : "
						+ std::string(e.what()) + ")");
	}
	return occ;
}

void AnalyzerDBAPI::validateDependency(int id_prior, int id_after) {
	string strPrior = boost::lexical_cast<std::string>(id_prior);
	string strAfter = boost::lexical_cast<std::string>(id_after);
	string req = "Select id from Dependency where id_prior = " + strPrior
			+ " and id_after = " + strAfter + ";";
	int rowCount = 0;
	try {
		rowCount = executeQuery(req);
		if (!rowCount) {
			throw std::runtime_error(
					"No dependency matching the criteria.\n(Details : " + req
							+ ")");
		}
		string idRule = m_queryResult[0][0];
		req = "Update Dependency set validity = 1 where id = " + idRule + ";";
		insertQuery(req);
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem validating dependency.\n(Details : "
						+ std::string(e.what()) + ")");
	}
}

vector<string> AnalyzerDBAPI::getAfters(int id_prior) {
	string req =
			"Select a.name from MetaAgent a, Dependency r where a.id = r.id_after and r.id_prior = "
					+ boost::lexical_cast<std::string>(id_prior)
					+ " and r.validity = 1;";

	vector<string> afters;
	try {
		executeQuery(req);

		vector<vector<string> >::iterator line;

		for (line = m_queryResult.begin(); line != m_queryResult.end();
				line++) {
			afters.push_back((*line)[0]);
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving dependencies.\n(Details : "
						+ std::string(e.what()) + ")");
	}

	return afters;

}

vector<string> AnalyzerDBAPI::getBefores(int id_after) {
	string req =
			"Select a.name from MetaAgent a, Dependency r where a.id = r.id_prior and r.id_after = "
					+ boost::lexical_cast<std::string>(id_after)
					+ " and r.validity = 1;";

	vector<string> priors;
	try {
		executeQuery(req);

		vector<vector<string> >::iterator line;

		for (line = m_queryResult.begin(); line != m_queryResult.end();
				line++) {
			priors.push_back((*line)[0]);
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving dependencies.\n(Details : "
						+ std::string(e.what()) + ")");
	}

	return priors;

}

/*
 * This method returns a map where the key is the pair (MetaAgent name, action)
 * and the value a unique id.
 * the id starts at 1, so that we can then introduce a artificial 0
 */
map<pair<string, string>, int> AnalyzerDBAPI::getRecoveryRulesMap() {
	set<pair<string, string> > pairSet; //MaName, action
	map<pair<string, string>, int> rulesMap;
	string req =
			"select m.name, r.action1 from RecoveryRule r, MetaAgent m where m.id = r.id_ma1;";
	try {
		executeQuery(req);
		vector<vector<string> >::iterator line;

		for (line = m_queryResult.begin(); line != m_queryResult.end();
				line++) {
			pair<string, string> myPair((*line)[0], (*line)[1]);
			pairSet.insert(myPair);

		}
		req =
				"select m.name, r.action2 from RecoveryRule r, MetaAgent m where m.id = r.id_ma2";
		executeQuery(req);
		for (line = m_queryResult.begin(); line != m_queryResult.end();
				line++) {
			pair<string, string> myPair((*line)[0], (*line)[1]);
			pairSet.insert(myPair);

		}

		int i = 1;
		set<pair<string, string> >::iterator setIt;
		for (setIt = pairSet.begin(); setIt != pairSet.end(); setIt++) {
			rulesMap[*setIt] = i++;
		}

	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving the recovery rule map.\n(Details : "
						+ std::string(e.what()) + ")");
	}

	return rulesMap;

}

vector<pair<int, int> > AnalyzerDBAPI::getRecoveryRulesTree(
		map<pair<string, string>, int> rulesMap, string direction) {
	vector<pair<int, int> > tree;
	string req;
	if (direction.compare("")) {
		req =
				"select id_ma1, action1, id_ma2, action2 from RecoveryRule where direction like '"
						+ direction + "';";
	} else {
		req = "select id_ma1, action1, id_ma2, action2 from RecoveryRule;";
	}

	try {
		executeQuery(req);
		vector<vector<string> > recoveryRules = m_queryResult;
		string req = "select id, name from MetaAgent;";
		map<string, string> idName;
		executeQuery(req);

		vector<vector<string> >::iterator line;

		for (line = m_queryResult.begin(); line != m_queryResult.end();
				line++) {
			idName[(*line)[0]] = (*line)[1];
		}

		for (line = recoveryRules.begin(); line != recoveryRules.end();
				line++) {
			string ma1 = idName[(*line)[0]];
			string action1 = (*line)[1];
			pair<string, string> pair1(ma1, action1);
			int id1 = rulesMap[pair1];
			string ma2 = idName[(*line)[2]];
			string action2 = (*line)[3];
			pair<string, string> pair2(ma2, action2);

			int id2 = rulesMap[pair2];

			pair<int, int> myEdge(id1, id2);
			tree.push_back(myEdge);
		}

	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving the recovery rule tree.\n(Details : "
						+ std::string(e.what()) + ")");
	}

	return tree;
}

std::vector<RecoveryHistory> AnalyzerDBAPI::getRecoveryHistory(
		std::string responsibleName) {

	std::vector<RecoveryHistory> history;
	string req =
			"select id, date, time, userEntry from RecoveryHistory where responsible like '"
					+ responsibleName + "';";
	try {
		executeQuery(req);
		vector<vector<string> > dbHistory = m_queryResult;

		for (vector<vector<string> >::iterator hist = dbHistory.begin();
				hist != dbHistory.end(); hist++) {
			std::string id = (*hist)[0];
			std::string date = (*hist)[1];
			std::string time = (*hist)[2];
			std::string userEntry = (*hist)[3];
			RecoveryHistory historyEvent(date, time, responsibleName,
					userEntry);

			req =
					"select metaAgent, action from RecoveryHistory_Action where id_recoveryHistory = "
							+ id + ";";
			executeQuery(req);
			std::vector<std::pair<std::string, std::string> > actions;
			std::vector<std::vector<std::string> > dbActions = m_queryResult;
			for (vector<vector<string> >::iterator act = dbActions.begin();
					act != dbActions.end(); act++) {
				std::string ma = (*act)[0];
				std::string action = (*act)[1];
				actions.push_back(std::make_pair(ma, action));
			}

			historyEvent.setActions(actions);

			req =
					"select metaAgent from RecoveryHistory_Problem where before_after like 'before' and id_recoveryHistory = "
							+ id + ";";
			executeQuery(req);
			std::vector<std::string> problemsBefore;
			std::vector<std::vector<std::string> > dbBefore = m_queryResult;
			for (vector<vector<string> >::iterator pb = dbBefore.begin();
					pb != dbBefore.end(); pb++) {
				std::string ma = (*pb)[0];
				problemsBefore.push_back(ma);
			}
			historyEvent.setProblemsBefore(problemsBefore);

			req =
					"select metaAgent from RecoveryHistory_Problem where before_after like 'after' and id_recoveryHistory = "
							+ id + ";";
			executeQuery(req);
			std::vector<std::string> problemsAfter;
			std::vector<std::vector<std::string> > dbAfter = m_queryResult;
			for (vector<vector<string> >::iterator pb = dbAfter.begin();
					pb != dbAfter.end(); pb++) {
				std::string ma = (*pb)[0];
				problemsAfter.push_back(ma);
			}
			historyEvent.setProblemsAfter(problemsAfter);

			history.push_back(historyEvent);

		}

	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving the History of responsible '"
						+ responsibleName + "'.\n(Details : "
						+ std::string(e.what()) + ")");
	}

	return history;

}

std::string AnalyzerDBAPI::getMaxId(std::string table) {
	std::string req = "select max(id) from " + table + ";";
	std::string max;
	try {
		if (!executeQuery(req)) {
			max = "0";
		} else {
			max = m_queryResult[0][0];
		}
	} catch (exception &e) {
		throw std::runtime_error(
				"Problem retrieving the max id of the table '" + table
						+ "'.\n(Details : " + std::string(e.what()) + ")");
	}

	return max;

}
void AnalyzerDBAPI::addRecoveryHistoryEvent(RecoveryHistory event) {

	std::string req =
			"insert into RecoveryHistory (date, time, responsible, userEntry) values ('"
					+ event.getDate() + "','" + event.getTime() + "', '"
					+ event.getResponsible() + "','" + event.getUserEntry()
					+ "');";
	try {
		insertQuery(req);
		std::string lastHistory = getMaxId("RecoveryHistory");
		for (std::vector<std::string>::const_iterator it =
				event.getProblemsBefore().begin();
				it != event.getProblemsBefore().end(); it++) {
			try {
				req =
						"insert into RecoveryHistory_Problem (id_recoveryHistory, metaAgent, before_after) values ( "
								+ lastHistory + ", '" + *it + "', 'before')";
				insertQuery(req);
			} catch (std::exception &e) {
				//Not so cool if we loose one, but okay, better than loosing the whole history
			}
		}

		for (std::vector<std::string>::const_iterator it =
				event.getProblemsAfter().begin();
				it != event.getProblemsAfter().end(); it++) {
			try {
				req =
						"insert into RecoveryHistory_Problem (id_recoveryHistory, metaAgent, before_after) values ( "
								+ lastHistory + ", '" + *it + "', 'after')";
				insertQuery(req);
			} catch (std::exception &e) {
				//Not so cool if we loose one, but okay, better than loosing the whole history
			}
		}

		for (std::vector<std::pair<std::string, std::string> >::const_iterator it =
				event.getActions().begin(); it != event.getActions().end();
				it++) {
			try {
				req =
						"insert into RecoveryHistory_Action (id_recoveryHistory, metaAgent, action) values ( "
								+ lastHistory + ", '" + (*it).first + "', '"
								+ (*it).second + "')";
				insertQuery(req);
			} catch (std::exception &e) {
				//Not so cool if we loose one, but okay, better than loosing the whole history
			}
		}

		std::vector<std::pair<std::string, std::string> > m_actions;

	} catch (std::exception &e) {
		stringstream error(stringstream::in | stringstream::out);
		error << "Problem adding a RecoveryHistory event : " << std::endl
				<< event << "(Details : " + std::string(e.what()) << ")";
		throw std::runtime_error(error.str());
	}

}

