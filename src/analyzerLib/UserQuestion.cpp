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


#include "UserQuestion.hpp"

UserQuestion::UserQuestion(string question) {
	m_question = question;
}

UserQuestion::UserQuestion(const UserQuestion & source) {
	m_question = source.m_question;
	m_answer = source.m_answer;
}

UserQuestion & UserQuestion::operator=(const UserQuestion & source) {
	m_question = source.m_question;
	m_answer = source.m_answer;
	return *this;
}

UserQuestion::~UserQuestion() {

}

void checkUserAction() {
}

string UserQuestion::getQuestion() const {
	return m_question;
}

void UserQuestion::setAnswer(string answer) {
	m_answer = answer;
	checkUserAction();
}

string UserQuestion::getAnswer() const {
	return m_answer;
}

ostream& operator<<(ostream& out, UserQuestion& q) {
	out << q.getQuestion() << endl;
	return out;
}

YesNoQuestion::YesNoQuestion(string question) :
		UserQuestion(question) {
}

YesNoQuestion::~YesNoQuestion() {
}

bool YesNoQuestion::isYes() const {
	return (!m_answer.compare("YES"));
}

bool YesNoQuestion::isNo() const {
	return (!m_answer.compare("NO"));
}

void YesNoQuestion::checkUserAction() {
	transform(m_answer.begin(), m_answer.end(), m_answer.begin(), ::toupper);
	int yes = m_answer.compare("Y");
	int no = m_answer.compare("N");
	if (yes && no)
		throw std::runtime_error("Please answer by 'y' or 'n'");
	//if we are here, it means the answer is either 'y' either 'n' so we can transform it in hard in teh answer
	if (!yes) { //we answered 'y'
		m_answer = "YES";
	} else if (!no) { // we answered 'n'
		m_answer = "NO";
	} else { //impossible!
		assert(!(yes && no));

	}
}

ProblemQuestion::ProblemQuestion(string question) :
		UserQuestion(question) {
}

ProblemQuestion::~ProblemQuestion() {
}

void ProblemQuestion::checkUserAction() {
//
//	string pbs = m_answer;
//	string pb;
//	int end = 0;
//
//	while (pbs.size()) {
//		end = pbs.find(";");
//		if ((end == string::npos) && (pbs.size())) { //Only one problem
//			throw std::runtime_error("Problem string not correctly formated");
//		}
//		pb = pbs.substr(0, end);
//		pbs = pbs.substr(end + 1);
//	}

	//There is no point in checking something here I think...

}

