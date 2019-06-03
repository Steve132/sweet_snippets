#define VERSION_MAJOR 2
#define VERSION_MINOR 1
#include<cstdint>
struct version_t {
	uint16_t major;
	uint16_t minor;
	uint32_t revision;
};

//c++11 raw string
static const char*const _git_full_version=R"VERSIONFETCH(
#include PROJECT_ROOT ".git/FETCH_HEAD"
)VERSIONFETCH";

static inline version_t get_version()
{
	version_t v;
  v.major=VERSION_MAJOR;
  v.minor=VERSION_MINOR;
  char ploc[9];
  memset(ploc,0,9);
  strncpy(ploc,_git_full_version,8);
  if(strlen(ploc)!=8)
  {
    v.revision=0xFFFFFFFF;
  }
  else
  {
    v.revision=strtoul(ploc,NULL,16);
  }
	return v;
}
const version_t compiled_version=get_version();
