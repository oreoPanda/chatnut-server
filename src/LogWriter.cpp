/*
 * Logging.cpp
 *
 *  Created on: Mar 8, 2016
 *      Author: i_fly_gliders
 */

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
		log_stream << "[" + object + "]" + msg + "." << std::endl;
	}

} /* namespace messaging */
