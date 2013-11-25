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

using namespace std;
namespace po = boost::program_options;

class PhrApi: public AbstractPhronesisClient {
public:
	void update(api_message e) {
	}

};

class ApiProperties {
public:
	std::map<std::string, std::string> properties;
//     std::string db_server;
//     std::string db_user;
//     std::string db_passwd;
//     int db_port;
//     std::string db_name;
// 
//     std::string rpc_conn;

	ApiProperties(std::map<std::string, std::string> & defProps) {
		properties = defProps;
	}

	ApiProperties(std::string fileName) {
		read(fileName);
	}

	void read(std::string fileName) {
		std::ifstream myfile(fileName.c_str());
		std::string line;

		std::vector < std::string > tokens;
		if (myfile.is_open()) {
			while (myfile.good()) {
				getline(myfile, line);
				//std::cout << line << std::endl;
				boost::trim_left(line);
				if (line[0] == '#') {
					//std::cout << " is a comment " << std::endl;
					continue;
				}
				boost::split(tokens, line, boost::is_any_of("="));
				if (tokens.size() != 2) {
					//std::cout << " is not recognized" << std::endl;
					continue;
				}
//				std::cout << "Token 0 --" << tokens[0] << "-- token 1 --"
//						<< tokens[1] << "--" << std::endl;
				properties[tokens[0]] = tokens[1];
			}
			myfile.close();
		} else {
			throw std::runtime_error("Cannot open file " + fileName);
		}
	}

	template<class T>
	T get(std::string name) {
		std::map<std::string, std::string>::const_iterator it;
		it = properties.find(name);
		if (it == properties.end())
			throw std::runtime_error("No such property " + name);

		T ret;
		try {
			ret = boost::lexical_cast < T > (it->second);
		} catch (boost::bad_lexical_cast & e) {
			throw std::runtime_error(e.what());
		}
		return ret;
	}
};

int main(int argc, char ** argv) {
	string line;
	vector < string > strs;
	vector < string > pbs;
	std::string fileName;
	std::map < std::string, std::string > defProps;
	defProps["db_server"] = "localhost";
	defProps["db_user"] = "root";
	defProps["db_passwd"] = "";
	defProps["db_port"] = "3306";
	defProps["db_name"] = "phronesisAnalyzer";
	defProps["rpc_conn"] = "http://localhost:8080/RPC2";
	defProps["icinga_output_file"] = "/tmp/outputIcinga.csv";

	ApiProperties apiProp(defProps);
	try {
		/** Define and parse the program options
		 */
		po::options_description desc("Options");
		desc.add_options()("help,h", "Print help messages")("conf,c",
				po::value < std::string > (&fileName),
				"configuration file (mandatory)");

		po::variables_map vm;
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm); // can throw

			/** --help option
			 */
			if (vm.count("help")) {
				std::cout
						<< "Reads json output of icinga, and forwards it to phronesis"
						<< std::endl << desc << std::endl;
				return 0;
			}

			if (!vm.count("conf")) {
				//std::string fileName = vm["conf"].as<std::string>();
				//apiProp.read(fileName);
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

		apiProp.read(fileName);

		ifstream myfile(apiProp.get<std::string>("icinga_output_file").c_str());

//		try {
//			phrApi.connectDatabase(apiProp.get<std::string>("db_server"),
//					apiProp.get<std::string>("db_user"),
//					apiProp.get<std::string>("db_passwd"),
//					apiProp.get<int>("db_port"),
//					apiProp.get<std::string>("db_name"));
//
//			PhrApi phrApi;
//			phrApi.run("localhost","7172");
//			//phrApi.connectRPC(apiProp.get<std::string>("rpc_conn"));
//		} catch (std::exception & e) {
//			std::cout << "Error at the connection : " << e.what() << std::endl;
//			return 1;
//		}

		if (myfile.is_open()) {
			while (myfile.good()) {
				getline(myfile, line);
				boost::split(strs, line, boost::is_any_of(";"));
				if (strs.size() == 7 and strs[0].compare("'Host'")) {
					string serviceName = strs[1];
					serviceName.erase(serviceName.begin());
					string::iterator end = serviceName.end();
					end--;

					serviceName.erase(end);
					pbs.push_back(serviceName);
				}
			}
			myfile.close();
		}

		else {
			cout << "Unable to open file";
			exit(1);
		}

		if (pbs.size()) {
			std::stringstream ss;
			for (size_t i = 0; i < pbs.size(); ++i) {
				ss << pbs[i];
				ss << ";";
			}
			std::string s = ss.str();
			std::cout << s << std::endl;
//			phrApi.giveProblemList(s);
			PhrApi phrApi;
			boost::thread t(&PhrApi::run, &phrApi, std::string("127.0.0.1"), std::string("7172"));
			sleep(1);
			if (!phrApi.isConnected()) {
				std::cout << "Could not connect" << std::endl;
			} else {
				phrApi.giveProblemList(s);
			}
			phrApi.quit();
			t.join();

		} else {
			std::cout << "No problem to report " << std::endl;
		}
		return 0;

		// application code here //

	} catch (std::exception& e) {
		std::cerr << "Unhandled Exception reached the top of main: " << e.what()
				<< ", application will now exit" << std::endl;
		return 1;

	}

}

