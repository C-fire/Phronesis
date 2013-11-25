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


#include "AnalyzerEngine.hpp"
#include "AnalyzerDBAPI.hpp"
#include "Configuration.hpp"
#include "InteractionManager.hpp"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <cstdlib>
#include <signal.h>
#include <cassert>
#include <unistd.h>
#include <stdexcept>
#include <syslog.h>
#include <stdlib.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <sys/stat.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
bool stopReceived = false;

///*!
// * @brief This structure contains information and data to start the threads
// */
//struct thread_info /* Used as argument to thread_start() */
//{ /*@{*/
//	SyncManager * syncManager; /*!< pointer to the SyncManager */
//	AnalyzerDBAPI * dbApi; /*!< pointer to the AnalyzerDBAPI */
//	InteractionManager * iManager; /*!< pointer to the InteractionManager */
//	bool checkAll; /*!< boolean set to true if we want to check all Agents. False otherwise */
//	char * pbList; /*!< list of problem to analyze given in the command line */
//	/*@}*/
//};

/*!
 * @brief demangle the c++ method names
 *
 * Used to have a human readable name of the
 * methods when catching the SIGSEGV signal
 *
 * @param symbol Symbol to demangle.
 * @return Human readable string
 */
std::string demangle(const char* symbol) {
	size_t size;
	int status;
	char temp[128];
	char* demangled;
	//first, try to demangle a c++ name
	if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp)) {
		if (NULL
				!= (demangled = abi::__cxa_demangle(temp, NULL, &size, &status))) {
			std::string result(demangled);
			free(demangled);
			return result;
		}
	}
	//if that didn't work, try to get a regular c symbol
	if (1 == sscanf(symbol, "%127s", temp)) {
		return temp;
	}

	//if all else fails, just return the symbol
	return symbol;
}

/*!
 * @brief Daemonize the process
 * @return 0 if everything went well, something else otherwise
 */
int daemonize() {

	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		return pid;
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	 we can exit the parent process. */
	if (pid > 0) {
		return pid;
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);

	/* Open any logs here */

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		return pid;
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		return pid;
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	// We don't close them because the cout observer is not
	// started if not needed. This avoids potential segfault
	// if there are still some places where "cout" is used.
//	close(STDIN_FILENO);
//	close(STDOUT_FILENO);
//	close(STDERR_FILENO);
	return 0;
}

/*!
 * @brief Redefines the signal handlers
 *
 *	When SIGINT is received, we set the boolean stopReceived to true, which will exit all thread gracefully.
 * @param sig signal number
 */
void Signal_Handler(int sig) /* signal handler function */
{
	switch (sig) {
	case SIGHUP:
		/* rehash the server */
		break;
	case SIGINT:
		/* 	Interrupt from keyboard  */
		stopReceived = true;
		break;
	case SIGTERM:
		/* finalize the server */
		exit(0);
		break;
	case SIGSTKFLT:
		exit(0);
		break;

// Handle the SIGSEGV signal.
// But we should rather let it die to have a coredump
//	case SIGSEGV: {
//		void* addresses[50];
//		int size;
//		size = backtrace(addresses, 50);
//		char** symbols = backtrace_symbols(addresses, size);
//		int x;
//		for (x = 0; x < size; ++x) {
//			printf("%s\n", demangle(symbols[x]).c_str());
//		}
//		free(symbols);
//		exit(1);
//		break;
//	}
	}
}

int main(int argc, char ** argv) {

	signal(SIGHUP, Signal_Handler); /* hangup signal */
	signal(SIGTERM, Signal_Handler); /* software termination signal from kill */
	signal(SIGINT, Signal_Handler);
	signal(SIGSTKFLT, Signal_Handler); /* 	Interrupt from keyboard  */
//		signal(SIGSEGV, Signal_Handler);

	std::map<std::string, std::string> defaultProperties;
	defaultProperties["agent_port"] = "7171";
	defaultProperties["interaction_port"] = "7172";
	defaultProperties["agent_timeout"] = "20";
	defaultProperties["db_host"] = "localhost";
	defaultProperties["db_user"] = "root";
	defaultProperties["db_passwd"] = "";
	defaultProperties["db_port"] = "3306";
	defaultProperties["db_name"] = "phronesisAnalyzer";
	defaultProperties["dependency_validation_threshold"] = "15";
	defaultProperties["use_syslog"] = "1";
	defaultProperties["coordinator_sort_algo"] = "ALWAYS_BEST";
	defaultProperties["rand_seed"] = "-1";

	Configuration phrProperties(defaultProperties);

	bool useSyslog = true; // Flag to use syslog

	bool checkAll = false; // boolean set to true if we want to check all Agents. False otherwise
	bool fullCheck = false; // flag to do a full check of all the Agents
	bool problemGiven = false; // flag to true if problems were given on the command line
	bool daemon = false; //flag to true if we want to run as a daemon

	std::string configurationFile; // path of the configuration file
	std::string problemList; // list of problems

	// Read the options
	try {
		/** Define and parse the program options
		 */
		po::options_description desc("Options");
		desc.add_options()("help,h", "Print help messages")("configuration,c",
				po::value<std::string>(&configurationFile),
				"configuration file (mandatory)")("list,l",
				po::value<std::string>(&problemList),
				"Give a list of problem to analyze")("all,a",
				"Check all the coordinators")("full,f", "Check all the agents")(
				"daemon,d", "Run the process as a daemon");

		po::variables_map vm;
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm); // can throw

			po::notify(vm); // throws on error, so do after help in case
							// there are any problems

			/** --help option
			 */
			if (vm.count("help")) {
				std::cout << "PhronesisAnalyzerCore help message" << std::endl
						<< desc << std::endl;
				return 0;
			}

			if (!vm.count("configuration")) {
				std::cout << "Giving a configuration file is mandatory"
						<< std::endl;
				std::cout << desc << std::endl;
				return 1;
			}

			if (vm.count("daemon")) {
				daemon = true;
			}

			if (vm.count("all")) {
				checkAll = true;
			}

			if (vm.count("full")) {
				fullCheck = true;
			}

			if (vm.count("list")) {
				problemGiven = true;
			}

			// If there is more than 1 of the exclusive options given (all, full, list), I stop
			// This boolean express the fact that you want to stop if
			// ( (all of them are set) OR ( the non xor) ) AND (at least one of them is selected)
			// a | b | c || y
			// 0 | 0 | 0 || 0
			// 0 | 0 | 1 || 0
			// 0 | 1 | 0 || 0
			// 0 | 1 | 1 || 1
			// 1 | 0 | 0 || 0
			// 1 | 0 | 1 || 1
			// 1 | 1 | 0 || 1
			// 1 | 1 | 1 || 1

			if (((fullCheck && checkAll && problemGiven)
					|| ((!(fullCheck ^ checkAll ^ problemGiven))))
					&& (fullCheck || checkAll || problemGiven)) {
				std::cout
						<< "Option --all, --full, and --list are mutually exclusive"
						<< std::endl;
				std::cout << desc << std::endl;
				return 1;

			}

			// If we want to run as a daemon, we should not have the options all, full or list set
			if (daemon) {
				if (fullCheck || checkAll || problemGiven) {
					std::cout
							<< "Option --all, --full, and --list are incompatible with the use of the --daemon option"
							<< std::endl;
					std::cout << desc << std::endl;
					return 1;
				}

			}

		} catch (po::error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return 1;
		}

		// Read the properties in the file
		phrProperties.read(configurationFile);

		// Set the random seed. If none is given, take a random random seed
		int rndSeed = phrProperties.get<int>("rand_seed");
		if (rndSeed == -1) {
			srand(time(NULL));
		} else {
			srand(rndSeed);
		}

		// Set the global values
		MetaAgent::agent_port = phrProperties.get<std::string>("agent_port");
		MetaAgent::agent_timeout = phrProperties.get<int>("agent_timeout");
		RuleManager::dependency_validation_threshold = phrProperties.get<int>(
				"dependency_validation_threshold");

		// Set the sorting algorithm pointer
		std::string sortAlgo = phrProperties.get<std::string>(
				"coordinator_sort_algo");
		if (sortAlgo.compare("ALWAYS_BEST") == 0) {
			Coordinator::sortAlgo = Coordinator::ALWAYS_BEST;
		} else if (sortAlgo.compare("WEIGHTED_RND") == 0) {
			Coordinator::sortAlgo = Coordinator::WEIGHTED_RND;
		} else {
			std::cout << "Unknown coordinator_sort_algo value " << sortAlgo
					<< std::endl;
			return 1;
		}

		useSyslog = phrProperties.get<int>("use_syslog");

		// If we want to daemonize, do it
		if (daemon) {
			if (daemonize()) {
				return 0;
			}
		}

		// I think I should move this to the syslog observer....
		openlog("phrAnalyzerCore", LOG_PID | LOG_CONS, LOG_USER);
		syslog(LOG_INFO, "Phronesis Analyzer Core V 1.0 started");

		SyncManager syncManager;

		// Create the database API
		AnalyzerDBAPI dbApi("mysql", phrProperties.get<std::string>("db_host"),
				phrProperties.get<std::string>("db_user"),
				phrProperties.get<std::string>("db_passwd"),
				phrProperties.get<int>("db_port"),
				phrProperties.get<std::string>("db_name"));

		// Create the InteractionManager
		InteractionManager iManager(&syncManager);

		// Create the AnalyzerEngine
		AnalyzerEngine analyzerEngine(&dbApi, &iManager);

		// Start the InteractionManager thread
		boost::thread iManagerThread(&InteractionManager::run, &iManager);

		// Create the io_service, the endpoint and the api_server for the api connection
		boost::asio::io_service io_service;
		tcp::endpoint endpoint(tcp::v4(),
				phrProperties.get<int>("interaction_port"));
		api_server server(io_service, endpoint, iManager);

		// Start the thread of the io_service
		boost::thread apiThread(
				boost::bind(&boost::asio::io_service::run, &io_service));

		// Create the coutObserver
		CoutObserver coutObserver(iManager);
		boost::thread coutObserverThread;
		// Connect it and run it if we are not daemonize
		if (!daemon) {

			coutObserver.connect();
			coutObserverThread = boost::thread(&CoutObserver::run,
					&coutObserver);
		}

		// Create the syslogObserver
		SyslogObserver syslogObserver(iManager);
		boost::thread syslogObserverThread;

		// Connect it and run it if the flag is set
		if (useSyslog) {

			syslogObserver.connect();
			syslogObserverThread = boost::thread(&SyslogObserver::run,
					&syslogObserver);
		}

		// If checkAll, we run the checkAll method of the analyzer engine in the thread, and exit
		if (checkAll) {

			boost::thread analyzerEngineThread(&AnalyzerEngine::checkAll,
					&analyzerEngine);
			analyzerEngineThread.join();
			stopReceived = true;
		} else if (fullCheck) { // If fullCheck, we run the fullCheck method of the analyzer engine in the thread, and exit

			boost::thread analyzerEngineThread(&AnalyzerEngine::fullCheck,
					&analyzerEngine);
			analyzerEngineThread.join();
			stopReceived = true;
		} else if (problemList.size()) { // If problems were given, we start the analyzerEngine with this problem list
			boost::thread analyzerEngineThread(&AnalyzerEngine::analyze,
					&analyzerEngine, problemList);
			analyzerEngineThread.join();
			stopReceived = true;
		} else { //Otherwise we start the normal running of infinit waiting
			boost::thread analyzerEngineThread(&AnalyzerEngine::run,
					&analyzerEngine);
			analyzerEngineThread.join();
		}

		// Stop the io_service thread and the InteractionManager thread
		io_service.stop();
		iManagerThread.join();

		//If not running as a daemon, stop the cout observer thread
		if (!daemon) {
			coutObserverThread.join();
		}
		//If we were using it, stop the syslog observer thread
		if (useSyslog) {
			syslogObserverThread.join();
		}

		return 0;

	} catch (std::exception& e) {
		std::cerr << "Unhandled Exception reached the top of main: " << e.what()
				<< ", application will now exit" << std::endl;
		return 02;

	}

}
