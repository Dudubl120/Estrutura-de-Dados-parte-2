#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include "dinamic_vector.h"
#include "linkedlist.h"
#include <strings.h>

void menu_print(){
    printf("[Sistema]\n");
    printf("Como gostaria de proceder?\n");
    printf("1 - Consultar pacientes\n");
    printf("2 - Atualizar pacientes\n");
    printf("3 - Remover pacientes\n");
    printf("4 - Adicionar pacientes\n");
    printf("5 - Imprimir todos os pacientes\n");
    printf("6 - Limpar terminal\n");
    printf("Q - Sair do sistema\n");
    
}

int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8"); //Without this, the program may not handle UTF-8 characters correctly


    const char *filename = "bd_paciente.csv";
    char flag[10]; // To store user options for the main menu
    char option[10]; // To store user options for consultation
    char search[256]; // To store search input in consultation

    /* Step 1: Create the dynamic vector */
    Dinamic_Vector *dv = dv_create();  // exit(1) on failure, but dv_create never returns NULL */
    if (dv == NULL) {
        return 1;
    }

    /* Step 2: Read CSV into dv (each row → one LinkedList of heterogeneous fields) */
    if (dv_read_from_csv(dv, filename) != 0) {
        dv_free(dv);
        return 1;
    }


    printf("HealthSys Log in!\n");
    printf("\n");
    printf("Bem Vindo ao sistema de gerenciamento de clientes!\n");
    menu_print();


    while (strcasecmp(flag, "Q") != 0) {
        printf("\n");
        printf("[Usuario]\n");
        scanf("%s", flag);
        if (strcmp(flag, "1") == 0) {
            printf("\nConsultando pacientes...\n");
            
            printf("[Sistema]\n");
            printf("Escolha o modo de consulta:\n");
            printf("1 - Por nome\n");
            printf("2 - Por CPF\n");
            printf("3 - Retornar ao menu principal\n");
            printf("\n[Usuario]\n");
            scanf("%s", option);

            if (strcmp(option, "1") == 0) {
                printf("\n[Sistema]\nDigite o nome:\n[Usuario]\n");
                scanf("%s", search);
                dv_consult_by_field(dv, search, 2); // Name is in column 2
            } else if (strcmp(option, "2") == 0) {
                printf("\n[Sistema]\nDigite o CPF:\n[Usuario]\n");
                scanf("%s", search);
                dv_consult_by_field(dv, search, 1); // CPF is in 1
            } else if (strcmp(option, "3") == 0) {
                continue; // Return to main menu
                menu_print(); // If this isn't here may cause confusion for the user
            } else {
                printf("Opção inválida, tente novamente.\n");
            }
            
        } else if (strcmp(flag, "2") == 0) {
            printf("\n[Sistema]\nDigite o ID do paciente para atualizar:\n[Usuario]\n");
            int id;
            scanf("%d", &id);

            printf("\n[Sistema]\nDigite o índice do campo para atualizar (0: ID, 1: CPF, 2: Nome, 3: Idade, 4: Data_Cadastro):\n[Usuario]\n");
            int field_index;
            scanf("%d", &field_index);

            printf("\n[Sistema]\nDigite o novo valor:\n[Usuario]\n");
            char new_value[256];
            scanf("%s", new_value);

            Field new_field;
            if (field_index == 0 || field_index == 3) {
                new_field.type = FIELD_INT;
                new_field.i = atoi(new_value);
            } else {
                new_field.type = FIELD_STRING;
                new_field.s = strdup(new_value);
            }

            LinkedList *row = dv_get(dv, id - 1);
            ll_update_field(row, field_index, new_field);

        } else if (strcmp(flag, "3") == 0) {
            printf("\n[Sistema]\nDigite o ID do paciente para remover:\n[Usuario]\n");
            int id;
            scanf("%d", &id);

            LinkedList *row = dv_get(dv, id - 1);
            ll_free(row);
            dv_insert(dv, NULL); // Mark as removed

        } else if (strcmp(flag, "4") == 0) {
            printf("\n[Sistema]\nAdicionando um novo paciente...\n");

            LinkedList *new_row = ll_create();
            for (int i = 0; i < 5; i++) {
                printf("\n[Sistema]\nDigite o valor para o campo %d:\n[Usuario]\n", i);
                char value[256];
                scanf("%s", value);

                Field new_field;
                if (i == 0 || i == 3) {
                    new_field.type = FIELD_INT;
                    new_field.i = atoi(value);
                } else {
                    new_field.type = FIELD_STRING;
                    new_field.s = strdup(value);
                }

                ll_append_field(new_row, new_field);
            }

            dv_insert(dv, new_row);
        

        } else if (strcmp(flag, "5") == 0) {
            printf("\nImprimindo todos os pacientes...\n");

            dv_print_all(dv);  // This will print all rows
        } else if (strcasecmp(flag, "6") == 0) {
            
            system("clear"); // Hopefully this works on Linux (didn't test it btw)
        
            break;
        } else if (strcasecmp(flag, "Q") == 0) {
            printf("\nSaindo do sistema...\n");
            break;
        } else {
            printf("Opção inválida, tente novamente.\n");
        }
        
        printf("\n");
        menu_print(); // Print the menu again after each operation

        


    }
    
    /* Step 4: Free each LinkedList inside dv, then free dv itself */
    for (int i = 0; i < dv_size(dv); i++) {
        LinkedList *row = dv_get(dv, i);  /* exit(1) if invalid */
        ll_free(row);
    }
    dv_free(dv);

    return 0;
}