#include <stdio.h>
#include "usfstring.h"

#define STRINGTEST_ASSERT(_VALUE, _EXPECT, _FAILMSG) \
	if ((_VALUE) != (_EXPECT)) { \
		printf("stringtest: %s failure, aborting.\n", _FAILMSG); \
		exit(1); \
	}

i32 main(void) {
	/* usfstring.c test */

	printf("stringtest: Starting test!\n");

	STRINGTEST_ASSERT(!usf_sstartswith("nifty handbag", "nifty"), 0, "sstartwith");
	STRINGTEST_ASSERT(!usf_sstartswith("nifty handbag", "handbag"), 1, "sstartwith");
	printf("stringtest: sstartswith OK\n");

	STRINGTEST_ASSERT(!usf_sendswith("nifty handbag", "nifty"), 1, "sendswith");
	STRINGTEST_ASSERT(!usf_sendswith("nifty handbag", "handbag"), 0, "sendswith");
	printf("stringtest: sendswith OK\n");

	STRINGTEST_ASSERT(usf_scount("nifty handbag", 'n'), 2, "scount");
	printf("stringtest: scount OK\n");

	char mutstr[512] = {'h', 'e', 'l', 'l', 'o', '\0' };
	STRINGTEST_ASSERT(usf_sreplace(mutstr, 'l', 'w'), 2, "sreplace");
	STRINGTEST_ASSERT(strcmp(mutstr, "hewwo"), 0, "sreplace");
	printf("stringtest: sreplace OK\n");

	u64 nsplits, n;
	char str[512] = {'a', '\n', 'b', '\n', 'c', '\n', '\0'};
	char **split;
	split = usf_scsplit(str, '\n', &nsplits);
	STRINGTEST_ASSERT(nsplits, 4, "scsplit");
	for (n = 0; n < 3; n++) STRINGTEST_ASSERT(*split[n], (char) ('a' + n), "scsplit");
	STRINGTEST_ASSERT(*split[3], '\0', "scsplit");
	printf("stringtest: scsplit OK\n");

	char low[512];
	memcpy(low, "aéàëAÉÀË", sizeof("aéàëAÉÀË"));
	usf_supper(low);
	STRINGTEST_ASSERT(strcmp(low, "AÉÀËAÉÀË"), 0, "supper");
	printf("stringtest: supper OK\n");

	char high[512];
	memcpy(high, "aéàëAÉÀË", sizeof("aéàëAÉÀË"));
	usf_slower(high);
	STRINGTEST_ASSERT(strcmp(high, "aéàëaéàë"), 0, "slower");
	printf("stringtest: slower OK\n");

	STRINGTEST_ASSERT(!usf_txtcontainsline(split, 4, "a"), 0, "txtcontainsline");
	STRINGTEST_ASSERT(!usf_txtcontainsline(split, 4, "d"), 1, "txtcontainsline");
	printf("stringtest: txtcontainsline OK\n");

	usf_reversetxtlines(split, 4);
	STRINGTEST_ASSERT(strcmp(split[0], ""), 0, "reversetxtlines");
	STRINGTEST_ASSERT(strcmp(split[1], "c"), 0, "reversetxtlines");
	STRINGTEST_ASSERT(strcmp(split[2], "b"), 0, "reversetxtlines");
	STRINGTEST_ASSERT(strcmp(split[3], "a"), 0, "reversetxtlines");
	printf("stringtest: reversetxtlines OK\n");

	char cat[512];
	usf_strcat(cat, 512, 4, "this", "is", "a", "string");
	STRINGTEST_ASSERT(strcmp(cat, "thisisastring"), 0, "reversetxtlines");
	printf("stringtest: strcat OK\n");

	usf_free(split);

	printf("stringtest: usfstring OK (ALL TESTS PASSED)\n");
	return 0;
}
