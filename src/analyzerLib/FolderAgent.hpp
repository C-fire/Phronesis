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


#ifndef FOLDER_AGENT_HPP
#define FOLDER_AGENT_HPP

/*!
 * \file FolderAgent.hpp
 * \brief FolderAgent class
 * \author Christophe Haen
 */

#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <boost/regex.hpp>

#include "Agent.hpp"
#include "Server.hpp"
#include "RecoveryEngineAbstract.hpp"
#include "FileAgent.hpp"

using namespace std;

class RecoveryEngineAbstract;

/*! \class FolderAgent
 * \brief Class to describe a Folder
 *
 * It is never used to analyze a folder, but rather create fileAgent for all the content of the folder
 *
 */
class FolderAgent: public Agent {
protected:

	string m_dirname; /*!< path of the file */

	//expected values
	string m_owner; /*!< expected owner of the file */
	string m_group; /*!< expected group of the file */
	string m_perm; /*!< expected permissions of the file */
	string m_attr; /*!< expected attributes of the file */
	vector<string> m_patterns; /*!< list of patterns (regular expressions) supposed to match the file */

	bool m_no_such_folder; /*!< flag set to true if the folder does not exist */
	bool m_notMounted; /*!< flag set to true if the path where the folder is supposed to be is not mounted */

	FileAgent * m_filePtr; /*!< Pointer to a FileAgent which keeps a temporary object created to analyze the content of the folder. */

	int m_maxDepth; /*!< depth at which we check the folder */

	std::vector<std::string> m_filenameFilters; /*!< Only the filenames matching any of these filters will be analyzed */

	/*!
	 * Perform a few initialization of flags and pointers
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
	 *  \param[in] dirname : name of the folder (with the full path)
	 *  \param[in] owner : owner of the file
	 *  \param[in] group : group of the file
	 *  \param[in] perm : permissions of the file
	 *  \param[in] attr : attributes of the file
	 *  \param[in] patterns : list of patters that should match the file
	 *  \param[in] maxDepth : maximum depth at which we check the folder (0 is no limit)
	 *  \param[in] filenameFilters : filters on the name of the file. Only the files matching one or more of the filter will be analyzed
	 */
	FolderAgent(int id, string name, Server * server, string dirname,
			string owner, string group, string perm, string attr,
			vector<string> patterns, int maxDepth,
			std::vector<std::string> filenameFilters);

	/*!
	 * @brief Default constructor
	 */
	FolderAgent();

	/*!
	 * @brief Copy constructor
	 * @param source source FileAgent
	 */
	FolderAgent(const FolderAgent & source);

	using Agent::operator=;
	/*!
	 * @brief Assignment operator
	 * This operator uses Agent::operator=
	 * @param source source FileAgent
	 * @return *this
	 */
	FolderAgent & operator=(const FolderAgent& source);

	/*!
	 *  \brief Destructor
	 *
	 *  Destructor of the FolderAgent class
	 */
	virtual ~FolderAgent();

	bool analyze();

	/*!
	 * \brief Setter for the filename/path
	 *
	 * \param path : path to the file
	 */
	void setPath(string path);

	bool triggerRecover(RecoveryEngineAbstract * re) {
		return re->recover(m_filePtr);
	}

};

#endif
