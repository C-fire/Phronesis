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


#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

/*!
 * \file Configuration.hpp
 * \brief Configuration class
 *
 * This class is used to read the configuration file and keeps the configuration.
 *
 * \author Christophe Haen
 */

#include <map>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>

/*!
 * \class Configuration
 * \brief Read and keeps the configuration
 *
 * This class will read a configuration file formated like "variable = value".
 * Everything is stored as an std::string. Lines starting with '#' are ignored
 */
class Configuration {
public:
	std::map<std::string, std::string> properties; /*!< List of properties read in the file (variable : value) */

	/*!
	 * @brief Constructor with default values for the properties
	 *
	 * @param defProps Default values the properties should have
	 */
	Configuration(std::map<std::string, std::string> & defProps) {
		properties = defProps;
	}

	/*!
	 * @brief Constructor which reads the file immediately
	 * @param fileName Path to the configuration file
	 */
	Configuration(std::string fileName) {
		read(fileName);
	}

	/*!
	 * @brief Read the configuration file
	 *
	 * The file format should be "variable = value".
	 * Lines starting with '#' are considered comment.
	 *
	 * @throw std::runtime_error if the file cannot be opened
	 *
	 * @param fileName Path to the configuration file
	 */
	void read(std::string fileName) {
		std::ifstream myfile(fileName.c_str());
		std::string line;

		std::vector<std::string> tokens;
		if (myfile.is_open()) {
			while (myfile.good()) {
				getline(myfile, line);

				// remove all the trailing space
				boost::trim_left(line);

				// Ignore if line starts with '#'
				if (line[0] == '#') {
					continue;
				}

				// Split the line at the level of the '=' sign
				boost::split(tokens, line, boost::is_any_of("="));

				// Ignore line if more than the parameter name and value
				if (tokens.size() != 2) {
					continue;
				}
				// remove excessive space before and after
				boost::trim(tokens[0]);
				boost::trim(tokens[1]);

				// Store in the map
				properties[tokens[0]] = tokens[1];
			}
			myfile.close();
		} else {
			throw std::runtime_error("Cannot open file " + fileName);
		}
	}

	/*!
	 * @brief Get the value of a configuration parametre
	 *
	 * The parameter is stored as an std::string, but casted
	 * as type T using boost::lexical_cast
	 *
	 * @throw std::runtime_error In case the property is not found
	 * @throw std::runtime_error In case the value cannot be cast in the expected type
	 * @param name Name of the parameter
	 * @tparam T return type of the parameter's value
	 * @return value of the parameter
	 */
	template<class T>
	T get(std::string name) {
		std::map<std::string, std::string>::const_iterator it;
		it = properties.find(name);

		// If the property is not found, throw exception
		if (it == properties.end())
			throw std::runtime_error("No such property " + name);

		// Retrieve the element and try to cast it into the required type T.
		// Throw if it fails.
		T ret;
		try {
			ret = boost::lexical_cast<T>(it->second);
		} catch (boost::bad_lexical_cast & e) {
			throw std::runtime_error(e.what());
		}
		return ret;
	}
};

#endif
