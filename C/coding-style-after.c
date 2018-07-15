/* to be able to compile it */
#define _GNU_SOURCE /* for asprintf */
#include <stdlib.h>
#include <stdio.h> 
#include <errno.h>


typedef struct list /* No need in _s suffix if we use 'struct *' form*/
{
	struct list *next; /* NULL for the last item in a list */
	int data; /* Container should be indiffferent for type data it holds */
} list_s; /* '*_t' types are reserved by POSIX */


/* 
 * Linux prefers multi-line comments in this manner.
 * Counts the number of items in a list.
 * TODO: add a recursion check.
 */
int count_list_items(const list_s *head) {
	int counter = 0;

	/* there's no need in recursive calls */
	do {
		counter++;
	} while (head->next);

	return counter;
}

/* 
 * Inserts a new list item after the one specified as the argument.
 * Use 'int' type instead of 'void' to be able to return an error.
 */
int insert_next_to_list(list_s *item, int data) {
	/* An original line was too complex */
	/* sizeof(*new_item) in case of type being replaced */
	list_s *new_item = malloc(sizeof(*new_item));
	if (!new_item) {
		//TODO: panic
		return -ENOMEM;
	}
	
	new_item->data = data;
	new_item->next = item->next;
	
	//TODO: lock
	item->next = new_item;
	//TODO: unlock
}

/* 
 * Removes an item following the one specificed as the argument.
 */
void remove_next_from_list(list_s *item) {
	list_s *item_to_remove;

	if (item->next) {
		/* There was a wrong replace-free sequence */
		item_to_remove = item->next;
		//TODO: lock
		item->next = item_to_remove->next;
		//TODO: unlock
		free(item_to_remove);
	}
}

/*
 * Returns item data as text.
 */
char *item_data(const list_s *list)
{
	/* 
	 * You must not place such data on a stack!
	 * And it have to be initialized of course.
	 * So... sounds like it is work for asprintf()
	 */
	char *buf;

	asprintf(&buf, "%d", list->data);

	return buf;
}
