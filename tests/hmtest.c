#include "usflib2.h"
#include <stdio.h>
#include <time.h>

#define STRHM_SAMPLE_SIZE 1000000
#define STRHM_DEL_OFFSET 1000
#define INTHM_SAMPLE_SIZE 1000000
#define INTHM_DEL_OFFSET 1000

double test(uint64_t, int);

int main() {
    uint64_t i, val;
	char snum[100];
	usf_hashmap *hm;

	printf("Testing string hashmap integrity...\n");

	hm = usf_newhm();

	printf("Constructing...\n");
	for (i = 0; i < STRHM_SAMPLE_SIZE; i++) {
		sprintf(snum, "%lu", i);
		usf_strhmput(hm, snum, USFDATAU(i));
	}

	printf("Testing access...\n");
	for (i = 0; i < STRHM_SAMPLE_SIZE; i++) {
		sprintf(snum, "%lu", i);
		if ((val = usf_strhmget(hm, snum).u) != i)
			printf("Error ! Index %lu got back %lu\n", i, val);
	}

	printf("Testing deletion...\n");
	for (i = 0; i < STRHM_SAMPLE_SIZE; i += STRHM_DEL_OFFSET) {
		sprintf(snum, "%lu", i);
		if ((val = usf_strhmdel(hm, snum).u) != i)
			printf("Error ! Deletion at %lu got back %lu\n", i, val);
	}

	printf("Retesting access...\n");
	for (i = 0; i < STRHM_SAMPLE_SIZE; i++) {
		sprintf(snum, "%lu", i);
		if ((val = usf_strhmget(hm, snum).u) != i && i % STRHM_DEL_OFFSET)
			printf("Error ! Index %lu got back %lu\n", i, val);
	}

	usf_freestrhm(hm);

	printf("Testing integer hashmap integrity...\n");

	hm = usf_newhm();

	printf("Constructing...\n");
	for (i = 0; i < INTHM_SAMPLE_SIZE; i++) {
		usf_inthmput(hm, i, USFDATAU(i));
	}

	printf("Testing access...\n");
	for (i = 0; i < INTHM_SAMPLE_SIZE; i++) {
		if ((val = usf_inthmget(hm, i).u) != i)
			printf("Error ! Index %lu got back %lu\n", i, val);
	}

	printf("Testing deletion...\n");
	for (i = 0; i < INTHM_SAMPLE_SIZE; i += INTHM_DEL_OFFSET) {
		if ((val = usf_inthmdel(hm, i).u) != i)
			printf("Error ! Deletion at %lu got back %lu\n", i, val);
	}

	printf("Retesting access...\n");
	for (i = 0; i < INTHM_SAMPLE_SIZE; i++) {
		if ((val = usf_inthmget(hm, i).u) != i && i % INTHM_DEL_OFFSET)
			printf("Error ! Index %lu got back %lu\n", i, val);
	}

	usf_freehm(hm);

    srand(time(NULL));

	printf("Computing string hashmap average access time...\n");

    for (i = 2; i < 10000; i *= 2) {
        printf("Avg. access for size %lu: %f ns\n", i, test(i, 1));
    }

	printf("Computing integer hashmap average access time...\n");

    for (i = 2; i < 10000; i *= 2) {
        printf("Avg. access for size %lu: %f ns\n", i, test(i, 0));
    }

    return 0;
}

double test(uint64_t cycles, int string) {
    uint64_t i, j;
	char snum[100];
	struct timespec start, end;
	uint64_t avg = 0, globalavg = 0;

	usf_hashmap *hm;
	hm = usf_newhm();

    for (i = 0; i < cycles; i++) {
		if (string) {
			sprintf(snum, "%lu", i);
			usf_strhmput(hm, snum, USFDATAU(i));
		} else {
			usf_inthmput(hm, i, USFDATAU(i));
		}
    }

    for (i = 0; i < cycles; i++) {
		avg = 0;

		if (string) {
			sprintf(snum, "%lu", i);

			for (j = 0; j < 1000; j++) {
				clock_gettime(CLOCK_MONOTONIC, &start);
				usf_strhmget(hm, snum);
				clock_gettime(CLOCK_MONOTONIC, &end);

				avg += (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
			}
		} else {
			for (j = 0; j < 1000; j++) {
				clock_gettime(CLOCK_MONOTONIC, &start);
				usf_inthmget(hm, i);
				clock_gettime(CLOCK_MONOTONIC, &end);

				avg += (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
			}
		}

		globalavg += avg / 1000;
	}

    /* Cleanup */
	if (string) {
		usf_freestrhm(hm);
	} else {
		usf_freehm(hm);
	}

    return (double) globalavg/cycles;
}

