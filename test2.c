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
	e->name = malloc(strlen(name));
	strcpy(e->name, name);
	e->number = number;
	return e;
}

void destroy_entry(struct entry *e)
{
	free(e->name);
	free(e);
}

size_t djb2(const char *str, size_t n)
{
	size_t k = 5381;
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
			unsigned long h = djb2(name, strlen(name));
			struct entry **e = phonebook_search(book, h);
			if (e != NULL) {
				(*e)->number = num;
				puts("Changed");
			} else if (phonebook_insert(book, h, new_entry(name, num)) != NULL) {
				puts("Inserted");
			} else {
				puts("Failed");
			}
		} else if (sscanf(buf, "get %s", name) >= 1) {
			struct entry **e = phonebook_search(book, djb2(name, strlen(name)));
			if (e != NULL)
				printf("%s: %ld\n", (*e)->name, (*e)->number);
			else
				puts("Not found");
		} else if (sscanf(buf, "del %s", name) >= 1) {
			struct entry **e = phonebook_remove(book, djb2(name, strlen(name)));
			if (e != NULL) {
				destroy_entry(*e);
				puts("Deleted");
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
