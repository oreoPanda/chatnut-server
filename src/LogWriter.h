/*
 * LogWriter.h
 *
 *  Created on: Mar 8, 2016
 *      Author: i_fly_gliders
 */

#ifndef SRC_LOGWRITER_H_
#define SRC_LOGWRITER_H_

#include <iostream>
#include <string>
#include <string.h>

namespace fileio
{

	class LogWriter
	{
	public:
		LogWriter(std::ostream & err, std::ostream & log);
		virtual ~LogWriter();
		void error(std::string const & object, std::string const & msg) const;
		void error(std::string const & object, std::string const & msg, int errnum) const;
		void log(std::string const & object, std::string const & msg) const;
	private:
		std::ostream & error_stream;
		std::ostream & log_stream;
	};

} /* namespace messaging */

#endif /* SRC_LOGWRITER_H_ */
