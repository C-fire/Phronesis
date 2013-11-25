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


#include <iostream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <vector>
#include <string>
#include <boost/algorithm/string/classification.hpp>
#include <sstream>
//#include "PhronesisAnalyzerClientApi.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/program_options.hpp>
#include "phronesisAnalyzerClientApi.hpp"
#include <list>

#include <signal.h>
#include <sys/stat.h>


using namespace std;
namespace po = boost::program_options;

class phrFileOutputApi: public AbstractPhronesisClient {
public:
	std::list<api_message> m_list;
	std::string m_outputFile;
	int m_bufferSize;

	std::vector<std::string> names;

	phrFileOutputApi(std::string outputFile, int bufferSize) :
			AbstractPhronesisClient() {
		m_outputFile = outputFile;
		m_bufferSize = bufferSize;

		names.push_back("NEW_MSG");
		names.push_back("NEW_LOG");
		names.push_back("NEW_QUESTION");
		names.push_back("NEW_YN_QUESTION");
		names.push_back("NEW_PB_QUESTION");
		names.push_back("ACK_MSG");
		names.push_back("ANSWER_QUESTION");
		names.push_back("CURRENT_PB");
		names.push_back("GIVE_PB");
		names.push_back("ERROR_TEXT");
	}

	void update(api_message e) {
		m_list.push_front(e);
		if (m_bufferSize != -1)
			while (m_list.size() > m_bufferSize)
				m_list.pop_back();
		dumpInFile();
	}

	void dumpInFile() {
		std::ofstream file;
		file.open(m_outputFile.c_str());
		file << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << std::endl;
		file << "<entries>" << std::endl;
		for (std::list<api_message>::iterator it = m_list.begin();
				it != m_list.end(); it++) {
			file << "<entry>" << std::endl;
			file << "\t<type>" << names[(*it).m_type] << "</type>" << std::endl;
			std::string escaped = (*it).m_text;
			boost::replace_all(escaped, "<", "&lt;");
			boost::replace_all(escaped, ">", "&gt;");
			file << "\t<text>" << escaped << "</text>" << std::endl;
			file << "\t<date>" << (*it).m_date << "</date>" << std::endl;
			file << "\t<time>" << (*it).m_time << "</time>" << std::endl;

			file << "</entry>" << std::endl;
			//file << *it << endl;
		}
		file << "</entries>" << std::endl;
		file.close();
	}

};

void usage(po::options_description & desc) {
	std::cout
			<< "Phronesis API to output the Core messages into a XML file buffer. The format is the following : "
			<< std::endl << "<entries>" << std::endl << "\t<entry>" << std::endl
			<< "\t\t<type></type>" << std::endl << "\t\t<text></text>"
			<< std::endl << "\t\t<date></date>" << std::endl
			<< "\t\t<time></time>" << std::endl << "\t</entry>" << std::endl
			<< "\t<entry>" << std::endl << "\t\t..." << std::endl
			<< "\t</entry>" << std::endl << "</entries>" << std::endl
			<< std::endl << desc << std::endl;
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
		std::cout << "SIGHUP" << std::endl;
		break;
	case SIGINT:
		/* 	Interrupt from keyboard  */
		std::cout << "SIGINT" << std::endl;
		exit(0);

		break;
	case SIGTERM:
		/* finalize the server */
		std::cout << "SIGTERM" << std::endl;
		exit(0);
		break;
	case SIGSTKFLT:
		std::cout << "TATATSDSDSDS" << std::endl;
		exit(0);
		break;
	}
}

void daemonize() {

	/* Our process ID and Session ID */
	pid_t pid, sid;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}
	/* If we got a good PID, then
	 we can exit the parent process. */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* Change the file mode mask */
	umask(0);

	/* Open any logs here */

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Change the current working directory */
	if ((chdir("/")) < 0) {
		/* Log the failure */
		exit(EXIT_FAILURE);
	}

	/* Close out the standard file descriptors */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int main(int argc, char ** argv) {

	signal(SIGHUP, Signal_Handler); /* hangup signal */
	signal(SIGTERM, Signal_Handler); /* software termination signal from kill */
	signal(SIGINT, Signal_Handler);
	signal(SIGSTKFLT, Signal_Handler); /* 	Interrupt from keyboard  */

	std::string outputFile;
	std::string server;
	std::string port = "7172";
	int bufferSize = 30;
	bool daemonify = false;

	try {
		/** Define and parse the program options
		 */
		po::options_description desc("Options");
		desc.add_options()("help,h", "Print help messages")("daemon,d",
				"Daemonize")("output,o", po::value<std::string>(&outputFile),
				"output file (mandatory)")("server,s",
				po::value<std::string>(&server),
				"Phronesis server address (mandatory)")("port,p",
				po::value<std::string>(&port),
				"Phronesis server interaction port (default 7172)")("buffer,b",
				po::value<int>(&bufferSize),
				"Size of the message buffer (default 30; unlimited -1)");

		po::variables_map vm;
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm); // can throw

			/** --help option
			 */
			if (vm.count("help")) {
				usage(desc);
				return 0;
			}

			if (vm.count("daemon")) {
				daemonify = true;
			}

			if (!vm.count("server")) {
				usage(desc);
				return 1;
			}

			if (!vm.count("output")) {
				usage(desc);
				return 1;
			}

			po::notify(vm); // throws on error, so do after help in case
							// there are any problems
		} catch (po::error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			usage(desc);
			return 1;
		}

		if (daemonify)
			daemonize();

		phrFileOutputApi phrApi(outputFile, bufferSize);
		boost::thread t(&phrFileOutputApi::run, &phrApi, server, port);
		while (1)
			sleep(1);
		phrApi.quit();
		t.join();

		return 0;

		// application code here //

	} catch (std::exception& e) {
		std::cerr << "Unhandled Exception reached the top of main: " << e.what()
				<< ", application will now exit" << std::endl;
		return 1;

	}

}

