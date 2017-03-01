////
#ifndef OTEESTREAM_H
#define OTEESTREAM_H

#include <ostream>
#include <cstdio>  //for EOF


/**	\brief An std::ostream that redirects output to two child std::ostreams. */
class oteestream : public std::ostream
{
protected:
	class oteestreambuf : public std::streambuf
	{
	protected:
		std::streambuf *const o1;
		std::streambuf *const o2;

		virtual std::streamsize xsputn ( const char * s, std::streamsize n );
		virtual int sync();
		virtual int overflow(int c=EOF);
	public:
		oteestreambuf(std::streambuf *const to1,std::streambuf *const to2);
	};

	oteestreambuf my_teestreambuf;
public:
	/**	\brief Constructs an oteestream using two std::ostream objects.

		\param[in] tos1	An std::ostream to be written to.
		\param[in] tos2	An std::ostream to be written to.
	*/
	oteestream(std::ostream& tos1,std::ostream& tos2);
};


#endif
