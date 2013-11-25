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


#include "Message.hpp"


char LIMIT_NAMES[16][22] = { "MAX_CPU_TIME", "MAX_FILE_SIZE", "MAX_DATA_SIZE",
		"MAX_STACK_SIZE", "MAX_CORE_FILE_SIZE", "MAX_RESIDENT_SET",
		"MAX_PROCESSES", "MAX_OPEN_FILES", "MAX_LOCKED_MEMORY",
		"MAX_ADDRESS_SPACE", "MAX_FILE_LOCKS", "MAX_PENDING_SIGNALS",
		"MAX_MSGQUEUE_SIZE", "MAX_NICE_PRIORITY", "MAX_REALTIME_PRIORITY",
		"MAX_REALTIME_TIMEOUT" };

