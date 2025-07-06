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
Dinamic_Vector *dv_create(void) {
    Dinamic_Vector *dv = (Dinamic_Vector *)malloc(sizeof(Dinamic_Vector));
    if (dv == NULL) {
        exit(1);
    }
    dv->n = 0;
    dv->n_max = initial_cap;
    dv->v = (LinkedList **)malloc(sizeof(LinkedList *) * dv->n_max);
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
static void dv_reallocate(Dinamic_Vector *dv) {
    if (dv == NULL) {
        exit(1);
    }
    dv->n_max *= 2;
    LinkedList **new_block = (LinkedList **)realloc(dv->v, sizeof(LinkedList *) * dv->n_max);
    if (new_block == NULL) {
        exit(1);
    }
    dv->v = new_block;
}

/*
 * Insert 'list_ptr' at the end of 'dv'. Resize if needed.
 * If dv==NULL or list_ptr==NULL, exit(1).
 */
void dv_insert(Dinamic_Vector *dv, LinkedList *list_ptr) {
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
int dv_size(const Dinamic_Vector *dv) {
    if (dv == NULL) {
        return 0;
    }
    return dv->n;
}

/*
 * Return the LinkedList* stored at index i.
 * If dv==NULL or i out of bounds, exit(1).
 */
LinkedList *dv_get(const Dinamic_Vector *dv, int i) {
    if (dv == NULL || i < 0 || i >= dv->n) {
        exit(1);
    }
    return dv->v[i];
}

/*
 * Free the dynamic vector itself (array + struct).  
 * Does NOT free the LinkedList* contents. If dv==NULL, do nothing.
 */
void dv_free(Dinamic_Vector *dv) {
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
 *
 * This handles lines such as "7,123,Mario,,2024-07-11" → 
 *   ["7", "123", "Mario", "", "2024-07-11"]
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
 * Read the CSV file 'filename' and append each row into 'dv'.
 *
 *  - CSV must have a first line header → skip it.  
 *  - Each subsequent non-empty line:  
 *      * split_csv_line(line, &num_tokens) → exactly 5 tokens.  
 *      * Create LinkedList *row_list = ll_create().  
 *      * For each field index 0..4:  
 *          - If token is empty string: Field.type=FIELD_NULL.  
 *          - Else if index==0 or index==3: convert token to int → FIELD_INT.  
 *          - Else (1,2,4): strdup(token) → FIELD_STRING.  
 *        Append that Field to row_list via ll_append_field().  
 *      * dv_insert(dv, row_list).  
 *      * Free the temporary token strings and array.  
 *
 * Return 0 on success; return 1 on any error (file open or memory failure).
 */
int dv_read_from_csv(Dinamic_Vector *dv, const char *filename) {
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
        LinkedList *row_list = ll_create();  /* exit(1) on failure */

        /* For columns 0..4 build a Field and append */
        for (int idx = 0; idx < num_tokens; idx++) {
            Field f;
            if (tokens[idx][0] == '\0') {
                /* empty string → treat as NULL field */
                f.type = FIELD_NULL;
                f.i = 0;
                f.s = NULL;
            } else if (idx == 0 || idx == 3) {
                /* ID or Age: integer column */
                f.type = FIELD_INT;
                f.i = atoi(tokens[idx]);
                f.s = NULL;
            } else {
                /* CPF, Name or Date: string column */
                f.type = FIELD_STRING;
                f.s = strdup(tokens[idx]);
                if (f.s == NULL) {
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
            ll_append_field(row_list, f);  /* exit(1) on malloc failure */
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
 * Print every LinkedList in 'dv'. For each i in [0..dv_size(dv)-1]:
 *   prints "Row i: " then ll_print(that list).  
 * If dv==NULL or empty, prints nothing.
 */
void dv_print_all(const Dinamic_Vector *dv) {
    if (dv == NULL) {
        return;
    }
    int total = dv_size(dv);
    printf("ID CPF Nome Idade Data_Cadastro\n");

    for (int i = 0; i < total; i++) {
        LinkedList *row = dv_get(dv, i);  /* exit(1) if dv==NULL or out of bounds */
        ll_print(row);
    }
}
Field *get_field_by_index(const Dinamic_Vector *dv, int line, int column) {
   if (dv == NULL || line < 0 || line >= dv_size(dv)) {
      return NULL;
   }
   LinkedList *row = dv_get(dv, line);
   if (row == NULL || column < 0 || (row != NULL && column >= ll_size(row))) {
      return NULL;
   }
   ListNode *node = row->first;
   for (int i = 0; i < column && node != NULL; i++) {
      node = node->next;
   }
   if (node != NULL) {
      return &node->field;
   }
   return NULL;
}

void dv_consult_by_field(const Dinamic_Vector *dv, const char *search, int field_index) {
   if (dv == NULL || search == NULL || field_index < 0 || field_index > 4) {
       printf("Erro: Parâmetros inválidos.\n");
       return;
   }

   printf("ID CPF Nome Idade Data_Cadastro\n");

   size_t search_len = strlen(search); // Store the length of 'search' before the loop
   int found = 0; // Flag to track if any match is found

   for (int i = 0; i < dv_size(dv); i++) {
       LinkedList *row = dv_get(dv, i);
       Field *field = get_field_by_index(dv, i, field_index);

       if (field != NULL && field->type == FIELD_STRING && strncasecmp(field->s, search, search_len) == 0) {
           ll_print(row); // Print the entire row if the field matches
           found = 1; // Set flag to indicate a match was found
       }
   }

   if (!found) {
       printf("Nenhum usuário registrado com essas credenciais.\n");
   }
}