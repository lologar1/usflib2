#include <stdio.h>
#include "usfhashmap.h"
#include "usfstd.h"
#include "usftime.h"

#define TESTSZ 100000
#define PERFSZ 100000

i32 main(void) {
	/* usfhashmap.c test */

	u64 i, r;
	static thread_local char s[512]; /* Used as key */
	usf_hashiter iter;
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

	usf_hmclear(hashmap);

	/* Combined */
	for (i = 0; i < TESTSZ; i++) sprintf(s, "%"PRIu64, i), usf_strhmput(hashmap, s, USFDATAU(i + 3));
	for (i = 0; i < TESTSZ; i++) usf_inthmput(hashmap, i, USFDATAU(i + 3));
	for (i = 0; i < TESTSZ; i++) {
		sprintf(s, "%"PRIu64, i);
		if (usf_strhmget(hashmap, s).u != i + 3) {
			printf("hashmaptest: string hashmap contents mismatch at %"PRIu64" while expecting "
					"%"PRIu64", aborting.\n", usf_strhmget(hashmap, s).u, i + 3);
			exit(1);
		}

		if (usf_inthmget(hashmap, i).u != i + 3) {
		printf("hashmaptest: integer hashmap contents mismatch at %"PRIu64" while expecting "
				"%"PRIu64", aborting.\n", usf_inthmget(hashmap, i).u, i + 3);
			exit(1);
		}
	}
	for (i = 0; i < TESTSZ; i += 3) if ((r = usf_inthmdel(hashmap, i).u) != i + 3) {
		printf("hashmaptest: inthmdel returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
				r, i);
		exit(1);
	}
	for (i = 0; i < TESTSZ; i += 2) {
		sprintf(s, "%"PRIu64, i);
		if ((r = usf_strhmdel(hashmap, s).u) != i + 3) {
			printf("hashmaptest: strhmdel returned bad value %"PRIu64" while expecting %"PRIu64", aborting.\n",
					r, i);
			exit(0);
		}
	}

	u64 n;
	for (n = 0, usf_hmiterstart(hashmap, &iter); usf_hmiternext(&iter); n++) {
		if (iter.entry->flag == USF_HASHMAP_KEY_INTEGER) {
			if (iter.entry->value.u != iter.entry->key.u + 3
					|| (iter.entry->value.u - 3) % 9 == 0) {
				printf("hashmaptest: iterator returned bad value %"PRIu64", aborting.\n", iter.entry->value.u);
				exit(1);
			}
		} else if (iter.entry->flag == USF_HASHMAP_KEY_STRING) {
			u64 ikey = strtou64(iter.entry->key.p, NULL, 10);
			if (iter.entry->value.u != ikey + 3
					|| (iter.entry->value.u - 3) % 6 == 0) {
				printf("hashmaptest: iterator returned bad value %"PRIu64", aborting.\n", iter.entry->value.u);
				exit(1);
			}
		} else {
			printf("hashmaptest: iterator returned bad flag %"PRIu8", aborting.\n", iter.entry->flag);
			exit(1);
		}
	}
	if (n != hashmap->size) {
		printf("hashmaptest: size doesn't match number of entries %"PRIu64" vs %"PRIu64", aborting.\n",
				n, hashmap->size);
		exit(1);
	}
	printf("hashmaptest: combined put/get/del/iter OK\n");

	usf_freehm(hashmap);

	/* CONCURRENT TESTS */
	printf("hashmaptest: Starting concurrency test!\n");
	hashmap = usf_newhm_ts();
	u64 mapsize;

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
	mapsize = hashmap->size;
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < mapsize; i++) {
		sprintf(s, "%"PRIu64, i * 2);
		if (usf_strhmget(hashmap, s).u != 0) {
			printf("hashmaptest: strhmdel deleted element is still present at %"PRIu64", aborting.\n",
					usf_strhmget(hashmap, s).u);
			exit(11);
		}
	}
	printf("hashmaptest: strhmdel OK\n");
	usf_freehm(hashmap);

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
	mapsize = hashmap->size;
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for
#endif
	for (i = 0; i < mapsize; i++) if (usf_inthmget(hashmap, i * 2).u != 0) {
		printf("hashmaptest: inthmdel deleted element is still present at %"PRIu64" when expecting zero.\n",
				usf_inthmget(hashmap, i).u);
		exit(14);
	}
	printf("hashmaptest: inthmdel OK\n");
	usf_freehm(hashmap);

	/* PERFORMANCE TESTS */

	printf("hashmaptest: Starting performance tests!\n");
	struct timespec start, end;
	f64 time;
	u64 randvals[PERFSZ], cyclesz, ncycles;
	for (i = 0; i < PERFSZ; i++) randvals[i] = usf_hash((u64) rand());

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		hashmap = usf_newhm();
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_inthmput(hashmap, randvals[i], USFDATAU(i));
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freehm(hashmap);

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
	usf_freehm(hashmap);

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		hashmap = usf_newhm();
		for (i = 0; i < cyclesz; i++) usf_inthmput(hashmap, randvals[i], USFDATAU(i));
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_inthmdel(hashmap, randvals[i]);
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freehm(hashmap);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("hashmaptest: inthmdel: %f ns (max sample sz %d).\n", time / ncycles, PERFSZ);

	printf("hashmaptest: usfhashmap OK (ALL TESTS PASSED)\n");
	return 0;
}
