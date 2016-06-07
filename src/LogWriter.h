/*LogWriter.h*/

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
