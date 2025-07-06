#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * A FieldType indicates what kind of data a node holds.
 * - FIELD_INT:    an integer value  
 * - FIELD_STRING: a null-terminated string  
 * - FIELD_NULL:   an explicit “NULL” placeholder (no data)
 */
typedef enum {
    FIELD_INT,
    FIELD_STRING,
    FIELD_NULL
} FieldType;

/*
 * A Field is a small struct that holds either an int or a string.
 * If type == FIELD_INT, 'i' is valid.  
 * If type == FIELD_STRING, 's' is a malloc’d string.  
 * If type == FIELD_NULL, neither is used.
 */
typedef struct Field {
    FieldType type;
    int i;      /* used if type == FIELD_INT */
    char *s;    /* used if type == FIELD_STRING */
} Field;

/*
 * A doubly linked list node that holds exactly one Field.
 */
typedef struct ListNode {
    Field field;               /* this node's field */
    struct ListNode *next;     /* next node in the list */
    struct ListNode *prev;     /* previous node in the list */
} ListNode;

/*
 * A doubly linked list. 'count' tracks how many nodes are present.
 * 'first' points to the head; 'last' points to the tail.
 */
typedef struct LinkedList {
    int count;        /* number of nodes */
    ListNode *first;  /* pointer to first node */
    ListNode *last;   /* pointer to last node */
} LinkedList;

/**
 * Create and return a new, empty linked list.
 * If malloc fails, prints nothing and exits(1).
 */
LinkedList *ll_create(void);

/**
 * Append a Field 'f' to the end of list 'l'.  
 * Takes ownership of 'f' (i.e., if f.type==FIELD_STRING, f.s must be malloc’d).
 * Exits(1) on malloc failure or if l == NULL.
 */
void ll_append_field(LinkedList *l, Field f);

/**
 * Print all non-null fields in 'l'.  
 * For each node, if field.type==FIELD_INT: print "%d ".  
 * If field.type==FIELD_STRING: print "%s ".  
 * If field.type==FIELD_NULL: skip (print nothing).  
 * After all nodes, print a newline.  
 * If l==NULL or empty, prints just a newline.
 */
void ll_print(const LinkedList *l);

/**
 * Return the number of nodes currently in 'l'.  
 * If l==NULL, returns 0.
 */
int ll_size(const LinkedList *l);

/**
 * Return 1 if 'l' is NULL or has zero nodes; else return 0.
 */
int ll_is_empty(const LinkedList *l);

/**
 * Update a field in a specific node of the linked list.
 * If the field is a string, it will be replaced with a new value.
 * If the field is an integer, it will be updated directly.
 * Returns 0 on success, or 1 if the index is invalid.
 */
int ll_update_field(LinkedList *l, int index, Field new_field);

/**
 * Remove a node from the linked list at a specific index.
 * Frees any allocated memory for the field.
 * Returns 0 on success, or 1 if the index is invalid.
 */
int ll_remove_node(LinkedList *l, int index);

/**
 * Add a new node to the linked list at a specific position.
 * If the position is invalid, appends the node to the end of the list.
 * Returns 0 on success, or 1 if memory allocation fails.
 */
int ll_add_node(LinkedList *l, int position, Field new_field);

/**
 * Free every node in 'l', free any malloc’d strings inside fields,
 * then free 'l' itself. Safe if l==NULL.
 */
void ll_free(LinkedList *l);

#endif /* LINKEDLIST_H */