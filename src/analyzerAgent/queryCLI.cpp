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


#include "Client.hpp"

using boost::asio::ip::tcp;

template<class T>
T callFunction(std::string address, Request tm) {
	// IP et port de destination

	T answer;
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(tcp::v4(), address, "7171");
	boost::asio::ip::tcp::resolver::iterator iter;
	try {
		iter = resolver.resolve(query);
	} catch (boost::system::system_error & e) {
		throw std::runtime_error(e.what());
	}
	boost::asio::ip::tcp::resolver::iterator end; // End marker.
	boost::asio::ip::tcp::endpoint endpoint = *iter;
	while (iter != end) {
		endpoint = *iter++;
	}
	client<T> c(io_service, endpoint);

	boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

	c.write(tm);

	while (!c.m_receivedAnswer && !c.m_timeout)
		;
	if (c.m_timeout) {
		if (!c.m_receivedAnswer) {
			std::stringstream ss;
			ss << "Timeout for request " << tm;
			throw std::runtime_error(ss.str());
		}
	}

	answer = c.getAnswer();

	c.close(); // Ferme la connection
	t.join(); // On rejoint le thread

	return answer;

}
//
//int main(int argc, char ** argv) {
//	try {
//		std::string destination = argv[1];
//		std::map<std::string, bool> require;
//		require["cpu"] = true;
//		require["memory"] = true;
//		require["swap"] = true;
//		require["diskSpace"] = true;
//		require["inodes"] = true;
//		require["cpuConsumers"] = true;
//		require["memConsumers"] = true;
//		Request tm(Request::ENVIRONMENT, "", require);
//		envInfo env = callFunction<envInfo>(destination, tm);
//		std::cout << "In the main " << std::endl;
//		std::cout << env << std::endl;
////
////		require.clear();
////		require["cpu"] = true;
////		require["mem"] = false;
////		Request tm2(Request::PROCESS_NAME, "/usr/bin/dadada", require);
////		procInfo proc = callFunction<procInfo>("127.0.0.1", tm2);
////		std::cout << "In the main " << std::endl;
////		std::cout << proc << std::endl;
//
//		require.clear();
//		Request tm3(Request::PID_TREE, "/usr/sbin/hald", require);
//		pidTreeInfo pidTree = callFunction<pidTreeInfo>(destination, tm3);
//		std::cout << "In the main " << std::endl;
//		std::cout << pidTree << std::endl;
//
//		require.clear();
//		require["user"] = true;
//		require["cpu"] = true;
//		require["memory"] = true;
//		require["limits"] = true;
//		require["limitStatus"] = true;
//		Request tm4(Request::PROCESS_PID, "2407", require);
//		procInfo procI = callFunction<procInfo>(destination, tm4);
//		std::cout << "In the main " << std::endl;
//		std::cout << procI << std::endl;
//
//		require.clear();
//		require["owner"] = true;
//		require["group"] = true;
//		require["permissions"] = true;
//		require["attributes"] = true;
//		require["md5"] = true;
//		require["regex"] = true;
//		std::vector<std::string> regex;
//		regex.push_back(".*Dome JF.*");
//		regex.push_back("Dome JF");
//		regex.push_back("Dome JU");
//		regex.push_back("+3368897..27");
//		regex.push_back("\\+3368897..27");
//		regex.push_back("3368897..27");
//		//Request tm5(Request::FILE, "/home/chaen/dbData.db", require);
//		Request tm5(Request::FILE, "/home/chaen/Contacts.csv", require);
//		tm5.setRegex(regex);
//		fileInfo fileI = callFunction<fileInfo>(destination, tm5);
//		std::cout << "In the main " << std::endl;
//		std::cout << fileI << std::endl;
//		std::map<std::string, std::string>::const_iterator it;
//		for (it = fileI.m_runtimeErrors.begin();
//				it != fileI.m_runtimeErrors.end(); it++)
//			std::cout << "ERROR : " << it->first << " -> " << it->second
//					<< std::endl;
//
//		require.clear();
//		Request tm6(Request::MOUNTPOINT_PATH, "/usr/sbin/hald", require);
//		MountpointInfo mountpointI1 = callFunction<MountpointInfo>(destination,
//				tm6);
//		std::cout << "In the main " << std::endl;
//		std::cout << mountpointI1 << std::endl;
//
//		require.clear();
//		Request tm7(Request::MOUNTPOINT_PATH, "/home/chaen/ICINGA/tests",
//				require);
//		MountpointInfo mountpointI2 = callFunction<MountpointInfo>(destination,
//				tm7);
//		std::cout << "In the main " << std::endl;
//		std::cout << mountpointI2 << std::endl;
//
//		require.clear();
//		Request tm8(Request::MOUNTPOINT_PATH,
//				"/dfshome/chaen/Desktop/Thumbs.db", require);
//		MountpointInfo mountpointI3 = callFunction<MountpointInfo>(destination,
//				tm8);
//		std::cout << "In the main " << std::endl;
//		std::cout << mountpointI3 << std::endl;
//
//	} catch (std::exception& e) {
//		std::cerr << "Exception: " << e.what() << "\n";
//	}
//
//	return 0;
//}

void usage(std::string progName) {
	std::cout << "Usage : " << progName
			<< " -s <server> -t <queryType> -v <value>" << std::endl;
	std::cout << "<queryType> can be :" << std::endl;
	std::cout
			<< "\tenvironment : to get information about the environment of the server"
			<< std::endl;
	std::cout
			<< "\tpidtree : to get information about the process tree of a given process"
			<< std::endl;
	std::cout << "\tpid : to get information about a given pid" << std::endl;
	std::cout << "\tfile : to get information about a given file" << std::endl;
	std::cout << "\tmount : to get information about a given mountpoint"
			<< std::endl;

	exit(1);
}

int main(int argc, char ** argv) {
	std::string server, type, value;
	int c;
	int index;

	opterr = 0;

	while ((c = getopt(argc, argv, "s:t:v:")) != -1)
		switch (c) {
		case 's':
			server = optarg;
			break;
		case 't':
			type = optarg;
			break;
		case 'v':
			value = optarg;
			break;
		case '?':
			if ((optopt == 's') || (optopt == 't') || (optopt == 'v'))
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);

			usage(argv[0]);
			break;
		default:
			usage(argv[0]);
			break;
		}

	std::cout << "server " << server << " type " << type << " value " << value
			<< std::endl;

	std::map<std::string, bool> require;
	std::map<std::string, std::string> extraArgs;
	for (index = optind; index < argc; index++) {
		printf("Non-option argument %s\n", argv[index]);
		std::string arg = argv[index++];
		std::string val = argv[index];
		extraArgs[arg] = val;

	}

	if (!type.compare("environment")) {
		Request tm(Request::ENVIRONMENT, "");
		envInfo env = callFunction<envInfo>(server, tm);
		std::cout << "Environment " << std::endl;
		std::cout << env << std::endl;
	} else if (!type.compare("pidtree")) {
		Request tm(Request::PID_TREE, value);
		pidTreeInfo pidTree = callFunction<pidTreeInfo>(server, tm);
		std::cout << "pidTree" << std::endl;
		std::cout << pidTree << std::endl;
	} else if (!type.compare("pid")) {
		Request tm(Request::PROCESS_PID, value);
		procInfo procI = callFunction<procInfo>(server, tm);
		std::cout << "PID " << std::endl;
		std::cout << procI << std::endl;
	} else if (!type.compare("file")) {
		Request tm(Request::FILE, value);
		fileInfo fileI = callFunction<fileInfo>(server, tm);
		std::cout << "File " << std::endl;
		std::cout << fileI << std::endl;
	} else if (!type.compare("mount")) {
		Request tm(Request::MOUNTPOINT_PATH, value);
		tm.setExtraArguments(extraArgs);
		MountpointInfo mountpointI = callFunction<MountpointInfo>(server, tm);
		std::cout << "Mountpoint " << std::endl;
		std::cout << mountpointI << std::endl;
	} else if (!type.compare("alive")) {
		Request tm(Request::ALIVE, "");
		AliveMsg aliveI = callFunction<AliveMsg>(server, tm);
		std::cout << "Alive " << std::endl;
		std::cout << aliveI << std::endl;
	} else if (!type.compare("folder")) {
		Request tm(Request::FOLDER_CONTENT, value);
		tm.setExtraArguments(extraArgs);
		folderInfo folderI = callFunction<folderInfo>(server, tm);
		std::cout << "Folder content " << std::endl;
		std::cout << folderI << std::endl;
	} else {
		usage(argv[0]);
	}
	return 0;

}

