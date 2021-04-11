#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rhmap.h"

struct entry {
	char *name;
	long number;
};

DECLARE_RHMAP(phonebook, struct entry *)

struct entry *new_entry(char *name, long number)
{
	struct entry *e = malloc(sizeof(*e));
	e->name = strdup(name);
	e->number = number;
	return e;
}

void destroy_entry(struct entry *e)
{
	free(e->name);
	free(e);
}

unsigned long long djb2(const char *str, size_t n)
{
	unsigned long long k = 5381;
	while (n --> 0)
		k = (k << 5) + k + *str++;
	return k;
}

#define MAX_ENTRIES 10
int main()
{
	struct phonebook_bucket *mem = malloc(MAX_ENTRIES * sizeof(*mem));
	struct phonebook *book = malloc(sizeof(*book));

	phonebook_init(book, mem, MAX_ENTRIES * sizeof(*mem));

	puts("Available commands:");
	puts("set <name> <number>");
	puts("get <name>");
	puts("del <name>");

	for (;;) {
		char buf[160], name[80];
		long num;
		if (fgets(buf, sizeof(buf), stdin) == NULL)
			break;
		if (sscanf(buf, "set %s %ld", name, &num) >= 2) {
			if (phonebook_insert(book, djb2(name, strlen(name)-1), new_entry(name, num)) != NULL)
				puts("Success");
			else
				puts("Failure");
		} else if (sscanf(buf, "get %s", name) >= 1) {
			struct entry **e = phonebook_search(book, djb2(name, strlen(name)-1));
			if (e != NULL)
				printf("%s: %ld\n", (*e)->name, (*e)->number);
			else
				puts("Not found");
		} else if (sscanf(buf, "del %s", name) >= 1) {
			struct entry **e = phonebook_remove(book, djb2(name, strlen(name)-1));
			if (e != NULL) {
				destroy_entry(*e);
				puts("Success");
			} else {
				puts("Not found");
			}
		} else {
			puts("Unknown command");
		}
	}

	phonebook_clear(book, destroy_entry);
	free(mem);
	free(book);
	return 0;
}
