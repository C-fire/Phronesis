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


#ifndef FILE_AGENT_HPP
#define FILE_AGENT_HPP

/*!
 * \file FileAgent.hpp
 * \brief FileAgent class
 * \author Christophe Haen
 */

#include <iostream>
#include <vector>
#include <stdio.h> //meat and potatoes
#include <string.h>

#include "Agent.hpp"
#include "Server.hpp"
#include "EnvironmentAgent.hpp"
#include "RecoveryEngineAbstract.hpp"

using namespace std;

class FileRecovery;
class RecoveryEngineAbstract;

/*! \class FileAgent
 * \brief Class for all the file concerns
 *
 *  The class is responsible for checking the proper state of a given file
 *  It inherits from the Agent class.
 */
class FileAgent: public Agent {
protected:

	string m_filename; /*!< path of the file */

	//expected values
	string m_owner; /*!< expected owner of the file */
	string m_group; /*!< expected group of the file */
	string m_perm; /*!< expected permissions of the file */
	string m_attr; /*!< expected attributes of the file */
	string m_md5; /*!< expected md5 checksum of the file */
	vector<string> m_patterns; /*!< list of patterns (regular expressions) supposed to match the file */

	//true values
	string m_true_owner; /*!< real owner of the file */
	string m_true_group; /*!< real group of the file */
	string m_true_perm; /*!< real permissions of the file */
	string m_true_attr; /*!< real attributes of the file */
	string m_true_md5; /*!< real md5 checksum of the file */

	MountpointInfo m_mountpoint; /*!< Information about the mountpoint on which the file is supposed to be */

	bool m_no_such_file; /*!< boolean set to true if the file does not exist*/
	bool m_notMounted; /*!< boolean set to true if the file is in a path which is not mounted */
	bool m_isStale; /*!< boolean set to true if the file is stale (specific to NFS?)*/
	bool m_owner_pb; /*!< boolean set to true if the owner is not correct */
	bool m_group_pb; /*!< boolean set to true if the group is not correct */
	bool m_perm_pb; /*!< boolean set to true if the permissions are not correct */
	bool m_attr_pb; /*!< boolean set to true if the attributes are not correct */
	bool m_md5_pb; /*!< boolean set to true if the md5 checksum is not correct */
	bool m_patterns_pb; /*!< boolean set to true if one or more pattern don't match the file*/
	vector<bool> m_patterns_pb_detail; /*!< vector of boolean to detail which pattern did not match */

	/*!
	 * @brief Initialize the flags to false
	 */
	void init();

public:

	/*!
	 *  \brief Constructor
	 *
	 *   Constructor of the FileAgent class
	 *  \param[in] id : unique identifier of the MetaAgent
	 *  \param[in] name : unique name of the MetaAgent
	 *  \param[in] server : pointer to the server on which the Agent is
	 *  \param[in] filename : name of the file (with the full path)
	 *  \param[in] owner : owner of the file
	 *  \param[in] group : group of the file
	 *  \param[in] perm : permissions of the file
	 *  \param[in] attr : attributes of the file
	 *  \param[in] md5 : md5 checksum of the file
	 *  \param[in] patterns : list of patters that should match the file
	 */
	FileAgent(int id, string name, Server * server, string filename,
			string owner, string group, string perm, string attr, string md5,
			vector<string> patterns);

	/*!
	 * @brief Default constructor
	 */
	FileAgent();

	/*!
	 * @brief Copy constructor
	 * @param source source FileAgent
	 */
	FileAgent(const FileAgent & source);

	using Agent::operator=;
	/*!
	 * @brief Assignment operator
	 * This operator uses Agent::operator=
	 * @param source source FileAgent
	 * @return *this
	 */
	FileAgent & operator=(const FileAgent& source);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the FileAgent class
	 */
	virtual ~FileAgent();


	bool analyze();

	/*!
	 * \brief Setter for the filename/path
	 *
	 * \param path : path to the file
	 */
	void setPath(string path);

	bool triggerRecover(RecoveryEngineAbstract * re) {
		return re->recover(this);
	}

	friend class FileRecovery;
	friend class ProcessRecovery;
};

#endif
