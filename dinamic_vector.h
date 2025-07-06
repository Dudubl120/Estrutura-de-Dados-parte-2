#ifndef DINAMIC_VECTOR_H
#define DINAMIC_VECTOR_H

#include "linkedlist.h"

/*
 * A dynamic array (vector) whose elements are pointers to LinkedList.
 * Each LinkedList represents one row of the CSV, with heterogeneous fields.
 */
typedef struct dinamic_vector {
    int n;           /* number of elements currently stored */
    int n_max;       /* current capacity (max elements before realloc) */
    LinkedList **v;  /* array of pointers to LinkedList */
} Dinamic_Vector;

/**
 * Create and return a new, empty dynamic vector.
 * If malloc fails, exits(1).
 */
Dinamic_Vector *dv_create(void);

/**
 * Insert 'list_ptr' at the end of 'dv'; if dv is full, its capacity doubles.
 * If dv==NULL or list_ptr==NULL or realloc fails, exits(1).
 */
void dv_insert(Dinamic_Vector *dv, LinkedList *list_ptr);

/**
 * Return how many elements are currently stored in 'dv'; if dv==NULL, returns 0.
 */
int dv_size(const Dinamic_Vector *dv);

/**
 * Return the LinkedList* stored at index 'i'.  
 * If dv==NULL or i is out of bounds, exits(1).
 */
LinkedList *dv_get(const Dinamic_Vector *dv, int i);

/**
 * Free the dynamic vector itself (the array and the struct).  
 * Does NOT free the LinkedList* elements; caller must free each list separately.
 * Safe if dv==NULL.
 */
void dv_free(Dinamic_Vector *dv);

/**
 * Read a CSV file and push each row into 'dv' as a LinkedList of heterogeneous fields.
 *
 *   - The CSV is expected to have exactly 5 columns per row:
 *       0: ID           (integer)
 *       1: CPF          (string)
 *       2: Nome         (string)
 *       3: Idade        (integer)
 *       4: Data_Cadastro(string)
 *
 *   - The first line of the CSV is treated as a header and skipped entirely.
 *   - For each subsequent line:
 *       * Split by commas, preserving empty fields.
 *       * Build a LinkedList:
 *           - If a field is empty, append FIELD_NULL.
 *           - If column index is 0 or 3, and token is non-empty, convert to int → FIELD_INT.
 *           - Otherwise (index 1,2,4 with non-empty token), strdup(token) → FIELD_STRING.
 *       * dv_insert(dv, that LinkedList).
 *
 * Returns 0 on success; returns 1 on any error:
 *   - File open failure
 *   - Memory allocation failure
 */
int dv_read_from_csv(Dinamic_Vector *dv, const char *filename);

/**
 * Print every LinkedList stored in 'dv'.  
 * For each i in [0..dv_size(dv)-1], prints:
 *   Row i: <non-null fields in that list>
 * If dv==NULL or empty, prints nothing.
 */
void dv_print_all(const Dinamic_Vector *dv);

void dv_consult_by_field(const Dinamic_Vector *dv, const char *query, int field_index);

#endif 