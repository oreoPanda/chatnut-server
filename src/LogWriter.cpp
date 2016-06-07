/*Logging.cpp*/

/*Copyright (C) 2016 Jonas Fuglsang-Petersen*/

/*This file is part of chatnut-server.*/

/*chatnut-server is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

chatnut-server is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with chatnut.  If not, see <http://www.gnu.org/licenses/>.*/

#include "LogWriter.h"

namespace fileio
{

	LogWriter::LogWriter(std::ostream & err, std::ostream & log)
	:error_stream(err), log_stream(log)
	{

	}

	LogWriter::~LogWriter()
	{

	}

	//TODO add timestamps to logs and errors

	void LogWriter::error(std::string const & object, std::string const & msg) const
	{
		error_stream << "[" + object + "]" + " error: " + msg + "." << std::endl;
	}

	/*write an error to the specified error stream*/
	void LogWriter::error(std::string const & object, std::string const & msg, int errnum) const
	{
		error_stream << "[" + object + "]" + " error: " + msg + ": " + strerror(errnum) << std::endl;
	}

	void LogWriter::log(std::string const & object, std::string const & msg) const
	{
		log_stream << "[" + object + "] " + msg + "." << std::endl;
	}

} /* namespace messaging */
