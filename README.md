clog
=====
c++ simple logging library ( boost log wrapper )

![Alt Text](https://media.giphy.com/media/xT9DgPZCzcARto3J16/giphy.gif)

Requirement
=======
boost 1.65.1

Platforms
======
windows(vc2013+)

Features
=====
using boost log

multi thread

Usage Examples
=====

```cpp
#include <clog.h>

struct my_type
{
	int i;
	template<typename OStream>
	friend OStream& operator<<(OStream& os, const my_type &c)
	{
		return os << "[my_type i=" << c.i << "]";
	}
};

using namespace boost::log::trivial;

int main()
{
	CLog clog;
	clog.set_severity_min(info);
	clog.add_text_file_sink();
	//clog.add_vs_debug_output_sink();
	
	CLOG(trace) << L"테스트";
	CLOG(debug) << L"테스트";
	CLOG(info) << L"테스트";
	CLOG(warning) << L"테스트";
	CLOG(error) << L"테스트";
	CLOG(fatal) << my_type{ 14 };

	return 0;
}
```
<!--
ref
======
 from this code https://gist.github.com/FiaDot/9127467
add some feature and modify
(wide char, dll, etc...)-->


