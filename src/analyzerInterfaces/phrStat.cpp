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


#include <string>
#include <vector>
#include <sqlite3.h>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/timer.hpp>
#include <boost/program_options.hpp>

#include "phronesisAnalyzerClientApi.hpp"

using namespace std;
namespace po = boost::program_options;

#include <sys/time.h>

class Timer {
public:
	Timer() :
			start_time_(now()) {
	}

	void restart() {
		start_time_ = now();
	}

	double elapsed() const {
		double t = now();

		return double(t - start_time_);
	}

private:
	double now() const {
		timeval t;
		gettimeofday(&t, NULL);
		return t.tv_sec + (t.tv_usec / 1000000.0);
	}

	double start_time_;
};

class AggregatorIteration {
protected:
	int m_diagnostic;
	int m_solution;
public:
	AggregatorIteration() :
			m_diagnostic(0), m_solution(0) {

	}

	AggregatorIteration(const AggregatorIteration & src) {
		m_diagnostic = src.m_diagnostic;
		m_solution = src.m_solution;
	}

	void setDiagnostic(int diag) {
		m_diagnostic = diag;
	}

	void setSolution(int sol) {
		m_solution = sol;
	}

	int getDiagnostic() const {
		return m_diagnostic;
	}

	int getSolution() const {
		return m_solution;
	}
};

class AggregatorAnalyze {
protected:
	std::string m_date;
	std::string m_time;
	int m_maAnalyze;
	int m_maVisited; /*! in principle should be the same than m_maAnalyze */
	int m_unsolved;

	std::vector<AggregatorIteration> m_aggIterations;

public:
	AggregatorAnalyze(std::string date, std::string time) :
			m_date(date), m_time(time), m_maAnalyze(0), m_maVisited(0), m_unsolved(
					0) {

	}

	AggregatorAnalyze(const AggregatorAnalyze & src) {
		m_date = src.m_date;
		m_time = src.m_time;
		m_maAnalyze = src.m_maAnalyze;
		m_maVisited = src.m_maVisited;
		m_unsolved = src.m_unsolved;
		m_aggIterations = src.m_aggIterations;
	}

	int maAnalyze() {
		m_maAnalyze++;
		return m_maAnalyze;
	}

	void addAggIteration(AggregatorIteration & iter) {
		m_aggIterations.push_back(iter);
	}

	bool setVisited(int visited) {
		m_maVisited = visited;
		return (m_maVisited == m_maAnalyze);
	}

	int addUnsolved() {
		m_unsolved++;
		return m_unsolved;
	}

	const std::vector<AggregatorIteration>& getAggIterations() const {
		return m_aggIterations;
	}

	const std::string& getDate() const {
		return m_date;
	}

	int getMaAnalyze() const {
		return m_maAnalyze;
	}

	int getMaVisited() const {
		return m_maVisited;
	}

	void setMaVisited(int maVisited) {
		m_maVisited = maVisited;
	}

	const std::string& getTime() const {
		return m_time;
	}

	int getUnsolved() const {
		return m_unsolved;
	}
};

class Database {
public:
	Database(char* filename) {
		database = NULL;
		open(filename);
	}
	~Database() {
	}
	;

	bool open(char* filename) {
		if (sqlite3_open(filename, &database) == SQLITE_OK) {
			createTables();
			return true;
		}
		return false;
	}

	vector<vector<string> > query(std::string query) {
		sqlite3_stmt *statement;
		vector<vector<string> > results;

		if (sqlite3_prepare_v2(database, query.c_str(), -1, &statement,
				0) == SQLITE_OK) {
			int cols = sqlite3_column_count(statement);
			int result = 0;
			while (true) {
				result = sqlite3_step(statement);

				if (result == SQLITE_ROW) {
					vector<string> values;
					for (int col = 0; col < cols; col++) {
						values.push_back(
								(char*) sqlite3_column_text(statement, col));
					}
					results.push_back(values);
				} else {
					break;
				}
			}

			sqlite3_finalize(statement);
		}

		string error = sqlite3_errmsg(database);
		if (error != "not an error")
			cout << query << " " << error << endl;

		return results;
	}
	void close() {
		sqlite3_close(database);
	}

	void printRet(std::vector<std::vector<std::string> > & reqRet) {
		std::cout << "Return has " << reqRet.size() << " lines" << std::endl;

		for (std::vector<std::vector<std::string> >::iterator line =
				reqRet.begin(); line != reqRet.end(); line++) {
			for (std::vector<std::string>::iterator col = (*line).begin();
					col != (*line).end(); col++) {
				std::cout << "-" << *col << "-";
			}
			std::cout << std::endl;
		}

	}

	int incrementCounter(std::string counterName, int counts) {
		std::vector<std::vector<std::string> > reqRet = query(
				"select id from Counter where name like '" + counterName
						+ "';");
		printRet(reqRet);
		if (!reqRet.size()) {
			query(
					"insert into Counter (name, count) values ('" + counterName
							+ "', " + boost::lexical_cast<std::string>(counts)
							+ ");");

		} else {
			query(
					"update Counter set count = count + "
							+ boost::lexical_cast<std::string>(counts)
							+ " where id = " + reqRet[0][0] + ";");
		}

		reqRet = query(
				"select count from Counter where name like '" + counterName
						+ "';");
		printRet(reqRet);
		return boost::lexical_cast<int>(reqRet[0][0]);

	}

	void saveAggAnalyze(AggregatorAnalyze & ag) {
		std::cout << "Save aggregatorAnalye object" << std::endl;
		std::string req =
				"INSERT INTO aggAnalyze (date, time, visited, maAnalyze, unsolved) values ('"
						+ ag.getDate() + "', '" + ag.getTime() + "', "
						+ boost::lexical_cast<std::string>(ag.getMaVisited())
						+ ", "
						+ boost::lexical_cast<std::string>(ag.getMaAnalyze())
						+ ", "
						+ boost::lexical_cast<std::string>(ag.getUnsolved())
						+ ");";
		std::cout << req << std::endl;
		query(req);

		std::vector<std::vector<std::string> > ret = query(
				"select max(id) from aggAnalyze;");
		std::string idAgg = ret[0][0];

		vector<AggregatorIteration> iters = ag.getAggIterations();
		for (std::vector<AggregatorIteration>::iterator it = iters.begin();
				it != iters.end(); it++) {
			query(
					"insert into aggIteration (idAnalyze, diagnostic, solution) values("
							+ idAgg + ", "
							+ boost::lexical_cast<std::string>(
									(*it).getDiagnostic()) + ", "
							+ boost::lexical_cast<std::string>(
									(*it).getSolution()) + ");");
		}

	}

	void beginTransaction() {
		char * errorMsg;
		sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, &errorMsg);
		if (errorMsg) {
			std::cout << "Error beginning the transaction " << errorMsg
					<< std::endl;
			sqlite3_free(errorMsg);
		}
	}

	void endTransaction() {
		char * errorMsg;
		sqlite3_exec(database, "END TRANSACTION", NULL, NULL, &errorMsg);
		if (errorMsg) {
			std::cout << "Error ending the transaction " << errorMsg
					<< std::endl;
			sqlite3_free(errorMsg);
		}
	}

private:
	sqlite3 *database;
	void createTables() {
		std::vector<std::vector<std::string> > reqRet;
		reqRet = query("select count(id) from Counter;");
		if (!reqRet.size()) {
			beginTransaction();
			query(
					"CREATE TABLE Counter(id INTEGER PRIMARY KEY AUTOINCREMENT , name TEXT, count INT);");
			query(
					"CREATE TABLE aggAnalyze( id INTEGER PRIMARY KEY AUTOINCREMENT , date TEXT, time TEXT, visited INT, maAnalyze INT, unsolved INT);");
			query(
					"CREATE TABLE aggIteration( id INTEGER PRIMARY KEY AUTOINCREMENT , idAnalyze INT, diagnostic INT, solution INT, FOREIGN KEY (idAnalyze) REFERENCES aggAnalyze(id));");
			endTransaction();
		}
	}
};

boost::asio::io_service io;

class PhrStat: public AbstractPhronesisClient {
private:
	Database & m_db;
	std::vector<AggregatorAnalyze *> m_analBuffer;
	std::map<std::string, int> m_counterBuffer;
	AggregatorAnalyze * m_curAnalyze;
	AggregatorIteration * m_curIter;
//	boost::asio::deadline_timer m_timer;
	boost::mutex m_threadMutex;
	Timer & m_t; // start timing

//	boost::shared_ptr<boost::asio::deadline_timer> m_ptrTimer;

	boost::shared_ptr<boost::asio::deadline_timer> sync_Timer;

	void pushLatestAnalyze() {
		if (m_curAnalyze) {
			if (m_curIter) {
				m_curAnalyze->addAggIteration(*m_curIter);
				delete m_curIter;
				m_curIter = NULL;
			} else {
				std::cout << "Very strange, there was no loop in this analyze?"
						<< std::endl;
			}
			m_threadMutex.lock();
			m_analBuffer.push_back(m_curAnalyze);
			std::cout << "in m_analBuffer " << m_analBuffer.size() << std::endl;
			m_threadMutex.unlock();
			m_curAnalyze = NULL;
		}
	}

	void newAggregatorAnalyze(std::string date, std::string time) {
		pushLatestAnalyze();
		m_curAnalyze = new AggregatorAnalyze(date, time);

	}

	void newAggregatorIteration() {
		if (m_curIter) {
			m_curAnalyze->addAggIteration(*m_curIter);
			delete m_curIter;
			m_curIter = NULL;
		} else {
			std::cout << "First loop of this analyze" << std::endl;
		}
		m_curIter = new AggregatorIteration();
	}

	int incrementCounter(std::string counterName) {
		m_threadMutex.lock();

		std::map<std::string, int>::iterator it = m_counterBuffer.find(
				counterName);
		if (it == m_counterBuffer.end())
			m_counterBuffer[counterName] = 1;
		else
			m_counterBuffer[counterName] = m_counterBuffer[counterName] + 1;

		m_threadMutex.unlock();

		return it->second;

		//return m_db.incrementCounter(counterName);

	}

//	void flushToDb(const boost::system::error_code& error) {
//
//		std::cout << m_t.elapsed() << " FLUSH!!!! " << error << std::endl;
//		if (error != boost::asio::error::operation_aborted) {
//			std::cout << "I enter!" << std::endl;
//
//			m_threadMutex.lock();
//			std::vector<AggregatorAnalyze *> localAnalBuffer = m_analBuffer;
//			m_analBuffer.clear();
//			std::map<std::string, int> localCounterBUffer = m_counterBuffer;
//			m_counterBuffer.clear();
//			m_threadMutex.unlock();
//			for (std::vector<AggregatorAnalyze *>::iterator it =
//					localAnalBuffer.begin(); it != localAnalBuffer.end();
//					it++) {
//				m_db.saveAggAnalyze(**it);
//			}
//
//			for (std::map<std::string, int>::iterator it =
//					localCounterBUffer.begin(); it != localCounterBUffer.end();
//					it++) {
//				m_db.incrementCounter(it->first, it->second);
//
//			}
//
//		}
//		m_timer.expires_from_now(boost::posix_time::seconds(5));
//		m_timer.async_wait(
//				boost::bind(&PhrStat::flushToDb, this,
//						boost::asio::placeholders::error));
//
//	}
//
//	void flushToDbPtr(const boost::system::error_code& error) {
//		std::cout << m_t.elapsed() << " FLUSH_PTR!!!! " << error << std::endl;
//		m_ptrTimer->expires_from_now(boost::posix_time::seconds(5));
//		m_ptrTimer->async_wait(
//				boost::bind(&PhrStat::flushToDbPtr, this,
//						boost::asio::placeholders::error));
//	}

public:
	void flushToDbSync() {

		sync_Timer = boost::shared_ptr<boost::asio::deadline_timer>(
				new boost::asio::deadline_timer(io));
		while (1) {
			sync_Timer->expires_from_now(boost::posix_time::seconds(5));
			sync_Timer->wait();
			std::cout << m_t.elapsed() << " FLUSH_SYNC!!!! " << std::endl;

			pushLatestAnalyze();
			m_threadMutex.lock();
			std::vector<AggregatorAnalyze *> localAnalBuffer = m_analBuffer;
			m_analBuffer.clear();
			std::map<std::string, int> localCounterBUffer = m_counterBuffer;
			m_counterBuffer.clear();
			m_threadMutex.unlock();
			m_db.beginTransaction();
			for (std::vector<AggregatorAnalyze *>::iterator it =
					localAnalBuffer.begin(); it != localAnalBuffer.end();
					it++) {
				m_db.saveAggAnalyze(**it);
			}

			for (std::map<std::string, int>::iterator it =
					localCounterBUffer.begin(); it != localCounterBUffer.end();
					it++) {
				m_db.incrementCounter(it->first, it->second);

			}
			m_db.endTransaction();

		}

	}
	PhrStat(std::string serverAddr, std::string port, Database db,
			Timer & timer) :
			AbstractPhronesisClient(serverAddr, port), m_db(db), sync_Timer(
					new boost::asio::deadline_timer(io)), m_t(timer) {
		m_curAnalyze = NULL;
		m_curIter = NULL;
//		m_timer.expires_from_now(boost::posix_time::seconds(5));
//		m_timer.async_wait(
//				boost::bind(&PhrStat::flushToDb, this,
//						boost::asio::placeholders::error));
//
//		m_ptrTimer->expires_from_now(boost::posix_time::seconds(5));
//		m_ptrTimer->async_wait(
//				boost::bind(&PhrStat::flushToDbPtr, this,
//						boost::asio::placeholders::error));
	}

	~PhrStat() {
		if (m_curAnalyze) {
			delete m_curAnalyze;
		}
		if (m_curIter) {
			delete m_curIter;
		}
	}

	void update(api_message e) {
		std::cout << "update at " << m_t.elapsed() << std::endl;
//		m_timer.expires_from_now(boost::posix_time::seconds(5));
//		m_ptrTimer->expires_from_now(boost::posix_time::seconds(5));
		sync_Timer->expires_from_now(boost::posix_time::seconds(5));

		if (e.m_type == api_message::STATISTIC) {
			std::cout << e << std::endl;
			if (e.m_text.compare("aggregator_analyze") == 0) {
				std::cout << "NEW_ANALYZE" << std::endl;
				newAggregatorAnalyze(e.m_date, e.m_time);
			} else if (e.m_text.compare("aggregator_analyze_loop") == 0) {
				newAggregatorIteration();
				std::cout << "NEW_LOOP" << std::endl;
			} else if (e.m_text.compare("visited") == 0) {
				if (m_curAnalyze)
					m_curAnalyze->setVisited(e.m_id);
				std::cout << "NEW_VISITED" << std::endl;
			} else if (e.m_text.compare("aggregator_unsolved_counter") == 0) {
				std::cout << "NEW_UNSOLVED" << std::endl;
				if (m_curAnalyze)
					m_curAnalyze->addUnsolved();
			} else {
				std::cout << "NEW_COUNTER" << std::endl;
				incrementCounter(e.m_text);
				if ((boost::ends_with(e.m_text, "_analyze"))
						&& !(boost::starts_with(e.m_text, "aggregator"))) {
					if (m_curAnalyze)
						m_curAnalyze->maAnalyze();

				}
			}

		}
	}

};

int main(int argc, char ** argv) {

	std::string server = "127.0.0.1";
	std::string port = "7172";

	std::string dbFilename;

	try {
		/** Define and parse the program options
		 */
		po::options_description desc("Options");
		desc.add_options()("help,h", "Print help messages")("database,d",
				po::value<std::string>(&dbFilename),
				"(mandatory) Sqlite3 file where to put the statistics")(
				"server,s", po::value<std::string>(&server),
				"Server to connect to (default localhost)")("port,p",
				po::value<std::string>(&port),
				"The port the server is listening to (default 7172)");

		po::variables_map vm;
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm); // can throw

			/** --help option
			 */
			if (vm.count("help")) {
				std::cout
						<< "Utility program to interact with the phronesisAnalyzer"
						<< std::endl << desc << std::endl;
				return 0;
			}

			if (!vm.count("database")) {
				std::cout << "database option mandatory" << std::endl;
				std::cout << desc << std::endl;
				return 1;

			}

			po::notify(vm); // throws on error, so do after help in case
							// there are any problems
		} catch (po::error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return 1;
		}

		char * writable = new char[dbFilename.size() + 1];
		std::copy(dbFilename.begin(), dbFilename.end(), writable);
		writable[dbFilename.size()] = '\0'; // don't forget the terminating 0

		// don't forget to free the string after finished using it
		Database db(writable);
		delete[] writable;
		Timer timer;
		PhrStat phrStat("localhost", "7172", db, timer);
		boost::asio::io_service::work RunForever(io);
		boost::thread t(&PhrStat::defaultRun, &phrStat);
		boost::thread t2(&PhrStat::flushToDbSync, &phrStat);

		io.run();
		t.join();

	} catch (std::exception& e) {
		std::cerr << "Unhandled Exception reached the top of main: " << e.what()
				<< ", application will now exit" << std::endl;
		return 1;

	}

	return 0;
}

//
//
//#include <boost/asio.hpp>
//#include <boost/asio/time_traits.hpp>
//#include <boost/thread.hpp>
//#include <boost/bind.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
//#include <iostream>
//using namespace std;
//
//typedef boost::asio::time_traits<boost::posix_time::ptime> time_traits_t;
//int main() {
//	boost::asio::io_service Service;
//	boost::shared_ptr<boost::thread> Thread;
//	boost::asio::io_service::work RunForever(Service);
//	Thread = boost::shared_ptr<boost::thread>(
//			new boost::thread(
//					boost::bind(&boost::asio::io_service::run, &Service)));
//	boost::shared_ptr<boost::asio::deadline_timer> Timer(
//			new boost::asio::deadline_timer(Service));
//	while (1) {
//		boost::posix_time::time_duration Duration;
//		Duration = boost::posix_time::microseconds(45000);
//		boost::posix_time::ptime Start = time_traits_t::now();
//		boost::posix_time::ptime Deadline = Start + Duration;
//		boost::system::error_code Error;
//		size_t Result = Timer->expires_at(Deadline, Error);
//		cout << Result << ' ' << Error << ' ';
//		Timer->wait(Error);
//		cout << Error << ' ';
//		boost::posix_time::ptime End =
//				boost::posix_time::microsec_clock::local_time();
//		(cout << "Duration = " << (End - Start).total_milliseconds()
//				<< " milliseconds" << endl).flush();
//	}
//	return 0;
//}
//
