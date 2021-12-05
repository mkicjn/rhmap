#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ezrhmap.h"

typedef enum {false, true} bool; /* C89 compat */

struct entry {
	char *name;
	long number;
};

struct entry *entry_create(char *name, long number)
{
	struct entry *e = malloc(sizeof(*e));
	e->name = malloc(strlen(name)+1);
	strcpy(e->name, name);
	e->number = number;
	return e;
}

void entry_destroy(struct entry *e)
{
	if (e == NULL)
		return;
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

size_t strhash(char *s)
{
	return djb2(s, strlen(s));
}

EZRHMAP_DECLARE(phonebook, strhash, char *, struct entry *, entry_destroy)

int main()
{
	struct phonebook *book = phonebook_create();

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
			phonebook_set(book, name, entry_create(name, num));
		} else if (sscanf(buf, "get %s", name) >= 1) {
			struct entry **e = phonebook_get(book, name);
			if (e == NULL)
				puts("Not found");
			else
				printf("%s: %ld\n", (*e)->name, (*e)->number);
		} else if (sscanf(buf, "del %s", name) >= 1) {
			struct entry **e = phonebook_del(book, name);
			if (e == NULL)
				puts("Not found");
		} else {
			puts("Unknown command");
		}
	}

	phonebook_destroy(book);
	return 0;
}
