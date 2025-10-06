#include "usfdynarr.h"

usf_dynarr *usf_newda(uint64_t size) {
	usf_dynarr *da = malloc(sizeof(usf_dynarr));
	usf_data *array = malloc(sizeof(usf_data) * size);

	if (da == NULL || array == NULL) return NULL;

	da->array = array;
	da->size = 0;
	da->capacity = size;

	return da;
}

usf_dynarr *usf_arrtodyn(usf_data *arr, uint64_t size) {
	/* Transforms an array of usf_data to a dynamic array */
	usf_dynarr *da;
	usf_data *array;

	da = usf_newda(size);
	if (da == NULL) return NULL;

	if (arr) {
		array = da->array;
		memcpy(array, arr, size * sizeof(uint64_t)); /* Copy old array to dynamic one */
		free(arr);
	}

	return da;
}

usf_data usf_daappend(usf_dynarr *da, usf_data data) {
	return usf_daset(da, da->size, data);
}

usf_data usf_daget(usf_dynarr *da, uint64_t index) {
	if (da == NULL || index >= da->size) return USFNULL;
	return da->array[index];
}

usf_data usf_daset(usf_dynarr *da, uint64_t index, usf_data data) {
	uint64_t newsize;

	if (da == NULL) return USFNULL;

	if (da->capacity <= index) {
		/* Resize */
		newsize = index * 2 + 1; /* Prevent size locking at 0 */

		da->array = realloc(da->array, sizeof(usf_data) * newsize);
		da->capacity = newsize;
	}

	/* Set virtual size as if it were just enough for this element */
	if (da->size <= index) da->size = index + 1;

	da->array[index] = data;

	return data;
}

void usf_freeda(usf_dynarr *da) {
	/* Frees a dynamic array as if it were malloc'ed and free() was called */
	free(da -> array);
	free(da);
}
