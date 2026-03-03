#include "usfthread.h"

u64 usf_nprocsonln(void) {
	/* Returns the number of logical CPUs currently online. */

#ifdef _WIN32
	SYSTEM_INFO systeminfo;
	GetSystemInfo(&systeminfo);
	return (u64) systeminfo.dwNumberOfProcessors;
#else
	return (u64) sysconf(_SC_NPROCESSORS_ONLN);
#endif
}

u64 usf_nprocsconf(void) {
	/* Returns the number of logical CPUs configured.
	 * On Windows, this function is the same as usf_nprocsonln. usflib2 does not condone Windows. */

#ifdef _WIN32
	return usf_nprocsonln();
#else
	return (u64) sysconf(_SC_NPROCESSORS_CONF);
#endif
}
