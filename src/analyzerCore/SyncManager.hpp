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


#ifndef SYNC_MANAGER_HPP
#define SYNC_MANAGER_HPP

/*!
 * \file SyncManager.hpp
 * \brief SyncManager class
 * \author Christophe Haen
 */

#include <semaphore.h>
//#include "global.h"

/*!
 * @class SyncManager
 * @brief This class is responsible for the thread synchronization
 *
 * It is used to synchronize the threads, especially with what concerns
 * the UserInteraction
 *
 */
class SyncManager {

private:
//	sem_t mutex; /*!< maybe not used anymore...*/
	sem_t M_read; /*!< mutex for the sync on the readers attribute*/
	sem_t M_write; /*!< mutex for the sync between writer */
	sem_t S_write; /*!< mutex to avoid to write while someone reads */
	int readers; /*!< amounter of readers */

public:

	/*!
	 * @brief Constructor
	 *
	 * Initializes all the semaphores
	 */
	SyncManager();

	/*!
	 * @brief Destructor
	 *
	 * Destroy all the semaphores
	 */
	~SyncManager();

	/*!
	 * @brief Start to write in a shared resource
	 */
	void start_write();

	/*!
	 * @brief Finish to write in a shared resource
	 */
	void end_write();

	/*!
	 * @brief Start to read a shared resource
	 */
	void start_read();

	/*!
	 * @brief Finish to read a shared resource
	 */
	void end_read();

};

#endif
