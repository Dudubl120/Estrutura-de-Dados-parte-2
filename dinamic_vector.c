#include "dinamic_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* initial capacity: static int can be adjusted if needed */
static int initial_cap = 4;

/*
 * Create and return an empty dynamic vector.
 * On any malloc failure, exit(1).
 */
struct Dinamic_Vector *dv_create(void) {
    struct Dinamic_Vector *dv = (struct Dinamic_Vector *)malloc(sizeof(struct Dinamic_Vector));
    if (dv == NULL) {
        exit(1);
    }
    dv->n = 0;
    dv->n_max = initial_cap;
    dv->v = (struct LinkedList **)malloc(sizeof(struct LinkedList *) * dv->n_max);
    if (dv->v == NULL) {
        free(dv);
        exit(1);
    }
    return dv;
}

/*
 * Double the capacity of 'dv->v' when dv->n == dv->n_max.
 * On realloc failure or dv==NULL, exit(1).
 */
static void dv_reallocate(struct Dinamic_Vector *dv) {
    if (dv == NULL) {
        exit(1);
    }
    dv->n_max *= 2;
    struct LinkedList **new_block = (struct LinkedList **)realloc(dv->v, sizeof(struct LinkedList *) * dv->n_max);
    if (new_block == NULL) {
        exit(1);
    }
    dv->v = new_block;
}

/*
 * Insert 'list_ptr' at the end of 'dv'. Resize if needed.
 * If dv==NULL or list_ptr==NULL, exit(1).
 */
void dv_insert(struct Dinamic_Vector *dv, struct LinkedList *list_ptr) {
    if (dv == NULL || list_ptr == NULL) {
        exit(1);
    }
    if (dv->n == dv->n_max) {
        dv_reallocate(dv);
    }
    dv->v[dv->n++] = list_ptr;
}

/*
 * Return how many elements are stored in dv. If dv==NULL, return 0.
 */
int dv_size(const struct Dinamic_Vector *dv) {
    if (dv == NULL) {
        return 0;
    }
    return dv->n;
}

/*
 * Return the LinkedList* stored at index i.
 * If dv==NULL or i out of bounds, exit(1).
 */
struct LinkedList *dv_get(const struct Dinamic_Vector *dv, int i) {
    if (dv == NULL || i < 0 || i >= dv->n) {
        exit(1);
    }
    return dv->v[i];
}

/*
 * Free the dynamic vector itself (array + struct).  
 * Does NOT free the LinkedList* contents. If dv==NULL, do nothing.
 */
void dv_free(struct Dinamic_Vector *dv) {
    if (dv == NULL) {
        return;
    }
    free(dv->v);
    free(dv);
}

/*
 * Helper to split a CSV line into exactly 5 tokens, preserving empty fields.
 * Returns a newly malloc’d array of 5 char* (each strndup’d, possibly empty string).
 * *num_tokens is set to 5.  
 * On any malloc failure, exit(1).  
 */
static char **split_csv_line(const char *line, int *num_tokens) {
    /* We expect 5 columns per line. Allocate array of length 5. */
    int expected = 5;
    char **fields = (char **)malloc(sizeof(char *) * expected);
    if (fields == NULL) {
        exit(1);
    }

    int start = 0;
    int field_index = 0;
    int len = (int)strlen(line);

    for (int i = 0; i <= len && field_index < expected; i++) {
        if (line[i] == ',' || line[i] == '\n' || line[i] == '\r' || line[i] == '\0') {
            /* substring is line[start..i-1], length = i-start */
            int sublen = i - start;
            /* Remove any trailing '\r' or '\n' characters inside the field */
            while (sublen > 0 && (line[start + sublen -1] == '\r' || line[start + sublen -1] == '\n')) {
                sublen--;
            }
            char *token = (char *)malloc(sublen + 1);
            if (token == NULL) {
                /* free previously allocated tokens */
                for (int j = 0; j < field_index; j++) {
                    free(fields[j]);
                }
                free(fields);
                exit(1);
            }
            if (sublen > 0) {
                memcpy(token, &line[start], sublen);
            }
            token[sublen] = '\0';
            fields[field_index++] = token;
            start = i + 1;
        }
    }
    /* if fewer than expected fields (e.g., missing trailing columns), fill with empty */
    while (field_index < expected) {
        char *empty = (char *)malloc(1);
        if (empty == NULL) {
            for (int j = 0; j < field_index; j++) {
                free(fields[j]);
            }
            free(fields);
            exit(1);
        }
        empty[0] = '\0';
        fields[field_index++] = empty;
    }

    *num_tokens = expected;
    return fields;
}

/**
 * Reads a CSV file and appends each data row to the dynamic vector 'dv'.
 * Skips the header. For each non-empty line, splits into 5 fields:
 *   - Fields 0 and 3: convert to int (FIELD_INT), or FIELD_NULL if empty.
 *   - Fields 1, 2, and 4: store as string (FIELD_STRING), or FIELD_NULL if empty.
 * Each row is stored as a LinkedList and inserted into 'dv'.
 * Frees all temporary memory. Returns 0 on success, 1 on error.
 */
int dv_read_from_csv(struct Dinamic_Vector *dv, const char *filename) {
    if (dv == NULL || filename == NULL) {
        return 1;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return 1;
    }

    char line[1024];
    /* Read and discard header line */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 1;
    }

    /* Process each subsequent line */
    while (fgets(line, sizeof(line), fp) != NULL) {
        /* Skip blank or too-short lines */
        if (line[0] == '\n' || line[0] == '\r' || strlen(line) < 2) {
            continue;
        }

        int num_tokens = 0;
        char **tokens = split_csv_line(line, &num_tokens);
        /* num_tokens will always be 5 */

        /* Create a linked list for this row */
        struct LinkedList *row_list = ll_create();  /* exit(1) on failure */

        /* For columns 0..4 build a Field and append */
        for (int idx = 0; idx < num_tokens; idx++) {
            struct Field field;
            if (tokens[idx][0] == '\0') {
                /* empty string → treat as NULL field */
                field.type = FIELD_NULL;
                field.i = 0;
                field.s = NULL;
            } else if (idx == 0 || idx == 3) {
                /* ID or Age: integer column */
                field.type = FIELD_INT;
                field.i = atoi(tokens[idx]);
                field.s = NULL;
            } else {
                /* CPF, Name or Date: string column */
                field.type = FIELD_STRING;
                field.s = strdup(tokens[idx]);
                if (field.s == NULL) {
                    /* on failure, clean up and exit */
                    free(tokens[idx]);
                    for (int j = idx + 1; j < num_tokens; j++) {
                        free(tokens[j]);
                    }
                    free(tokens);
                    ll_free(row_list);
                    fclose(fp);
                    return 1;
                }
            }
            ll_append_field(row_list, field);  /* exit(1) on malloc failure */
        }

        /* Insert this row’s list into dv */
        dv_insert(dv, row_list);  /* exit(1) if dv==NULL */

        /* Free the temporary tokens */
        for (int i = 0; i < num_tokens; i++) {
            free(tokens[i]);
        }
        free(tokens);
    }

    fclose(fp);
    return 0;
}

/**
 * Write all data from the dynamic vector to a CSV file.
 * Creates the header line and then writes each record.
 * 
 * Returns 0 on success; returns 1 on any error.
 */
int dv_write_to_csv(const struct Dinamic_Vector *dv, const char *filename) {
    if (dv == NULL || filename == NULL) {
        return 1;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        return 1;
    }

    // Write header
    fprintf(fp, "ID,CPF,Nome,Idade,Data_Cadastro\n");

    // Write each record
    for (int i = 0; i < dv_size(dv); i++) {
        struct LinkedList *row = dv_get(dv, i);
        if (row == NULL) continue;

        struct ListNode *node = row->first;
        int field_count = 0;

        while (node != NULL && field_count < 5) {
            if (field_count > 0) {
                fprintf(fp, ",");
            }

            if (node->field.type == FIELD_INT) {
                fprintf(fp, "%d", node->field.i);
            } else if (node->field.type == FIELD_STRING && node->field.s != NULL) {
                fprintf(fp, "%s", node->field.s);
            }
            // For FIELD_NULL or NULL strings, print nothing (empty field)

            node = node->next;
            field_count++;
        }

        // Fill remaining fields if the row has fewer than 5 fields
        while (field_count < 5) {
            if (field_count > 0) {
                fprintf(fp, ",");
            }
            field_count++;
        }

        fprintf(fp, "\n");
    }

    fclose(fp);
    return 0;
}

/**
 * Print every LinkedList in 'dv'. For each i in [0..dv_size(dv)-1]:
 *   prints "Row i: " then ll_print(that list).  
 * If dv==NULL or empty, prints nothing.
 */
void dv_print_all(const struct Dinamic_Vector *dv) {
    if (dv == NULL) {
        return;
    }
    int total = dv_size(dv);
    printf("ID CPF Nome Idade Data_Cadastro\n");

    for (int i = 0; i < total; i++) {
        struct LinkedList *row = dv_get(dv, i);  /* exit(1) if dv==NULL or out of bounds */
        ll_print(row);
    }
}
struct Field *get_field_by_index(const struct Dinamic_Vector *dv, int line, int column) {
   if (dv == NULL || line < 0 || line >= dv_size(dv)) {
      return NULL;
   }
   struct LinkedList *row = dv_get(dv, line);
   if (row == NULL || column < 0 || (row != NULL && column >= ll_size(row))) {
      return NULL;
   }
   struct ListNode *node = row->first;
   for (int i = 0; i < column && node != NULL; i++) {
      node = node->next;
   }
   if (node != NULL) {
      return &node->field;
   }
   return NULL;
}

void dv_consult_by_field(const struct Dinamic_Vector *dv, const char *search, int field_index) {
   if (dv == NULL || search == NULL || field_index < 0 || field_index > 4) {
       printf("Erro: Parâmetros inválidos.\n");
       return;
   }

   printf("ID CPF Nome Idade Data_Cadastro\n");

   size_t search_len = strlen(search); // Store the length of 'search' before the loop
   int found = 0; // Flag to track if any match is found

   for (int i = 0; i < dv_size(dv); i++) {
       struct LinkedList *row = dv_get(dv, i);
       struct Field *field = get_field_by_index(dv, i, field_index);

       if (field != NULL && field->type == FIELD_STRING && strncasecmp(field->s, search, search_len) == 0) {
           ll_print(row); // Print the entire row if the field matches
           found = 1; // Set flag to indicate a match was found
       }
   }

   if (!found) {
       printf("Nenhum usuário registrado com essas credenciais.\n");
   }
}

/**
 * Reassign IDs for all rows in the vector, starting from 1.
 * Assumes ID is always the first field (index 0).
 */
void dv_reassign_ids(struct Dinamic_Vector *dv) {
    if (!dv) return;
    for (int i = 0; i < dv->n; i++) {
        struct LinkedList *row = dv->v[i];
        if (row && row->first) {
            row->first->field.type = FIELD_INT;
            row->first->field.i = i + 1;
        }
    }
}

/**
 * Remove the row at index 'idx' from the vector, shifting others left.
 * Frees the LinkedList at that position.
 */
void dv_remove(struct Dinamic_Vector *dv, int idx) {
    if (!dv || idx < 0 || idx >= dv->n) return;
    ll_free(dv->v[idx]);
    for (int i = idx; i < dv->n - 1; i++) {
        dv->v[i] = dv->v[i + 1];
    }
    dv->n--;
    dv_reassign_ids(dv);
}

/**
 * Free all LinkedLists stored in the dynamic vector and then free the vector itself.
 * This provides complete memory cleanup for the entire data structure.
 */
void dv_free_all(struct Dinamic_Vector *dv) {
    if (dv == NULL) {
        return;
    }
    
    // Free each LinkedList inside the vector
    for (int i = 0; i < dv_size(dv); i++) {
        struct LinkedList *row = dv_get(dv, i);
        ll_free(row);
    }
    
    // Free the dynamic vector itself
    dv_free(dv);
}