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


#include "phronesisAnalyzerClientApi.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <fstream>

#include <map>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <boost/serialization/version.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;

//#define REPETITION 100
//#define TOT 4
//
//#define NFSD  1
//#define EXPORTF  2
//#define NFSFILE2  4
//#define HTTP1  8
//#define INDEX1  16
//#define HTTP2  32
//#define INDEX2  64
//#define MYSQLD  128
//
//int g_NFSD = 0;
//int g_EXPORTF = 0;
//int g_NFSFILE2 = 0;
//int g_HTTP1 = 0;
//int g_INDEX1 = 0;
//int g_HTTP2 = 0;
//int g_INDEX2 = 0;
//int g_MYSQLD = 0;

int realPb;
int myRnd() {
	int r = rand();
	std::cout << "RAND " << r << std::endl;
	return r;
}

class Alarm {
public:
	std::string m_name;
	int m_counter;
	bool m_active;

	Alarm(std::string name) {
		m_name = name;
		m_counter = 0;
		m_active = false;
	}

	Alarm() {
		m_name = "noName";
		m_counter = 0;
		m_active = false;
	}

	Alarm(const Alarm &a) {
		m_name = a.m_name;
		m_counter = a.m_counter;
		m_active = a.m_active;
	}

	Alarm & operator=(const Alarm& a) {
		m_name = a.m_name;
		m_counter = a.m_counter;
		m_active = a.m_active;
		return (*this);
	}

	void on() {
		m_active = true;
	}

	void off() {
		m_active = false;
	}

	void reinitialize() {
		m_counter = 0;
		m_active = false;
	}
};

class Proba {
public:
	std::string m_name;
	std::vector<int> m_proba;
	std::vector<std::string> m_problem;
	std::vector<std::vector<std::string> > m_alarms;
	std::map<std::string, std::vector<std::string> > m_extra_problem; //problem that it creates

	std::vector<int> m_counters;
	std::vector<bool> m_status;
	Proba() {
	}
	;

	Proba(const Proba& p) {
		m_name = p.m_name;
		m_proba = p.m_proba;
		m_problem = p.m_problem;
		m_alarms = p.m_alarms;
		m_counters = p.m_counters;
		m_status = p.m_status;
		m_extra_problem = p.m_extra_problem;
	}
	Proba &operator=(const Proba& p) {
		m_name = p.m_name;
		m_proba = p.m_proba;
		m_problem = p.m_problem;
		m_alarms = p.m_alarms;
		m_counters = p.m_counters;
		m_status = p.m_status;
		m_extra_problem = p.m_extra_problem;
		return (*this);
	}

	friend ostream& operator<<(ostream& out, const Proba& p) {
		out << p.m_name << std::endl;
		for (unsigned int i = 0; i < p.m_proba.size(); i++) {
			out << "\t" << p.m_problem[i] << " : " << p.m_proba[i] << " (";
			for (std::vector<std::string>::const_iterator it =
					p.m_alarms[i].begin(); it != p.m_alarms[i].end(); it++) {
				out << *it << ",";
			}
			out << ")" << std::endl;
		}
		return out;
	}

	void reinitialize() {
		m_status.clear();
		for (unsigned int i = 0; i < m_proba.size(); i++) {
			m_status.push_back(false);
		}
	}
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & m_name;
		ar & m_proba;
		ar & m_problem;
		ar & m_extra_problem;
		ar & m_alarms;
		ar & m_counters;
		ar & m_status;
	}

};

BOOST_CLASS_VERSION(Proba, 0)

bool randCauses(std::vector<Proba> & proba,
		std::map<std::string, Alarm> & alarms, int & origProblemSize,
		int maxConcurentAlarm) {
	std::vector<std::string> copyProbaName;
	std::cout << "<<RAND>> <<RAND>>" << std::endl;
	bool generated = false;
	realPb = 0;
	for (std::vector<Proba>::iterator curProba = proba.begin();
			curProba != proba.end(); curProba++) {
		(*curProba).reinitialize();
		copyProbaName.push_back(curProba->m_name);
	}

	int curConcurentAlarm = 0;

	for (int i = 0; i < proba.size(); i++) {
		if (curConcurentAlarm == maxConcurentAlarm) {
			std::cout << "Reach max of concurrent alarm" << std::endl;
			break;
		} else {
			std::cout << " curConcurentAlarm " << curConcurentAlarm << " ( < "
					<< maxConcurentAlarm << ")" << std::endl;
		}

		int sa = myRnd() % copyProbaName.size();
		std::string sa_name = copyProbaName[sa];
		int ra = myRnd();
		std::cout << "Looking at " << sa_name << std::endl;

		std::vector<Proba>::iterator curProba;
		for (curProba = proba.begin(); curProba != proba.end(); curProba++) {
			if (sa_name.compare(curProba->m_name) == 0) {
				break;
			}
		}
		if (curProba == proba.end()) {
			std::cout << "PROBLEM FINDING THE PROBA" << std::endl;
			exit(1);
		}
		std::vector<std::string>::iterator nth = copyProbaName.begin() + sa;
		copyProbaName.erase(nth);
		if (!(ra % proba.size())) { //we will have a problem on the current proba
			curConcurentAlarm++;
			realPb++;
			generated = true;
			std::cout << "We will have a problem on " << (*curProba).m_name
					<< std::endl;
			int pId = 0;
			//			if ((*curProba).m_name.compare("Mysql")) {
			ra = myRnd();
			int r = ra % 100;

			while ((pId < (((*curProba).m_proba).size()))
					&& (r >= (((*curProba).m_proba)[pId]))) {
				pId++;
			}
			std::cout << "The pId is " << pId << " r is " << r
					<< " and we have a problem size of "
					<< ((*curProba).m_proba).size() << std::endl;
			//			}

			std::cout << "Problem on " << ((*curProba).m_problem)[pId]
					<< std::endl;
			(((*curProba).m_counters)[pId])++;
			((*curProba).m_status)[pId] = true;
			(alarms[(*curProba).m_name]).on();
			(alarms[(*curProba).m_name]).m_counter++;
			std::cout << "Alarms on ";
			;
			for (std::vector<std::string>::const_iterator alarm =
					(*curProba).m_alarms[pId].begin();
					alarm != ((*curProba).m_alarms)[pId].end(); alarm++) {
				alarms[*alarm].on();
				std::cout << *alarm << " ";
			}
			std::cout << std::endl;

			std::cout << "Extra problems created " << std::endl;
			for (std::vector<std::string>::iterator extraPb =
					(*curProba).m_extra_problem[((*curProba).m_problem)[pId]].begin();
					extraPb != (*curProba).m_extra_problem[((*curProba).m_problem)[pId]].end(); extraPb++) {
				for (std::vector<Proba>::iterator pIt = proba.begin();
						pIt != proba.end(); pIt++) {
					for (int pbId = 0; pbId != (*pIt).m_problem.size();
							pbId++) {
						if ((*extraPb).compare((*pIt).m_problem[pbId]) == 0)

							std::cout << *extraPb << " extra problem created on "
									<< (*pIt).m_name << std::endl;
						(*pIt).m_status[pbId] = true;
						break;

					}
				}
			}
		}

	}
//
//	for (std::vector<Proba>::iterator curProba = proba.begin();
//			curProba != proba.end(); curProba++) {
//		int ra = myRnd();
//		std::cout << "Looking at " << (*curProba).m_name << std::endl;
//		if (!(ra % proba.size())) { //we will have a problem on the current proba
//			realPb++;
//			generated = true;
//			std::cout << "We will have a problem on " << (*curProba).m_name
//					<< std::endl;
//			int pId = 0;
////			if ((*curProba).m_name.compare("Mysql")) {
//			ra = myRnd();
//			int r = ra % 100;
//
//			while ((pId < (((*curProba).m_proba).size()))
//					&& (r >= (((*curProba).m_proba)[pId]))) {
//				pId++;
//			}
//			std::cout << "The pId is " << pId << " r is " << r
//					<< " and we have a problem size of "
//					<< ((*curProba).m_proba).size() << std::endl;
////			}
//
//			std::cout << "Problem on " << ((*curProba).m_problem)[pId]
//					<< std::endl;
//			(((*curProba).m_counters)[pId])++;
//			((*curProba).m_status)[pId] = true;
//			(alarms[(*curProba).m_name]).on();
//			(alarms[(*curProba).m_name]).m_counter++;
//			std::cout << "Alarms on ";
//			;
//			for (std::vector<std::string>::const_iterator alarm =
//					(*curProba).m_alarms[pId].begin();
//					alarm != ((*curProba).m_alarms)[pId].end(); alarm++) {
//				alarms[*alarm].on();
//				std::cout << *alarm << " ";
//			}
//			std::cout << std::endl;
//		}
//	}

	for (std::map<std::string, Alarm>::const_iterator al = alarms.begin();
			al != alarms.end(); al++) {
		if ((al->second).m_active)
			origProblemSize++;
	}

	return generated;

}

void dumpFile(std::string fileName, std::map<std::string, std::string> values) {
	ofstream myfile;
	myfile.open(fileName.c_str());
	for (std::map<std::string, std::string>::const_iterator it = values.begin();
			it != values.end(); it++) {
		myfile << it->first << ":" << it->second << endl;
	}
	myfile.close();
}

//create = 1 : we make the problem
// create = 0 : we repair
void dealProblem(int create, std::vector<Proba> & proba,
		std::map<std::string, Alarm> & alarms) {
	std::map<std::string, std::string> values;

	for (std::vector<Proba>::iterator curProba = proba.begin();
			curProba != proba.end(); curProba++) {

		for (unsigned int i = 0; i < (*curProba).m_proba.size(); i++) {
			if (create) {
//				if (((*curProba).m_status[i])
//						|| (alarms[curProba->m_name].m_active)) {
				if (((*curProba).m_status[i])) {
					values[(*curProba).m_problem[i]] = "1";
				}

			} else {

				values[(*curProba).m_problem[i]] = "0";
			}

		}
	}

	dumpFile("/tmp/phrSimuFile", values);
}

void updateProblems(string solved, std::map<std::string, Alarm> & alarms,
		std::map<std::string, std::vector<std::string> > rules,
		std::vector<std::string> orderedAlarms) {

	if (!alarms[solved].m_counter) {
		std::cout << "no problem on it. bad diagnostic" << std::endl;
		return; //no problem on it. bad diagnostic
	}

	std::vector<std::string> rule = rules[solved];
	for (std::vector<std::string>::iterator r = rule.begin(); r != rule.end();
			r++) {
		if ((alarms[*r].m_active)) {
			std::cout << "There is still a problem on " << alarms[*r].m_name
					<< "So I don't decrease" << std::endl;
			return;
		}
	}

//we decrease the counter
	alarms[solved].m_counter--;
	std::cout << "After decrease " << alarms[solved].m_counter << std::endl;

	for (std::vector<std::string>::iterator it = orderedAlarms.begin();
			it != orderedAlarms.end(); it++) {
		std::cout << "Updating status of " << *it << endl;
		Alarm & alarm = alarms[*it];
		bool newState = false;
		std::cout << "alarm.m_counter = " << alarm.m_counter << std::endl;
		newState = newState || alarm.m_counter;
		std::vector<std::string> rule = rules[*it];
		for (std::vector<std::string>::iterator r = rule.begin();
				r != rule.end(); r++) {
			std::cout << *r << "  " << (alarms[*r].m_active) << std::endl;
			newState = newState || (alarms[*r].m_active);
		}

		alarm.m_active = newState;

	}

}

string pbToStr(std::map<std::string, Alarm> &pb) {
	string pbstr = "";
	std::map<std::string, Alarm>::iterator it;

	for (it = pb.begin(); it != pb.end(); it++) {
		if ((it->second).m_active)
			pbstr += (it->second).m_name + ";";
	}

	return pbstr;

}

class AutomTestConsole: public AbstractPhronesisClient {
public:
	bool yn_questionReceived;
	bool pb_questionReceived;
	bool m_messageReceived;

	std::vector<std::string> diagnosedList;
	bool diagnosedReceived;

	int visitedAmount;
	bool visitedAmountReceived;

	api_message msg;

	AutomTestConsole() {
		yn_questionReceived = false;
		pb_questionReceived = false;
		m_messageReceived = false;
		diagnosedReceived = false;
		visitedAmount = 0;
		visitedAmountReceived = false;
	}

	virtual void update(api_message e) {
		if (e.m_type == api_message::NEW_MSG) {
//			std::cout << "[NEW_MESSAGE] " << e << endl;
			if (boost::starts_with(e.m_text, "DIAGNOSED:")) {
				std::string diagnosedListStr = e.m_text.substr(10);
				boost::split(diagnosedList, diagnosedListStr,
						boost::is_any_of(","));
				//we remove the last one which is empty
				diagnosedList.pop_back();
				diagnosedReceived = true;
				this->ackMessage(e.m_id);
			} else if (boost::starts_with(e.m_text, "VISITED:")) {
				std::string amount = e.m_text.substr(8);
				visitedAmountReceived = true;
				visitedAmount = boost::lexical_cast<int>(amount);
				this->ackMessage(e.m_id);
			}

		} else if (e.m_type == api_message::NEW_LOG) {
			//std::cout << "[NEW LOG] ";

		} else if (e.m_type == api_message::NEW_QUESTION) {
			//std::cout << "[NEW QUESTION] ";
		} else if (e.m_type == api_message::NEW_YN_QUESTION) {
			//std::cout << "[NEW YN QUESTION] ";
			yn_questionReceived = true;

			//m_mainWindow.addYesNoQuestion(0, "dadada");
		} else if (e.m_type == api_message::NEW_PB_QUESTION) {
			//std::cout << "[CURRENT_PB] ";
			pb_questionReceived = true;
			msg = e;
			//m_mainWindow.addYesNoQuestion(0, "dadada");
		}
		//std::cout << e << std::endl;
	}

};

void mySleep(int nanosec) {
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = nanosec;
	nanosleep(&ts, NULL);
}

void readConfigurationFile(std::string fileName,
		std::map<std::string, Alarm> & alarms,
		std::map<std::string, std::vector<std::string> > &rules,
		std::vector<std::string> & orderedAlarms, std::vector<Proba> & proba,
		int & repetition, int & rndSeed, int & maxConcurentAlarm) {
	string content, lineRead;
	std::ostringstream ss;

	ifstream myfile(fileName.c_str());
	if (myfile.is_open()) {
		while (myfile) {
			getline(myfile, lineRead);
			ss << lineRead << std::endl;
		}
		content = ss.str();
		myfile.close();
	}

	else {
		cout << "Unable to open file";
		exit(1);
	}
	std::vector<std::string> lines;
	boost::split(lines, content, boost::is_any_of("\n"));
	std::vector<std::string>::iterator line = lines.begin();
	while ((!(*line).length()) || ((*line)[0] == '#'))
		line++;
//we read the random seed
	rndSeed = boost::lexical_cast<int>(*line);
	line++;
//we read how many repetition
	repetition = boost::lexical_cast<int>(*line);
	line++;
	maxConcurentAlarm = boost::lexical_cast<int>(*line);
	line++;
	while ((!(*line).length()) || ((*line)[0] == '#'))
		line++;
//here we should have the first line with the ordered entity
	boost::split(orderedAlarms, *line, boost::is_any_of(";"));
	for (std::vector<std::string>::iterator it = orderedAlarms.begin();
			it != orderedAlarms.end(); it++) {
		Alarm entity(*it);
		alarms[*it] = entity;
	}
	line++;

	while ((!(*line).length()) || ((*line)[0] == '#'))
		line++;
//now we should have the rules
	if (boost::contains(*line, "=")) {
		while ((line != lines.end()) && ((*line).length())) {
			std::vector<std::string> ruleEqual, rulePlus;
			boost::split(ruleEqual, *line, boost::is_any_of("="));
			boost::split(rulePlus, ruleEqual[1], boost::is_any_of("+"));
			rules[ruleEqual[0]] = rulePlus;
			line++;
		}
	}

	while ((!(*line).length()) || ((*line)[0] == '#'))
		line++;
//now we should have the proba
//Nfs::NFSFile2:2:Site2;Web;

	std::map<std::string, std::map<int, std::vector<std::string> > > probaTxt; //< NFS : < proba : <entity;Alarm> > >
	while ((line != lines.end()) && ((*line).length())) {
		std::vector<std::string> probaColumn;
		boost::split(probaColumn, *line, boost::is_any_of(":"));
		std::string coordName = probaColumn[0];
		std::cout << "coordName " << coordName << std::endl;
		std::string entityName = probaColumn[1];
		std::cout << "entityName " << entityName << std::endl;
		int probaValue = boost::lexical_cast<int>(probaColumn[2]);
		std::cout << "probaValue " << probaValue << std::endl;
		std::string alarmList = probaColumn[3];
		std::cout << "alarmList " << alarmList << std::endl;

		std::map<std::string, std::map<int, std::vector<std::string> > >::iterator coordIt =
				probaTxt.find(coordName);
		if (coordIt == probaTxt.end()) {
			std::cout << "I create a new Inside map" << std::endl;
			std::map<int, std::vector<std::string> > insideMap;
			probaTxt[coordName] = insideMap;
		} else {
			std::cout << "The inside map already exists" << std::endl;
		}

		std::map<int, std::vector<std::string> > & coordMap =
				probaTxt[coordName];
		std::map<int, std::vector<std::string> >::iterator probaId =
				coordMap.find(probaValue);

		if (probaId == coordMap.end()) {
			std::cout << "I assign the inside vector" << std::endl;
			vector<std::string> insideVector;
			insideVector.push_back(entityName);
			insideVector.push_back(alarmList);
			coordMap[probaValue] = insideVector;
		} else {
			std::cout << "Something is wrong, I already have an entry for "
					<< probaValue << endl;
			exit(1);
		}
		line++;
	}

	std::vector<std::string> probaOrder = orderedAlarms;
//	probaOrder.push_back("Site1");
//	probaOrder.push_back("Site2");
//	probaOrder.push_back("Mysql");
//
//	for (std::map<std::string, std::map<int, std::vector<std::string> > >::iterator coord =
//			probaTxt.begin(); coord != probaTxt.end(); coord++) {
	for (std::vector<std::string>::iterator itpo = probaOrder.begin();
			itpo != probaOrder.end(); itpo++) {
		Proba p;
		//p.m_name = coord->first;
		p.m_name = *itpo;
		if (probaTxt.find(p.m_name) == probaTxt.end()) {
			std::cout << "No proba for " << p.m_name << std::endl;
			continue;
		}
		std::cout << "New proba object for " << p.m_name << std::endl;

//		for (std::map<int, std::vector<std::string> >::iterator pValue =
//				(coord->second).begin(); pValue != (coord->second).end();
//				pValue++) {
		for (std::map<int, std::vector<std::string> >::iterator pValue =
				(probaTxt[*itpo]).begin(); pValue != (probaTxt[*itpo]).end();
				pValue++) {
			p.m_proba.push_back(pValue->first);
			p.m_counters.push_back(0);
			p.m_problem.push_back((pValue->second)[0]);
			std::vector<std::string> splitSemi;
			boost::split(splitSemi, (pValue->second)[1], boost::is_any_of(";"));
			p.m_alarms.push_back(splitSemi);

		}

		proba.push_back(p);

	}

	while ((!(*line).length()) || ((*line)[0] == '#'))
		line++;
//Now we check the extra problems
	while ((line != lines.end()) && ((*line).length())) {
		std::vector<std::string> extraProblemLine;
		boost::split(extraProblemLine, *line, boost::is_any_of(":"));
		std::string origPb = extraProblemLine[0];
		std::cout << "origPb " << origPb << std::endl;
		std::string extraPbsStr = extraProblemLine[1];
		std::vector<std::string> extraPbs;
		boost::split(extraPbs, extraPbsStr, boost::is_any_of(";"));

		for (std::vector<Proba>::iterator pIt = proba.begin();
				pIt != proba.end(); pIt++) {
			if (find((*pIt).m_problem.begin(), (*pIt).m_problem.end(), origPb)
					!= (*pIt).m_problem.end()) {
				std::cout << origPb << " goes with " << (*pIt).m_name
						<< std::endl;
				(*pIt).m_extra_problem[origPb] = extraPbs;
				break;
			}
		}

		line++;
	}

}

bool areProblemsRemaining(std::map<std::string, Alarm> & alarms) {

	for (std::map<std::string, Alarm>::const_iterator it = alarms.begin();
			it != alarms.end(); it++) {
		if ((it->second).m_active) {
			std::cout << it->first << " is active" << std::endl;
			return true;
		}
	}
	return false;

}

bool dumpInXml = false;
ofstream xmlFile;

void writeStat(std::string line) {
	std::cout << line << std::endl;
	if (dumpInXml)
		xmlFile << line << std::endl;

}

void displayPartialStatsAgent(map<int, int> statsAgent, int rep) {
	map<int, int>::iterator stat;
	cout << "<<<<<<<<<< STATS AGENT " << rep << " >>>>>>>>>>" << endl << endl;

	for (stat = statsAgent.begin(); stat != statsAgent.end(); stat++) {
		cout << "+" << stat->first << " : " << stat->second << " ("
				<< (stat->second * 100 / (float) rep) << "%)" << endl;
	}
	cout << endl;

}

int main(int argc, char ** argv) {

	map<int, int> stats;
	map<int, map<int, int> > perRealPb; //when i real problems, solved in opt + j, k times
	map<int, int> realPbOccur;
	map<int, int> servPbOccur;
	map<int, map<int, int> > perAlarmPb; //when i alarm problems, solved in opt + j, k times

	map<int, int> statsAgent;
	map<int, map<int, int> > perRealPbAgent; //when i real problems, solved in opt + j, k times
	map<int, int> realPbOccurAgent;
	map<int, int> servPbOccurAgent;
	map<int, map<int, int> > perAlarmPbAgent; //when i alarm problems, solved in opt + j, k times

	map<int, int>::iterator stat;
	map<int, map<int, int> >::iterator it;

//	map<string, vector<string> > rules;

	map<int, string> questions;
	map<int, string>::iterator quest;

	map<int, string> messages;
	map<int, string>::iterator msg;

	int repetition;
	int rndSeed;
	int maxConcurentAlarm;

//	vector<string> problems, origProblems;
	int origProblemSize = 0;
	int pbGenerated = false;

	std::map<std::string, Alarm> alarms;
	std::vector<std::string> orderedAlarms;
	std::map<std::string, std::vector<std::string> > rules;

	std::vector<Proba> proba;

	std::string dumpFile;
	std::string confFile;
	std::string xmlFilename;
	bool readDumpFile = false;
	bool gaveUp = false;

//	try {
	/** Define and parse the program options
	 */
	po::options_description desc("Options");
	desc.add_options()("help,h", "Print help messages")("configuration,c",
			po::value<std::string>(&confFile),
			"Configuration file of the simulation")("dump,d",
			po::value<std::string>(&dumpFile),
			"(mandatory) File in which to dump/read the stats")("read,r",
			"Read mode. The file is read and the stats displayed")("xml,x",
			po::value<std::string>(&xmlFilename),
			"File in which to dump/read the stats in xml");

	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm); // can throw

		/** --help option
		 */
		if (vm.count("help")) {
			std::cout << "Simulation program for the PhronesisAnalyzer"
					<< std::endl << desc << std::endl;
			return 0;
		}

		if (vm.count("read")) {
			readDumpFile = true;
		}

		if (!vm.count("configuration")) {
			if (!vm.count("read")) {

				std::cout << "Configuration file mandatory" << std::endl;
				std::cout << desc << std::endl;
				return 1;
			}

		}
		if (!vm.count("dump")) {

			std::cout << "Dump file mandatory" << std::endl;
			std::cout << desc << std::endl;
			return 1;

		}

		if (vm.count("xml")) {
			dumpInXml = true;
		}
		po::notify(vm); // throws on error, so do after help in case
						// there are any problems
	} catch (po::error& e) {
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	if (!readDumpFile) {
		readConfigurationFile(confFile, alarms, rules, orderedAlarms, proba,
				repetition, rndSeed, maxConcurentAlarm);

		srand(rndSeed);

		for (std::vector<Proba>::const_iterator pr = proba.begin();
				pr != proba.end(); pr++) {
			std::cout << *pr << std::endl;
		}

		AutomTestConsole phrApi;

		boost::thread t(&AutomTestConsole::run, &phrApi, string("127.0.0.1"),
				"7172");

//we ack the first message telling about the start

		for (int i = 0; i < repetition; i++) {
			gaveUp = false;
			cout << "****************** REPETITION " << i
					<< " ******************" << endl;
//		origProblems.clear();
			origProblemSize = 0;
			for (std::map<std::string, Alarm>::iterator oa = alarms.begin();
					oa != alarms.end(); oa++) {
				(oa->second).reinitialize();
			}
			int agentVisited = 0;
			int attempts = 0;
			do {
				pbGenerated = randCauses(proba, alarms, origProblemSize,
						maxConcurentAlarm);
			} while (!pbGenerated);
//		cout << "this will trigger : ";
//		vector<string>::iterator da;
//		for (da = origProblems.begin(); da != origProblems.end(); da++) {
//			cout << *da << ", ";
//		}
			cout << endl;

			dealProblem(1, proba, alarms);

			do {
				do {

					mySleep(10000);
				} while (!phrApi.pb_questionReceived);
				phrApi.pb_questionReceived = false;

				string pbStr = pbToStr(alarms);
				cout << " I answer " << pbStr << endl;

				phrApi.giveProblemList(pbStr);
				//clear here is new
				questions.clear();
				do {

					mySleep(10000);
				} while ((!phrApi.yn_questionReceived)
						&& (!phrApi.pb_questionReceived));

				if (phrApi.pb_questionReceived) {
					gaveUp = true;
					std::cout << "I give up on some stuff!" << std::endl;
				} else {
					phrApi.yn_questionReceived = false;

					std::vector<std::string> diagnosedList =
							phrApi.diagnosedList;

					cout << "Solved : --";

					for (std::vector<std::string>::const_iterator solved =
							diagnosedList.begin();
							solved != diagnosedList.end(); solved++) {
						cout << *solved << "--";
						updateProblems(*solved, alarms, rules, orderedAlarms);
						dealProblem(1, proba, alarms);
					}
					cout << endl;
					//I AM HERE, I JUST RECEIVED THE LIST OF SOLVED PROBLEM
					attempts++;

					//Now we need to say if we apply the suggestions or not
					// so far just always answer yes
					phrApi.answerQuestion(0, "y");
					//} while (problems.size());
				}
			} while (areProblemsRemaining(alarms) && !gaveUp);
			do {
				mySleep(10000);
			} while (!phrApi.pb_questionReceived);
			if (!gaveUp) {
				phrApi.pb_questionReceived = false;

				phrApi.giveProblemList(""); //all solved
			}

			do {
				mySleep(10000);
			} while (!phrApi.visitedAmountReceived);
			phrApi.visitedAmountReceived = false;

			dealProblem(0, proba, alarms);

			agentVisited = phrApi.visitedAmount;

//		cout << "Total problems : " << origProblems.size() << endl;
			cout << "Total problems : " << origProblemSize << endl;
			cout << "Real problems  : " << realPb << endl;
			cout << "Attempts to fix: " << attempts << endl;
			cout << "Visited agents : " << agentVisited << endl;

			int diff = attempts - realPb; // ideally 0

			stat = stats.find(diff);
			if (stat == stats.end()) {
				stats[diff] = 1;
			} else {
				stats[diff]++;
			}

			it = perRealPb.find(realPb);
			if (it == perRealPb.end()) {
				perRealPb[realPb][diff] = 1;
			} else {
				map<int, int> inMap = perRealPb[realPb];
				stat = inMap.find(diff);
				if (stat == inMap.end()) {
					perRealPb[realPb][diff] = 1;
				} else {
					perRealPb[realPb][diff]++;
				}
			}

//		int servPb = origProblems.size();
			int servPb = origProblemSize;

			it = perAlarmPb.find(servPb);
			if (it == perAlarmPb.end()) {
				perAlarmPb[servPb][diff] = 1;
			} else {
				map<int, int> inMap = perAlarmPb[servPb];
				stat = inMap.find(diff);
				if (stat == inMap.end()) {
					perAlarmPb[servPb][diff] = 1;
				} else {
					perAlarmPb[servPb][diff]++;
				}
			}

			stat = realPbOccur.find(realPb);
			if (stat == realPbOccur.end()) {
				realPbOccur[realPb] = 1;
			} else {
				realPbOccur[realPb]++;
			}

			stat = servPbOccur.find(servPb);
			if (stat == servPbOccur.end()) {
				servPbOccur[servPb] = 1;
			} else {
				servPbOccur[servPb]++;
			}

			int diffAgent = agentVisited - realPb;
			stat = statsAgent.find(diffAgent);
			if (stat == statsAgent.end()) {
				statsAgent[diffAgent] = 1;
			} else {
				statsAgent[diffAgent]++;
			}

			it = perRealPbAgent.find(realPb);
			if (it == perRealPbAgent.end()) {
				perRealPbAgent[realPb][diffAgent] = 1;
			} else {
				map<int, int> inMap = perRealPbAgent[realPb];
				stat = inMap.find(diffAgent);
				if (stat == inMap.end()) {
					perRealPbAgent[realPb][diffAgent] = 1;
				} else {
					perRealPbAgent[realPb][diffAgent]++;
				}
			}

			it = perAlarmPbAgent.find(servPb);
			if (it == perAlarmPbAgent.end()) {
				perAlarmPbAgent[servPb][diffAgent] = 1;
			} else {
				map<int, int> inMap = perAlarmPbAgent[servPb];
				stat = inMap.find(diffAgent);
				if (stat == inMap.end()) {
					perAlarmPbAgent[servPb][diffAgent] = 1;
				} else {
					perAlarmPbAgent[servPb][diffAgent]++;
				}
			}

			stat = realPbOccurAgent.find(realPb);
			if (stat == realPbOccurAgent.end()) {
				realPbOccurAgent[realPb] = 1;
			} else {
				realPbOccurAgent[realPb]++;
			}

			stat = servPbOccurAgent.find(servPb);
			if (stat == servPbOccurAgent.end()) {
				servPbOccurAgent[servPb] = 1;
			} else {
				servPbOccurAgent[servPb]++;
			}

			if ((i == 10) || (i == 20) || (i == 30) || (i == 40) || (i == 50)
					|| (i == 70) || (i == 100) || (i == 125) || (i == 150)
					|| (i == 175) || (i == 200) || (i == 225) || (i == 250)
					|| (i == 275) || (i == 300) || (i == 350) || (i == 400)
					|| (i == 450) || (i == 500))
				displayPartialStatsAgent(statsAgent, i + 1);

		}

		//Dump everything to the file
		std::ofstream ofs(dumpFile.c_str());
		// save data to archive
		{
			boost::archive::text_oarchive oa(ofs);
			// write class instance to archive
			oa << proba;
			oa << stats;
			oa << repetition;
			oa << perRealPb;
			oa << realPbOccur;
			oa << perAlarmPb;
			oa << servPbOccur;
			oa << statsAgent;
			oa << perRealPbAgent;
			oa << realPbOccurAgent;
			oa << perAlarmPbAgent;
			oa << servPbOccurAgent;
			oa << rndSeed;

			// archive and stream closed when destructors are called
		}

	} else { //we read the dump file
		{
			// create and open an archive for input
			std::ifstream ifs(dumpFile.c_str());
			boost::archive::text_iarchive ia(ifs);
			// read class state from archive
			ia >> proba;
			ia >> stats;
			ia >> repetition;
			ia >> perRealPb;
			ia >> realPbOccur;
			ia >> perAlarmPb;
			ia >> servPbOccur;
			ia >> statsAgent;
			ia >> perRealPbAgent;
			ia >> realPbOccurAgent;
			ia >> perAlarmPbAgent;
			ia >> servPbOccurAgent;
			ia >> rndSeed;
			// archive and stream closed when destructors are called
		}

	}

	if (dumpInXml) {
		xmlFile.open(xmlFilename.c_str());
		xmlFile << "<?xml	version = \"1.0\" encoding=\"utf-8\" ?>" << endl;
		;
		xmlFile << "<phrStats>" << endl;
	}
	if (dumpInXml) {
		xmlFile << "<summary>" << endl;
	}
	cout << "<<<<<<<<<<<< SUMMARY >>>>>>>>>>>>" << endl << endl;
//	cout << "g_NFSD = " << g_NFSD << endl;
//	cout << "g_EXPORTF = " << g_EXPORTF << endl;
//	cout << "g_NFSFILE2 = " << g_NFSFILE2 << endl;
//	cout << "g_HTTP1 = " << g_HTTP1 << endl;
//	cout << "g_INDEX1 = " << g_INDEX1 << endl;
//	cout << "g_HTTP2 = " << g_HTTP2 << endl;
//	cout << "g_INDEX2 = " << g_INDEX2 << endl;
//	cout << "g_MYSQLD = " << g_MYSQLD << endl;

	int totalPb = 0;

	for (std::vector<Proba>::iterator pbIt = proba.begin(); pbIt != proba.end();
			pbIt++) {
		for (unsigned int pbItId = 0; pbItId < ((*pbIt).m_proba).size();
				pbItId++) {
			cout << (*pbIt).m_problem[pbItId] << " = "
					<< (*pbIt).m_counters[pbItId] << endl;
			totalPb += (*pbIt).m_counters[pbItId];
			if (dumpInXml) {
				xmlFile << "\t<metaAgent>" << endl;
				xmlFile << "\t\t<name>" << (*pbIt).m_problem[pbItId]
						<< "</name>" << endl;
				xmlFile << "\t\t<counter>" << (*pbIt).m_counters[pbItId]
						<< "</counter>" << endl;
				xmlFile << "\t</metaAgent>" << endl;

			}
		}
	}

	cout << endl;
	cout << "Total : " << totalPb << endl;
	if (dumpInXml) {
		xmlFile << "\t<total>" << totalPb << "</total>" << endl;
	}
	if (dumpInXml) {
		xmlFile << "</summary>" << endl;
	}
	cout << "<<<<<<<<<<<<< STATS >>>>>>>>>>>>>" << endl << endl;
	if (dumpInXml) {
		xmlFile << "<globalStats>" << endl;
	}
	for (stat = stats.begin(); stat != stats.end(); stat++) {
		cout << "+" << stat->first << " : " << stat->second << " ("
				<< (stat->second * 100 / (float) repetition) << "%)" << endl;
		if (dumpInXml) {
			xmlFile << "\t<resolution>" << endl;
			xmlFile << "\t\t<extraAttempts>" << stat->first
					<< "</extraAttempts>" << endl;
			xmlFile << "\t\t<amount>" << stat->second << "</amount>" << endl;
			xmlFile << "\t\t<amountPercent>"
					<< (stat->second * 100 / (float) repetition)
					<< "</amountPercent>" << endl;
			xmlFile << "\t</resolution>" << endl;
		}

	}
	cout << endl;
	if (dumpInXml) {
		xmlFile << "</globalStats>" << endl;
	}

	cout << "<<<<<<<<<<<<< In DB >>>>>>>>>>>>>" << endl << endl;

	cout << "<<<<<<<<<<<<< RULES >>>>>>>>>>>>>" << endl << endl;

	cout << "<<<<<<<<<< PER REAL PB >>>>>>>>>>" << endl << endl;
	if (dumpInXml) {
		xmlFile << "<realPbStats>" << endl;
	}
	for (it = perRealPb.begin(); it != perRealPb.end(); it++) {
		cout << it->first << " problems :" << endl;
		if (dumpInXml) {
			xmlFile << "\t<realPbStat>" << endl;
			xmlFile << "\t\t<amountOfPb>" << it->first << "</amountOfPb>"
					<< std::endl;
		}
		for (stat = it->second.begin(); stat != it->second.end(); stat++) {
			cout << "\t+" << stat->first << " : " << stat->second << " ("
					<< (stat->second * 100 / (float) realPbOccur[it->first])
					<< "%)" << endl;

			if (dumpInXml) {
				xmlFile << "\t\t<resolution>" << endl;
				xmlFile << "\t\t\t<extraAttempts>" << stat->first
						<< "</extraAttempts>" << endl;
				xmlFile << "\t\t\t<amount>" << stat->second << "</amount>"
						<< endl;
				xmlFile << "\t\t\t<amountPercent>"
						<< (stat->second * 100 / (float) realPbOccur[it->first])
						<< "</amountPercent>" << endl;
				xmlFile << "\t\t</resolution>" << endl;
			}

		}
		if (dumpInXml) {
			xmlFile << "\t</realPbStat>" << endl;
		}
	}

	cout << endl;
	if (dumpInXml) {
		xmlFile << "</realPbStats>" << endl;
	}
	cout << "<<<<<<<<<< PER SERV PB >>>>>>>>>>" << endl << endl;
	if (dumpInXml) {
		xmlFile << "<alarmStats>" << endl;
	}
	for (it = perAlarmPb.begin(); it != perAlarmPb.end(); it++) {
		cout << it->first << " alarms :" << endl;
		if (dumpInXml) {
			xmlFile << "\t<alarmStat>" << endl;
			xmlFile << "\t\t<amountOfAlarm>" << it->first << "</amountOfAlarm>"
					<< std::endl;
		}

		for (stat = it->second.begin(); stat != it->second.end(); stat++) {
			cout << "\t+" << stat->first << " : " << stat->second << " ("
					<< (stat->second * 100 / (float) servPbOccur[it->first])
					<< "%)" << endl;

			if (dumpInXml) {
				xmlFile << "\t\t<resolution>" << endl;
				xmlFile << "\t\t\t<extraAttempts>" << stat->first
						<< "</extraAttempts>" << endl;
				xmlFile << "\t\t\t<amount>" << stat->second << "</amount>"
						<< endl;
				xmlFile << "\t\t\t<amountPercent>"
						<< (stat->second * 100 / (float) servPbOccur[it->first])
						<< "</amountPercent>" << endl;
				xmlFile << "\t\t</resolution>" << endl;
			}
		}
		if (dumpInXml) {
			xmlFile << "\t</alarmStat>" << endl;
		}
	}
	if (dumpInXml) {
		xmlFile << "</alarmStats>" << endl;
	}
	cout << endl;

	cout << "<<<<<<<<<< STATS AGENT >>>>>>>>>>" << endl << endl;
	if (dumpInXml) {
		xmlFile << "<globalVisitedStats>" << endl;
	}
	for (stat = statsAgent.begin(); stat != statsAgent.end(); stat++) {
		cout << "+" << stat->first << " : " << stat->second << " ("
				<< (stat->second * 100 / (float) repetition) << "%)" << endl;
		if (dumpInXml) {
			xmlFile << "\t<resolution>" << endl;
			xmlFile << "\t\t<extraVisited>" << stat->first << "</extraVisited>"
					<< endl;
			xmlFile << "\t\t<amount>" << stat->second << "</amount>" << endl;
			xmlFile << "\t\t<amountPercent>"
					<< (stat->second * 100 / (float) repetition)
					<< "</amountPercent>" << endl;
			xmlFile << "\t</resolution>" << endl;
		}
	}
	cout << endl;
	if (dumpInXml) {
		xmlFile << "</globalVisitedStats>" << endl;
	}
	cout << "<<<<<<< PER REAL PB AGENT >>>>>>>" << endl << endl;
	if (dumpInXml) {
		xmlFile << "<realPbVisitedStats>" << endl;
	}
	for (it = perRealPbAgent.begin(); it != perRealPbAgent.end(); it++) {
		cout << it->first << " problems :" << endl;
		if (dumpInXml) {
			xmlFile << "\t<realPbVisitedStat>" << endl;
			xmlFile << "\t\t<amountOfPb>" << it->first << "</amountOfPb>"
					<< std::endl;
		}
		for (stat = it->second.begin(); stat != it->second.end(); stat++) {
			cout << "\t+" << stat->first << " : " << stat->second << " ("
					<< (stat->second * 100 / (float) realPbOccurAgent[it->first])
					<< "%)" << endl;
			if (dumpInXml) {
				xmlFile << "\t\t<resolution>" << endl;
				xmlFile << "\t\t\t<extraVisited>" << stat->first
						<< "</extraVisited>" << endl;
				xmlFile << "\t\t\t<amount>" << stat->second << "</amount>"
						<< endl;
				xmlFile << "\t\t\t<amountPercent>"
						<< (stat->second * 100
								/ (float) realPbOccurAgent[it->first])
						<< "</amountPercent>" << endl;
				xmlFile << "\t\t</resolution>" << endl;
			}
		}
		if (dumpInXml) {
			xmlFile << "\t</realPbVisitedStat>" << endl;

		}
	}
	if (dumpInXml) {
		xmlFile << "</realPbVisitedStats>" << endl;
	}
	cout << endl;
	cout << "<<<<<<< PER SERV PB AGENT >>>>>>>" << endl << endl;
	if (dumpInXml) {
		xmlFile << "<alarmVisitedStats>" << endl;
	}
	for (it = perAlarmPbAgent.begin(); it != perAlarmPbAgent.end(); it++) {
		cout << it->first << " alarms :" << endl;
		if (dumpInXml) {
			xmlFile << "\t<alarmVisitedStat>" << endl;
			xmlFile << "\t\t<amountOfAlarm>" << it->first << "</amountOfAlarm>"
					<< std::endl;
		}
		for (stat = it->second.begin(); stat != it->second.end(); stat++) {
			cout << "\t+" << stat->first << " : " << stat->second << " ("
					<< (stat->second * 100 / (float) servPbOccurAgent[it->first])
					<< "%)" << endl;
			if (dumpInXml) {
				xmlFile << "\t\t<resolution>" << endl;
				xmlFile << "\t\t\t<extraVisited>" << stat->first
						<< "</extraVisited>" << endl;
				xmlFile << "\t\t\t<amount>" << stat->second << "</amount>"
						<< endl;
				xmlFile << "\t\t\t<amountPercent>"
						<< (stat->second * 100
								/ (float) servPbOccurAgent[it->first])
						<< "</amountPercent>" << endl;
				xmlFile << "\t\t</resolution>" << endl;
			}
		}
		if (dumpInXml) {
			xmlFile << "\t</alarmVisitedStat>" << endl;
		}
	}
	if (dumpInXml) {
		xmlFile << "</alarmVisitedStats>" << endl;
	}
//	} catch (std::exception& e) {
//		std::cerr << "Unhandled Exception reached the top of main: " << e.what()
//				<< ", application will now exit" << std::endl;
//		return 1;
//
//	}
	if (xmlFile) {
		xmlFile << "</phrStats>";
	}
	return 0;
}
