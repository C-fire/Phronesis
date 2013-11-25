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


#ifndef RECOVERY_UNIT_HPP
#define RECOVERY_UNIT_HPP

/*!
 * \file RecoveryUnit.hpp
 * \brief RecoveryUnit class
 *
 *	This class represents one atomic action in order to recover from a problem.
 *
 * \author Christophe Haen
 */

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

/**
 * \enum recoveryAction
 * \brief Recovery actions
 *
 * Those constants are all the actions that can be taken
 */
typedef enum {
	RECOVERY_FILE_CREATE, /*!< Create a file */
	RECOVERY_FILE_CHGRP, /*!< Change the group of a file */
	RECOVERY_FILE_CHOWN, /*!< Change the owner of a file */
	RECOVERY_FILE_CHMOD, /*!< Change the permissions of a file */
	RECOVERY_FILE_CHATTR, /*!< Change the advanced attributes of a file */
	RECOVERY_FILE_PATTERN, /*!< Modify the content of the file */
	RECOVERY_FILE_MD5, /*!< Change the md5 of the file (so modify the content...) */
	RECOVERY_FILE_MOUNT_MISSING, /*!< If the mount point is missing, try to re mount it. Redundant with RECOVERY_ENV_MOUNT */
	RECOVERY_PROCESS_START, /*!< Start the process */
	RECOVERY_PROCESS_STOP, /*!< Stop the process*/
	RECOVERY_PROCESS_RESTART, /*!< Restart the process */
	RECOVERY_PROCESS_ULIMIT, /*!< Change process's limits*/
	RECOVERY_PROCESS_CHUSER, /*!< Change process's user */
	RECOVERY_PROCESS_ENVIRONMENT, /*!< Change the process environment*/
	RECOVERY_SERVICE_START, /*!< Start the service */
	RECOVERY_SERVICE_STOP, /*!< Stop the service */
	RECOVERY_SERVICE_RESTART, /*!< Restart the service */
	RECOVERY_SERVICE_RELOAD, /*!< Reload the service */
	RECOVERY_SERVICE_CHUSER, /*!< Change the service's user */
	RECOVERY_SERVICE_ENVIRONMENT, /*!< Change the environment of the service */
	RECOVERY_ENV_MOUNT, /*!< Mount a mountpoint */
	RECOVERY_ENV_UMOUNT, /*!< Unmount a mountpoint*/
	RECOVERY_ENV_DISK, /*!< Make disk space */
	RECOVERY_ENV_INODE, /*!< Make inode space */
	RECOVERY_ENV_FSTAB, /*!< Correct the fstab file */
	RECOVERY_ENV_CPU, /*!< Decrease the CPU load */
	RECOVERY_ENV_OVERLOAD, /*!< Decrease the overload on the server not due to entities we are reponsible for */
	RECOVERY_ENV_SWAP, /*!< Decrease the swap consumption */
	RECOVERY_ENV_MEM, /*!< Decrease the memory consumption */
	RECOVERY_ENV_ON, /*!< Turn on the machine */
	RECOVERY_ENV_OFF, /*!< Turn off the machine */
	RECOVERY_ENV_REBOOT, /*!< Reboot the machine */
	RECOVERY_ENV_AGENT, /*!< Restart the Agent */
	RECOVERY_ERROR
/*!< Action sent when we don't know what to do */
} recoveryAction;

/**
 * \brief Returns the recoveryAction from it's name
 *
 * \param ra : name of the recoveryAction
 * \return The recoveryAction matching the name, RECOVERY_ERROR if not found
 */

recoveryAction recoveryActionFromString(string ra);

/**
 * \brief Returns the recoveryAction name
 *
 * \param ra : recoveryAction
 * \return The name of the recoveryAction given as parameter, "RECOVERY_ERROR" if not found
 */
string recoveryActionToString(recoveryAction ra);

/*!
 * \class RecoveryUnit
 * \brief This class contains all the information of one step for a complete recovery
 *
 */
class RecoveryUnit {
private:
	string m_maName; /*!< MetaAgent name on which the recovery applies */
	string m_server; /*!< Name of the server on which it is running */
	string m_problem; /*!< Description of the problem */
	string m_solution; /*!< Description of the solution to the problem */
	string m_advancedAdvice; /*!< Advanced advises concerning the solution */
	string m_command; /*!< Command that executes the solution */
	recoveryAction m_action; /*!< recoveryAction that describes the recovery step */
	string m_actionStr; /*!< String version of the RecoveryAction */

public:

	/*!
	 *  \brief Base Constructor
	 */
	RecoveryUnit();


	/*!
	 *  \brief Constructor
	 *
	 *  \param[in] maName : name of the MetaAgent on which we are working
	 *  \param[in] server : name of the Server on which we are working
	 *  \param[in] action : recoveryAction that we will take
	 */
	RecoveryUnit(string maName, string server, recoveryAction action);

	/*!
	 *  \brief Constructor
	 *
	 *  \param[in] maName : name of the MetaAgent on which we are working
	 *  \param[in] server : name of the Server on which we are working
	 *  \param[in] actionStr : name of the recoveryAction that we will take
	 */
	RecoveryUnit(string maName, string server, string actionStr);

	/*!
	 * @brief Copy constructor
	 * @param source source RecoveryUnit
	 */
	RecoveryUnit(const RecoveryUnit& source);

	/*!
	 * @brief Assignment operator
	 * @param source source RecoveryUnit
	 * @return *this
	 */
	RecoveryUnit & operator=(const RecoveryUnit & source);

	/*!
	 *  \brief Getter for the problem description
	 *  \return The problem description
	 */
	string getProblem() const;

	/*!
	 *  \brief Getter for the Server name
	 *  \return The Server name
	 */
	string getServer() const;


	/*!
	 *  \brief Getter for the solution description
	 *  \return The description of the solution
	 */
	string getSolution() const;

	/*!
	 *  \brief Getter for the description of the RecoveryUnit
	 *  \return A string containing the problem, the solution, and advanced advices
	 */
	string getDescription() const;

	/*!
	 *  \brief Getter for the recoveryAction to be taken
	 *  \return The recoveryAction
	 */
	recoveryAction getAction() const;

	/*!
	 *  \brief Getter for the name of the recoveryAction
	 *  \return The name of the recoveryAction
	 */
	string getActionStr() const;

	/*!
	 *  \brief Getter for the advanced advice
	 *  \return Advanced advice
	 */
	string getAdvancedAdvice() const;

	/*!
	 *  \brief Getter for the command to execute the action
	 *  \return Command to execute the action
	 */
	string getCommand() const;

	/*!
	 *  \brief Getter for the MetaAgent name
	 *  \return The MetaAgent name
	 */
	string getMaName() const;

	/*!
	 *  \brief Setter for the problem description
	 *  \param[in] problem : the problem description
	 */
	void setProblem(string problem);

	/*!
	 *  \brief Setter for the solution description
	 *  \param[in] solution : the description of the solution
	 */
	void setSolution(string solution);

	/*!
	 *  \brief Setter for the recoveryAction to be taken
	 *  \param[in] action : the recoveryAction
	 */
	void setAction(recoveryAction action);

	/*!
	 *  \brief Setter for the name of the recoveryAction
	 *  \param[in] actionStr : the name of the recoveryAction
	 */
	void setActionStr(string actionStr);

	/*!
	 *  \brief Setter for the advanced advice
	 *  \param[in] advancedAdvice : Advanced advice
	 */
	void setAdvancedAdvice(string advancedAdvice);

	/*!
	 *  \brief Setter for the command to execute the action
	 *  \return command : command to execute the action
	 */
	void setCommand(string command);

	/*!
	 *  \brief Setter for the MetaAgent name
	 *  \param[in] maName : MetaAgent name
	 */
	void setMaName(string maName);

	/*!
	 *  \brief Setter for the Server name
	 *  \param[in] server : the Server name
	 */
	void setServer(string server);


	/*!
	 *  \brief Output stream operator
	 *
	 *  The output stream operator. It calls the printOperator method
	 *
	 *	\param[in] recUnit : reference to the RecoveryUnit to display
	 *	\param[in,out] Stream : the output stream
	 *  \return The output stream
	 */
	friend std::ostream& operator <<(std::ostream&,
			const RecoveryUnit& recUnit);

};


/*!
 *  \brief RecoveryUnit == operator
 *
 *	Compares if the MetaAgent names and the recoveryActions are the same
 *
 *	\param[in] r1 : reference to the first RecoveryUnit
 *	\param[in] r2 : reference to the second RecoveryUnit
 *
 *	\return true if r1 and r2 have the same MetaAgent names and recoveryActions, false otherwise
 */
bool operator==(RecoveryUnit const &r1, RecoveryUnit const &r2);

#endif
