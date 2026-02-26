#include <stdio.h>
#include "usfskiplist.h"
#include "usfmath.h"
#include "usftime.h"

#define TESTSZ 100000
#define PERFSZ 100000

i32 main(void) {
	/* usfskiplist.c test */

	u64 i, r;
	usf_skiplist *skiplist;

	/* NORMAL TESTS */

	printf("skiplisttest: Starting test!\n");
	skiplist = usf_newsk();

	for (i = 0; i < TESTSZ; i++) usf_skset(skiplist, i, USFDATAU(i));
	for (i = 0; i < TESTSZ; i++) if (usf_skget(skiplist, i).u != i) {
		printf("skiplisttest: skiplist contents mismatch at %"PRIu64" while expecting %"PRIu64", aborting.\n",
				usf_skget(skiplist, i).u, i);
		exit(1);
	}
	printf("skiplisttest: skset OK\n");
	printf("skiplisttest: skget OK\n");

	for (i = 0; i < TESTSZ; i += 2) if ((r = usf_skdel(skiplist, i).u) != i) {
		printf("skiplisttest: skdel returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
				r, i);
		exit(2);
	}
	for (i = 0; i < skiplist->size; i++) if (usf_skget(skiplist, i * 2).u != 0) {
		printf("hashmaptest: skdel deleted element is still present at %"PRIu64" when expecting zero.\n",
				usf_skget(skiplist, i * 2).u);
	}
	printf("skiplisttest: skdel OK\n");
	usf_freesk(skiplist);

	/* CONCURRENT TESTS */
	printf("skiplisttest: Starting concurrency test!\n");
	skiplist = usf_newsk_ts();

#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ; i++) usf_skset(skiplist, i, USFDATAU(i));
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ; i++) if (usf_skget(skiplist, i).u != i) {
		printf("skiplisttest: skiplist contents mismatch at %"PRIu64" while expecting %"PRIu64", aborting.\n",
				usf_skget(skiplist, i).u, i);
		exit(3);
	}
	printf("skiplisttest: skset OK\n");
	printf("skiplisttest: skget OK\n");

#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ; i += 2) if ((r = usf_skdel(skiplist, i).u) != i) {
		printf("skiplisttest: skdel returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
				r, i);
		exit(4);
	}
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < skiplist->size; i++) if (usf_skget(skiplist, i * 2).u != 0) {
		printf("hashmaptest: skdel deleted element is still present at %"PRIu64" when expecting zero.\n",
				usf_skget(skiplist, i * 2).u);
	}
	printf("skiplisttest: skdel OK\n");
	usf_freesk(skiplist);

	/* PERFORMANCE TESTS */
	printf("skiplisttest: Starting performance tests!\n");
	struct timespec start, end;
	double time;
	u64 randvals[PERFSZ], cyclesz, ncycles;
	for (i = 0; i < PERFSZ; i++) randvals[i] = usf_hash((u64) rand());

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		skiplist = usf_newsk();
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_skset(skiplist, randvals[i], USFDATAU(i));
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freesk(skiplist);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("skiplisttest: skset: %f ns (max sample sz %d).\n", time / ncycles, PERFSZ);

	skiplist = usf_newsk(); /* Dummy skiplist */
	for (i = 0; i < PERFSZ; i++) usf_skset(skiplist, randvals[i], USFDATAU(i));

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_skget(skiplist, randvals[i]);
		clock_gettime(CLOCK_MONOTONIC, &end);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("skiplisttest: skget: %f ns (max sample sz %d).\n", time / ncycles, PERFSZ);
	usf_freesk(skiplist);

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		skiplist = usf_newsk();
		for (i = 0; i < cyclesz; i++) usf_skset(skiplist, randvals[i], USFDATAU(i));
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_skdel(skiplist, randvals[i]);
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freesk(skiplist);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("skiplisttest: skdel: %f ns (max sample sz %d).\n", time / ncycles, PERFSZ);

	printf("skiplisttest: usfskiplist OK (ALL TESTS PASSED)\n");
	return 0;
}
