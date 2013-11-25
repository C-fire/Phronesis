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


#ifndef PHR_CONSOLE_HPP
#define PHR_CONSOLE_HPP
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <vector>
#include <string>
#include <boost/algorithm/string/classification.hpp>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/program_options.hpp>

#include "phronesisAnalyzerClientApi.hpp"
#include "PhronesisConsole.hpp"

/*!
 * @class coutConsole
 * Prints all the message on the console
 */
class coutConsole: public AbstractPhronesisClient {
public:
	void update(api_message e) {
		switch (e.m_type) {
		case api_message::NEW_MSG: {
			std::cout << "[NEW_MESSAGE] (" << e.m_id << ") : " << e.m_text;
			break;
		}
		case api_message::NEW_LOG: {
			std::cout << "[NEW LOG] " << e.m_text;
			break;
		}
		case api_message::NEW_QUESTION: {
			std::cout << "[NEW QUESTION] (" << e.m_id << ")	: " << e.m_text;
			break;
		}
		case api_message::NEW_YN_QUESTION: {
			std::cout << "[YES/NO] (" << e.m_id << ")	: " << e.m_text;
			break;
		}
		case api_message::NEW_PB_QUESTION: {
			std::cout << "[PROBLEM] (" << e.m_id << ")	: " << e.m_text;
			break;
		}
		case api_message::ERROR_TEXT: {
			std::cout << "[ERROR] : " << e.m_text;
			break;
		}
		default:
			std::cout << "[UNKNOWN TYPE] " << e;
			break;
		}
		std::cout << std::endl;
	}
}
;

enum {
	NONE, OBSERVE, QUESTION, MESSAGE, SET_VETO, GET_VETO
};

/*!
 * @class answerQuestion
 * @brief Used to answer question
 *
 * This should not be used anymore since there are now
 * helpers directly in AbstractPhronesisClient.
 */
class answerQuestion: public AbstractPhronesisClient {
private:
	int m_id; //!< id of the question
	std::string m_text; //!< answer to the question
public:

	/*!
	 * @brief Constructor
	 * @param id id of the question
	 * @param text answer to the question
	 */
	answerQuestion(int id, std::string text) :
			m_id(id), m_text(text) {

	}

	/*!
	 * Overwrite the original method : we don't loop anymore,
	 * we just send the message and exit after 1 second
	 * @param c
	 */
	void loop(api_client & c) {
		api_message msg(api_message::ANSWER_QUESTION, m_id, m_text);
		std::cout << "I will write " << msg << std::endl;
		c.write(msg);
		sleep(1);
	}

	/*!
	 * Prints the error message in the console
	 * @param msg Message received
	 */
	void update(api_message msg) {
		if (msg.m_type == api_message::ERROR_TEXT)
			std::cout << msg << std::endl;
	}
};

class SetVetoInterface: public AbstractPhronesisClient {
private:
	bool m_value;
	std::string m_name;
public:

	SetVetoInterface(std::string name, bool value) :
			m_value(value), m_name(name) {

	}
	void loop(api_client & c) {
		setVeto(m_name, m_value);
		sleep(1);
	}

	void update(api_message e) {
		if (e.m_type == api_message::ERROR_TEXT)
			std::cout << e << std::endl;
	}
};

#define START_COLOR "\033[1;"
#define START_RED START_COLOR"31m"
#define START_GREEN START_COLOR"32m"
#define START_YELLOW START_COLOR"33m"
#define START_BLUE START_COLOR"34m"
#define END_COLOR "\033[0m"

class phrUtilsInterface: public AbstractPhronesisClient {
protected:
	int m_mode;

public:

	phrUtilsInterface(std::string serverAddr, std::string port, int mode =
			OBSERVE) :
			AbstractPhronesisClient(serverAddr, port) {
		m_mode = mode;

	}

	void update(api_message e) {
		switch (m_mode) {
		case OBSERVE: {
			switch (e.m_type) {
			case api_message::NEW_MSG: {
				std::cout << START_YELLOW << "[NEW_MESSAGE] (" << e.m_id
						<< ") : " << e.m_text << END_COLOR;
				break;
			}
			case api_message::NEW_LOG: {
				std::cout << "[NEW LOG] " << e.m_text;
				break;
			}
			case api_message::NEW_QUESTION: {
				std::cout << START_BLUE << "[NEW QUESTION] (" << e.m_id
						<< ")	: " << e.m_text << END_COLOR;
				break;
			}
			case api_message::NEW_YN_QUESTION: {
				std::cout << START_BLUE << "[YES/NO] (" << e.m_id << ")	: "
						<< e.m_text << END_COLOR;
				break;
			}
			case api_message::NEW_PB_QUESTION: {
				std::cout << START_BLUE << "[PROBLEM] " << e.m_text
						<< END_COLOR;
				break;
			}
			case api_message::ERROR_TEXT: {
				std::cout << START_RED << "[ERROR] : " << e.m_text << END_COLOR;
				break;
			}
			case api_message::STATISTIC: {
				std::cout << "[STATISTIC] : " << e.m_text << " " << e.m_id;
				break;
			}
			case api_message::CURRENT_PB: {
				std::cout << START_GREEN << "[CURRENT PROBLEM] : " << e.m_text
						<< END_COLOR;
				break;
			}
			case api_message::NEW_VETO: {
				std::cout << "[VETO] " << e.m_text << " : " << e.m_id
						<< std::endl;
				break;
			}
			default:
				std::cout << "[UNKNOWN TYPE] " << e;
				break;
			}
			std::cout << std::endl;

			break;
		}
		case GET_VETO: {
			switch (e.m_type) {
			case api_message::NEW_VETO: {
				std::cout << e.m_text << " : " << e.m_id << std::endl;
				break;
			}
			}
			break;
		}
		default: {
			if (e.m_type == api_message::ERROR_TEXT)
				std::cout << e << std::endl;
			break;
		}
		}
	}
}
;
#endif
