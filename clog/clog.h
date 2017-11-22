#pragma once
#define BOOST_LOG_DYN_LINK

#include <boost/log/trivial.hpp>
#include <boost/log/common.hpp>

#pragma warning(disable: 4244) // possible loss of data

#define BOOST_LOG_ALL_LINK
#ifdef CLOG_EXPORTS
#define CLOG_API __declspec(dllexport)
#else
#define CLOG_API __declspec(dllimport)
#endif

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(slg, boost::log::sources::wseverity_logger< boost::log::trivial::severity_level >)

#define CLOG(lvl) BOOST_LOG_STREAM_SEV(slg::get(), lvl)

class CLOG_API CLog
{
	void global_attribute();
	void add_console_sink();
public:
	CLog();
	~CLog();
	
	void add_vs_debug_output_sink();
	void add_text_file_sink();
	void set_severity_min(boost::log::trivial::severity_level lv);
};