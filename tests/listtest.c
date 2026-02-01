#include <stdio.h>
#include "usflist.h"

#define TESTSZ 100000
#define PERFSZ 200000

u64 freeindex_;
u64 freedvalues_[TESTSZ];

static void freevalues(u64 value);

i32 main(void) {
	/* usflist.c test
	 * Only testing usf_listu64 as all other implementations are equivalent */

	u64 i, r;
	usf_listu64 *list;

	/* NORMAL TESTS */

	printf("listtest: Starting test!\n");
	list = usf_newlistu64();

	for (i = 0; i < TESTSZ; i++) usf_listu64add(list, i * 2);
	for (i = 0; i < TESTSZ; i++) if (list->array[i] != i * 2) {
		printf("listtest: list contents mismatch, got %"PRIu64" while expecting %"PRIu64", aborting.\n",
				list->array[i], i * 2);
		exit(1);
	}
	printf("listtest: listadd OK\n");

	for (i = 0; i < TESTSZ; i++) usf_listu64ins(list, i * 2 + 1, i * 2 + 1);
	for (i = 0; i < TESTSZ * 2; i++) if (list->array[i] != i) {
		printf("listtest: list contents mismatch, got %"PRIu64" while expecting %"PRIu64", aborting.\n",
				list->array[i], i);
		exit(2);
	}
	printf("listtest: listins OK\n");

	for (i = 0; i < TESTSZ * 2; i++) if (usf_listu64get(list, i) != i) {
		printf("listtest: listget returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
				usf_listu64get(list, i), i);
		exit(3);
	}
	printf("listtest: listget OK\n");

	for (i = 0; i < list->size; i++) usf_listu64set(list, i, TESTSZ * 2 - 1 - i);
	for (i = 0; i < TESTSZ * 2; i++) if (list->array[i] != TESTSZ * 2 - 1 - i) {
		printf("listtest: list contents mismatch, got %"PRIu64" while expecting %"PRIu64", aborting.\n",
				list->array[i], (u64) TESTSZ * 2 - 1);
		exit(4);
	}
	printf("listtest: listset OK\n");

	for (i = 0; i < TESTSZ; i++) if ((r = usf_listu64del(list, TESTSZ*2 - 1 - i*2)) != i*2) {
		printf("listtest: listdel returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n", r, i);
		exit(5);
	}
	freeindex_ = 0;
	usf_freelistu64func(list, freevalues); /* Test if the right values are leftover */
	for (i = 1; i < TESTSZ; i++) if (freedvalues_[i - 1] <= freedvalues_[i]) {
		printf("listtest: listdel leftover sequence should be decreasing, but got %"PRIu64" -> %"PRIu64
				"instead, aborting.\n", freedvalues_[i - 1], freedvalues_[i]);
		exit(6);
	}
	printf("listtest: listdel OK\n");

	/* CONCURRENT TESTS */

	printf("listtest: Starting concurrency test!\n");
	list = usf_newlistu64_ts();
	for (i = 0; i < TESTSZ * 2; i++) usf_listu64add(list, i);

#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ * 2; i++) if (usf_listu64get(list, i) != i) {
		printf("listtest: listget returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
				usf_listu64get(list, i), i);
		exit(3);
	}
	printf("listtest: listget OK\n");

#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < TESTSZ * 2; i++) usf_listu64set(list, i, TESTSZ * 2 - 1 - i);
	for (i = 0; i < TESTSZ * 2; i++) if (list->array[i] != TESTSZ * 2 - 1 - i) {
		printf("listtest: list contents mismatch, got %"PRIu64" while expecting %"PRIu64", aborting.\n",
				list->array[i], (u64) TESTSZ * 2 - 1);
		exit(4);
	}
	printf("listtest: listset OK\n");

	usf_freelistu64(list);

	/* PERFORMANCE TESTS */

	printf("listtest: Starting performance tests!\n");
	struct timespec start, end;
	double time;
	u64 randvals[PERFSZ], cyclesz, ncycles;
	for (i = 0; i < PERFSZ; i++) randvals[i] = usf_hash((u64) rand());

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		list = usf_newlistu64();
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_listu64set(list, randvals[i] % cyclesz, randvals[i]);
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freelistu64(list);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("listtest: listset: %f ns (max sample size %d).\n", time / ncycles, PERFSZ);

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		list = usf_newlistu64();
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_listu64ins(list, randvals[i] % cyclesz, randvals[i]);
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freelistu64(list);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("listtest: listins: %f ns (max sample size %d).\n", time / ncycles, PERFSZ);

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		list = usf_newlistu64();
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_listu64add(list, randvals[i]);
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freelistu64(list);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("listtest: listadd: %f ns (max sample size %d).\n", time / ncycles, PERFSZ);

	list = usf_newlistu64(); /* Dummy list to get values on */
	for (i = 0; i < PERFSZ; i++) usf_listu64set(list, i, randvals[i]);

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_listu64get(list, randvals[i] % cyclesz);
		clock_gettime(CLOCK_MONOTONIC, &end);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("listtest: listget: %f ns (max sample size %d).\n", time / ncycles, PERFSZ);

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_listu64del(list, randvals[i] % cyclesz);
		clock_gettime(CLOCK_MONOTONIC, &end);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("listtest: listdel: %f ns (max sample size %d).\n", time / ncycles, PERFSZ);

	usf_freelistu64(list);

	printf("listtest: usflist OK (ALL TESTS PASSED)\n");
	return 0;
}

static void freevalues(u64 value) { freedvalues_[freeindex_++] = value; }
