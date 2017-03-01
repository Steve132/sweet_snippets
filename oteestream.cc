////
#include "oteestream.h"


oteestream::oteestreambuf::oteestreambuf(std::streambuf *const to1,std::streambuf *const to2)
	:
	o1(to1),o2(to2)
{}

oteestream::oteestream(std::ostream& tos1,std::ostream& tos2):
	std::ostream(&my_teestreambuf),
	my_teestreambuf(tos1.rdbuf(),tos2.rdbuf())
{}

std::streamsize oteestream::oteestreambuf::xsputn ( const char * s, std::streamsize n )
{
	std::streamsize num=o1->sputn(s,n);
	n=(num < n) ? num : n;
	return o2->sputn(s,n);
}

int oteestream::oteestreambuf::sync()
{
	int ret;
	ret=o1->pubsync();
	if (ret!=0)
		return ret;
	ret=o2->pubsync();
	if (ret!=0)
		return ret;
	return 0;
}
int oteestream::oteestreambuf::overflow(int c)
{
	int ret;

	ret=o1->sputc(c);
	if (ret==EOF)
		return ret;
	ret=o2->sputc(c);
	if (ret==EOF)
		return ret;
	return c;
}

