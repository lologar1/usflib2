#include <stdio.h>
#include "usfhashmap.h"
#include "usftime.h"

#define TESTSZ 100000
#define PERFSZ 100000

i32 main(void) {
	/* usfhashmap.c test */

	u64 i, r;
	static thread_local char s[512]; /* Used as key */
	usf_data *entry;
	usf_hashmap *hashmap;

	/* NORMAL TESTS */

	printf("hashmaptest: Starting test!\n");
	hashmap = usf_newhm();

	for (i = 0; i < TESTSZ; i++) sprintf(s, "%"PRIu64, i), usf_strhmput(hashmap, s, USFDATAU(i));
	for (i = 0; i < TESTSZ; i++) {
		sprintf(s, "%"PRIu64, i);
		if (usf_strhmget(hashmap, s).u != i) {
			printf("hashmaptest: hashmap contents mismatch at %"PRIu64" while expecting %"PRIu64", aborting.\n",
					usf_strhmget(hashmap, s).u, i);
			exit(1);
		}
	}
	printf("hashmaptest: strhmput OK\n");
	printf("hashmaptest: strhmget OK\n");

	for (i = 0; i < TESTSZ; i += 2) {
		sprintf(s, "%"PRIu64, i);
		if ((r = usf_strhmdel(hashmap, s).u) != i) {
			printf("hashmaptest: strhmdel returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
					r, i);
			exit(2);
		}
	}
	for (i = 0; i < hashmap->size; i++) {
		sprintf(s, "%"PRIu64, i * 2);
		if (usf_strhmget(hashmap, s).u != 0) {
			printf("hashmaptest: strhmdel deleted element is still present at %"PRIu64", aborting.\n",
					usf_strhmget(hashmap, s).u);
			exit(3);
		}
	}
	printf("hashmaptest: strhmdel OK\n");

	i = 0;
	while ((entry = usf_strhmnext(hashmap, &i))) {
		if ((entry[1].u & 1) == 0) {
			printf("hashmaptest: strhmnext returns entry that shouldn't be present at %"PRIu64", aborting.\n",
					entry[1].u);
			exit(4);
		}
	}
	printf("hashmaptest: strhmnext OK\n");

	usf_hmclear(hashmap); /* Test clearing function */

	for (i = 0; i < TESTSZ; i++) usf_inthmput(hashmap, i, USFDATAU(i));
	for (i = 0; i < TESTSZ; i++) if (usf_inthmget(hashmap, i).u != i) {
		printf("hashmaptest: hashmap contents mismatch at %"PRIu64" while expecting %"PRIu64", aborting.\n",
				usf_inthmget(hashmap, i).u, i);
		exit(5);
	}
	printf("hashmaptest: inthmput OK\n");
	printf("hashmaptest: inthmget OK\n");

	for (i = 0; i < TESTSZ; i += 2) if ((r = usf_inthmdel(hashmap, i).u) != i) {
		printf("hashmaptest: inthmdel returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
				r, i);
		exit(6);
	}
	for (i = 0; i < hashmap->size; i++) if (usf_inthmget(hashmap, i * 2).u != 0) {
		printf("hashmaptest: inthmdel deleted element is still present at %"PRIu64" when expecting zero.\n",
				usf_inthmget(hashmap, i * 2).u);
		exit(7);
	}
	printf("hashmaptest: inthmdel OK\n");

	i = 0;
	while ((entry = usf_inthmnext(hashmap, &i))) {
		if ((entry[1].u & 1) == 0) {
			printf("hashmaptest: inthmnext returns entry that shouldn't be present at %"PRIu64", aborting.\n",
					entry[1].u);
			exit(8);
		}
	}
	printf("hashmaptest: inthmnext OK\n");
	usf_freeinthm(hashmap);

	/* CONCURRENT TESTS */
	printf("hashmaptest: Starting concurrency test!\n");
	hashmap = usf_newhm_ts();

#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ; i++) sprintf(s, "%"PRIu64, i), usf_strhmput(hashmap, s, USFDATAU(i));
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ; i++) {
		sprintf(s, "%"PRIu64, i);
		if (usf_strhmget(hashmap, s).u != i) {
			printf("hashmaptest: hashmap contents mismatch at %"PRIu64" while expecting %"PRIu64", aborting.\n",
					usf_strhmget(hashmap, s).u, i);
			exit(9);
		}
	}
	printf("hashmaptest: strhmput OK\n");
	printf("hashmaptest: strhmget OK\n");

#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ; i += 2) {
		sprintf(s, "%"PRIu64, i);
		if ((r = usf_strhmdel(hashmap, s).u) != i) {
			printf("hashmaptest: strhmdel returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
					r, i);
			exit(10);
		}
	}
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < hashmap->size; i++) {
		sprintf(s, "%"PRIu64, i * 2);
		if (usf_strhmget(hashmap, s).u != 0) {
			printf("hashmaptest: strhmdel deleted element is still present at %"PRIu64", aborting.\n",
					usf_strhmget(hashmap, s).u);
			exit(11);
		}
	}
	printf("hashmaptest: strhmdel OK\n");
	usf_freestrhm(hashmap);

	hashmap = usf_newhm_ts();
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ; i++) usf_inthmput(hashmap, i, USFDATAU(i));
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ; i++) if (usf_inthmget(hashmap, i).u != i) {
		printf("hashmaptest: hashmap contents mismatch at %"PRIu64" while expecting %"PRIu64", aborting.\n",
				usf_inthmget(hashmap, i).u, i);
		exit(12);
	}
	printf("hashmaptest: inthmput OK\n");
	printf("hashmaptest: inthmget OK\n");

#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ; i += 2) if ((r = usf_inthmdel(hashmap, i).u) != i) {
		printf("hashmaptest: inthmdel returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
				r, i);
		exit(13);
	}
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < hashmap->size; i++) if (usf_inthmget(hashmap, i * 2).u != 0) {
		printf("hashmaptest: inthmdel deleted element is still present at %"PRIu64" when expecting zero.\n",
				usf_inthmget(hashmap, i).u);
		exit(14);
	}
	printf("hashmaptest: inthmdel OK\n");
	usf_freeinthm(hashmap);

	/* PERFORMANCE TESTS */

	printf("hashmaptest: Starting performance tests!\n");
	struct timespec start, end;
	double time;
	u64 randvals[PERFSZ], cyclesz, ncycles;
	for (i = 0; i < PERFSZ; i++) randvals[i] = usf_hash((u64) rand());

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		hashmap = usf_newhm();
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_inthmput(hashmap, randvals[i], USFDATAU(i));
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freeinthm(hashmap);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("hashmaptest: inthmput: %f ns (max sample sz %d).\n", time / ncycles, PERFSZ);

	hashmap = usf_newhm(); /* Dummy hashmap */
	for (i = 0; i < PERFSZ; i++) usf_inthmput(hashmap, randvals[i], USFDATAU(i));

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_inthmget(hashmap, randvals[i]);
		clock_gettime(CLOCK_MONOTONIC, &end);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("hashmaptest: inthmget: %f ns (max sample sz %d).\n", time / ncycles, PERFSZ);
	usf_freeinthm(hashmap);

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		hashmap = usf_newhm();
		for (i = 0; i < cyclesz; i++) usf_inthmput(hashmap, randvals[i], USFDATAU(i));
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_inthmdel(hashmap, randvals[i]);
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freeinthm(hashmap);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("hashmaptest: inthmdel: %f ns (max sample sz %d).\n", time / ncycles, PERFSZ);

	printf("hashmaptest: usfhashmap OK (ALL TESTS PASSED)\n");
	return 0;
}
