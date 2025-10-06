#include <stdio.h>

int main() {
	int a = 65;
	int b = -65;

	printf("%d %d %d %d\n", a / 16, b / 16, a >> 4, b >> 4);
	printf("Done. %d\n", BUFSIZ);
	return 0;
}
