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


#include "phrUtils.hpp"
namespace po = boost::program_options;

int main(int argc, char ** argv) {
	std::string server = "127.0.0.1";
	std::string port = "7172";

	int mode = NONE;
	int id;
	std::string text;
	std::string vetoName;
	bool vetoValue;

	try {
		/** Define and parse the program options
		 */
		po::options_description desc("Options");
		desc.add_options()("help,h", "Print help messages")("observe,o",
				"List the questions")("answer,a",
				"Answer questions (requires -i and -t)")("id,i",
				po::value<int>(&id),
				"Id of the question to answer or message to acknowledge (needs -a or -m)")(
				"text,t", po::value<std::string>(&text), "Text of the answer")(
				"server,s", po::value<std::string>(&server),
				"Server to connect to (default localhost)")("port,p",
				po::value<std::string>(&port),
				"The port the server is listening to (default 7172)")("veto,v",
				"Use the veto mode. Without other options, it will list all the veto")(
				"veto-value,b", po::value<bool>(&vetoValue),
				"Value of the veto to set (requires -v)")("veto-name,n",
				po::value<std::string>(&vetoName),
				"name of the veto(requires -v). if -b is specified, this veto is set, otherwise it is read");

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

			if (vm.count("answer")) {

				if (mode == NONE) {
					mode = QUESTION;
					if ((!vm.count("id")) || (!vm.count("text"))) {
						std::cout << desc << std::endl;
						return 1;

					}
				} else {
					std::cout << desc << std::endl;
					return 1;
				}
			}

			if (vm.count("observe")) {

				if (mode == NONE) {
					mode = OBSERVE;
					if ((vm.count("id")) || (vm.count("text"))) {
						std::cout << desc << std::endl;
						return 1;
					}
				} else {
					std::cout << desc << std::endl;
					return 1;
				}
			}

			if (vm.count("veto")) {
				if (mode == NONE) {
					if ((vm.count("veto-value")) && (!vm.count("veto-name"))) {
						std::cout << desc << std::endl;
						return 1;
					}
					if ((vm.count("veto-value")) && (vm.count("veto-name"))) {
						mode = SET_VETO;
					} else {
						mode = GET_VETO;
					}

				} else {
					std::cout << desc << std::endl;
					return 1;
				}
			}

			po::notify(vm); // throws on error, so do after help in case
							// there are any problems
		} catch (po::error& e) {
			std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
			std::cerr << desc << std::endl;
			return 1;
		}

		std::cout << "My mode is " << mode << " and the options are " << id
				<< " " << text << std::endl;

		switch (mode) {
		case QUESTION: {
			answerQuestion c(id, text);
			c.run(server, port);
			break;
		}
		case OBSERVE: {
			phrUtilsInterface c(server, port, OBSERVE);
			//coutConsole c;
			c.defaultRun();
			break;
		}

		case GET_VETO: {
			phrUtilsInterface c(server, port, GET_VETO);
			c.defaultRun();
			break;
		}

		case SET_VETO: {
			phrUtilsInterface c(server, port, SET_VETO);
			c.setVeto(vetoName, vetoValue);
			break;
		}

		}

	} catch (std::exception& e) {
		std::cerr << "Unhandled Exception reached the top of main: " << e.what()
				<< ", application will now exit" << std::endl;
		return 1;

	}

}
