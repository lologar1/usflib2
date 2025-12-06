#include <x86intrin.h>
#include <stdio.h>
#include "usfhashmap.h"

#define HMTEST_INTEGRITY_TEST_SIZE (1LU << 20)
#define HMTEST_DELETION_STRIDE 7
#define HMTEST_ACCESS_TEST_MAXSIZE (1LU << 20) /* Will double size each test */
#define HMTEST_SAMPLE_SIZE 64

#define LOG(MSG, ...) fprintf(stderr, "hmtest: " MSG, ##__VA_ARGS__)
#define LOGFAIL(HMTYPE, TESTTYPE) LOG("Incorrect value (" HMTYPE " hashmap failure) during " TESTTYPE \
		" test at index %lu (got back %lu), aborting test.\n", i, val)

int32_t main(void) {
	/* Test the usflib2 hashmap */
	uint64_t i, val;
	char s[512]; /* Way beyond max chars for numerical representation of int64 maximum */
	LOG("Starting usfhashmap test\n");

	LOG("Testing hashmap integrity...\n");
	usf_hashmap *strhm, *inthm;
	strhm = usf_newhm(); inthm = usf_newhm();

	/* Construct */
	for (i = 0; i < HMTEST_INTEGRITY_TEST_SIZE; i++) {
		sprintf(s, "%lu", i);
		usf_strhmput(strhm, s, USFDATAU(i));
		usf_inthmput(inthm, i, USFDATAU(i));
	}

	/* Test */
	for (i = 0; i < HMTEST_INTEGRITY_TEST_SIZE; i++) {
		sprintf(s, "%lu", i); val = usf_strhmget(strhm, s).u;
		if (val != i) { LOGFAIL("string", "integrity"); break; }

		val = usf_inthmget(inthm, i).u;
		if (val != i) { LOGFAIL("integer", "integrity"); break; }
	}
	LOG("OK!\n");

	LOG("Testing hashmap deletion...\n");
	for (i = 0; i < HMTEST_INTEGRITY_TEST_SIZE; i += HMTEST_DELETION_STRIDE) {
		sprintf(s, "%lu", i); val = usf_strhmdel(strhm, s).u;
		if (val != i) { LOGFAIL("string", "deletion"); break; }

		val = usf_inthmdel(inthm, i).u;
		if (val != i) { LOGFAIL("integer", "deletion"); break; }
	}

	usf_freestrhm(strhm); usf_freehm(inthm); /* Cleanup */
	LOG("OK!\n");

	LOG("Testing integer hashmap access time for sizes 2^n up to %lu\n", HMTEST_ACCESS_TEST_MAXSIZE);
	uint64_t j, loops;
	uint64_t start, end, cycles;

	for (i = 16; i <= HMTEST_ACCESS_TEST_MAXSIZE; i <<= 1) {
		/* Populate */
		inthm = usf_newhm();
		for (j = 0; j < i; j++) usf_inthmput(inthm, j, USFDATAU(j));

		cycles = 0;
		for (loops = 0; loops < HMTEST_SAMPLE_SIZE; loops++) {

			start = __rdtsc();
			for (j = 0; j < i; j++) (void) usf_inthmget(inthm, j);
			end = __rdtsc();

			cycles += end - start;
		}

		usf_freehm(inthm);

		LOG("Size %lu took %f CPU cycles per access\n", i, (double) cycles / (HMTEST_SAMPLE_SIZE * i));
	}

	LOG("Note that cache effects can make an O(1) access grow with size.\n");

}
