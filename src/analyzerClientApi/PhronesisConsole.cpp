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


//#include "PhronesisConsole.hpp"
//
////int main(int argc, char ** argv) {
////	try {
////		tcp::endpoint endpoint(
////				boost::asio::ip::address::from_string("127.0.0.1"), 7172);
////		boost::asio::io_service io_service;
////
////		api_client c(io_service, endpoint);
////		console* recept = new console;
////		c.add_observer(recept);
////		boost::thread t(
////				boost::bind(&boost::asio::io_service::run, &io_service));
////		recept->run();
////		c.close(); // Ferme la connection
////		t.join(); // On rejoint le thread
////		delete recept;
////	} catch (std::exception& e) {
////		std::cerr << "Exception: " << e.what() << "\n";
////	}
////
////}
//
//int main(int argc, char ** argv) {
//	try {
//		coutConsole c;
//		c.run("127.0.0.1", "7172");
//
//	} catch (std::exception& e) {
//		std::cerr << "Exception: " << e.what() << "\n";
//	}
//
//}
