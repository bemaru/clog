#pragma once
#include "stdafx.h"
#include "clog.h"
#include <boost/core/null_deleter.hpp>
#include <boost/locale/generator.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/exception_handler_feature.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/exception_handler.hpp>
#include <boost/log/utility/record_ordering.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>


namespace src = boost::log::sources;
namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

CLog::CLog()
{
	std::locale::global(std::locale(""));
	GlobalAttribute();
}

CLog::~CLog(void)
{
	logging::core::get()->flush();
	logging::core::get()->remove_all_sinks();
}

struct handler
{
	void operator()(const logging::runtime_error &ex) const
	{
		std::cerr << "boost::log::runtime_error: " << ex.what() << '\n';
	}

	void operator()(const std::exception &ex) const
	{
		std::cerr << "std::exception: " << ex.what() << '\n';
	}
};

void CLog::SetSeverityMin(boost::log::trivial::severity_level lv) {

	logging::core::get()->set_filter
		(
		logging::trivial::severity >= lv
		);
}



void CLog::GlobalAttribute()
{
	logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
	logging::core::get()->add_global_attribute("RecordID", attrs::counter< unsigned int >());

	//There is no cross platform way that I know.
#ifdef _WIN32
	WCHAR moduleFileName[MAX_PATH];
	GetModuleFileName(NULL, moduleFileName, MAX_PATH);
	PathRemoveFileSpec(moduleFileName);
	SetCurrentDirectory(moduleFileName);
#endif
	/*logging::core::get()->set_exception_handler(
	logging::make_exception_handler<logging::runtime_error, std::exception>(handler{}));*/
}


void trace_formatter(logging::record_view const& rec, logging::wformatting_ostream& strm)
{
	strm << "@" << logging::extract< std::string >("File", rec);
	strm << "(" << logging::extract< int >("Line", rec) << ")";
	strm << " " << logging::extract< std::string >("Function", rec) << std::endl;

	strm << "[" << logging::extract< unsigned int >("RecordID", rec) << "] ";
	strm << "[" << logging::extract< boost::posix_time::ptime >("TimeStamp", rec) << "] ";
	strm << "[" << rec[logging::trivial::severity] << "] ";
	strm << rec[expr::wmessage];
}


void CLog::AddConsoleSink(bool trace)
{
	boost::shared_ptr<sinks::wtext_ostream_backend> backend = boost::make_shared<sinks::wtext_ostream_backend>();
	backend->add_stream(boost::shared_ptr<std::wostream>(&std::wclog, boost::null_deleter()));
	backend->auto_flush(true);

	typedef sinks::synchronous_sink<sinks::wtext_ostream_backend> sink_t;
	boost::shared_ptr<sink_t> sink(new sink_t(backend));


	if (trace) {
		sink->set_formatter(&trace_formatter);
	}
	else {
		sink->set_formatter(
			expr::format(L"[%1%] [%2%] [%3%] %4%")
			% expr::attr< unsigned int >("RecordID")
			% expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
			% logging::trivial::severity
			% expr::message
			);
	}


	// The sink will perform character code conversion as needed, according to the locale set with imbue()
	/*std::locale loc = boost::locale::generator()("en_US.UTF-8");
	sink->imbue(loc);*/

	logging::core::get()->add_sink(sink);
}

void CLog::AddDebugOutputSink()
{
	boost::shared_ptr<sinks::wdebug_output_backend> backend = boost::make_shared<sinks::wdebug_output_backend>();

	typedef sinks::synchronous_sink<sinks::wdebug_output_backend> sink_t;
	boost::shared_ptr<sink_t> sink(new sink_t(backend));

	sink->set_formatter(
		expr::format(L"[%1%] [%2%] [%3%] %4%\n")
		% expr::attr< unsigned int >("RecordID")
		% expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
		% logging::trivial::severity
		% expr::message
		);

	logging::core::get()->add_sink(sink);
}

void CLog::AddTextFileSink()
{
	int max_storage_size_ = 20 * 1024 * 1024;
	int max_file_size_ = 1 * 1024 * 1024;
	long file_ordering_window_sec_ = 1;

	char moduleFileName[MAX_PATH];
	GetModuleFileNameA(NULL, moduleFileName, MAX_PATH);
	boost::filesystem::path p(moduleFileName);

	char computerName[256];
	DWORD dwSize = 256;
	GetComputerNameA(computerName, &dwSize); //Get the computer name

	std::string file_name_format;
	file_name_format += "log\\";
	file_name_format += p.filename().string();
	file_name_format += ".";
	file_name_format += computerName;
	//file_name_format += ".%Y-%m-%d_%H-%M-%S_%N.log";
	file_name_format += ".%Y-%m-%d_%N.log";

	boost::shared_ptr< sinks::text_file_backend > file_backend = boost::make_shared< sinks::text_file_backend >(
		keywords::file_name = file_name_format,
		keywords::rotation_size = max_file_size_,      //setting_.max_file_size_,


		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),


		//keywords::time_based_rotation = sinks::file::rotation_at_time_interval(boost::posix_time::minutes(1)),

		keywords::format = "[%TimeStamp%] (%Severity%) : %Message%"

		//keywords::min_free_space = max_storage_size_ + max_file_size_
		);

	file_backend->auto_flush(true);

	typedef sinks::asynchronous_sink<
		sinks::text_file_backend,
		sinks::unbounded_ordering_queue<
		logging::attribute_value_ordering<
		unsigned int,
		std::less< unsigned int >
		>
		>
	> sink_t;

	boost::shared_ptr< sink_t > sink(new sink_t(
		file_backend,
		keywords::order = logging::make_attr_ordering("RecordID", std::less< unsigned int >()),
		keywords::ordering_window = boost::posix_time::seconds(file_ordering_window_sec_)
		));

	sink->set_formatter(
		expr::format("[%1%] [%2%] [%3%] %4%")
		% expr::attr< unsigned int >("RecordID")
		% expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f")
		% logging::trivial::severity
		% expr::message
		);

	sink->locked_backend()->set_file_collector(
		sinks::file::make_collector(
		keywords::target = "log",
		keywords::max_size = max_storage_size_
		//keywords::max_size = max_storage_size_
		)
		);


	sink->locked_backend()->scan_for_files(sinks::file::scan_matching);
	logging::core::get()->add_sink(sink);
}
