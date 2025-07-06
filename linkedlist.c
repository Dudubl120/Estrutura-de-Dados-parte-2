#include "linkedlist.h"

/*
 * Create and return a new, empty linked list.
 * If malloc fails, exit(1).
 */
struct LinkedList *ll_create(void) {
    struct LinkedList *l = (struct LinkedList *)malloc(sizeof(struct LinkedList));
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
void ll_append_field(struct LinkedList *l, struct Field f) {
    if (l == NULL) {
        exit(1);
    }

    struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
    if (node == NULL) {
        if (f.type == FIELD_STRING) {
            free(f.s);
        }
        exit(1);
    }

    node->field = f;
    node->next = NULL;
    node->prev = l->last;

    if (l->last != NULL) {
        l->last->next = node;
    } else {
        l->first = node;
    }
    l->last = node;
    l->count++;
}

/*
 * Update multiple fields in a LinkedList row.
 * For each parameter, if it is "-", the field is not updated.
 */
int ll_update_fields(struct LinkedList *l, const char *cpf, const char *nome, const char *idade, const char *data) {
    if (!l) return 1;
    struct ListNode *cur = l->first;
    int idx = 0;
    while (cur) {
        if (idx == 1 && cpf && strcmp(cpf, "-") != 0) { // CPF
            free(cur->field.s);
            cur->field.s = strdup(cpf);
        }
        if (idx == 2 && nome && strcmp(nome, "-") != 0) { // Name
            free(cur->field.s);
            cur->field.s = strdup(nome);
        }
        if (idx == 3 && idade && strcmp(idade, "-") != 0) { // Age
            cur->field.i = atoi(idade);
        }
        if (idx == 4 && data && strcmp(data, "-") != 0) { // Register_Date
            free(cur->field.s);
            cur->field.s = strdup(data);
        }
        cur = cur->next;
        idx++;
    }
    return 0;
}

/*
 * Remove all fields in the linked list (frees strings, but not the nodes).
 */
void ll_remove_all_fields(struct LinkedList *l) {
    if (!l) return;
    struct ListNode *cur = l->first;
    while (cur) {
        if (cur->field.type == FIELD_STRING && cur->field.s)
            free(cur->field.s);
        cur = cur->next;
    }
}

/*
 * Remove a node from the linked list at a specific index.
 */
int ll_remove_node(struct LinkedList *l, int index) {
    if (l == NULL || index < 0 || index >= l->count) {
        return 1;
    }

    struct ListNode *current = l->first;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    if (current->prev != NULL) {
        current->prev->next = current->next;
    } else {
        l->first = current->next;
    }

    if (current->next != NULL) {
        current->next->prev = current->prev;
    } else {
        l->last = current->prev;
    }

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
        return 1;
    }

    struct ListNode *new_node = (struct ListNode *)malloc(sizeof(struct ListNode));
    if (new_node == NULL) {
        return 1;
    }

    new_node->field = new_field;
    new_node->next = NULL;
    new_node->prev = NULL;

    if (position <= 0 || l->count == 0) {
        new_node->next = l->first;
        if (l->first != NULL) {
            l->first->prev = new_node;
        }
        l->first = new_node;
        if (l->last == NULL) {
            l->last = new_node;
        }
    } else if (position >= l->count) {
        new_node->prev = l->last;
        if (l->last != NULL) {
            l->last->next = new_node;
        }
        l->last = new_node;
        if (l->first == NULL) {
            l->first = new_node;
        }
    } else {
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
 * Create a new LinkedList with fields initialized from the given parameters.
 */
struct LinkedList *ll_create_from_fields(int id, const char *cpf, const char *nome, int idade, const char *data) {
    struct LinkedList *l = ll_create();
    struct Field f;
    f.type = FIELD_INT; f.i = id; f.s = NULL;
    ll_append_field(l, f);
    f.type = FIELD_STRING; f.s = strdup(cpf);
    ll_append_field(l, f);
    f.type = FIELD_STRING; f.s = strdup(nome);
    ll_append_field(l, f);
    f.type = FIELD_INT; f.i = idade; f.s = NULL;
    ll_append_field(l, f);
    f.type = FIELD_STRING; f.s = strdup(data);
    ll_append_field(l, f);
    return l;
}

/*
 * Print all non-null fields in 'l'. Then print a newline.
 * If l==NULL, just prints a newline.
 */
void ll_print(const struct LinkedList *l) {
    if (l == NULL) {
        printf("\n");
        return;
    }
    struct ListNode *cur = l->first;
    while (cur != NULL) {
        struct Field f = cur->field;
        if (f.type == FIELD_INT) {
            printf("%d ", f.i);
        } else if (f.type == FIELD_STRING) {
            printf("%s ", f.s);
        }
        cur = cur->next;
    }
    printf("\n");
}

/**
 * Create a deep copy of a LinkedList (row).
 */
struct LinkedList *ll_copy(const struct LinkedList *src) {
    if (!src) return NULL;
    struct LinkedList *copy = ll_create();
    struct ListNode *cur = src->first;
    while (cur) {
        struct Field f;
        f.type = cur->field.type;
        if (f.type == FIELD_STRING && cur->field.s) {
            f.s = strdup(cur->field.s);
        } else {
            f.i = cur->field.i;
            f.s = NULL;
        }
        ll_append_field(copy, f);
        cur = cur->next;
    }
    return copy;
}

/*
 * Return number of nodes in l. If l==NULL, return 0.
 */
int ll_size(const struct LinkedList *l) {
    if (l == NULL) {
        return 0;
    }
    return l->count;
}

/*
 * Return 1 if l is NULL or has zero nodes; else return 0.
 */
int ll_is_empty(const struct LinkedList *l) {
    if (l == NULL) {
        return 1;
    }
    return (l->count == 0 ? 1 : 0);
}

/*
 * Free every node in l, free any malloc’d strings inside fields,
 * then free l itself. Safe if l==NULL.
 */
void ll_free(struct LinkedList *l) {
    if (l == NULL) {
        return;
    }
    struct ListNode *cur = l->first;
    while (cur != NULL) {
        struct ListNode *next = cur->next;
        if (cur->field.type == FIELD_STRING) {
            free(cur->field.s);
        }
        free(cur);
        cur = next;
    }
    free(l);
}