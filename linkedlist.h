#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * FieldType indicates the type of data stored in a Field.
 */
enum FieldType {
    FIELD_INT,
    FIELD_STRING,
    FIELD_NULL
};

/*
 * Field struct: holds either an integer or a string.
 * If type == FIELD_INT, use 'i'.
 * If type == FIELD_STRING, use 's' (must be malloc'd).
 * If type == FIELD_NULL, neither is used.
 */
struct Field {
    enum FieldType type;
    int i;
    char *s;
};

/*
 * ListNode struct: a node in a doubly linked list, holding a Field.
 */
struct ListNode {
    struct Field field;
    struct ListNode *next;
    struct ListNode *prev;
};

/*
 * LinkedList struct: a doubly linked list of Fields.
 */
struct LinkedList {
    int count;
    struct ListNode *first;
    struct ListNode *last;
};

/**
 * Create and return a new, empty linked list.
 * If malloc fails, exits(1).
 */
struct LinkedList *ll_create(void);

/**
 * Append a Field 'f' to the end of list 'l'.
 * Takes ownership of 'f' (if FIELD_STRING, f.s must be malloc'd).
 * Exits(1) on malloc failure or if l == NULL.
 */
void ll_append_field(struct LinkedList *l, struct Field f);

/**
 * Print all non-null fields in 'l'.
 * For each node, if field.type==FIELD_INT: print "%d ".
 * If field.type==FIELD_STRING: print "%s ".
 * If field.type==FIELD_NULL: skip (print nothing).
 * After all nodes, print a newline.
 * If l==NULL or empty, prints just a newline.
 */
void ll_print(const struct LinkedList *l);

/**
 * Return the number of nodes currently in 'l'.
 * If l==NULL, returns 0.
 */
int ll_size(const struct LinkedList *l);

/**
 * Return 1 if 'l' is NULL or has zero nodes; else return 0.
 */
int ll_is_empty(const struct LinkedList *l);

/**
 * Remove a node from the linked list at a specific index.
 * Frees any allocated memory for the field.
 * Returns 0 on success, or 1 if the index is invalid.
 */
int ll_remove_node(struct LinkedList *l, int index);

/**
 * Add a new node to the linked list at a specific position.
 * If the position is invalid, appends the node to the end of the list.
 * Returns 0 on success, or 1 if memory allocation fails.
 */
int ll_add_node(struct LinkedList *l, int position, struct Field new_field);

/**
 * Free every node in 'l', free any malloc’d strings inside fields,
 * then free l itself. Safe if l==NULL.
 */
void ll_free(struct LinkedList *l);

/**
 * Remove all fields in the linked list (frees strings, but not the nodes).
 */
void ll_remove_all_fields(struct LinkedList *l);

/**
 * Create a deep copy of a LinkedList (row).
 * Returns a pointer to the new copy, or NULL if src is NULL.
 */
struct LinkedList *ll_copy(const struct LinkedList *src);

/**
 * Update multiple fields in a LinkedList row.
 * For each parameter, if it is "-", the field is not updated.
 * Returns 0 on success.
 */
int ll_update_fields(struct LinkedList *l, const char *cpf, const char *nome, const char *idade, const char *data);

/**
 * Create a new LinkedList (row) from the given fields.
 * Returns a pointer to the new LinkedList.
 */
struct LinkedList *ll_create_from_fields(int id, const char *cpf, const char *nome, int idade, const char *data);

#endif /* LINKEDLIST_H */