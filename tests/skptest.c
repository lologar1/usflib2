#include <x86intrin.h>
#include <stdio.h>
#include "usfskiplist.h"

#define SKPTEST_INTEGRITY_TEST_SIZE (1LU << 21)
#define SKPTEST_DELETION_STRIDE 7
#define SKPTEST_ACCESS_TEST_MAXSIZE (1LU << 21) /* Will double size each test */
#define SKPTEST_SAMPLE_SIZE (1LU << 16)

#define LOG(MSG, ...) fprintf(stderr, "skptest: " MSG, ##__VA_ARGS__)
#define LOGFAIL(TESTTYPE) LOG("Incorrect value (skiplist failure) during " TESTTYPE \
		" test at index %lu (got back %lu), aborting test.\n", i, val)

int32_t main() {
	/* Test the usflib2 skiplist */
	uint64_t i, val;
	LOG("Starting skiplist test\n");

	LOG("Testing skiplist integrity...\n");
	usf_skiplist *skiplist;
	skiplist = usf_newsk();
	for (i = 0; i < SKPTEST_INTEGRITY_TEST_SIZE; i++) {
		usf_skset(skiplist, i, USFDATAU(i));
	}
	for (i = 0; i < SKPTEST_INTEGRITY_TEST_SIZE; i++) {
		if (i != (val = usf_skget(skiplist, i).u)) {
			LOGFAIL("integrity");
			break;
		}
	}
	LOG("OK!\n");

	LOG("Testing skiplist deletion...\n");
	for (i = 0; i < SKPTEST_INTEGRITY_TEST_SIZE; i += SKPTEST_DELETION_STRIDE) {
		usf_skdel(skiplist, i);
		if ((val = usf_skget(skiplist, i).u) != 0) {
			LOGFAIL("deletion");
		}
	}
	for (i = 0; i < SKPTEST_INTEGRITY_TEST_SIZE; i++) {
		if (i % SKPTEST_DELETION_STRIDE == 0) continue;
		if (i != (val = usf_skget(skiplist, i).u)) {
			LOGFAIL("integrity");
		}
	}
	LOG("OK!\n");
	usf_freesk(skiplist);

	LOG("Testing skiplist middle element access time for sizes up to %lu\n", SKPTEST_ACCESS_TEST_MAXSIZE);
	uint64_t j, loops, start, end, cycles;

	skiplist = usf_newsk();
	for (i = 4; i <= SKPTEST_ACCESS_TEST_MAXSIZE; i <<= 1) {
		/* Make */
		for (j = 0; j < i; j++) usf_skset(skiplist, j, USFDATAU(j));

		cycles = 0;
		for (loops = 0; loops < SKPTEST_SAMPLE_SIZE; loops++) {
			start = __rdtsc();
			(void) usf_skget(skiplist, i/2);
			end = __rdtsc();

			cycles += end - start;
		}

		LOG("Size %lu took %f CPU cycles per access\n", i, (double) cycles / SKPTEST_SAMPLE_SIZE);
	}
}
