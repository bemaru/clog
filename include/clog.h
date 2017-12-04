//          Copyright Joe Coder 2004 - 2006.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
//
//		   .__
//	  ____ |  |   ____   ____
//	_/ ___\|  |  /  _ \ / ___\
//	\  \___|  |_(  <_> ) /_/  >
//	 \___  >____/\____/\___  /
//		 \/           /_____/
//
//
//	@author	:	jhkwak
//	@brief	:	simple c++ logger using boost log
//				If you want to modify it, please refer to the link below.
//				http://www.boost.org/doc/libs/1_65_1/libs/log/doc/html/index.html

#pragma once
#define BOOST_LOG_DYN_LINK
#define BOOST_PHOENIX_NO_VARIADIC_FUNCTION_EVAL
#define BOOST_NO_CXX11_VARIADIC_TEMPLATES

#include <boost/log/trivial.hpp>
#include <boost/log/common.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#pragma warning(disable: 4244) // possible loss of data

#define BOOST_LOG_ALL_LINK
#ifdef CLOG_EXPORTS
#define CLOG_API __declspec(dllexport)
#else
#define CLOG_API __declspec(dllimport)
#endif

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(slg, boost::log::sources::wseverity_logger< boost::log::trivial::severity_level >)

#define CLOG(lvl) BOOST_LOG_SEV(slg::get(), lvl)	\
	<< boost::log::add_value("File", __FILE__)		\
	<< boost::log::add_value("Line",__LINE__)		\
	<< boost::log::add_value("Function", __FUNCTION__)
//<< boost::log::add_value("Function", BOOST_CURRENT_FUNCTION)

class CLOG_API CLog
{
	void GlobalAttribute();
public:
	CLog();
	~CLog();

	void AddConsoleSink(bool trace = false);
	void AddDebugOutputSink();
	void AddTextFileSink();
	void SetSeverityMin(boost::log::trivial::severity_level lv);
};
