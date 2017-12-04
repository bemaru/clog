#include "stdafx.h"

#include "clog.h"
using namespace boost::log::trivial;
int main()
{
	CLog clog;
	bool useTrace = false;
	clog.AddConsoleSink(useTrace);
	clog.AddDebugOutputSink();
	clog.AddTextFileSink();
	clog.SetSeverityMin(info);

	std::string str = "narrow character";
	std::wstring wstr = L"wide character";
	std::wstring wstr2 = L"한글";

	CLOG(trace) << str;
	CLOG(debug) << wstr;
	CLOG(info) << wstr2;
	CLOG(warning) << str.c_str() << " " << wstr.c_str() << " " << wstr2.c_str();
	CLOG(error) << str << " " << wstr << " " << wstr2;
	CLOG(fatal) << L"nice!";

	return 0;
}

