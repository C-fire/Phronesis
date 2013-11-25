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


#ifndef TREATMENT_HPP
#define TREATMENT_HPP

/*!
 * \file Treatment.hpp
 * \brief Treatment class
 * \author Christophe Haen
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <stdexcept>   // for exception, runtime_error, out_of_range
#include <string.h>
#include "Message.hpp"
#include "connection.hpp"

//namespace bp = ::boost::process;

using boost::asio::ip::tcp;

class server;
class Treatment;
/*! \typedef boost::shared_ptr<Treatment> Treatment_ptr;
 *  a boost::shared_ptr on a Treatment
 */
typedef boost::shared_ptr<Treatment> Treatment_ptr; //!< shared pointer to a Treatment.

extern char LIMIT_NAMES[16][22];

/*!
 * \class Treatment
 * \brief The Treatment class is responsible for treating the requests.
 *
 * This class listen to a request, fetch the information, and send them
 * back to the client.
 * It is based on the boost::shared_ptr to avoid having to delete anything in the PhronesisAgent class.
 * There is one instance of Treatment per Client request
 *
 */
class Treatment: public boost::enable_shared_from_this<Treatment> {
public:

	/*!
	 *  \brief Destructor
	 *
	 *	Destructor of Treatment
	 */
	~Treatment();

	/*!
	 *  \brief Create an instance of Treatment
	 *
	 *  This function instantiate a Treatment, and have it wait for connection.
	 *  It returns a shared pointer to it.
	 *
	 *	\param[in] tcp_connection : a shared pointer to the tcp_connection instance created by the PhronesisAgent
	 *
	 *	\return a boost::shared_ptr to the Treatment
	 */
	static Treatment_ptr create(connection_ptr tcp_connection);

	static int command_timeout;

private:

	/*!
	 * \brief Constructor
	 *
	 * Private constructor to enforce the use of boost::shared_ptr.
	 * Use the Treatment::create() method
	 *
	 * @param tcp_connection : a shared pointer to the tcp_connection instance created by the PhronesisAgent
	 */
	Treatment(connection_ptr tcp_connection);

	/*!
	 * \brief Waits for a user request.
	 *
	 * What is read is stored in Treatment::m_request
	 *
	 */
	void wait_for_data();

	/*!
	 * \brief Handler once the data has been sent to the client.
	 *
	 * MORE TO BE ADDED
	 *
	 * @param error error in writing
	 */
	void handle_write(const boost::system::error_code& error);

	/*!
	 * \brief Handle the request read by the Treatment.
	 *
	 * The method Treatment::wait_for_data sets Treatment::handle_read as a handler
	 * when it has received the user Request.
	 * When the data are dealt with, it sends the data to the client,
	 * and sets Treatment::handle_write as a handler.
	 *
	 * MORE TO BE ADDED
	 *
	 * @param error error in reading
	 */
	void handle_read(const boost::system::error_code& error);

	/*!
	 * \brief Starts a new thread to execute the command and wait for signal to kill
	 *
	 * This method just start Treatment::CommandThreadChild in a new thread,
	 * and wait for the interrupt() method to be called if necessary
	 *
	 *
	 * @param[in,out] ret reference to a string which will contain the output
	 * @param[in] exec the command to execute
	 */
	void CommandThread(std::string & ret, std::string exec);

	/*!
	 * \brief Execute the command
	 *
	 * This method just uses the boost::process library to execute the command
	 * passed in argument. The output of the command is written into the ret string.
	 *
	 * @param[in,out] ret reference to a string which will contain the output
	 * @param[in] exec the command to execute
	 */
	void CommandThreadChild(std::string & ret, std::string exec);

	/*!
	 * \brief Execute a shell command.
	 *
	 * This method is the one meant to be called to execute a command, and relies on the
	 * CommandThread and CommandThreadChild methods.
	 * It starts a new thread executing CommandThread, and calls the interupt() method on it
	 * if it did not finish on time.
	 * The timeout is hard coded and is 2 seconds.
	 *
	 * @param command the command to execute
	 * @return the output of the execution
	 */
	std::string executeCommand(std::string command);

	/*!
	 * \brief call a Treatment method passed as parameter, deals with the result, and handles the exceptions
	 *
	 * Because all the methods used to get data about environment, file and process require similar
	 * error handling and have similar signature for the argument, this method serves as a wrapper.
	 * Thus we don't have to write many time the error handling, we do it once.
	 * The errorKey argument is used as a key in the errorMap of the message to be sent. It represents
	 * the value we are trying to get with this call (e.g. cpu, diskSpace...). If ever an exception is raised,
	 * we add an entry in the map with errorKey as a key, and the exception message as a value.
	 *
	 * @param f The method to call
	 * @param arg1 The first argument to pass to the method
	 * @param arg2 The second argument to pass to the method
	 * @param errorKey The value we are trying to measure
	 * @param ret A reference to where the return value of the method will be stored
	 * @param errorMap Reference to the message errorMap
	 */
	template<typename RET, typename ARG1, typename ARG2>
	void call(RET (Treatment::*f)(ARG1&, ARG2&), ARG1 & arg1, ARG2 & arg2,
			std::string errorKey, RET & ret,
			std::map<std::string, std::string> & errorMap);

	/*!
	 * \brief call a Treatment method passed as parameter, deals with the result, and handles the exceptions
	 *
	 * See Treatment::call<RET,ARG1,ARG2>
	 *
	 * @param f The method to call
	 * @param arg The first argument to pass to the method
	 * @param errorKey The value we are trying to measure
	 * @param ret A reference to where the return value of the method will be stored
	 * @param errorMap Reference to the message errorMap
	 */
	template<typename RET, typename ARG>
	void call(RET (Treatment::*f)(ARG&), ARG & arg, std::string errorKey,
			RET & ret, std::map<std::string, std::string> & errorMap);

	/*!
	 * \brief call a Treatment method passed as parameter, deals with the result, and handles the exceptions
	 *
	 * See Treatment::call<RET,ARG1,ARG2>
	 *
	 * @param f The method to call
	 * @param errorKey The value we are trying to measure
	 * @param ret A reference to where the return value of the method will be stored
	 * @param errorMap Reference to the message errorMap
	 */
	template<typename RET>
	void call(RET (Treatment::*f)(), std::string errorKey, RET & ret,
			std::map<std::string, std::string> & errorMap);

	/*!
	 * \brief Reduce several spaces into one
	 *
	 * Regardless of their positions in the string, if there are several spaces
	 * consecutive, they are reduced to one only.
	 *
	 * @param s the original string
	 * @return the string with excessive spaces removed
	 */
	std::string remove_excessive_spaces(const std::string& s);

	/*!
	 * @brief Return the amount of processor of the machine
	 *
	 * @return number of processor
	 */

	float getNumberOfProcessor();

	std::string m_cpuinfo; /*!< Path to the cpuinfo file */
	std::string m_loadavg; /*!< Path to the loadavg file */
	std::string m_meminfo; /*!< Path to the meminfo file*/

	/*!
	 * \brief Returns the 5mn average load of the environment
	 *
	 *	The data is obtained by looking at the loadavg file
	 *
	 * @return The 5mn average load
	 */
	float env_getLoad();

	/*!
	 * \brief Returns the percentage of used memory
	 *
	 * This method reads the total memory, the free memory, the memory used for cache and the memory used
	 * for buffer in the meminfo file. The result is given by ((memTotal - memFree - memCached - memBuf) * 100 / memTotal)
	 *
	 * @return The percentage of used memory
	 */
	float env_getUsedMemory();

	/*!
	 * \brief Returns the percentage of used swap space
	 *
	 * Reads the total swap and the free swap in the meminfo file and computes the percentage of used swap
	 * @return
	 */
	float env_getUsedSwap();

	/*!
	 * \brief Returns the amount of disk space used per file system
	 *
	 * The amount of space used per local file system is checked with the df command.
	 * It is returned as a map where the key is the mountpoint and the value is the percentage
	 * of space used
	 *
	 * @return Percentage of disk space used
	 */
	std::map<std::string, float> env_getUsedDiskSpace();

	/*!
	 * \brief Returns the amount of inodes used per file system
	 *
	 * The amount of inodes used per local file system is checked with the df command.
	 * It is returned as a map where the key is the mountpoint and the value is the percentage
	 * of inodes used
	 *
	 * @return Percentage of inode space used
	 */
	std::map<std::string, float> env_getUsedInodes();

	/*!
	 * \brief Returns a list of the 5 biggest Cpu consumers
	 *
	 * The list is obtained with the ps command. It returns triplet containing
	 * - the pid
	 * - the percentage of 1 cpu it is using
	 * - the command line of the process
	 *
	 * I should maybe also add who is the owner of the process...
	 * The reason for having a custom triplet class, and not using boost Tuple
	 * is because they are a night mare to serialize...
	 *
	 * CAUTION : the value returned are a snapshot. It can be extremely misleading,
	 * since a process can use 100% cpu for a very short time, and yet being running normally,
	 * without bringing the system down. Further version of this method should try to make
	 * kind of an average.
	 *
	 * @return The 5 biggest cpu consumers
	 */
	std::vector<triplet<std::string, float, std::string> > env_getTopCpuConsumers();

	/*!
	 * \brief Returns a list of the 5 biggest memory consumers
	 *
	 * The list is obtained with the ps command. It returns triplet containing
	 * - the pid
	 * - the percentage of memory it is using
	 * - the command line of the process
	 *
	 * I should maybe also add who is the owner of the process...
	 * The reason for having a custom triplet class, and not using boost Tuple
	 * is because they are a night mare to serialize...
	 *
	 * CAUTION : the value returned are a snapshot. It can be extremely misleading,
	 * since a process can use 100% memory for a very short time, and yet being running normally,
	 * without bringing the system down. Further version of this method should try to make
	 * kind of an average.
	 *
	 * @return The 5 biggest memory consumers
	 */
	std::vector<triplet<std::string, float, std::string> > env_getTopMemoryConsumers();

	/*!
	 * \brief Returns the information about the mountpoint required for a file to be there
	 *
	 * Returns the information based on the fstab file
	 * Caution : automount not supported yet, since it does not write anything to the fstab
	 *
	 * @return Mountpoint information
	 */
	MountpointInfo mount_CheckMountPointForFile(std::string & path);

	/*!
	 * \brief Checks whether a given mountpoint is mounted
	 *
	 * This method looks into the /proc/mounts file, and make sure that the
	 * device, the dir and the type are coherent with the mountpoint given as
	 * argument. We don't take the options into account for now.
	 *
	 * @return bool true if the Mountpoint is there, false otherwise
	 */
	bool mount_isMountpointMounted(MountpointInfo & mountpoint);

	/*!
	 * @brief Check whether a mountpoint is RW from a given user
	 *
	 * This method actually tries to create a file using the provided uid.
	 * The bool which is returns gives the information whether the folder
	 * is writable or not, but more information are writen into the mounpoint
	 * object (such as if it is mounted rw, or if we simply cannot write).
	 *
	 * @param mountpoint Information about the mountpoint
	 * @param uid uid of the user we want to use to try the writing
	 * @return true if writable, false if not.
	 */

	bool mount_isMountPointReadWriteForUser(MountpointInfo & mountpoint,
			std::string & uid);

	/*!
	 * \brief Returns the tree of pid for the process name passed as parameter
	 *
	 * This method will build a complete tree of the processes, and return only
	 * the part of the tree which concerns our process.
	 * The tree is obtained from the ps command.
	 *
	 * The map returned has as key a parent pid, while the vector in value is the list of child pid
	 * @param procName the command we are interested in
	 * @return The pid tree
	 */
	std::map<int, std::vector<int> > proc_getPidTree(std::string & procName);

	/*!
	 * \brief Returns a vector of information about a given pid
	 *
	 * This method returns the following values :
	 * - user
	 * - pid
	 * - %CPU
	 * - %MEM
	 * - VSZ
	 * - RSS
	 * - TTY
	 * - STAT
	 * - START TIME
	 * - COMMAND
	 *
	 * All those information come from the ps command
	 *
	 * The method is meant to be used by all the other method that need particular fields of the ps output
	 *
	 * @param pid The pid of the process we are interested in
	 * @return Vector of information about the pid
	 */
	std::vector<std::string> proc_getPsLine(std::string & pid);

	/*!
	 * @brief Returns the command line visible in ps for the given process
	 *
	 * @param pid pid of the process we are interested in
	 * @return the full command line
	 */
	std::string proc_getFullCommandLine(std::string & pid);

	/*!
	 * \brief Returns the user of a process
	 *
	 * Returns the user of a running process from its pid
	 * as indicated by ps.
	 *
	 * @param pid The pid of the process we are interested in
	 * @return Username running the process
	 */
	std::string proc_getProcessUser(std::string & pid);

	/*!
	 * \brief Returns the percentage of cpu used by a process
	 *
	 * Returns the percentage of cpu used by a running process from its pid
	 * as indicated by ps.
	 *
	 * @param pid The pid of the process we are interested in
	 * @return Percentage of cpu used
	 */
	float proc_getProcessCpuUsage(std::string & pid);

	/*!
	 * \brief Returns the percentage of memory used by a process
	 *
	 * Returns the percentage of memory used by a running process from its pid
	 * as indicated by ps.
	 *
	 * @param pid The pid of the process we are interested in
	 * @return Percentage of memory used
	 */
	float proc_getProcessMemoryUsage(std::string & pid);

	/*!
	 * \brief Returns the system limits set for a process
	 *
	 * This returns the soft and the hard limits set for a process.
	 * They are obtained by reading the /proc/[pid]/limits file.
	 * The "unlimited" value is represented by the constant LIMIT_UNLIMITED
	 *
	 * The result is returned as a map. The keys are the name of the limit, as defined in
	 * LIMIT_NAMES. The values are pairs : the first member is the soft limit, the second is the hard one.
	 *
	 * @param pid The pid of the process we are interested in
	 * @return System limits set for the process
	 */
	std::map<std::string, std::pair<unsigned long int, unsigned long int> > proc_getProcessLimits(
			std::string & pid);

	/*!
	 * \brief Returns the status of the system limits for a process
	 *
	 * This returns the current value of a ressource which is limited by a system limit.
	 * The values are gathered thanks to the /proc/[pid]/stats and /proc/[pid]/status files
	 * and the /proc/[pid]/fd folder.
	 *
	 * The result is returned as a map. The keys are the name of the limit, as defined in
	 * LIMIT_NAMES. The values are the current status.
	 *
	 * @param pid The pid of the process we are interested in
	 * @return Status of the current system limits for the process
	 */
	std::map<std::string, unsigned long int> proc_getProcessLimitStatus(
			std::string & pid);

	/*!
	 * \brief Checks if a file is present
	 * @param fileName Name of the file we want to check
	 * @return True if the file is present, false otherwise
	 */
	bool file_fileExists(std::string & fileName);

	/*!
	 * \brief Returns the owner of a file
	 *
	 * @param fileName Name of the file we are interested in
	 * @return The owner of the file
	 */
	std::string file_getFileOwner(std::string & fileName);

	/*!
	 * \brief Returns the group of a file
	 *
	 * @param fileName Name of the file we are interested in
	 * @return The group to which the file belong
	 */
	std::string file_getFileGroup(std::string & fileName);

	/*!
	 * \brief Returns the permissions of a file
	 *
	 * The permissions are returned according to the linux octal standard :
	 * - 1 : execute
	 * - 2 : write
	 * - 4 : read
	 * The sum of the bit gives the complete permissions.
	 *
	 * The number returned has 3 digits : owner, group, others
	 *
	 * It does not return additonnal information like the sticky bit or so
	 * @param fileName Name of the file we are interested in
	 * @return Permissions of a file
	 */
	std::string file_getFilePermissions(std::string & fileName);

	/*!
	 * \brief Returns the extended attributes of a file
	 * Because it is file system dependant, so far we only return
	 * "-----------------e-"...
	 *
	 * @param fileName Name of the file we are interested in
	 * @return Extended attributes of the file
	 */
	std::string file_getFileAttributes(std::string & fileName);

	/*!
	 * \brief Returns the md5 check some of a file
	 *
	 *	The checksum is obtained by using the md5sum linux command
	 *
	 * @param fileName Name of the file we are interested in
	 * @return The md5 checksum
	 */
	std::string file_getFileMd5(std::string & fileName);

	/*!
	 * \Brief Check a file content against a list of regular expressions
	 *
	 * The method will load the content of the file, and evaluate against it every
	 * regular expression in the vector passed as parameter. The regular expressions
	 * should follow the POSIX standard.
	 *
	 * The map which is returned contained as key the regular expressions given as parameters.
	 * The value is a boolean set to true if the regular expression matched the file content, false otherwise.
	 *
	 * @param fileName Name of the file we are interested in
	 * @param regex List of regular expression to check
	 * @return A map containing the matching status of each regular expressions
	 */
	std::map<std::string, bool> file_getFileRegex(std::string & fileName,
			std::vector<std::string> & regex);

	/*!
	 * @brief Lists the content of a folder
	 *
	 * @param folderName name of the folder we want to list
	 * @return content of it
	 */
	std::vector<std::string> folder_getFolderContent(std::string & folderName,
			std::map<std::string, std::string> & extraArgs);

	connection_ptr m_tcp_connection; /*! boost::shared_ptr to the tcp_connection used to communicate with the client */
	Request m_request; /*! The Request sent by the user */

	bool is_leaving; /*! boolean probably not useful... */

	boost::thread::id m_currentCommandThreadId; /*!< the id of the current command thread being executed. Used to protect from concurrent write (not possible with normal mutex)*/
	boost::mutex m_threadIdMutex;
};

#endif
