#include "version.hpp"
#include <cstring>
#include <cstdlib>
#include <iomanip>

#define VERSION_MAJOR 2
#define VERSION_MINOR 1

//c++11 raw string
static const char*const _git_full_version = R"VERSIONFETCH(
#include "../.git/FETCH_HEAD"
)VERSIONFETCH";

static inline version_t get_version()
{
	version_t v;
	v.major = VERSION_MAJOR;
	v.minor = VERSION_MINOR;
	char ploc[9];
	memset(ploc, 0, 9);
	strncpy(ploc, _git_full_version, 8);
	if (strlen(ploc) != 8)
	{
		v.revision = 0xFFFFFFFF;
	}
	else
	{
		v.revision = strtoul(ploc, NULL, 16);
	}
	return v;
}
const version_t compiled_version = get_version();

std::ostream& operator<<(std::ostream& out, const version_t& vs)
{
	std::ios::fmtflags f(out.flags());
	out << vs.major << "." << vs.minor << ".";
	out << std::hex << std::setfill('0') << std::setw(8) << vs.revision;
	out.flags(f);
	return out;
}
