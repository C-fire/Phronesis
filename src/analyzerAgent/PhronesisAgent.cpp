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


#include "PhronesisAgent.hpp"

PhronesisAgent::PhronesisAgent(boost::asio::io_service& io_service,
		const tcp::endpoint& endpoint) :
		m_io_service(io_service), m_acceptor(io_service, endpoint) {
	std::cout << "Creation d'un serveur " << std::endl;

	wait_for_connection();
}

void PhronesisAgent::wait_for_connection() {
	connection_ptr new_connection(new tcp_connection(m_io_service));

	m_acceptor.async_accept(new_connection->socket(),
			boost::bind(&PhronesisAgent::handle_accept, this,
					boost::asio::placeholders::error, new_connection));
}

void PhronesisAgent::handle_accept(const boost::system::error_code& error,
		connection_ptr new_connection) {
	if (!error) {
		std::string s =
				new_connection.get()->socket().remote_endpoint().address().to_string();
		std::cout << "Accepted connection from " << s << std::endl;
		Treatment_ptr session = Treatment::create(new_connection);

		//Shall we call wait_for_connection even if there is an error?
		wait_for_connection();
	} else {
		std::cerr << "Connection refusee" << std::endl;
	}
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

	std::map<std::string, std::string> defaultProperties;
	defaultProperties["agent_port"] = "7171";
	defaultProperties["command_timeout"] = "20";
	defaultProperties["use_syslog"] = "1";

	Configuration phrProperties(defaultProperties);

	std::string configurationFile;
	bool runInteractive = false;

	try {
		/** Define and parse the program options
		 */
		po::options_description desc("Options");
		desc.add_options()("help,h", "Print help messages")("configuration,c",
				po::value<std::string>(&configurationFile),
				"configuration file (mandatory)")("interactive,i",
				"Run interactively (not as a daemon)");

		po::variables_map vm;
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm); // can throw

			po::notify(vm); // throws on error, so do after help in case
							// there are any problems
			/** --help option
			 */
			if (vm.count("help")) {
				std::cout << "PhronesisAgent help message" << std::endl << desc
						<< std::endl;
				return 0;
			}

			if (!vm.count("configuration")) {
				std::cout << desc << std::endl;
				return 1;
			}

			if (vm.count("interactive"))
				runInteractive = true;

		} catch (po::error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return 1;
		}

		phrProperties.read(configurationFile);
		Treatment::command_timeout = phrProperties.get<int>("command_timeout");

		if (!runInteractive)
			daemonize();

		try {
			boost::asio::io_service io_service;
			tcp::endpoint endpoint(tcp::v4(),
					phrProperties.get<int>("agent_port"));
			PhronesisAgent server(io_service, endpoint);

			io_service.run();
		} catch (std::exception& e) {
			std::cerr << "Exception: " << e.what() << "\n";
		}

	} catch (std::exception& e) {
		std::cerr << "Unhandled Exception reached the top of main: " << e.what()
				<< ", application will now exit" << std::endl;
		return 1;

	}
}
