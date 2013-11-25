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


#include "Treatment.hpp"

int Treatment::command_timeout;

Treatment::Treatment(connection_ptr tcp_connection) :
		m_tcp_connection(tcp_connection) {
	is_leaving = false;
	m_cpuinfo = "/proc/cpuinfo";
	m_loadavg = "/proc/loadavg";
	m_meminfo = "/proc/meminfo";
}

Treatment_ptr Treatment::create(connection_ptr tcp_connection) {
	std::cout << "Treatment create" << std::endl;
	Treatment_ptr session(new Treatment(tcp_connection));
	session->wait_for_data();
	std::cout << "Treatment create about to return" << std::endl;
	return session;
}

Treatment::~Treatment() {
	std::cout << "Session detruite" << std::endl;
}

void Treatment::wait_for_data() {
	std::cout << "Treatment wait for data " << std::endl;

	m_tcp_connection->async_read(m_request,
			boost::bind(&Treatment::handle_read, shared_from_this(),
					boost::asio::placeholders::error));
}

template<typename RET, typename ARG1, typename ARG2>
void Treatment::call(RET (Treatment::*f)(ARG1&, ARG2&), ARG1 & arg1,
		ARG2 & arg2, std::string errorKey, RET& ret,
		std::map<std::string, std::string> & errorMap) {
	try {
		ret = (this->*f)(arg1, arg2);
	} catch (std::exception &e) {
		errorMap[errorKey] = e.what();
	} catch (...) {
		errorMap[errorKey] = "Caught an unknown exception.";
	}
}

template<typename RET, typename ARG>
void Treatment::call(RET (Treatment::*f)(ARG&), ARG & arg, std::string errorKey,
		RET& ret, std::map<std::string, std::string> & errorMap) {
	try {
		ret = (this->*f)(arg);
	} catch (std::exception &e) {
		errorMap[errorKey] = e.what();
	} catch (...) {
		errorMap[errorKey] = "Caught an unknown exception.";
	}
}

template<typename RET>
void Treatment::call(RET (Treatment::*f)(), std::string errorKey, RET& ret,
		std::map<std::string, std::string> & errorMap) {
	try {
		ret = (this->*f)();
	} catch (std::exception &e) {
		errorMap[errorKey] = e.what();
	} catch (...) {
		errorMap[errorKey] = "Caught an unknown exception.";
	}
}

void Treatment::handle_read(const boost::system::error_code &error) {
	std::cout << "Entering treatment handle_read" << std::endl;
	if (!error) {

		bool fetchEverything = m_request.getFetchEverything();
		std::map<std::string, bool> req = m_request.getRequire();
		switch (m_request.getType()) {
		case Request::ALIVE: {
			AliveMsg alive;
			alive.m_msg = "Who's there?";
			m_tcp_connection->async_write(alive,
					boost::bind(&Treatment::handle_write, shared_from_this(),
							boost::asio::placeholders::error));

			break;
		}
		case Request::ENVIRONMENT: {
			std::cout << "I am asked things about environment" << std::endl;
			envInfo envI;
			//std::map<std::string, bool> req = m_request.getRequire();

			/*if (req["cpu"])
			 envI.m_cpu = env_getLoad();
			 if (req["memory"])
			 envI.m_mem = env_getUsedMemory();
			 if (req["swap"])
			 envI.m_swap = env_getUsedSwap();
			 if (req["diskSpace"])
			 envI.m_diskSpace = env_getUsedDiskSpace();
			 if (req["diskInode"])
			 envI.m_inodes = env_getUsedInodes();
			 if (req["cpuConsumers"])
			 envI.m_cpuConsumers = env_getTopCpuConsumers();
			 if (req["memConsumers"])
			 envI.m_memConsumers = env_getTopMemoryConsumers();
			 */

			if (fetchEverything || req["cpu"])
				call<float>(&Treatment::env_getLoad, "cpu", envI.m_cpu,
						envI.m_runtimeErrors);
			if (fetchEverything || (req["memory"]))
				call<float>(&Treatment::env_getUsedMemory, "memory", envI.m_mem,
						envI.m_runtimeErrors);
			if (fetchEverything || (req["swap"]))
				call<float>(&Treatment::env_getUsedSwap, "swap", envI.m_swap,
						envI.m_runtimeErrors);
			if (fetchEverything || (req["diskSpace"]))
				call<std::map<std::string, float> >(
						&Treatment::env_getUsedDiskSpace, "diskSpace",
						envI.m_diskSpace, envI.m_runtimeErrors);
			if (fetchEverything || (req["inodes"]))
				call<std::map<std::string, float> >(
						&Treatment::env_getUsedInodes, "diskInode",
						envI.m_inodes, envI.m_runtimeErrors);
			if (fetchEverything || (req["cpuConsumers"]))
				call<std::vector<triplet<std::string, float, std::string> > >(
						&Treatment::env_getTopCpuConsumers, "cpuConsumers",
						envI.m_cpuConsumers, envI.m_runtimeErrors);
			if (fetchEverything || (req["memConsumers"]))
				call<std::vector<triplet<std::string, float, std::string> > >(
						&Treatment::env_getTopMemoryConsumers, "memConsumers",
						envI.m_memConsumers, envI.m_runtimeErrors);
			std::cout << "I will send " << std::endl;
			std::cout << envI << std::endl;

			m_tcp_connection->async_write(envI,
					boost::bind(&Treatment::handle_write, shared_from_this(),
							boost::asio::placeholders::error));
			break;
		}
			/*case Request::PROCESS_NAME: {
			 std::cout << "I am asked things about process from its name"
			 << std::endl;
			 procInfo procI;
			 procI.m_cpu = 3.5;

			 m_tcp_connection->async_write(
			 procI,
			 boost::bind(&Treatment::handle_write, shared_from_this(),
			 boost::asio::placeholders::error));

			 break;

			 }*/
		case Request::PID_TREE: {
			std::cout << "I am asked the pid tree of " << m_request.getTarget()
					<< std::endl;
			pidTreeInfo pidTreeI;
			std::string pid = m_request.getTarget();
			call<std::map<int, std::vector<int> >, std::string>(
					&Treatment::proc_getPidTree, pid, "pidTree",
					pidTreeI.m_pidTree, pidTreeI.m_runtimeErrors);
			m_tcp_connection->async_write(pidTreeI,
					boost::bind(&Treatment::handle_write, shared_from_this(),
							boost::asio::placeholders::error));
			break;
		}

		case Request::MOUNTPOINT_PATH: {
			std::cout << "I am asked mountpoint about file "
					<< m_request.getTarget() << std::endl;
			MountpointInfo mountpointI;
			std::string fileName = m_request.getTarget();
			call<MountpointInfo, std::string>(
					&Treatment::mount_CheckMountPointForFile, fileName,
					"mountpoint", mountpointI, mountpointI.m_runtimeErrors);

			call<bool, MountpointInfo>(&Treatment::mount_isMountpointMounted,
					mountpointI, "isMounted", mountpointI.m_mounted,
					mountpointI.m_runtimeErrors);

			if (fetchEverything || req["isRw"]) {
				std::string uid = "0";
				if (m_request.hasExtraArgument("uid")) {
					uid = m_request.getExtraArgument("uid");
				}
				call<bool, MountpointInfo, std::string>(
						&Treatment::mount_isMountPointReadWriteForUser,
						mountpointI, uid, "isRw", mountpointI.m_isRw,
						mountpointI.m_runtimeErrors);
			}

			m_tcp_connection->async_write(mountpointI,
					boost::bind(&Treatment::handle_write, shared_from_this(),
							boost::asio::placeholders::error));
			break;
		}
		case Request::PROCESS_PID: {
			std::cout << "I am asked things about process from its PID "
					<< std::endl;
			procInfo procI;
			//std::map<std::string, bool> req = m_request.getRequire();
			std::string pid = m_request.getTarget();
			/*if (req["user"])
			 procI.m_user = proc_getProcessUser(pid);
			 if (req["cpu"])
			 procI.m_cpu = proc_getProcessCpuUsage(pid);
			 if (req["memory"])
			 procI.m_mem = proc_getProcessMemoryUsage(pid);
			 if (req["limits"])
			 procI.m_limits = proc_getProcessLimits(pid);
			 if (req["limitStatus"])
			 procI.m_limitStatus = proc_getProcessLimitStatus(pid);
			 */

			if (fetchEverything || (req["cmdLine"])) {
				call<std::string, std::string>(
						&Treatment::proc_getFullCommandLine, pid, "cmdLine",
						procI.m_cmdLine, procI.m_runtimeErrors);
			}
			if (fetchEverything || (req["user"]))
				call<std::string, std::string>(&Treatment::proc_getProcessUser,
						pid, "user", procI.m_user, procI.m_runtimeErrors);
			//procI.m_user = proc_getProcessUser(pid);
			if (fetchEverything || (req["cpu"]))
				call<float, std::string>(&Treatment::proc_getProcessCpuUsage,
						pid, "cpu", procI.m_cpu, procI.m_runtimeErrors);
			if (fetchEverything || (req["memory"]))
				call<float, std::string>(&Treatment::proc_getProcessMemoryUsage,
						pid, "memory", procI.m_mem, procI.m_runtimeErrors);
			if (fetchEverything || (req["limits"]))
				call<
						std::map<std::string,
								std::pair<unsigned long int, unsigned long int> >,
						std::string>(&Treatment::proc_getProcessLimits, pid,
						"limits", procI.m_limits, procI.m_runtimeErrors);
			if (fetchEverything || (req["limitStatus"]))
				call<std::map<std::string, unsigned long int>, std::string>(
						&Treatment::proc_getProcessLimitStatus, pid,
						"limitStatus", procI.m_limitStatus,
						procI.m_runtimeErrors);

			m_tcp_connection->async_write(procI,
					boost::bind(&Treatment::handle_write, shared_from_this(),
							boost::asio::placeholders::error));
			break;
		}

		case Request::FILE: {
			std::cout << "I am asked things about file "
					<< m_request.getTarget() << std::endl;
			fileInfo fileI;
			std::string fileName = m_request.getTarget();

			fileI.m_fileExists = file_fileExists(fileName);

			/*if (req["owner"]){
			 try{
			 fileI.m_owner = file_getFileOwner(fileName);
			 }catch (std::exception &e) {
			 fileI.m_runtimeErrors["owner"] = e.what ();
			 } catch (...) {
			 fileI.m_runtimeErrors["owner"] = "Caught an unknown exception.";
			 }
			 }*/
			if (fetchEverything || (req["owner"]))
				call<std::string, std::string>(&Treatment::file_getFileOwner,
						fileName, std::string("owner"), fileI.m_owner,
						fileI.m_runtimeErrors);
			/*if (req["group"])
			 fileI.m_group = file_getFileGroup(fileName);
			 if (req["permissions"])
			 fileI.m_permissions = file_getFilePermissions(fileName);
			 if (req["attributes"])
			 fileI.m_attributes = file_getFileAttributes(fileName);
			 if (req["md5"])
			 fileI.m_md5 = file_getFileMd5(fileName);
			 */

			if (fetchEverything || (req["group"]))
				call<std::string, std::string>(&Treatment::file_getFileGroup,
						fileName, std::string("group"), fileI.m_group,
						fileI.m_runtimeErrors);
			if (fetchEverything || (req["permissions"]))
				call<std::string, std::string>(
						&Treatment::file_getFilePermissions, fileName,
						std::string("permissions"), fileI.m_permissions,
						fileI.m_runtimeErrors);
			if (fetchEverything || (req["attributes"]))
				call<std::string, std::string>(
						&Treatment::file_getFileAttributes, fileName,
						std::string("attributes"), fileI.m_attributes,
						fileI.m_runtimeErrors);
			if (fetchEverything || (req["md5"]))
				call<std::string, std::string>(&Treatment::file_getFileMd5,
						fileName, std::string("md5"), fileI.m_md5,
						fileI.m_runtimeErrors);
			if (fetchEverything || (req["regex"])) {
				std::vector<std::string> regex = m_request.getRegex();
				call<std::map<std::string, bool>, std::string,
						std::vector<std::string> >(
						&Treatment::file_getFileRegex, fileName, regex,
						std::string("regex"), fileI.m_regex,
						fileI.m_runtimeErrors);
			}
			m_tcp_connection->async_write(fileI,
					boost::bind(&Treatment::handle_write, shared_from_this(),
							boost::asio::placeholders::error));

			break;
		}
		case Request::FOLDER_CONTENT: {
			std::cout << "I am asked content about folder"
					<< m_request.getTarget() << std::endl;
			folderInfo folderI;
			std::string folderName = m_request.getTarget();
//			call<std::vector<std::string>, std::string>(
//					&Treatment::folder_getFolderContent, folderName, "content",
//					folderI.m_fileList, folderI.m_runtimeErrors);
			std::map<std::string, std::string> extraArgs =
					m_request.getExtraArgumentMap();
			call<std::vector<std::string>, std::string,
					std::map<std::string, std::string> >(
					&Treatment::folder_getFolderContent, folderName, extraArgs,
					std::string("content"), folderI.m_fileList,
					folderI.m_runtimeErrors);

//
//			std::cout << "Before sending " << std::endl;
//			for (std::vector<std::string>::const_iterator bla = folderI.m_fileList.begin(); bla != folderI.m_fileList.begin(); bla++){
//				std::cout << *bla << std::endl;
//			}

			m_tcp_connection->async_write(folderI,
					boost::bind(&Treatment::handle_write, shared_from_this(),
							boost::asio::placeholders::error));
			break;
		}
		default: {
			std::cout << "You talking to me??" << std::endl;
			break;
		}
		}

	} else {
		std::cout << "Error in handle_read "
				<< error.category().message(error.value()) << std::endl;
		if (!is_leaving) {
			is_leaving = true;
		}
	}

}

// void Treatment::deliver(const chat_message& msg)
// {
//     m_tcp_connection->async_write(msg,
//         boost::bind(&Treatment::handle_write, shared_from_this(),
//         boost::asio::placeholders::error)
//         );
// }

void Treatment::handle_write(const boost::system::error_code &error) {
	if (error && (!is_leaving)) {
		is_leaving = true;
	}

}
//
//void Treatment::CommandThreadChild(std::string & ret, std::string exec) {
//	std::cout << "In threadFunction2 Apres " << ret << std::endl;
//	bp::context ctx;
//	ctx.stdout_behavior = bp::capture_stream();
//	bp::child c = bp::launch_shell(exec, ctx);
//
//	bp::pistream &is = c.get_stdout();
//	std::ostringstream ss;
//	std::string line;
//	while (std::getline(is, line)) {
//		std::cout << "LINE " << line.size() << " " << line << std::endl;
//		ss << line << std::endl;
//	}
//
//	std::cout << "The stream " << ss.str() << std::endl;
//	ret = ss.str();
//	std::cout << "In threadFunction2 Apres " << ret << std::endl;
//
//}

void Treatment::CommandThreadChild(std::string & ret, std::string exec) {
	m_threadIdMutex.lock();
	m_threadIdMutex.unlock();
	std::ostringstream ss;
	FILE *in;
	char buff[16384];

	if (!(in = popen(exec.c_str(), "r"))) {
		throw std::runtime_error("Could not open pipe for output.");
		;
	}

	while (fgets(buff, sizeof(buff), in) != NULL) {
		ss << buff << std::endl;
	}
	pclose(in);
	// 2013-04-10
	// it sometimes happen that a CommandThreadChild does not exit
	// before the next one works. This provocates segfault
	// this test makes sure that only the current 'legitim' thread writes
	if (boost::this_thread::get_id() == m_currentCommandThreadId)
		ret = ss.str();

}

// The function that will be run by the thread
void Treatment::CommandThread(std::string & ret, std::string exec) {
	m_threadIdMutex.lock();
	boost::thread t(&Treatment::CommandThreadChild, this, boost::ref(ret),
			exec);
	m_currentCommandThreadId = t.get_id();
	m_threadIdMutex.unlock();
	// Never ending loop. Normally the thread will never finish
	while (t.timed_join(boost::posix_time::seconds(1)) == false) {
		try {
			// Interrupt can only occur in wait/sleep or join operation.
			// If you don't do that, call interuption_point().
			// Remove this line, and the thread will never be interrupted.
			boost::this_thread::interruption_point();
		} catch (const boost::thread_interrupted&) {
			// Thread interruption request received, break the loop
			std::cout << "- Thread interrupted. Exiting thread." << std::endl;
			break;
		}
	}
	// 2013-04-10
	// it seems that sometimes the CommandThreadChild was not exciting properly
	// this should help
	t.detach();
}

std::string Treatment::executeCommand(std::string command) {
	std::string retStr;
	boost::thread t(&Treatment::CommandThread, this, boost::ref(retStr),
			command);
	std::cout << "Wait for 2 seconds for the thread to stop." << std::endl;
	while (t.timed_join(boost::posix_time::seconds(Treatment::command_timeout))
			== false) {
		// Interupt the thread
		std::cout << "Thread not stopped, interrupt it now." << std::endl;
		t.interrupt();
		std::cout
				<< "Thread interrupt request sent. Wait to finish for 2 seconds again."
				<< std::endl;
		throw std::runtime_error("Thread interrupted.");
	}

	return retStr;

}

bool areCharsSpaceAndEqual(char a, char b) {
	return ((((a == ' ') && (b == ' ')) || ((a == '\t') && (b == '\t')))
			&& (a == b));
}

std::string Treatment::remove_excessive_spaces(const std::string& s) {
	std::string result;
	//consider consequtive chars equal only if they are both spaces.
	unique_copy(s.begin(), s.end(), back_inserter(result),
			areCharsSpaceAndEqual);
	return result;
}

float Treatment::getNumberOfProcessor() {
	std::string line;
	float nbOfProcessor = 0;
	std::ifstream cpuInfo(m_cpuinfo.c_str());
	if (cpuInfo.is_open()) {
		while (cpuInfo.good()) {
			getline(cpuInfo, line);
			if (boost::starts_with(line, "processor"))
				nbOfProcessor++;
		}
		cpuInfo.close();
	} else {
		throw std::runtime_error("Error opening file " + m_cpuinfo + ".");
	}
	return nbOfProcessor;
}

float Treatment::env_getLoad() {
	float load = 0;
	std::string line;
	float nbOfProcessor = 0;
	float loadFive = 0;

	nbOfProcessor = getNumberOfProcessor();

	std::ifstream loadavg(m_loadavg.c_str());
	if (loadavg.is_open()) {
		//There is only one line in this file
		getline(loadavg, line);

		std::vector<std::string> strs;
		boost::split(strs, line, boost::is_any_of(" "));
		loadFive = boost::lexical_cast<float>(strs[1]); //load at 5
		loadavg.close();
	} else {
		throw std::runtime_error("Error opening file " + m_loadavg + ".");
	}

	load = loadFive / nbOfProcessor;

	return load;

}

float Treatment::env_getUsedMemory() {
	float memFree = 0;
	float memCached = 0;
	float memBuf = 0;
	float memTotal = 0;
	std::vector<std::string> strs;
	std::vector<std::string>::iterator it;
	std::string line;

	std::ifstream memInfo(m_meminfo.c_str());
	if (memInfo.is_open()) {
		while (memInfo.good()) {
			getline(memInfo, line);
			if (boost::starts_with(line, "MemTotal")) {
				boost::split(strs, line, boost::is_space());
				it = strs.end();
				it--; //kB
				it--; //Number

				memTotal = boost::lexical_cast<float>(*it);
			} else if (boost::starts_with(line, "MemFree")) {
				boost::split(strs, line, boost::is_any_of(" \t"));
				it = strs.end();
				it--; //kB
				it--; //Number

				memFree = boost::lexical_cast<float>(*it);
			} else if (boost::starts_with(line, "Cached")) {
				boost::split(strs, line, boost::is_any_of(" \t"));
				it = strs.end();
				it--; //kB
				it--; //Number

				memCached = boost::lexical_cast<float>(*it);
			} else if (boost::starts_with(line, "Buffers")) {
				boost::split(strs, line, boost::is_any_of(" \t"));
				it = strs.end();
				it--; //kB
				it--; //Number

				memBuf = boost::lexical_cast<float>(*it);
			}
		}
		memInfo.close();
	} else {
		throw std::runtime_error("Error opening file " + m_meminfo + ".");
	}

	float mem = 0;
	if (memTotal)
		mem = ((memTotal - memFree - memCached - memBuf) * 100 / memTotal);

	return mem;

}

float Treatment::env_getUsedSwap() {
	float swapFree = 0;
	float swapTotal = 0;
	std::vector<std::string> strs;
	std::vector<std::string>::iterator it;
	std::string line;

	std::ifstream memInfo(m_meminfo.c_str());
	if (memInfo.is_open()) {
		while (memInfo.good()) {
			getline(memInfo, line);
			if (boost::starts_with(line, "SwapTotal")) {
				boost::split(strs, line, boost::is_space());
				it = strs.end();
				it--; //kB
				it--; //Number

				swapTotal = boost::lexical_cast<float>(*it);
			} else if (boost::starts_with(line, "SwapFree")) {
				boost::split(strs, line, boost::is_any_of(" \t"));
				it = strs.end();
				it--; //kB
				it--; //Number

				swapFree = boost::lexical_cast<float>(*it);
			}
		}
		memInfo.close();
	} else {
		throw std::runtime_error("Error opening file " + m_meminfo + ".");
	}

	float swap = 0;

	if (swapTotal)
		swap = ((swapTotal - swapFree) * 100 / swapTotal);

	return swap;

}

std::map<std::string, float> Treatment::env_getUsedDiskSpace() {
	std::map<std::string, float> diskMap;

	const std::string df = executeCommand("df -lP");
	std::vector<std::string> lines, strs;
	std::string line;
	boost::split(lines, df, boost::is_any_of("\n"));
	std::vector<std::string>::const_iterator it, it2;

	for (it = lines.begin(); it != lines.end(); it++) {
		if (it == lines.begin()) //skip the first title line
			continue;

		line = remove_excessive_spaces(*it);
		boost::split(strs, line, boost::is_space());
		if (strs.size() != 6)
			continue;
		it2 = strs.end();
		it2--; //mountpoint
		std::string mountpoint = *it2;
		it2--; // occupancy;
		std::string occStr = *it2;
		boost::trim_right_if(occStr, boost::is_any_of("%")); //remove the % at the end
		float occupancy = boost::lexical_cast<float>(occStr);

		diskMap[mountpoint] = occupancy;

	}

	return diskMap;
}

std::map<std::string, float> Treatment::env_getUsedInodes() {
	std::map<std::string, float> diskMap;

	const std::string df = executeCommand("df -liP");
	std::vector<std::string> lines, strs;
	std::string line;
	boost::split(lines, df, boost::is_any_of("\n"));
	std::vector<std::string>::const_iterator it, it2;

	for (it = lines.begin(); it != lines.end(); it++) {
		if (it == lines.begin()) //skip the first title line
			continue;

		line = remove_excessive_spaces(*it);
		boost::split(strs, line, boost::is_space());
		if (strs.size() != 6)
			continue;
		it2 = strs.end();
		it2--; //mountpoint
		std::string mountpoint = *it2;
		it2--; // occupancy;
		std::string occStr = *it2;
		boost::trim_right_if(occStr, boost::is_any_of("%")); //remove the % at the end
		float occupancy = boost::lexical_cast<float>(occStr);

		diskMap[mountpoint] = occupancy;

	}

	return diskMap;
}

std::vector<triplet<std::string, float, std::string> > Treatment::env_getTopCpuConsumers() {
	std::vector<triplet<std::string, float, std::string> > consumers;

	std::string cmdOutput = executeCommand("ps aux | sort -nrk +3 ");
//	std::string cmdOutput = executeCommand("cat /tmp/test");

	std::vector<std::string> lines, strs;
	std::string line;
	boost::split(lines, cmdOutput, boost::is_any_of("\n"));
	std::vector<std::string>::const_iterator it, it2;
	unsigned int maxConsumers = 5;

	for (it = lines.begin(); it != lines.end(); it++) {
		if (consumers.size() == maxConsumers)
			break;
		line = remove_excessive_spaces(*it);
		boost::split(strs, line, boost::is_space());
		if (strs.size() < 11) //not a line as we want?
			continue;

		if (!strs[0].compare("USER")) //sometimes the title line is repeated...
			continue;

		//If we are here, we are parsing process list with the current format :
		//USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
		//We want the pid, the %cpu and the command (we could add the user...)

		it2 = strs.begin();
		std::string user = *it2; //not used yet
		it2++;
		std::string pid = *it2;
		//for (int i = 0; i < 8; i++)
		it2++;

		std::string cpuStr = *it2;
		boost::trim_right_if(cpuStr, boost::is_any_of("%")); //remove the % at the end
		float cpu = boost::lexical_cast<float>(cpuStr);

		for (int i = 0; i < 8; i++)
			it2++;

		std::ostringstream ss;
		while (it2 != strs.end()) {
			ss << *it2 << " ";
			it2++;
		}

		std::string cmd = ss.str();

		triplet<std::string, float, std::string> consumer(pid, cpu, cmd);
		consumers.push_back(consumer);
	}

	return consumers;
}

std::vector<triplet<std::string, float, std::string> > Treatment::env_getTopMemoryConsumers() {
	std::vector<triplet<std::string, float, std::string> > consumers;

	std::string cmdOutput = executeCommand("ps aux | sort -nrk +4 ");
//	std::string cmdOutput = executeCommand("cat /tmp/test");

	std::vector<std::string> lines, strs;
	std::string line;
	boost::split(lines, cmdOutput, boost::is_any_of("\n"));
	std::vector<std::string>::const_iterator it, it2;
	unsigned int maxConsumers = 5;

	for (it = lines.begin(); it != lines.end(); it++) {
		if (consumers.size() == maxConsumers)
			break;
		line = remove_excessive_spaces(*it);
		//boost::trim_left(line);
		boost::split(strs, line, boost::is_space());
		if (strs.size() < 11) //not a line as we want?
			continue;

		//If we are here, we are parsing process list with the current format :
		//USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
		//We want the pid, the %cpu and the command (we could add the user...)

		it2 = strs.begin();
		std::string user = *it2; //not used yet
		it2++;
		std::string pid = *it2;
		//for (int i = 0; i < 8; i++)
		it2++;
		it2++;

		std::string memStr = *it2;
		boost::trim_right_if(memStr, boost::is_any_of("%")); //remove the % at the end
		float mem = boost::lexical_cast<float>(memStr);

		for (int i = 0; i < 7; i++)
			it2++;

		std::ostringstream ss;
		while (it2 != strs.end()) {
			ss << *it2 << " ";
			it2++;
		}

		std::string cmd = ss.str();

		triplet<std::string, float, std::string> consumer(pid, mem, cmd);
		consumers.push_back(consumer);
	}

	return consumers;
}

//Does not take automount into account. FUCK AUTOMOUNT!!!!
MountpointInfo Treatment::mount_CheckMountPointForFile(std::string & path) {
	MountpointInfo mountpoint;

	char buf[PATH_MAX + 1]; /* not sure about the "+ 1" */
	char *res = realpath(path.c_str(), buf);

	//	/* There can be several reason to realpath failing
//	 * - if the symlink chain is broken somewhere
//	 * - if the file is not present : is the fs unmounted or not, we don't know
//	 *
//	 * First, let's try to get as far as we can with readlink
//	 */
//	if (!res) {
//		char bufTmp[PATH_MAX + 1];
//		strcpy(buf, path.c_str());
//		ssize_t len;
//		do {
//			len = readlink(buf, bufTmp, sizeof(bufTmp) - 1);
//			if (len != -1) {
//				strncpy(buf, bufTmp, len);
//				buf[len] = '\0';
//			}
//		} while (len != -1);
//
//	}

	/* If res is null, it means that realpath could not get
	 * until the end of the path, but buf still contains as
	 * far as it can get. So it does what I was doing manually with readlink
	 */
	if ((!res) && (!strlen(buf)))
		throw std::runtime_error("Problem getting the real path of " + path);

	std::string candidateDir;
	std::vector<std::string> candidateMountpoint;
	unsigned int sizeOfCandidate = 0;

	std::string line;
	std::vector<std::string> strs;
	std::ifstream fstab("/etc/fstab");
	if (fstab.is_open()) {
		while (fstab.good()) {
			getline(fstab, line);
			std::string cleanLine = remove_excessive_spaces(line);
			boost::split(strs, cleanLine, boost::is_space());
			if (strs.size() >= 4) {
				if (line[0] == '#')
					continue;
				if (boost::starts_with(buf, strs[1])) {
					if (strs[1].size() > sizeOfCandidate) {
						candidateMountpoint = strs;
						sizeOfCandidate = strs[1].size();
						candidateDir = strs[1];
					}
				}
			}
		}
		fstab.close();
	} else {
		throw std::runtime_error("Error opening file /etc/fstab.");
	}

	if (sizeOfCandidate == 0)
		throw std::runtime_error(
				"No candidate of mountpoint for " + path + ". How strange...");

	mountpoint.m_device = candidateMountpoint[0];
	mountpoint.m_dir = candidateMountpoint[1];
	mountpoint.m_type = candidateMountpoint[2];
	std::string allOpts;
	for (std::vector<std::string>::const_iterator it =
			candidateMountpoint.begin() + 3; it != candidateMountpoint.end();
			it++) {
		allOpts += *it + ",";
	}

	mountpoint.m_options = allOpts;

	return mountpoint;

}

bool Treatment::mount_isMountpointMounted(MountpointInfo & mountpoint) {
	bool isMounted = false;
	std::string line;
	std::vector<std::string> strs;
	// should eventually consider replacing /proc/mounts by /etc/mtab, or using both
	std::ifstream mounts("/proc/mounts");
	if (mounts.is_open()) {
		while ((mounts.good()) && (!isMounted)) {
			getline(mounts, line);
			std::string cleanLine = remove_excessive_spaces(line);
			boost::split(strs, cleanLine, boost::is_space());
			if (strs.size() >= 4) {
				if (strs[0].compare(mountpoint.m_device))
					continue;
				if (strs[1].compare(mountpoint.m_dir))
					continue;
				if (strs[2].compare(mountpoint.m_type))
					continue;

				isMounted = true;

			}
		}
		mounts.close();
	} else {
		throw std::runtime_error("Error opening file /proc/mounts");
	}

	return isMounted;
}

bool Treatment::mount_isMountPointReadWriteForUser(MountpointInfo & mountpoint,
		std::string & uid) {

	uid_t oeuid;

	oeuid = geteuid(); //In principle, should be 0 (root), but just in case...
	if (seteuid(boost::lexical_cast<int>(uid)) == -1) {
		throw std::runtime_error(
				"Error changing the uid to " + uid + " : " + strerror(errno));
	}
//	neuid = geteuid();

	// Check the ro/rw options, and if it is actually true
	//we assume that no other option contains rw...
	if (boost::contains(mountpoint.m_options, "rw")) {
		mountpoint.m_shouldBeRw = true;
		std::string rndName = mountpoint.m_dir + "/.phrTestFile_"
				+ boost::lexical_cast<std::string>(rand()) + "_removeIfFound";
		std::ofstream outfile(rndName.c_str());
		if (outfile.is_open()) {
			remove(rndName.c_str());
			mountpoint.m_isRw = true;
		} else {
			mountpoint.m_isRw = false;
		}
		outfile.close();

	} else {
		mountpoint.m_shouldBeRw = false;
	}
	if (seteuid(boost::lexical_cast<int>(oeuid)) == -1) {
		throw std::runtime_error(
				"Error changing the uid to " + uid + " : " + strerror(errno));
	}
	return mountpoint.m_isRw;
}

//Consider using a boost graph?
//Consider getting rid of the * notation for kernel process
// So far there is only one instance of the process which is found -> not true anymore I think. To be checked
std::map<int, std::vector<int> > Treatment::proc_getPidTree(
		std::string & procName) {
	std::map<int, std::vector<int> > fullPidTree;
	std::map<int, std::vector<int> > pidTree;
	std::map<int, std::vector<int> >::iterator itMap;
	int startPid = 0;
	bool startFound = false;
	int pid = 0, ppid = 0;
	std::string cmd;
	std::vector<std::string>::const_iterator it, it2;
	std::vector<int> toFollow;

	std::string formatedProcName;
	char fcc = procName[0];
	if (fcc == '*') { //kernel process...
		formatedProcName = "[" + procName.substr(1) + "]";
	} else {
		formatedProcName = procName;
	}

	std::string cmdOutput = executeCommand("ps -ef");
//UID        PID  PPID  C STIME TTY          TIME CMD
	std::vector<std::string> lines, fields;
	std::string line;
	boost::split(lines, cmdOutput, boost::is_any_of("\n"));

	for (it = lines.begin(); it != lines.end(); it++) {
		if (it == lines.begin()) //skip the first title line
			continue;
		line = remove_excessive_spaces(*it);
		boost::split(fields, line, boost::is_space());

		if (fields.size() < 8)
			continue;

		pid = boost::lexical_cast<int>(fields[1]);
		ppid = boost::lexical_cast<int>(fields[2]);

		it2 = fields.begin();
		for (int j = 0; j < 7; j++)
			it2++;

		std::ostringstream ss;
		while (it2 != fields.end()) {
			ss << *it2 << " ";
			it2++;
		}

		std::string cmd = ss.str();
		//if (!startFound) {

		//starts_with does not work for prossess started with interpreter such as perl or python
		if (boost::contains(cmd, formatedProcName)) {
			//if (boost::starts_with(cmd, formatedProcName)) {
			startPid = pid;
			startFound = true;
			toFollow.push_back(pid);
		}
		//}

		itMap = fullPidTree.find(ppid);
		if (itMap != fullPidTree.end()) {
			(itMap->second).push_back(pid);
		} else {
			std::vector<int> newVec;
			newVec.push_back(pid);
			fullPidTree[ppid] = newVec;
		}

	}

	if (!startFound)
		throw std::runtime_error(
				"Could not find the process starting with \"" + procName
						+ "\" in the process list.");

//toFollow.push_back(startPid);

	while (toFollow.size()) {
		int cur = toFollow.back();
		toFollow.pop_back();
		itMap = fullPidTree.find(cur);
		std::vector<int> children;
		if (itMap != fullPidTree.end()) {
			children = itMap->second;
			for (unsigned int i = 0; i < children.size(); i++)
				toFollow.push_back(children[i]);
		}
		pidTree[cur] = children;
	}

	return pidTree;

}

std::vector<std::string> Treatment::proc_getPsLine(std::string & pid) {
	std::string cmdOutput = executeCommand("ps up " + pid);
	std::vector<std::string> lines, fields;
	std::vector<std::string>::const_iterator it;
	boost::split(lines, cmdOutput, boost::is_any_of("\n"));

//We should get 1 line for the title, 1 line of data, but ebcause of parsing there might me more
//USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
	std::string line;
	bool found = false;
	for (it = lines.begin(); it != lines.end(); it++) {
		if (it == lines.begin()) //we skip the title line
			continue;
		line = remove_excessive_spaces(*it);
		if (!line.size())
			continue;
		boost::split(fields, line, boost::is_space());
		if (fields.size() >= 11) { //this should be our line
			found = true;
			break;
		}

	}

	if (!found)
		throw std::runtime_error(
				"Could not find a process line with pid " + pid + ".");
	return fields;
}

std::string Treatment::proc_getFullCommandLine(std::string & pid) {
	std::string cmdLine;
	std::string cmdFileName = "/proc/" + pid + "/cmdline";
	std::ifstream cmdFile(cmdFileName.c_str());
	if (cmdFile.is_open()) {
		getline(cmdFile, cmdLine);
		cmdFile.close();
	} else {
		throw std::runtime_error("Error opening file " + cmdFileName + ".");
	}
	return cmdLine;

}

std::string Treatment::proc_getProcessUser(std::string & pid) {

	std::vector<std::string> psLine = proc_getPsLine(pid);
	std::string user = psLine[0];
	return user;
}

float Treatment::proc_getProcessCpuUsage(std::string & pid) {
	int samples = 1;
	float cpu = 0;
	float nbOfProc = getNumberOfProcessor();
	for (int i = 0; i < samples; i++) {
		std::vector<std::string> psLine = proc_getPsLine(pid);

//		cpu += boost::lexical_cast<float>(psLine[2]);
		//sleep(1);
		float curCpu = boost::lexical_cast<float>(psLine[2]);
		//if it is more than 100 (101 just in case),
		// it is using multiple proc to reach this usage.
		// it is not good at all, because for example you could have
		// 4 processor, and a proc that uses 100% 2 of them,
		// and I will return 50%. But well, no better for the time being...
		if (curCpu > 101) {
			curCpu = curCpu / nbOfProc;
		}
		cpu += curCpu;
	}

	cpu /= (float) samples;

	return cpu;
}

float Treatment::proc_getProcessMemoryUsage(std::string & pid) {
	int samples = 1;
	float mem = 0;

	for (int i = 0; i < samples; i++) {
		std::vector<std::string> psLine = proc_getPsLine(pid);
		mem += boost::lexical_cast<float>(psLine[3]);
		//sleep(1);
	}

	mem /= (float) samples;

	return mem;
}

//Limit                     Soft Limit           Hard Limit           Units
//Max cpu time              unlimited            unlimited            seconds
//Max file size             unlimited            unlimited            bytes
//Max data size             unlimited            unlimited            bytes
//Max stack size            8388608              unlimited            bytes
//Max core file size        0                    unlimited            bytes
//Max resident set          unlimited            unlimited            bytes
//Max processes             unlimited            unlimited            processes
//Max open files            1024                 1024                 files
//Max locked memory         65536                65536                bytes
//Max address space         unlimited            unlimited            bytes
//Max file locks            unlimited            unlimited            locks
//Max pending signals       16382                16382                signals
//Max msgqueue size         819200               819200               bytes
//Max nice priority         20                   20
//Max realtime priority     0                    0
//Max realtime timeout      unlimited            unlimited            us
std::map<std::string, std::pair<unsigned long int, unsigned long int> > Treatment::proc_getProcessLimits(
		std::string & pid) {
	std::map<std::string, std::pair<unsigned long int, unsigned long int> > limits;

	bool isFirstLine = true;
	std::string rawLine, line;
	std::vector<std::string> fields;
	std::string limitPath("/proc/" + pid + "/limits");
	std::ifstream limitFile(limitPath.c_str());
	int idCurLimit = 0;
	std::string softStr, hardStr;

	/*	char NAMES[16][22] = { "MAX_CPU_TIME", "MAX_FILE_SIZE", "MAX_DATA_SIZE",
	 "MAX_STACK_SIZE", "MAX_CORE_FILE_SIZE", "MAX_RESIDENT_SET",
	 "MAX_PROCESSES", "MAX_OPEN_FILES", "MAX_LOCKED_MEMORY",
	 "MAX_ADDRESS_SPACE", "MAX_FILE_LOCKS", "MAX_PENDING_SIGNALS",
	 "MAX_MSGQUEUE_SIZE", "MAX_NICE_PRIORITY", "MAX_REALTIME_PRIORITY",
	 "MAX_REALTIME_TIMEOUT" };
	 */
	if (limitFile.is_open()) {
		while (limitFile.good() && (idCurLimit != 15)) {
			getline(limitFile, rawLine);
			if (isFirstLine) { //We skip the title line
				isFirstLine = false;
				continue;
			}

			line = remove_excessive_spaces(rawLine);
			boost::trim_right(line);
			boost::split(fields, line, boost::is_space());
			int fieldsLength = fields.size();
			if ((idCurLimit == 13) || (idCurLimit == 14)) { //No units for the 13th and 14th
				softStr = fields[fieldsLength - 2];
				hardStr = fields[fieldsLength - 1];
			} else {
				softStr = fields[fieldsLength - 3];
				hardStr = fields[fieldsLength - 2];
			}

			//int soft = -1;
			unsigned long int soft = LIMIT_UNLIMITED;
			if (softStr.compare("unlimited"))
				soft = boost::lexical_cast<unsigned long int>(softStr);

			//int hard = -1;
			unsigned long int hard = LIMIT_UNLIMITED;
			if (hardStr.compare("unlimited"))
				hard = boost::lexical_cast<unsigned long int>(hardStr);

			std::pair<unsigned long int, unsigned long int> limit(soft, hard);
			limits[LIMIT_NAMES[idCurLimit]] = limit;
			idCurLimit++;
		}
		limitFile.close();
	} else {
		throw std::runtime_error("Error opening file " + limitPath + ".");
	}

//This one is set manually because is really weird
	limits["MAX_REALTIME_TIMEOUT"] = std::pair<unsigned long int,
			unsigned long int>(-1, -1);

	return limits;
}

std::map<std::string, unsigned long int> Treatment::proc_getProcessLimitStatus(
		std::string & pid) {
	std::map<std::string, unsigned long int> limitStatus;

	std::string rawLine, line, field;
	std::vector<std::string> statFields, strs;
	std::map<std::string, std::string> statusFields;
	std::string statPath("/proc/" + pid + "/stat");
	std::ifstream statFile(statPath.c_str());
	std::string statusPath("/proc/" + pid + "/status");
	std::ifstream statusFile(statusPath.c_str());
	DIR *dp;
	struct dirent *ep;
	std::string fileName;
	std::string fdDir = "/proc/" + pid + "/fd/";
	size_t fileSize = 0;

	int numberOfFiles = 0;
	unsigned int maxFileSize = 0;

	// If the process is a kernel process, many field will not be available in the status file
	bool isKernelProc = false;

	if (!proc_getFullCommandLine(pid).size())
		isKernelProc = true;

	if (statFile.is_open()) {
		getline(statFile, rawLine);
		line = remove_excessive_spaces(rawLine);
		boost::split(statFields, line, boost::is_space());

		statFile.close();
	} else {
		throw std::runtime_error("Error opening file " + statPath + ".");
	}

	if (statusFile.is_open()) {
		while (statusFile.good()) {
			getline(statusFile, rawLine);
			boost::replace_all(rawLine, "\t", " ");
			line = remove_excessive_spaces(rawLine);
			boost::split(strs, line, boost::is_space());

			if (strs.size() < 2)
				continue;
			boost::trim_right_if(strs[0], boost::is_any_of(":"));
			statusFields[strs[0]] = strs[1];
		}
		statusFile.close();
	} else {
		throw std::runtime_error("Error opening file " + statusPath + ".");
	}

	dp = opendir(fdDir.c_str());
	if (dp != NULL) {
		while ((ep = readdir(dp))) {
			numberOfFiles++;
			fileName = fdDir + ep->d_name;
			struct stat st;
			if (stat(fileName.c_str(), &st)) {
				// We should maybe differentiate case why we cannot stat a file
			} else {
				fileSize = st.st_size;
				maxFileSize = (fileSize > maxFileSize) ? fileSize : maxFileSize;

			}
		}
		(void) closedir(dp);
	} else {
		throw std::runtime_error("Could not open directory " + fdDir + ".");
	}

	unsigned long int defaultValue = 0;

	limitStatus["MAX_CPU_TIME"] = boost::lexical_cast<unsigned long int>(
			statFields[13]);

	limitStatus["MAX_FILE_SIZE"] = maxFileSize;

	if (!isKernelProc)
		limitStatus["MAX_DATA_SIZE"] = boost::lexical_cast<unsigned long int>(
				statusFields["VmData"]);
	else
		limitStatus["MAX_DATA_SIZE"] = defaultValue;

	if (!isKernelProc)
		limitStatus["MAX_STACK_SIZE"] = boost::lexical_cast<unsigned long int>(
				statusFields["VmStk"]);
	else
		limitStatus["MAX_STACK_SIZE"] = defaultValue;

	// Non relevant so we just append 0
	limitStatus["MAX_CORE_FILE_SIZE"] = 0;

	if (!isKernelProc)
		limitStatus["MAX_RESIDENT_SET"] =
				boost::lexical_cast<unsigned long int>(statusFields["VmRSS"]);
	else
		limitStatus["MAX_RESIDENT_SET"] = defaultValue;

	limitStatus["MAX_PROCESSES"] = boost::lexical_cast<unsigned long int>(
			statusFields["Threads"]);

	limitStatus["MAX_OPEN_FILES"] = numberOfFiles;

	if (!isKernelProc)
		limitStatus["MAX_LOCKED_MEMORY"] =
				boost::lexical_cast<unsigned long int>(statusFields["VmLck"]);
	else
		limitStatus["MAX_LOCKED_MEMORY"] = defaultValue;

	if (!isKernelProc)
		limitStatus["MAX_ADDRESS_SPACE"] =
				boost::lexical_cast<unsigned long int>(statusFields["VmSize"]);
	else
		limitStatus["MAX_ADDRESS_SPACE"] = defaultValue;

	//Could not find for process, so just append 0
	limitStatus["MAX_FILE_LOCKS"] = 0;

	limitStatus["MAX_PENDING_SIGNALS"] = boost::lexical_cast<unsigned long int>(
			statusFields["ShdPnd"]);

	//Ignored, because weird ... (man getrlimit, RLIMIT_MSGQUEUE)
	limitStatus["MAX_MSGQUEUE_SIZE"] = 0;

	limitStatus["MAX_NICE_PRIORITY"] = boost::lexical_cast<unsigned long int>(
			statFields[18]);

	limitStatus["MAX_REALTIME_PRIORITY"] =
			boost::lexical_cast<unsigned long int>(statFields[17]);

	limitStatus["MAX_REALTIME_TIMEOUT"] = 0;

	return limitStatus;
}

bool Treatment::file_fileExists(std::string & fileName) {
	bool exists = false;
	try {
		std::ifstream file(fileName.c_str());
		if (file.is_open())
			exists = true;
		file.close();
	} catch (...) {
	}

	return exists;
}

std::string Treatment::file_getFileOwner(std::string & fileName) {
	std::string owner;
	struct stat st;
	struct passwd * pw;
	if (stat(fileName.c_str(), &st))
		throw std::runtime_error("Cannot stat file " + fileName + ".");

	pw = getpwuid(st.st_uid);
	if (pw == NULL) {
		std::stringstream ss(std::stringstream::in);
		ss << st.st_uid;
		throw std::runtime_error(
				"Cannot get passwd entry matchi uid " + ss.str() + ".");
	}
	owner = pw->pw_name;
	return owner;

}

std::string Treatment::file_getFileGroup(std::string & fileName) {
	std::string group;
	struct stat st;
	struct group * gr;
	if (stat(fileName.c_str(), &st))
		throw std::runtime_error("Cannot stat file " + fileName + ".");

	gr = getgrgid(st.st_gid);
	if (gr == NULL) {
		std::stringstream ss(std::stringstream::in);
		ss << st.st_gid;
		throw std::runtime_error(
				"Cannot get group entry matchi gid " + ss.str() + ".");
	}
	group = gr->gr_name;
	return group;
}

std::string Treatment::file_getFilePermissions(std::string & fileName) {
	struct stat st;
	if (stat(fileName.c_str(), &st))
		throw std::runtime_error("Cannot stat file " + fileName + ".");
	mode_t mode = st.st_mode;
	mode &= (511); // (511 = 000000000111111111)
	std::stringstream stream;
	stream << std::oct << mode;
	std::string perm(stream.str());
	return perm;
}

//This is FS specific.... I will just return a constant string so far...
std::string Treatment::file_getFileAttributes(std::string & fileName) {
	std::string attr = "-----------------e-";
	return attr;
}

std::string Treatment::file_getFileMd5(std::string & fileName) {
	std::string cmdOut = executeCommand("md5sum " + fileName);
	std::string line = remove_excessive_spaces(cmdOut);
	std::vector<std::string> strs;
	boost::split(strs, line, boost::is_space());
	if (strs.size() < 2)
		throw std::runtime_error(
				"Could not get md5 checksum for file " + fileName + ".");
	std::string md5 = strs[0];
	return md5;
}

std::map<std::string, bool> Treatment::file_getFileRegex(std::string & fileName,
		std::vector<std::string> & regex) {
	std::string fileContent, line;
	std::map<std::string, bool> regexMap;
	boost::regex re;
	std::ostringstream ss;
	std::ifstream fs(fileName.c_str());
	if (fs.is_open()) {
		while (fs.good()) {
			getline(fs, line);
			ss << line << std::endl;
		}
		fileContent = ss.str();
	} else {
		throw std::runtime_error("Cannot open the file " + fileName);
	}
	for (std::vector<std::string>::iterator it = regex.begin();
			it != regex.end(); it++) {
		std::cout << "Try to match " << *it << std::endl;
		// Match the string against the current regular expression
		try {
			re.assign(*it, boost::regex_constants::extended);
//			regexMap[*it] = regex_match(fileContent, re);
			regexMap[*it] = regex_search(fileContent, re);

		} catch (boost::regex_error& e) {
			std::cout << *it << " is not a valid regular expression: \""
					<< e.what() << "\"" << std::endl;

			regexMap[*it] = false;
		}
	}
	return regexMap;
}

std::vector<std::string> Treatment::folder_getFolderContent(
		std::string & folderName,
		std::map<std::string, std::string> & extraArgs) {
//	std::cout << "Folder get content " << std::endl;
	for (std::map<std::string, std::string>::iterator iter = extraArgs.begin();
			iter != extraArgs.end(); iter++) {
		std::cout << "--" << iter->first << "--" << iter->second << "--"
				<< std::endl;
	}
	std::vector<std::string> content;
	int curDepth = 0;
	int maxDepth = 0;
	try {
		maxDepth = boost::lexical_cast<int>(extraArgs["maxDepth"]);
		curDepth = boost::lexical_cast<int>(extraArgs["curDepth"]);
		if (curDepth >= maxDepth)
			return content;
	} catch (std::exception &e) {

	}

	boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
	for (boost::filesystem::directory_iterator itr(folderName); itr != end_itr;
			++itr) {
		if (is_directory(itr->status())) {
			std::map<std::string, std::string> nextExtraArgs = extraArgs;
			nextExtraArgs["curDepth"] = boost::lexical_cast<std::string>(
					curDepth + 1);
			std::string nfn = itr->path().string();
			std::vector<std::string> subContent = folder_getFolderContent(nfn,
					nextExtraArgs);
			for (std::vector<std::string>::const_iterator sc =
					subContent.begin(); sc != subContent.end(); sc++) {
				content.push_back(*sc);
			}
		} else {
//			std::cout << "I see " << itr->path().string() << std::endl;
			content.push_back(itr->path().string());
		}
	}

	return content;
}
