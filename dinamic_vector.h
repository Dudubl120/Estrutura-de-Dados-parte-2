#ifndef DINAMIC_VECTOR_H
#define DINAMIC_VECTOR_H

#include "linkedlist.h"

/*
 * A dynamic array (vector) whose elements are pointers to struct LinkedList.
 * Each LinkedList represents one row of the CSV, with heterogeneous fields.
 */
struct Dinamic_Vector {
    int n;           /* number of elements currently stored */
    int n_max;       /* current capacity (max elements before realloc) */
    struct LinkedList **v;  /* array of pointers to LinkedList */
};

/**
 * Create and return a new, empty dynamic vector.
 * If malloc fails, exits(1).
 */
struct Dinamic_Vector *dv_create(void);

/**
 * Insert 'list_ptr' at the end of 'dv'; if dv is full, its capacity doubles.
 * If dv==NULL or list_ptr==NULL or realloc fails, exits(1).
 */
void dv_insert(struct Dinamic_Vector *dv, struct LinkedList *list_ptr);

/**
 * Return how many elements are currently stored in 'dv'; if dv==NULL, returns 0.
 */
int dv_size(const struct Dinamic_Vector *dv);

/**
 * Return the struct LinkedList* stored at index 'i'.  
 * If dv==NULL or i is out of bounds, exits(1).
 */
struct LinkedList *dv_get(const struct Dinamic_Vector *dv, int i);

/**
 * Free the dynamic vector itself (the array and the struct).  
 * Does NOT free the LinkedList* elements; caller must free each list separately.
 * Safe if dv==NULL.
 */
void dv_free(struct Dinamic_Vector *dv);

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
int dv_read_from_csv(struct Dinamic_Vector *dv, const char *filename);

/**
 * Write all data from the dynamic vector to a CSV file.
 * Creates a backup of the original file before writing.
 * 
 * Returns 0 on success; returns 1 on any error:
 *   - File open failure
 *   - Write operation failure
 */
int dv_write_to_csv(const struct Dinamic_Vector *dv, const char *filename);

/**
 * Print every LinkedList stored in 'dv'.  
 * For each i in [0..dv_size(dv)-1], prints:
 *   Row i: <non-null fields in that list>
 * If dv==NULL or empty, prints nothing.
 */
void dv_print_all(const struct Dinamic_Vector *dv);

/**
 * Get a specific field from a row and column in the dynamic vector.
 * Returns pointer to the Field, or NULL if invalid indices.
 */
struct Field *get_field_by_index(const struct Dinamic_Vector *dv, int line, int column);

/**
 * Consult patients by a specific field value.
 * Searches through all records and prints matching ones.
 * The field to search is specified by its index (e.g., 1=CPF, 2=Name, etc.).
 */
void dv_consult_by_field(const struct Dinamic_Vector *dv, const char *search, int field_index);


/**
 * Consult patients by a specific field value.
 * Searches through all records and prints matching ones.
 * The field to search is specified by its index (e.g., 1=CPF, 2=Name, etc.).
 */
void dv_reassign_ids(struct Dinamic_Vector *dv);

/**
 * Reassign sequential IDs to all records in the dynamic vector.
 * This is used after removing records to maintain sequential numbering.
 */

 /**
 * Remove a record at the specified index from the dynamic vector.
 * After removal, IDs are automatically reassigned to maintain sequential numbering.
 */
void dv_remove(struct Dinamic_Vector *dv, int idx);


/**
 * Remove a record at the specified index from the dynamic vector.
 * After removal, IDs are automatically reassigned to maintain sequential numbering.
 */
void dv_free_all(struct Dinamic_Vector *dv);

#endif