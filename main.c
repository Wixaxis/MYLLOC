#include <stdio.h>
#include "custom_unistd.h"
#include "mylloc.h"
#define MODE 1 //0-Own tests, 1-Teacher's tests
#if MODE
#define TEACHER
#else
#define SELF
#endif

#ifdef SELF
int main(int argc, char **argv)
{

	// Tutaj można pisać kod
	// Zapoznaj się z funkcją main w bloku #if...#endif
	heap_setup();
	int *value = heap_malloc(sizeof(int));
	*value = 30;
	printf("hello world\nvalue = %d\n", *value);
	heap_dump_debug_information();
	int *pile1 = heap_malloc(sizeof(int) * 100);
	if (NULL != pile1)
		for (int i = 0; i < 100; i++)
			pile1[i] = i;
	else
		printf("allocating pile1 of size 100 (int) failed :c\n");
	double *pile2 = heap_malloc(sizeof(double) * 100);
	if (NULL != pile2)
		for (int i = 0; i < 100; i++)
			pile2[i] = 0.678 - i;
	else
		printf("allocating pile2 of size 100 (double) failed :c\n");

	if (NULL != pile1)
		for (int i = 0; i < 100; i++)
			printf("%d ", pile1[i]);
	printf("\n");
	if (NULL != pile2)
		for (int i = 0; i < 100; i++)
			printf("%lf ", pile2[i]);
	printf("\n");
	if (NULL != pile1)
		for (int i = 0; i < 100; i++)
			printf("%d ", pile1[i]);
	printf("\n");
	heap_free(value);
	display_errs();
	heap_dump_debug_information();
	heap_free(pile1);
	heap_free(pile2);
	heap_dump_debug_information();
	return 0;
}
#endif

#ifdef TEACHER
#include "custom_unistd.h"

int main(int argc, char **argv)
{
	int status = heap_setup();
	assert(status == 0);

	// parametry pustej sterty
	size_t free_bytes = heap_get_free_space();
	size_t used_bytes = heap_get_used_space();

	void *p1 = heap_malloc(8 * 1024 * 1024);  // 8MB
	void *p2 = heap_malloc(8 * 1024 * 1024);  // 8MB
	void *p3 = heap_malloc(8 * 1024 * 1024);  // 8MB
	void *p4 = heap_malloc(16 * 1024 * 1024); // 16MB
	// display_errs();
	assert(p1 != NULL);						  // malloc musi się udać
	assert(p2 != NULL);						  // malloc musi się udać
	assert(p3 != NULL);						  // malloc musi się udać
	assert(p4 == NULL);						  // nie ma prawa zadziałać

	status = heap_validate();
	assert(status == 0); // sterta nie może być uszkodzona

	// zaalokowano 3 bloki
	assert(heap_get_used_blocks_count() == 3);

	// zajęto 24MB sterty; te 2000 bajtów powinno
	// wystarczyć na wewnętrzne struktury sterty
	assert(
		heap_get_used_space() >= 24 * 1024 * 1024 &&
		heap_get_used_space() <= 24 * 1024 * 1024 + 2000);

	// zwolnij pamięć
	heap_free(p1);
	heap_free(p2);
	heap_free(p3);

	// wszystko powinno wrócić do normy
	assert(heap_get_free_space() == free_bytes);
	assert(heap_get_used_space() == used_bytes);

	// już nie ma bloków
	assert(heap_get_used_blocks_count() == 0);

	return 0;
}
#endif