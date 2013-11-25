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


/*

STILL BUGGY!!!
DO NOT USE IT!!

The problem is that... well I don't know what's wrong.
Simply starting the new thread for the run method in python does not work
*/






#include "phronesisAnalyzerClientApi.hpp"
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>
#include <boost/python/str.hpp>
#include <boost/python/operators.hpp>
using namespace boost::python;


class ObserverWrap : public Observer, public wrapper<Observer>{
public:
   void update(api_message data) {
       this->get_override("update")(data);
   }
};

// Callback class
class pyAbstractPhronesisClient : public AbstractPhronesisClient
{
 public:
   pyAbstractPhronesisClient(PyObject* self) : AbstractPhronesisClient(), m_self(self) {}
   pyAbstractPhronesisClient(PyObject* self, const AbstractPhronesisClient & api) : AbstractPhronesisClient(api), m_self(self) {}

   //void update(api_message e){ std::cout << "KABOOM" << std::endl; return call_method<void, api_message>(m_self, "update", e); }
   void update(api_message e){
      std::cout << "Seconde etapee" <<std::endl;
      return call_method<void, api_message>(m_self, "update", e);
   }

/*  void default_update( AbstractPhronesisClient & self_, api_message & e) {
      std::cout << "Premiere etape" << std::endl;
      return self_.AbstractPhronesisClient::update(e);
   }
*/
 private:
   PyObject* const m_self;
};


BOOST_PYTHON_MODULE(pyPhronesis)
{

    class_<api_message>("api_message")
          .def(init<int, int, std::string>())
          .def("reset",&api_message::reset)
          .def_readwrite("m_type",&api_message::m_type)
          .def_readwrite("m_id",&api_message::m_id)
          .def_readwrite("m_text",&api_message::m_text)
          .def_readwrite("m_date",&api_message::m_date)
          .def_readwrite("m_time",&api_message::m_time);

//    enum_<api_message::

    class_<ObserverWrap, boost::noncopyable>("Observer")
          .def("update", pure_virtual(&Observer::update));

/*    class_<AbstractPhronesisClient, pyAbstractPhronesisClient, bases<Observer> >("AbstractPhronesisClient")
          .def("isConnected", &AbstractPhronesisClient::isConnected)
          .def("update", &pyAbstractPhronesisClient::update)
          .def("answerQuestion", &AbstractPhronesisClient::answerQuestion)
          .def("ackMessage", &AbstractPhronesisClient::ackMessage)
          .def("giveProblemList", &AbstractPhronesisClient::giveProblemList)
          .def("loop", &AbstractPhronesisClient::loop)
          .def("quit", &AbstractPhronesisClient::quit)
          .def("run", &AbstractPhronesisClient::run);
*/

    class_<AbstractPhronesisClient, boost::noncopyable, boost::shared_ptr<pyAbstractPhronesisClient> >("AbstractPhronesisClient")
          .def("isConnected", &AbstractPhronesisClient::isConnected)
          .def("answerQuestion", &AbstractPhronesisClient::answerQuestion)
          .def("ackMessage", &AbstractPhronesisClient::ackMessage)
          .def("loop", &AbstractPhronesisClient::loop)
          .def("giveProblemList", &AbstractPhronesisClient::giveProblemList)
          .def("quit", &AbstractPhronesisClient::quit)
          .def("run", &AbstractPhronesisClient::run);
}
