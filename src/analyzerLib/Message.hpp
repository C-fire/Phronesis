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


#ifndef TASK_MSG_H
#define TASK_MSG_H

/*!
 * \file Message.hpp
 * \brief All the Message classes that are needed for Phronesis to interact
 *
 * All the classes defined here are used for communication either between
 * the Core and the Remote Agent, or the Core and the API
 *
 * \author Christophe Haen
 */

#include <string>
#include <map>
#include <list>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

/*!
 \def LIMIT_UNLIMITED
 number that represents infinity for the limits
 */
#define LIMIT_UNLIMITED 4294967295

/*!
 * Names of the limits
 */
extern char LIMIT_NAME[16][22];

/*!
 * @class triplet
 * @brief This class is a templated class used to manager triplet
 *
 * We could have used the boost::tuple, however it was
 * a nightmare to serialize vector of tuples, so we wrote our own
 *
 */
template<typename T1, typename T2, typename T3>
class triplet {
private:
	T1 _first; //!< First member of the triplet
	T2 _second; //!< Second member of the triplet
	T3 _third; //!< Third member of the triplet
public:
	/*!
	 * @brief Constructor
	 * @param f First member of the triplet
	 * @param s Second member of the triplet
	 * @param t Third member of the triplet
	 */
	inline triplet(T1 f, T2 s, T3 t) {
		_first = f;
		_second = s;
		_third = t;
	}

	/*!
	 * @brief Copy constructor
	 * @param src Triplet to copy
	 */
	inline triplet(const triplet<T1, T2, T3> & src) {
		_first = src._first;
		_second = src._second;
		_third = src._third;
	}

	/*!
	 * @brief Default constructor
	 */
	inline triplet() {

	}

	/*!
	 * @brief Getter for the first element of the triplet
	 * @return The first element
	 */
	inline T1 getFirst() const {
		return _first;
	}

	/*!
	 * @brief Getter for the second element of the triplet
	 * @return The second element
	 */
	inline T2 getSecond() const {
		return _second;
	}

	/*!
	 * @brief Getter for the third element of the triplet
	 * @return The third element
	 */
	inline T3 getThird() const {
		return _third;
	}

	/*!
	 * @brief Setter for the first element of the triplet
	 * @param first first element of the triplet
	 */
	inline void setFirst(T1 first) {
		_first = first;
	}

	/*!
	 * @brief Setter for the second element of the triplet
	 * @param second second element of the triplet
	 */
	inline void setSecond(T2 second) {
		_second = second;
	}

	/*!
	 * @brief Setter for the third element of the triplet
	 * @param third third element of the triplet
	 */
	inline void setThird(T3 third) {
		_third = third;
	}

	/*!
	 * @brief Serialize the triplet
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & _first & _second & _third;
	}

	/*!
	 * @brief Assignment operator
	 * @param src triplet to copy
	 * @return reference to this
	 */
	inline triplet& operator=(const triplet & src) {
		_first = src._first;
		_second = src._second;
		_third = src._third;

		return *this;
	}
};

/*!
 * @class Request
 * @brief Represents the question one can ask to a Remote Agent
 */
class Request {
public:

	/*!
	 * All the possible types of request
	 */
	enum requestType {
		ALIVE, //!< Like a Ping to check whether the Remote Agent is alive and responding
		ENVIRONMENT,    //!< Request about the environment
		PID_TREE,       //!< Requests the pid tree from a process name
		PROCESS_NAME,   //!< PROCESS_NAME
		PROCESS_PID,    //!< Requests information about a process from its PID
		FILE,           //!< Requests information about a file from its path
		MOUNTPOINT_PATH,           //!< MOUNTPOINT_PATH
		FOLDER_CONTENT  //!< Request about the content of a folder from its path
	};

private:

	requestType m_type; //!< Type of the request
	std::string m_target; //!< Contains either the path to the file,process name, pid... depending on the request type
	std::map<std::string, bool> m_require; //!< Map containing the information we want to query
	std::vector<std::string> m_regex; //!< Used for passing the regex a file as to match
	std::map<std::string, std::string> m_extraArguments; //!< Used to passed extra arguments, like the max depths
	bool m_fetchEverything; //!< When set to true, m_require is ignored and one collects all the possible information

	/*!
	 * @brief Used by the stream operator
	 * @param out stream to write in
	 * @return stream given as input
	 */
	std::ostream& printOperator(std::ostream& out) const {
		out << "m_type " << m_type << " m_target " << m_target
				<< " fetchEverything " << m_fetchEverything;
		std::map<std::string, bool>::const_iterator it = m_require.begin();

		out << "Requires : " << std::endl;
		for (it = m_require.begin(); it != m_require.end(); it++) {
			out << it->first << "-> " << it->second << std::endl;
			;
		}

		std::map<std::string, std::string>::const_iterator its =
				m_extraArguments.begin();
		out << "ExtraArgument : " << std::endl;
		for (its = m_extraArguments.begin(); its != m_extraArguments.end();
				its++) {
			out << its->first << " -> " << its->second << std::endl;
		}

		return out;
	}

	/*!
	 * @brief Stream operator
	 * @param out Stream to write in
	 * @param msg Request to write in the stream
	 * @return The stream given as input
	 */
	friend std::ostream& operator<<(std::ostream& out, const Request& msg) {
		return msg.printOperator(out);
	}

public:

	/*!
	 * @brief Constructor
	 *
	 * @param type Type of Request
	 * @param target Target for our request
	 * @param require List of information we want
	 */
	Request(requestType type, std::string target,
			std::map<std::string, bool> require) :
			m_type(type), m_target(target), m_require(require) {
		m_fetchEverything = false;
	}

	/*!
	 * @brief Constructor
	 *
	 * This constructor will set the fetchEverything flat to true,
	 * so we will gather all the information possible about the target
	 *
	 * @param type Type of Request
	 * @param target Target for our request
	 */
	Request(requestType type, std::string target) :
			m_type(type), m_target(target) {
		m_fetchEverything = true;
	}

	/*!
	 * @brief Default constructor
	 *
	 * Construct an ALIVE Request
	 */
	Request() {
		m_type = ALIVE;
		m_fetchEverything = false;
	}

	/*!
	 * @brief Copy constructor
	 * @param src Request to copy
	 */
	Request(const Request & src) {

		m_type = src.m_type;
		m_target = src.m_target;
		m_require = src.m_require;
		m_regex = src.m_regex;
		m_extraArguments = src.m_extraArguments;
		m_fetchEverything = src.m_fetchEverything;
	}

	/*!
	 * @brief Assignment operator
	 * @param src Request to copy
	 * @return Reference to this
	 */
	Request & operator=(const Request & src) {

		m_type = src.m_type;
		m_target = src.m_target;
		m_require = src.m_require;
		m_regex = src.m_regex;
		m_extraArguments = src.m_extraArguments;
		m_fetchEverything = src.m_fetchEverything;

		return *this;
	}

	/*!
	 * @brief Serialize the Request
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_type & m_target & m_require & m_regex & m_fetchEverything
				& m_extraArguments;
	}

	/*!
	 * @brief Getter for the required information
	 * @return Required information
	 */
	std::map<std::string, bool> getRequire() const {
		return m_require;
	}

	/*!
	 * @brief Getter for the target of our request
	 * @return Target of our request
	 */
	std::string getTarget() const {
		return m_target;
	}

	/*!
	 * @brief Getter for the type of Request
	 * @return Type of the Request
	 */
	requestType getType() const {
		return m_type;
	}

	/*!
	 * @brief Getter for the pattern list
	 * @return List of pattern the targeted file has to match
	 */
	std::vector<std::string> getRegex() const {
		return m_regex;
	}

	/*!
	 * @brief Getter for the fetchEverything flag
	 * @return True if we want to fetch all the information, false otherwise
	 */
	bool getFetchEverything() const {
		return m_fetchEverything;
	}

	/*!
	 * @brief Setter for the required information
	 * @param require Map of required information
	 */
	void setRequire(std::map<std::string, bool> require) {
		m_require = require;
	}

	/*!
	 * @brief Setter for the target
	 * @param target Our target
	 */
	void setTarget(std::string target) {
		m_target = target;
	}

	/*!
	 * @brief Setter for the requet type
	 * @param type Type of the Request
	 */
	void setType(requestType type) {
		m_type = type;
	}

	/*!
	 * @brief Setter for the list of pattern a file should match
	 * @param regex list of pattern
	 */
	void setRegex(std::vector<std::string> regex) {
		m_regex = regex;
	}

	/*!
	 * @brief Setter for the fetchEverything flag
	 * @param fetchEverything Flag value
	 */
	void setFetchEverything(bool fetchEverything) {
		m_fetchEverything = fetchEverything;
	}

	/*!
	 * @brief Add an extra argument to the Request
	 * @param arg name of the argument
	 * @param value value of the argument
	 */
	void addExtraArgument(std::string arg, std::string value) {
		m_extraArguments[arg] = value;
	}

	/*!
	 * @brief Setter for the extra arguments
	 * @param extraArgs Map of extra arguments and their values
	 */
	void setExtraArguments(std::map<std::string, std::string> extraArgs) {
		m_extraArguments = extraArgs;
	}

	/*!
	 * @brief Checks whether a given extra argument is given
	 * @param arg name of the extra argument
	 * @return True if the argument is given, false otherwise
	 */
	bool hasExtraArgument(std::string arg) {
		return (m_extraArguments.find(arg) != m_extraArguments.end());
	}

	/*!
	 * @brief Get the value of an extra argument
	 * @param arg Name of the extra argument
	 * @return Value of the extra argument
	 */
	std::string getExtraArgument(std::string arg) {
		return m_extraArguments[arg];
	}

	/*!
	 * @brief Getter for the map of extra arguments
	 * @return Map of extra arguments
	 */
	std::map<std::string, std::string> getExtraArgumentMap() const{
		return m_extraArguments;
	}

};

/*!
 * @class envInfo
 * @brief Contains information about the environment
 */
class envInfo {

public:

	float m_cpu; //!< cpu load
	float m_mem; //!< percentage of memory used
	float m_swap; //!< percentage of swap space used
	std::map<std::string, float> m_diskSpace; //!< Percentage of used space for each local mountpoint
	std::map<std::string, float> m_inodes; //!< Percentage of used inodes for each local mountpoint
	std::vector<triplet<std::string, float, std::string> > m_cpuConsumers; //!< list of top cpu consumers : pid - cpu consumed in % - command line
	std::vector<triplet<std::string, float, std::string> > m_memConsumers; //!< list of top memory consumers : pid - memory consumed in % - command line

	std::map<std::string, std::string> m_runtimeErrors; //!< List of exception that were thrown  when querying the information

	/*!
	 * @brief Default constructor
	 */
	envInfo() {
		m_cpu = 0;
		m_mem = 0;
		m_swap = 0;
	}

	/*!
	 * @brief Copy constructor
	 * @param src envInfo to copy
	 */
	envInfo(const envInfo & src) {
		m_cpu = src.m_cpu;
		m_mem = src.m_mem;
		m_swap = src.m_swap;
		m_diskSpace = src.m_diskSpace;
		m_inodes = src.m_inodes;
		m_cpuConsumers = src.m_cpuConsumers;
		m_memConsumers = src.m_memConsumers;
		m_runtimeErrors = src.m_runtimeErrors;
	}

	/*!
	 * @brief assignment operator
	 * @param src envInfo to copy
	 * @return reference to this
	 */
	envInfo & operator=(const envInfo & src) {
		m_cpu = src.m_cpu;
		m_mem = src.m_mem;
		m_swap = src.m_swap;
		m_diskSpace = src.m_diskSpace;
		m_inodes = src.m_inodes;
		m_cpuConsumers = src.m_cpuConsumers;
		m_memConsumers = src.m_memConsumers;
		m_runtimeErrors = src.m_runtimeErrors;
		return *this;
	}

	friend class boost::serialization::access;
	/*!
	 * @brief Serialize the envInfo
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_cpu & m_mem & m_swap & m_diskSpace & m_inodes & m_cpuConsumers
				& m_memConsumers & m_runtimeErrors;
	}

	/*!
	 * @brief Used by the stream operator
	 * @param out stream to write in
	 * @return stream given as input
	 */
	std::ostream& printOperator(std::ostream& out) const {
		out << "m_cpu " << m_cpu << " m_mem " << m_mem << " m_swap " << m_swap
				<< std::endl;

		std::map<std::string, float>::const_iterator it = m_diskSpace.begin();

		for (it = m_diskSpace.begin(); it != m_diskSpace.end(); it++) {
			out << it->first << "-> " << it->second << std::endl;
			;
		}
		out << "CPU consumers" << std::endl;
		std::vector<triplet<std::string, float, std::string> >::const_iterator it2;
		for (it2 = m_cpuConsumers.begin(); it2 != m_cpuConsumers.end(); it2++) {
			out << it2->getFirst() << "-> " << it2->getSecond() << "-> "
					<< it2->getThird() << std::endl;
			;
		}

		out << "Mem consumers" << std::endl;
		for (it2 = m_memConsumers.begin(); it2 != m_memConsumers.end(); it2++) {
			out << it2->getFirst() << "-> " << it2->getSecond() << "-> "
					<< it2->getThird() << std::endl;
			;
		}
		std::map<std::string, std::string>::const_iterator it3;
		for (it3 = m_runtimeErrors.begin(); it3 != m_runtimeErrors.end(); it3++)
			out << "ERROR : " << it3->first << " -> " << it3->second
					<< std::endl;

		return out;
	}

	/*!
	 * @brief Stream operator
	 * @param out Stream to write in
	 * @param env envInfo to write into the stream
	 * @return Stream given as input
	 */
	friend std::ostream& operator<<(std::ostream& out, const envInfo& env) {
		return env.printOperator(out);
	}

	/*!
	 * @brief Check whether a given information encountered an exception when querying the information
	 * @param target Information we are looking for
	 * @return True if an exception was thrown, false otherwise
	 */
	bool hasRuntimeError(std::string target) {
		return (m_runtimeErrors.find(target) != m_runtimeErrors.end());
	}

};

/*!
 * @class procInfo
 * @brief Contains information about a process
 */
class procInfo {

public:
	std::string m_user; //!< user of the process
	float m_cpu; //!< cpu consumption of the process
	float m_mem; //!< memory consumption of the process
	std::string m_cmdLine; //!< command line of the process, as seen in the linux ps command
	std::map<std::string, std::pair<unsigned long int, unsigned long int> > m_limits; //!< system limits of the process (limit name : < soft , hard > )
	std::map<std::string, unsigned long int> m_limitStatus; //!< limit status of the process (limit name : value)

	std::map<std::string, std::string> m_runtimeErrors; //!< List of exception that were thrown  when querying the information

	/*!
	 * @brief Default constructor
	 */
	procInfo() {
		m_cpu = 0;
		m_mem = 0;
	}

	/*!
	 * @brief Copy constructor
	 * @param src procInfo to copy
	 */
	procInfo(const procInfo & src) {
		m_user = src.m_user;
		m_cpu = src.m_cpu;
		m_mem = src.m_mem;
		m_cmdLine = src.m_cmdLine;
		m_limits = src.m_limits;
		m_limitStatus = src.m_limitStatus;
		m_runtimeErrors = src.m_runtimeErrors;
	}

	/*!
	 * @brief assignment operator
	 * @param src procInfo to copy
	 * @return reference to this
	 */
	procInfo & operator=(const procInfo & src) {
		m_user = src.m_user;
		m_cpu = src.m_cpu;
		m_mem = src.m_mem;
		m_cmdLine = src.m_cmdLine;
		m_limits = src.m_limits;
		m_limitStatus = src.m_limitStatus;
		m_runtimeErrors = src.m_runtimeErrors;

		return *this;
	}

	friend class boost::serialization::access;
	/*!
	 * @brief Serialize the envInfo
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_user & m_cpu & m_mem & m_cmdLine & m_limits & m_limitStatus
				& m_runtimeErrors;
	}

	/*!
	 * @brief Used by the stream operator
	 * @param out stream to write in
	 * @return stream given as input
	 */
	std::ostream& printOperator(std::ostream& out) const {
		out << "m_user " << m_user << " m_cpu " << m_cpu << " m_mem " << m_mem
				<< " m_fullLine " << m_cmdLine << std::endl;

		std::map<std::string, std::pair<unsigned long int, unsigned long int> >::const_iterator it;
		for (it = m_limits.begin(); it != m_limits.end(); it++) {
			out << it->first << "->" << it->second.first << " "
					<< it->second.second << std::endl;
		}
		out << std::endl;

		std::map<std::string, unsigned long int>::const_iterator it2;
		for (it2 = m_limitStatus.begin(); it2 != m_limitStatus.end(); it2++) {
			out << it2->first << "->" << it2->second << std::endl;
		}
		out << std::endl;

		std::map<std::string, std::string>::const_iterator it3;
		for (it3 = m_runtimeErrors.begin(); it3 != m_runtimeErrors.end(); it3++)
			out << "ERROR : " << it3->first << " -> " << it3->second
					<< std::endl;

		return out;
	}

	/*!
	 * @brief Stream operator
	 * @param out Stream to write in
	 * @param proc procInfo to write into the stream
	 * @return Stream given as input
	 */
	friend std::ostream& operator<<(std::ostream& out, const procInfo& proc) {
		return proc.printOperator(out);
	}

	/*!
	 * @brief Check whether a given information encountered an exception when querying the information
	 * @param target Information we are looking for
	 * @return True if an exception was thrown, false otherwise
	 */
	bool hasRuntimeError(std::string target) {
		return (m_runtimeErrors.find(target) != m_runtimeErrors.end());
	}

};

/*!
 * @class pidTreeInfo
 * @brief Contains the pid tree of a process
 */
class pidTreeInfo {

public:
	std::map<int, std::vector<int> > m_pidTree; //!< pid tree ( ppid : children pids )
	std::map<std::string, std::string> m_runtimeErrors; //!< List of exception that were thrown  when querying the information

	/*!
	 * @brief Default constructor
	 */
	pidTreeInfo() {

	}

	/*!
	 * @brief Copy constructor
	 * @param src pidTreeInfo to copy
	 */
	pidTreeInfo(const pidTreeInfo & src) {
		m_pidTree = src.m_pidTree;
		m_runtimeErrors = src.m_runtimeErrors;
	}

	/*!
	 * @brief assignment operator
	 * @param src pidTreeInfo to copy
	 * @return reference to this
	 */
	pidTreeInfo & operator=(const pidTreeInfo & src) {
		m_pidTree = src.m_pidTree;
		m_runtimeErrors = src.m_runtimeErrors;

		return *this;
	}

	friend class boost::serialization::access;
	/*!
	 * @brief Serialize the envInfo
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_pidTree & m_runtimeErrors;
	}

	/*!
	 * @brief Used by the stream operator
	 * @param out stream to write in
	 * @return stream given as input
	 */
	std::ostream& printOperator(std::ostream& out) const {
		out << "pidTree " << std::endl;
		std::map<int, std::vector<int> >::const_iterator it;
		std::vector<int>::const_iterator it2;
		for (it = m_pidTree.begin(); it != m_pidTree.end(); it++) {
			out << it->first << " -> ";
			for (it2 = it->second.begin(); it2 != it->second.end(); it2++) {
				out << *it2 << ",";
			}
			out << std::endl;
		}

		std::map<std::string, std::string>::const_iterator it3;
		for (it3 = m_runtimeErrors.begin(); it3 != m_runtimeErrors.end(); it3++)
			out << "ERROR : " << it3->first << " -> " << it3->second
					<< std::endl;

		return out;
	}

	/*!
	 * @brief Stream operator
	 * @param out Stream to write in
	 * @param proc pidTreeInfo to write into the stream
	 * @return Stream given as input
	 */
	friend std::ostream& operator<<(std::ostream& out,
			const pidTreeInfo& proc) {
		return proc.printOperator(out);
	}

	/*!
	 * @brief Check whether a given information encountered an exception when querying the information
	 * @param target Information we are looking for
	 * @return True if an exception was thrown, false otherwise
	 */
	bool hasRuntimeError(std::string target) {
		return (m_runtimeErrors.find(target) != m_runtimeErrors.end());
	}

};

/*!
 * @class fileInfo
 * @brief Contains the information about a file
 */
class fileInfo {

public:

	bool m_fileExists; //!< Flag set to true if the file is found
	std::string m_owner; //!< Owner of the file
	std::string m_group; //!< Group of the file
	std::string m_permissions; //!< Permissions of the file (3 octal format)
	std::string m_attributes; //!< Attributes of the file
	std::string m_md5; //!< md5 checksum of the file
	std::map<std::string, bool> m_regex; //!< Map containing the patterns as key, and a boolean which says whether it was matched or not

	std::map<std::string, std::string> m_runtimeErrors; //!< List of exception that were thrown  when querying the information

	/*!
	 * @brief Default constructor
	 */
	fileInfo() {
		m_fileExists = false;
	}

	/*!
	 * @brief Copy constructor
	 * @param src fileInfo to copy
	 */
	fileInfo(const fileInfo & src) {
		m_fileExists = src.m_fileExists;
		m_owner = src.m_owner;
		m_group = src.m_group;
		m_permissions = src.m_permissions;
		m_attributes = src.m_attributes;
		m_md5 = src.m_md5;
		m_regex = src.m_regex;
		m_runtimeErrors = src.m_runtimeErrors;
	}

	/*!
	 * @brief assignment operator
	 * @param src fileInfo to copy
	 * @return reference to this
	 */
	fileInfo & operator=(const fileInfo & src) {
		m_fileExists = src.m_fileExists;
		m_owner = src.m_owner;
		m_group = src.m_group;
		m_permissions = src.m_permissions;
		m_attributes = src.m_attributes;
		m_md5 = src.m_md5;
		m_regex = src.m_regex;
		m_runtimeErrors = src.m_runtimeErrors;

		return *this;
	}

	friend class boost::serialization::access;
	/*!
	 * @brief Serialize the fileInfo
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_fileExists & m_owner & m_group & m_permissions & m_attributes
				& m_md5 & m_runtimeErrors & m_regex;
	}

	/*!
	 * @brief Used by the stream operator
	 * @param out stream to write in
	 * @return stream given as input
	 */
	std::ostream& printOperator(std::ostream& out) const {
		out << "m_fileExists " << m_fileExists << " m_owner " << m_owner
				<< " m_group " << m_group << " m_permissions " << m_permissions
				<< " m_attributes " << m_attributes << " m_md5 " << m_md5
				<< std::endl;
		for (std::map<std::string, bool>::const_iterator it2 = m_regex.begin();
				it2 != m_regex.end(); it2++) {
			out << it2->first << " match " << it2->second << std::endl;
		}
		std::map<std::string, std::string>::const_iterator it;
		for (it = m_runtimeErrors.begin(); it != m_runtimeErrors.end(); it++)
			out << "ERROR : " << it->first << " -> " << it->second << std::endl;

		return out;
	}

	/*!
	 * @brief Stream operator
	 * @param out Stream to write in
	 * @param f fileInfo to write into the stream
	 * @return Stream given as input
	 */
	friend std::ostream& operator<<(std::ostream& out, const fileInfo& f) {
		return f.printOperator(out);
	}

	/*!
	 * @brief Check whether a given information encountered an exception when querying the information
	 * @param target Information we are looking for
	 * @return True if an exception was thrown, false otherwise
	 */
	bool hasRuntimeError(std::string target) {
		return (m_runtimeErrors.find(target) != m_runtimeErrors.end());
	}

};

/*!
 * @class MountpointInfo
 * @brief Contains information about a mountpoint
 */
class MountpointInfo {

public:
	std::string m_device; //!< Device which is mounted
	std::string m_dir; //!< Directory on which it is mounted
	std::string m_type; //!< File system type
	std::string m_options; //!< Mount options
	bool m_mounted; //!< Flag true if the file system is mounted
	bool m_shouldBeRw; //!< Flag true if the options contain "rw"
	bool m_isRw; //!< Flag true if it is actually possible to write (this is incomplete and should not be trust now)

	std::map<std::string, std::string> m_runtimeErrors; //!< List of exception that were thrown  when querying the information

	/*!
	 * @brief Default constructor
	 */
	MountpointInfo() {
		m_mounted = false;
		m_shouldBeRw = false;
		m_isRw = false;
	}

	/*!
	 * @brief Copy constructor
	 * @param src MountpointInfo to copy
	 */
	MountpointInfo(const MountpointInfo & src) {
		m_device = src.m_device;
		m_dir = src.m_dir;
		m_type = src.m_type;
		m_options = src.m_options;
		m_mounted = src.m_mounted;
		m_shouldBeRw = src.m_shouldBeRw;
		m_isRw = src.m_isRw;
		m_runtimeErrors = src.m_runtimeErrors;
	}

	/*!
	 * @brief assignment operator
	 * @param src MountpointInfo to copy
	 * @return reference to this
	 */
	MountpointInfo & operator=(const MountpointInfo & src) {
		m_device = src.m_device;
		m_dir = src.m_dir;
		m_type = src.m_type;
		m_options = src.m_options;
		m_mounted = src.m_mounted;
		m_shouldBeRw = src.m_shouldBeRw;
		m_isRw = src.m_isRw;
		m_runtimeErrors = src.m_runtimeErrors;
		return *this;
	}

	friend class boost::serialization::access;
	/*!
	 * @brief Serialize the MountpointInfo
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_device & m_dir & m_type & m_options & m_mounted & m_shouldBeRw
				& m_isRw & m_runtimeErrors;
	}

	/*!
	 * @brief Used by the stream operator
	 * @param out stream to write in
	 * @return stream given as input
	 */
	std::ostream& printOperator(std::ostream& out) const {
		out << "m_device " << m_device << " m_dir " << m_dir << " m_type "
				<< m_type << " m_options " << m_options << " m_mounted "
				<< m_mounted << " m_shouldBeRw " << m_shouldBeRw << " m_isRw "
				<< m_isRw << std::endl;

		std::map<std::string, std::string>::const_iterator it3;
		for (it3 = m_runtimeErrors.begin(); it3 != m_runtimeErrors.end(); it3++)
			out << "ERROR : " << it3->first << " -> " << it3->second
					<< std::endl;

		return out;
	}

	/*!
	 * @brief Stream operator
	 * @param out Stream to write in
	 * @param mount mountpointInfo to write into the stream
	 * @return Stream given as input
	 */
	friend std::ostream& operator<<(std::ostream& out,
			const MountpointInfo& mount) {
		return mount.printOperator(out);
	}

	/*!
	 * @brief Check whether a given information encountered an exception when querying the information
	 * @param target Information we are looking for
	 * @return True if an exception was thrown, false otherwise
	 */
	bool hasRuntimeError(std::string target) {
		return (m_runtimeErrors.find(target) != m_runtimeErrors.end());
	}

};

/*!
 * @class folderInfo
 * @brief Contains list of file and folder in a given folder
 */
class folderInfo {

public:

	std::vector<std::string> m_fileList; //!< List of files
	std::map<std::string, std::string> m_runtimeErrors; //!< List of exception that were thrown  when querying the information

	/*!
	 * @brief Default constructor
	 */
	folderInfo() {

	}

	/*!
	 * @brief Copy constructor
	 * @param src folderInfo to copy
	 */
	folderInfo(const folderInfo & src) {
		m_fileList = src.m_fileList;
		m_runtimeErrors = src.m_runtimeErrors;
	}

	/*!
	 * @brief assignment operator
	 * @param src folderInfo to copy
	 * @return reference to this
	 */
	folderInfo & operator=(const folderInfo & src) {
		m_fileList = src.m_fileList;
		m_runtimeErrors = src.m_runtimeErrors;

		return *this;
	}

	friend class boost::serialization::access;
	/*!
	 * @brief Serialize the folderInfo
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_fileList & m_runtimeErrors;
	}

	/*!
	 * @brief Used by the stream operator
	 * @param out stream to write in
	 * @return stream given as input
	 */
	std::ostream& printOperator(std::ostream& out) const {
		for (std::vector<std::string>::const_iterator it2 = m_fileList.begin();
				it2 != m_fileList.end(); it2++) {
			out << *it2 << std::endl;
		}
		std::map<std::string, std::string>::const_iterator it;
		for (it = m_runtimeErrors.begin(); it != m_runtimeErrors.end(); it++)
			out << "ERROR : " << it->first << " -> " << it->second << std::endl;

		return out;
	}

	/*!
	 * @brief Stream operator
	 * @param out Stream to write in
	 * @param f folderInfo to write into the stream
	 * @return Stream given as input
	 */
	friend std::ostream& operator<<(std::ostream& out, const folderInfo& f) {
		return f.printOperator(out);
	}

	/*!
	 * @brief Check whether a given information encountered an exception when querying the information
	 * @param target Information we are looking for
	 * @return True if an exception was thrown, false otherwise
	 */
	bool hasRuntimeError(std::string target) {
		return (m_runtimeErrors.find(target) != m_runtimeErrors.end());
	}

};

/*!
 * @class AliveMsg
 * @brief Simple ping-like message
 */
class AliveMsg {

public:
	std::string m_msg; //!< Dummy text message

	/*!
	 * @brief Default constructor
	 */
	AliveMsg() {

	}

	/*!
	 * @brief Copy constructor
	 * @param src AliveMsg to copy
	 */
	AliveMsg(const AliveMsg & src) {
		m_msg = src.m_msg;
	}

	/*!
	 * @brief assignment operator
	 * @param src AliveMsg to copy
	 * @return reference to this
	 */
	AliveMsg & operator=(const AliveMsg & src) {
		m_msg = src.m_msg;

		return *this;
	}

	friend class boost::serialization::access;
	/*!
	 * @brief Serialize the AliveMsg
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_msg;
	}

	/*!
	 * @brief Used by the stream operator
	 * @param out stream to write in
	 * @return stream given as input
	 */
	std::ostream& printOperator(std::ostream& out) const {
		out << m_msg;

		return out;
	}

	/*!
	 * @brief Stream operator
	 * @param out Stream to write in
	 * @param alive AliveMsg to write into the stream
	 * @return Stream given as input
	 */
	friend std::ostream& operator<<(std::ostream& out, const AliveMsg& alive) {
		return alive.printOperator(out);
	}

};

/*!
 * @class api_message
 * @brief Class used to exchange message between the Core and the API
 */
class api_message {
public:

	/*!
	 * @brief Default constructor
	 */
	api_message() {
		m_type = ERROR_TEXT;
		reset();
	}

	/*!
	 * @brief Constructor
	 *
	 * The id can be used either to reference a question or a message in the Core,
	 * but also to pass a integer value (like for statistic counters, or as boolean for veto). It is dependent
	 * on the message type.
	 *
	 * The text field contains the answer to a question or can be used like a "subject:
	 * (for example, the name of the statistic counter we want to increase)
	 *
	 * @param type Type of message
	 * @param id Integer to associate to the message
	 * @param text Text to associate to the message
	 */
	api_message(int type, int id, std::string text) :
			m_type(type), m_id(id), m_text(text) {
	}
	;

	/*!
	 * @brief perform initialization of the values
	 */
	void reset() {
		m_text.clear();
		m_date.clear();
		m_time.clear();
		m_id = -1;
	}

	int m_type; //!< Type of the event : NEW_MSG, etc.
	int m_id; //!< Index of the question/message in the Core, or values
	std::string m_text; //!< Text of the message
	std::string m_date; //!< Date at which the message was produced
	std::string m_time; //!< Time at which the message was produced

	/*!
	 * @brief Serialize the api_message
	 *
	 * This method is needed and used by boost::asio
	 * @param ar Archive in which to store the information
	 * @param version Version of the serialization
	 */
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & m_type & m_id & m_text & m_date & m_time;
	}

	/*!
	 * All the possible type of message
	 */
	enum {
		NEW_MSG, //!< A new message is delivered by the Core (id : index; msg : message)
		NEW_LOG, //!< A new log is delivered by the Core (id : -1 ; msg : log message)
		NEW_QUESTION, //!< A new question is asked by the Core. We normaly use NEW_YN_QUESTION or NEW_PB_QUESTION (id : index ; msg : question )
		NEW_YN_QUESTION, //!< A new Yes-or-No question is asked by the Core (id : index ; msg : question )
		NEW_PB_QUESTION, //!< The Core asks for a problem list (id : index ; msg : question )
		ACK_MSG, //!< Acknowledge a message (id : index ; msg : empty )
		ANSWER_QUESTION, //!< Answer a question (id : index ; msg : answer )
		CURRENT_PB, //!< Used by the Core to signal what are the problems being treated (id : -1 ; msg : problem list)
		GIVE_PB, //!< Give the problem list (id : -1 ; msg : problem list)
		SET_VETO, //!< Set the value of a Veto (id : used as boolean; msg : name of the veto)
		GET_VETO, //!< ASK the value of a Veto from the Core (id : -1 ; msg : name of the veto)
		NEW_VETO, //!< Used by the Core to give the value of a Veto (id : value of the veto; msg : name of the veto)
		STATISTIC, //!< Sends statistics to the observer (id : value (not always relevant, because sometimes the client might keeps the count); msg : statistic counter name)
		ERROR_TEXT //!< Error in the communication or in the processing (id : -1 ; msg : error message (exception) )
	};

	/*!
	 * @brief Stream operator
	 * @param out Stream to write in
	 * @param msg api_message to write into the stream
	 * @return Stream given as input
	 */
	friend std::ostream& operator <<(std::ostream & out, api_message & msg) {
		out << "m_type " << msg.m_type << " m_id " << msg.m_id << " m_text "
				<< msg.m_text << "m_date " << msg.m_date << " m_time "
				<< msg.m_time;
		return out;
	}
};

#endif
