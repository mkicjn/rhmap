#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "rhmap.h"

struct entry {
	char *name;
	long number;
};

struct entry *create_entry(char *name, long number)
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

DECLARE_RHMAP(phonebook, struct entry *)

#define INIT_SIZE 64
struct phonebook *create_phonebook(void)
{
	struct phonebook *p = malloc(sizeof(*p));
	struct phonebook_bucket *b = malloc(INIT_SIZE);
	phonebook_init(p, b, INIT_SIZE);
	return p;
}

void destroy_phonebook(struct phonebook *p)
{
	phonebook_clear(p, destroy_entry);
	free(p->buckets);
	free(p);
}

#define LOAD_THRESHOLD 0.9
bool maybe_resize(struct phonebook *p)
{
	size_t s;
	struct phonebook_bucket *b;
	if (phonebook_load_factor(p) < LOAD_THRESHOLD)
		return false;
	s = sizeof(*b) * phonebook_capacity(p) * 2;
	b = malloc(s);
	free(phonebook_rehash(p, b, s));
	return true;
}

size_t djb2(const char *str, size_t n)
{
	size_t k = 5381;
	while (n --> 0)
		k = (k << 5) + k + *str++;
	return k;
}

int main()
{
	struct phonebook *book = create_phonebook();

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
				puts("Updated");
			} else if (phonebook_insert(book, h, create_entry(name, num)) != NULL) {
				puts("Inserted");
			} else {
				puts("Failed");
			}
			if (maybe_resize(book))
				printf("Resized to %lu\n", phonebook_capacity(book));
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

	destroy_phonebook(book);
	return 0;
}
