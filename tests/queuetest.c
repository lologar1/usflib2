#include <stdio.h>
#include "usfqueue.h"
#include "usfmath.h"
#include "usftime.h"

#define TESTSZ 100000
#define PERFSZ 100000

i32 main(void) {
	/* usfqueue.c test */

	u64 i, r;
	usf_queue *queue;

	/* NORMAL TESTS */
	printf("queuetest: Starting test!\n");
	queue = usf_newqueue();

	for (i = 0; i < TESTSZ; i++) usf_enqueue(queue, USFDATAU(i));
	for (i = 0; i < TESTSZ; i++) if ((r = usf_dequeue(queue).u) != i) {
		printf("queuetest: queue contents mismatch, got %"PRIu64" while expecting %"PRIu64", aborting.\n",
				r, TESTSZ - 1 - i);
		exit(1);
	}
	printf("queuetest: enqueue OK\n");
	printf("queuetest: dequeue OK\n");
	usf_freequeue(queue);

	queue = usf_newqueue_ts();
#ifndef USFTEST_NO_PARALLEL
#pragma omp parallel for /* For ASan */
#endif
	for (i = 0; i < TESTSZ; i++) usf_enqueue(queue, USFDATAU(i));
	usf_freequeue(queue);

	/* PERFORMANCE TESTS */
	printf("queuetest: Starting performance tests!\n");
	struct timespec start, end;
	double time;
	u64 randvals[PERFSZ], cyclesz, ncycles;
	for (i = 0; i < PERFSZ; i++) randvals[i] = usf_hash((u64) rand());

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		queue = usf_newqueue();
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_enqueue(queue, USFDATAU(randvals[i]));
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freequeue(queue);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("queuetest: enqueue: %f ns (max sample size %d).\n", time / ncycles, PERFSZ);

	for (ncycles = time = 0, cyclesz = 16; cyclesz < PERFSZ; cyclesz <<= 1) {
		queue = usf_newqueue();
		for (i = 0; i < cyclesz; i++) usf_enqueue(queue, USFDATAU(randvals[i]));
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (i = 0; i < cyclesz; i++) usf_dequeue(queue);
		clock_gettime(CLOCK_MONOTONIC, &end);
		usf_freequeue(queue);

		time += usf_elapsedtimens(start, end);
		ncycles += cyclesz;
	}
	printf("queuetest: dequeue: %f ns (max sample size %d).\n", time / ncycles, PERFSZ);

	printf("queuetest: usfqueue OK (ALL TESTS PASSED)\n");
	return 0;
}
