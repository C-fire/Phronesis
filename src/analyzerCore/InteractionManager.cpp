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


#include "InteractionManager.hpp"

/*pthread_mutex_t m_questionMutex[NQUESTIONS];*/

InteractionManager::InteractionManager(SyncManager * syncManager) {
	m_pbListId = 0;
	this->syncManager = syncManager;
	this->m_vManager = NULL;
	for (int i = 0; i < NQUESTIONS; i++) {
		pthread_mutex_t mumu = PTHREAD_MUTEX_INITIALIZER; //to avoid the compiler warning "warning: extended initializer lists only available with -std=c++0x or -std=gnu++0x"
		m_questionMutex[i] = mumu;
		m_questions[i] = NULL;
		m_yn_questions[i] = NULL;
		m_pb_questions[i] = NULL;
	}
	for (int i = 0; i < NMSG; i++) {
		m_msg[i] = NULL;
	}
}

InteractionManager::~InteractionManager() {
//	for (int i = 0; i < NQUESTIONS; i++) {
//		if (m_questions[i]) {
//			delete (m_questions[i]);
//		}
//	}

	for (int i = 0; i < NMSG; i++) {
		if (m_msg[i]) {
			delete (m_msg[i]);
		}
	}

}

void InteractionManager::run() {

//	cout << "Starting the InteractionManager" << endl;
	while (!stopReceived) {
		sleep(1);
	}
	for (int i = 0; i < NQUESTIONS; i++) {
		if (m_questions[i]) {
			pthread_mutex_unlock(&(m_questionMutex[i]));
		}
	}

//	cout << "I exit the Interaction manager" << endl;
	//pthread_exit(0); /* exit thread */

}

void InteractionManager::ask(UserQuestion * question, int qId, bool pbList) {
	//int qId = addQuestion(question);

	// This is a safety to make sure that there is no question asked
	// after the run loop has already exited.
	// it has not been carefully tested so it maybe has funny feature :-)
	if (stopReceived)
		return;
	if (pbList)
		m_pbListId = qId;

	pthread_mutex_lock(&(m_questionMutex[qId]));
//	cout << "AnalyzerEngine " << pthread_self() << " My answer : "
//			<< question->getAnswer() << endl;
	addLog("Received answer : " + question->getAnswer());
	pthread_mutex_unlock(&(m_questionMutex[qId]));

}

void InteractionManager::ask(YesNoQuestion * question) {
	int qId = addQuestion(question);
	m_yn_questions[qId] = question;
	api_message msg(api_message::NEW_YN_QUESTION, qId, question->getQuestion());
	msg.m_date = getCurrentDate();
	msg.m_time = getCurrentTime();
	m_signal_notify(msg);
	ask(question, qId, false);

}

void InteractionManager::ask(ProblemQuestion * question) {
	int qId = addQuestion(question);
	m_pb_questions[qId] = question;
	api_message msg(api_message::NEW_PB_QUESTION, qId, question->getQuestion());
	msg.m_date = getCurrentDate();
	msg.m_time = getCurrentTime();
	m_signal_notify(msg);
	ask(question, qId, true);

}

int InteractionManager::addQuestion(UserQuestion * question) {
//	cout << "InteractionManager : " << " Someone asks : "
//			<< question->getQuestion() << endl;
	syncManager->start_write();
	int questionId = -1;
	for (int i = 0; i < NQUESTIONS; i++) {
		if (!m_questions[i]) {
			questionId = i;
			break;
		}
	}

//	cout << "InteractionManager : " << "I give questionId " << questionId
//			<< endl;

	if (questionId == -1) {
		//return -1;
		throw std::runtime_error(
				"No free slot for new question. Try again in a moment... No I am kidding this should not happen!!");
	}

	pthread_mutex_lock(&(m_questionMutex[questionId]));
	m_questions[questionId] = question;
	syncManager->end_write();
	return questionId;
}

void InteractionManager::addMsg(UserMsg * msg) {
	syncManager->start_write();
	int msgId = -1;
	for (int i = 0; i < NMSG; i++) {
		if (!m_msg[i]) {
			msgId = i;
			break;
		}
	}

	if (msgId != -1)
		m_msg[msgId] = msg;

//	cout << "InteractionManager : " << "I give mesgId " << msgId << endl;
	syncManager->end_write();
	api_message api_msg(api_message::NEW_MSG, msgId, msg->getMsg());
	api_msg.m_date = getCurrentDate();
	api_msg.m_time = getCurrentTime();
	m_signal_notify(api_msg);

}
//
//void InteractionManager::addMsg(string * msg) {
//	syncManager->start_write();
//	int msgId = -1;
//	for (int i = 0; i < NMSG; i++) {
//		if (!m_msg[i]) {
//			msgId = i;
//			break;
//		}
//	}
//	UserMsg * msgObj = new UserMsg(msg, INFO);
//
//	if (msgId != -1)
//		m_msg[msgId] = msgObj;
//
//	cout << "InteractionManager : " << "I give mesgId " << msgId << endl;
//	syncManager->end_write();
//	api_message api_msg(api_message::NEW_MSG, msgId, msgObj->getMsg());
//	m_signal_notify(api_msg);
//
//}

void InteractionManager::ackMsg(int id) {
	if (id < NMSG) {
		if (m_msg[id]) {
			delete m_msg[id];
			m_msg[id] = NULL;
		} else
			throw std::runtime_error(
					"No message at id " + boost::lexical_cast<std::string>(id));
	} else
		throw std::runtime_error(
				"No message with id " + boost::lexical_cast<std::string>(id));
}

void InteractionManager::setAnswer(int qId, string answer) {
	syncManager->start_write();

	if (qId < NQUESTIONS) {

		UserQuestion * q = m_questions[qId];

		if (q) {
			try {
				q->setAnswer(answer);
//				cout << "InteractionManager : "
//						<< "Answer is good, I release the lock" << endl;
				pthread_mutex_unlock(&(m_questionMutex[qId]));

				m_questions[qId] = NULL;
				m_yn_questions[qId] = NULL;
				m_pb_questions[qId] = NULL;

			} catch (exception &e) {
				addLog(e.what());
				syncManager->end_write();
				throw;
			}
		}
	}
	syncManager->end_write();
}

void InteractionManager::giveProblemList(string pbList) {

	if (m_pbListId == -1)
		throw std::runtime_error(
				"There is no question regarding the problem list");

	setAnswer(m_pbListId, pbList);
	m_pbListId = -1;

}

//string InteractionManager::listQuestions() {
//	string questions;
//	for (int i = 0; i < NQUESTIONS; i++) {
//		if (m_questions[i]) {
//			questions += boost::lexical_cast<std::string>(i) + ";"
//					+ m_questions[i]->getQuestion() + ";";
//		}
//	}
//	return questions;
//}
//
//string InteractionManager::listYesNoQuestions() {
//	string questions;
//	for (int i = 0; i < NQUESTIONS; i++) {
//		if (m_yn_questions[i]) {
//			questions += boost::lexical_cast<std::string>(i) + ";"
//					+ m_yn_questions[i]->getQuestion() + ";";
//		}
//	}
//	return questions;
//}

//string InteractionManager::listMsgs() {
//	string msgs;
//	for (int i = 0; i < NMSG; i++) {
//		if (m_msg[i]) {
////			stringstream o;
////			o << Helper::to_string<int>(i) << ";" << *(m_msg[i]) << ";";
////			msgs += o.str();
//
//			msgs += boost::lexical_cast<std::string>(i) + ";"
//					+ m_msg[i]->getMsg() + ";";
//		}
//	}
//	return msgs;
//}

std::string InteractionManager::getCurrentTime() const {
	struct tm *current;
	time_t now;

	time(&now);
	current = localtime(&now);
	char curTime[9];
	sprintf(curTime, "%.2d:%.2d:%.2d", current->tm_hour, current->tm_min,
			current->tm_sec);
	return std::string(curTime);

}

std::string InteractionManager::getCurrentDate() const {
	struct tm *current;
	time_t now;

	time(&now);
	current = localtime(&now);
	char curDate[11];
	sprintf(curDate, "%.2d-%.2d-%.4d", current->tm_mday, current->tm_mon + 1,
			current->tm_year + 1900);
	return std::string(curDate);
}

void InteractionManager::addLog(string log) {
	//We could maybe add in db here...
//	std::string logMsg = "[" + getCurrentDate() + " " + getCurrentTime() + "] "
//			+ log;
	m_log.push(log);
	if (m_log.size() > 100) {
		m_log.pop();
	}
//	syslog(LOG_INFO, logMsg.c_str());
	api_message msg(api_message::NEW_LOG, -1, log);
	msg.m_date = getCurrentDate();
	msg.m_time = getCurrentTime();
	m_signal_notify(msg);

}



void InteractionManager::setCurrentProblemList(std::string currentProblemList) {
	m_currentProblemList = currentProblemList;
	api_message msg(api_message::CURRENT_PB, -1, currentProblemList);
	msg.m_date = getCurrentDate();
	msg.m_time = getCurrentTime();
	m_signal_notify(msg);
}
std::string InteractionManager::getCurrentProblemList() const {
	return m_currentProblemList;
}

void InteractionManager::setVeto(std::string name, bool value) {
	m_vManager->setVeto(name, value);
}

void InteractionManager::setVetoManager(VetoManager * vm) {
	m_vManager = vm;
}

std::map<std::string, bool> InteractionManager::getVeto(std::string vetoName) {
	std::map<std::string, bool> vetos;
	if (vetoName.size()) {
		vetos[vetoName] = m_vManager->getVeto(vetoName);
	} else { //if there is no name, we return them all
		vetos = m_vManager->getAllVeto();
	}
	return vetos;
}

void InteractionManager::sendStatistic(std::string statName, int value ){
	api_message msg(api_message::STATISTIC, value, statName);
	msg.m_date = getCurrentDate();
	msg.m_time = getCurrentTime();
	m_signal_notify(msg);

}

api_client_pool::api_client_pool(api_server& server, InteractionManager & im) :
		m_server(server), m_subject(im) {
	m_subject.m_signal_notify.connect(
			boost::bind(&api_client_pool::OnNotice, this, _1));
}

void api_client_pool::join(api_session_ptr participant) {
	m_participants.insert(participant);

	YesNoQuestion ** yn_questions = m_subject.getYNQuestionList();

	for (int i = 0; i < NQUESTIONS; i++) {
		if (yn_questions[i]) {
			api_message msg(api_message::NEW_YN_QUESTION, i,
					yn_questions[i]->getQuestion());
			msg.m_date = "old";
			msg.m_time = "old";
			participant->deliver(msg);
		}
	}

	ProblemQuestion ** pb_questions = m_subject.getPbQuestionList();

	for (int i = 0; i < NQUESTIONS; i++) {
		if (pb_questions[i]) {
			api_message msg(api_message::NEW_PB_QUESTION, i,
					pb_questions[i]->getQuestion());
			msg.m_date = "old";
			msg.m_time = "old";
			participant->deliver(msg);
		}
	}

	UserMsg ** usr_msg = m_subject.getMsgList();
	for (int i = 0; i < NMSG; i++) {
		if (usr_msg[i]) {
			api_message msg(api_message::NEW_MSG, i, usr_msg[i]->getMsg());
			msg.m_date = "old";
			msg.m_time = "old";
			participant->deliver(msg);
		}
	}

	api_message msg(api_message::CURRENT_PB, -1,
			m_subject.getCurrentProblemList());
	msg.m_date = "old";
	msg.m_time = "old";
	participant->deliver(msg);

	std::map<std::string, bool> vetos = m_subject.getVeto();
	for (std::map<std::string, bool>::const_iterator veto = vetos.begin();
			veto != vetos.end(); veto++) {
		api_message msg(api_message::NEW_VETO, veto->second, veto->first);
		msg.m_date = "old";
		msg.m_time = "old";
		participant->deliver(msg);

	}
}

void api_client_pool::leave(api_session_ptr participant) {

	m_participants.erase(participant);
}



void api_client_pool::OnNotice(const api_message& msg) {
	deliver(msg);
}
void api_client_pool::deliver(const api_message& msg) {
//	std::for_each(m_participants.begin(), m_participants.end(),
//			boost::bind(&api_session::deliver, _1, boost::ref(msg)));
	for (std::set<api_session_ptr>::const_iterator it = m_participants.begin();
			it != m_participants.end(); it++) {
		(*it)->deliver(msg);
	}
}
//void api_client_pool::doSmtOnSubject(const api_message& msg) {
//	std::cout << "Okay, I will tell " << msg.m_text << " to the subject"
//			<< std::endl;
//	switch (msg.m_type) {
//	case api_message::ANSWER_QUESTION: {
//		m_subject.setAnswer(msg.m_id, msg.m_text);
//		break;
//	}
//	}
////m_subject.doWhatISay(msg.m_login);
//
//}

void api_session::deliver(const api_message& msg) {
	m_tcp_connection->async_write(msg,
			boost::bind(&api_session::handle_write, shared_from_this(),
					boost::asio::placeholders::error));
}

api_session::api_session(connection_ptr tcp_connection,
		api_client_pool_ptr room, InteractionManager & im) :
		m_tcp_connection(tcp_connection), m_room(room), m_subject(im) {
	is_leaving = false;
}
void api_session::wait_for_data() {
	m_tcp_connection->async_read(m_message,
			boost::bind(&api_session::handle_read, shared_from_this(),
					boost::asio::placeholders::error));
}

void api_session::handle_write(const boost::system::error_code& error) {
	api_client_pool_ptr room = m_room.lock();
	if (room && error && (!is_leaving)) {
		is_leaving = true;
		room->leave(shared_from_this());
	}
}
void api_session::handle_read(const boost::system::error_code& error) {
	api_client_pool_ptr room = m_room.lock();
	if (room) {
		if (!error) {
			// On demande a la room de transmettre le message a tout le monde
			//room->deliver(m_message);
			//room->doSmtOnSubject(m_message);
			//std::cout << "The msg " << m_message << std::endl;
			switch (m_message.m_type) {
			case api_message::ANSWER_QUESTION: {
				try {
					m_subject.setAnswer(m_message.m_id, m_message.m_text);
				} catch (exception & e) {
					api_message msg(api_message::ERROR_TEXT, -1, e.what());
					deliver(msg);
				}
				break;
			}
			case api_message::GIVE_PB: {
				try {
					m_subject.giveProblemList(m_message.m_text);
				} catch (exception & e) {
					api_message msg(api_message::ERROR_TEXT, -1, e.what());
					deliver(msg);
				}
				break;
			}
			case api_message::ACK_MSG: {
				try {
					m_subject.ackMsg(m_message.m_id);
				} catch (exception & e) {
					api_message msg(api_message::ERROR_TEXT, -1, e.what());
					deliver(msg);
				}
				break;
			}
			case api_message::SET_VETO: {

				m_subject.setVeto(m_message.m_text, m_message.m_id);
				break;
			}

			case api_message::GET_VETO: {
				std::map<std::string, bool> vetos = m_subject.getVeto(
						m_message.m_text);
				for (std::map<std::string, bool>::const_iterator veto =
						vetos.begin(); veto != vetos.end(); veto++) {
					api_message msg(api_message::NEW_VETO, veto->second,
							veto->first);
					msg.m_date = "old";
					msg.m_time = "old";
					deliver(msg);

				}
				break;
			}
			}

			// On relance une ecoute
			wait_for_data();
		} else {
			//std::cout << "ET BIM, I kick out" << std::endl;
			if (error.value() != 2) {
//				std::cout << "api_session::handle_read : (" << error.value()
//						<< ")" << error.category().message(error.value())
//						<< std::endl;
				m_subject.addLog(
						"api_session::handle_read : ("
								+ boost::lexical_cast<std::string>(
										error.value()) + ") "
								+ error.category().message(error.value()));
			}
			if (!is_leaving) {

				is_leaving = true;
				room->leave(shared_from_this());
			}
		}
	} else {
//		std::cout << "I cannot lock the room??" << std::endl;
	}
}
