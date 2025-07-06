#include "linkedlist.h"

/*
 * Create and return a new, empty linked list.
 * If malloc fails, exit(1).
 */
LinkedList *ll_create(void) {
    LinkedList *l = (LinkedList *)malloc(sizeof(LinkedList));
    if (l == NULL) {
        exit(1);
    }
    l->count = 0;
    l->first = NULL;
    l->last = NULL;
    return l;
}

/*
 * Append a Field 'f' at the end of list 'l'.
 * Takes ownership of f. If l==NULL or malloc fails → exit(1).
 */
void ll_append_field(LinkedList *l, Field f) {
    if (l == NULL) {
        exit(1);
    }

    ListNode *node = (ListNode *)malloc(sizeof(ListNode));
    if (node == NULL) {
        /* If f.type==FIELD_STRING, free f.s to avoid leak */
        if (f.type == FIELD_STRING) {
            free(f.s);
        }
        exit(1);
    }

    node->field = f;       /* copy the Field struct (including f.s or f.i) */
    node->next = NULL;
    node->prev = l->last;

    if (l->last != NULL) {
        l->last->next = node;
    } else {
        /* list was empty: this node is also first */
        l->first = node;
    }
    l->last = node;
    l->count++;
}

/*
 * Update a field in a specific node of the linked list.
 */
int ll_update_field(struct LinkedList *l, int index, struct Field new_field) {
    if (l == NULL || index < 0 || index >= l->count) {
        return 1; // Invalid index
    }

    struct ListNode *current = l->first;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    // Free the old field if it's a string
    if (current->field.type == FIELD_STRING) {
        free(current->field.s);
    }

    // Update the field
    current->field = new_field;
    return 0;
}

/*
 * Remove a node from the linked list at a specific index.
 */
int ll_remove_node(struct LinkedList *l, int index) {
    if (l == NULL || index < 0 || index >= l->count) {
        return 1; // Invalid index
    }

    struct ListNode *current = l->first;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    // Update pointers
    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
        l->first = current->next; // Removing the first node
    }

    if (current->next != NULL) {
        current->next->prev = current->prev;
    } else {
        l->last = current->prev; // Removing the last node
    }

    // Free the field if it's a string
    if (current->field.type == FIELD_STRING) {
        free(current->field.s);
    }

    free(current);
    l->count--;
    return 0;
}

/*
 * Add a new node to the linked list at a specific position.
 */
int ll_add_node(struct LinkedList *l, int position, struct Field new_field) {
    if (l == NULL) {
        return 1; // Invalid list
    }

    struct ListNode *new_node = (struct ListNode *)malloc(sizeof(struct ListNode));
    if (new_node == NULL) {
        return 1; // Memory allocation failure
    }

    new_node->field = new_field;
    new_node->next = NULL;
    new_node->prev = NULL;

    if (position <= 0 || l->count == 0) {
        // Add to the beginning
        new_node->next = l->first;
        if (l->first != NULL) {
            l->first->prev = new_node;
        }
        l->first = new_node;
        if (l->last == NULL) {
            l->last = new_node;
        }
    } else if (position >= l->count) {
        // Add to the end
        new_node->prev = l->last;
        if (l->last != NULL) {
            l->last->next = new_node;
        }
        l->last = new_node;
        if (l->first == NULL) {
            l->first = new_node;
        }
    } else {
        // Add in the middle
        struct ListNode *current = l->first;
        for (int i = 0; i < position; i++) {
            current = current->next;
        }
        new_node->next = current;
        new_node->prev = current->prev;
        if (current->prev != NULL) {
            current->prev->next = new_node;
        }
        current->prev = new_node;
    }

    l->count++;
    return 0;
}

/*
 * Print all non-null fields in 'l'. Then print a newline.
 * If l==NULL, just prints a newline.
 */
void ll_print(const LinkedList *l) {
    if (l == NULL) {
        printf("\n");
        return;
    }
    ListNode *cur = l->first;
    while (cur != NULL) {
        Field f = cur->field;
        if (f.type == FIELD_INT) {
            printf("%d ", f.i);
        } else if (f.type == FIELD_STRING) {
            printf("%s ", f.s);
        }
        /* if f.type == FIELD_NULL, skip printing entirely */
        cur = cur->next;
    }
    printf("\n");
}

/*
 * Return number of nodes in l. If l==NULL, return 0.
 */
int ll_size(const LinkedList *l) {
    if (l == NULL) {
        return 0;
    }
    return l->count;
}

/*
 * Return 1 if l is NULL or has zero nodes; else return 0.
 */
int ll_is_empty(const LinkedList *l) {
    if (l == NULL) {
        return 1;
    }
    return (l->count == 0 ? 1 : 0);
}

/*
 * Free every node in l, free any malloc’d strings inside fields,
 * then free l itself. Safe if l==NULL.
 */
void ll_free(LinkedList *l) {
    if (l == NULL) {
        return;
    }
    ListNode *cur = l->first;
    while (cur != NULL) {
        ListNode *next = cur->next;
        if (cur->field.type == FIELD_STRING) {
            free(cur->field.s);
        }
        free(cur);
        cur = next;
    }
    free(l);
}