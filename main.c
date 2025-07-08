#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dinamic_vector.h"
#include "linkedlist.h"

/**
 * Print the main menu options for the Hospital Patient Management System
 */
void print_menu(){
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

/**
 * Format CPF from digits-only string to XXX.XXX.XXX-XX format
 * If input already has formatting, returns it as-is
 * If input has exactly 11 digits, formats it properly
 */
void format_cpf(char *cpf) {
    // Only format if we have exactly 11 digits (no formatting)
    if (strlen(cpf) == 11) {
        char formatted[15];
        int format_idx, digit_idx = 0;
        for (format_idx = 0; format_idx < 14; format_idx++) {
            if (format_idx == 3 || format_idx == 7) {
                formatted[format_idx] = '.';
            } else if (format_idx == 11) {
                formatted[format_idx] = '-';
            } else {
                formatted[format_idx] = cpf[digit_idx++];
            }
        }
        formatted[14] = '\0';
        strcpy(cpf, formatted);
    }
    // If not 11 digits, leave the original input unchanged
}

/**
 * Format date from YYYYMMDD to YYYY-MM-DD format
 * If input already has formatting, returns it as-is
 * If input has exactly 8 digits, formats it properly
 */
void format_date(char *date_input) {
    // Only format if we have exactly 8 digits (no formatting)
    if (strlen(date_input) == 8) {
        char formatted[11];
        sprintf(formatted, "%.4s-%.2s-%.2s", 
                date_input,      // YYYY
                date_input + 4,  // MM  
                date_input + 6); // DD
        strcpy(date_input, formatted);
    }
}

int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8"); // Without this, the program may not handle UTF-8 characters correctly
    system("chcp 65001 > nul");
    
    //Variable declarations
    const char *filename = "bd_paciente.csv";
    char user_choice[10]; // To store user options for the main menu
    char search_input[256]; // To store search input in consultation
    char cpf[256], nome[256], idade[256], data[256];
    char confirm[10]; // To confirm updates or deletions

    /* Step 1: Create the dynamic vector */
    struct Dinamic_Vector *BDPaciente = dv_create();  // exit(1) on failure, but dv_create never returns NULL */
    if (BDPaciente == NULL) {
        return 1;
    }

    /* Step 2: Read CSV into patient_db (each row → one LinkedList of heterogeneous fields) */
    if (dv_read_from_csv(BDPaciente, filename) != 0) {
        dv_free(BDPaciente);
        return 1;
    }

    printf("HealthSys Log in!\n");
    printf("\n");
    printf("Bem Vindo ao sistema de gerenciamento de clientes!\n");
    print_menu();

    while (strcasecmp(user_choice, "Q") != 0) {
        printf("\n");
        printf("[Usuario]\n");
        scanf("%s", user_choice);
        
        if (strcmp(user_choice, "1") == 0) {
            printf("\nConsultando pacientes...\n");
            
            printf("[Sistema]\n");
            printf("Escolha o modo de consulta:\n");
            printf("1 - Por nome\n");
            printf("2 - Por CPF\n");
            printf("3 - Retornar ao menu principal\n");
            printf("\n[Usuario]\n");
            scanf("%s", user_choice);

            if (strcmp(user_choice, "1") == 0) {
                printf("\n[Sistema]\nDigite o nome:\n[Usuario]\n");
                scanf("%s", search_input);
                dv_consult_by_field(BDPaciente, search_input, 2); // Name is in column 2
            } else if (strcmp(user_choice, "2") == 0) {
                printf("\n[Sistema]\nDigite o CPF:\n[Usuario]\n");
                scanf("%s", search_input);
                dv_consult_by_field(BDPaciente, search_input, 1); // CPF is in column 1
            } else if (strcmp(user_choice, "3") == 0) {
                continue; // Return to main menu
                print_menu(); // If this isn't here may cause confusion for the user
            } else {
                printf("Opção inválida.\n");
            }
            
        } else if (strcmp(user_choice, "2") == 0) {
            printf("\n[Sistema]\nDigite o ID do registro a ser atualizado:\n[Usuario]\n");
            int id;
            scanf("%d%*c", &id); // %*c consome o \n
            if (id < 1 || id > dv_size(BDPaciente)) {
                printf("[Sistema]\nID inválido.\n");
                continue;
            }
            struct LinkedList *row = dv_get(BDPaciente, id - 1);
            struct LinkedList *preview = ll_copy(row);

            printf("\n[Sistema]\nDigite o novo valor para os campos CPF, Nome, Idade e Data_Cadastro (para manter o valor atual de um campo, digite '-'): \n[Usuario]\n");
            fgets(cpf, sizeof(cpf), stdin); cpf[strcspn(cpf, "\n")] = 0;
            fgets(nome, sizeof(nome), stdin); nome[strcspn(nome, "\n")] = 0;
            fgets(idade, sizeof(idade), stdin); idade[strcspn(idade, "\n")] = 0;
            fgets(data, sizeof(data), stdin); data[strcspn(data, "\n")] = 0;

            ll_update_fields(preview, cpf, nome, idade, data);

            printf("[Sistema]\nConfirma os novos valores para o registro abaixo? (S/N)\n");
            printf("ID CPF Nome Idade Data_Cadastro\n");
            ll_print(preview);
            fgets(confirm, sizeof(confirm), stdin);
            if (strcasecmp(confirm, "S\n") == 0 || strcasecmp(confirm, "S") == 0) {
                ll_update_fields(row, cpf, nome, idade, data);
                printf("[Sistema]\nRegistro atualizado com sucesso.\n");
            } else {
                printf("[Sistema]\nAtualização cancelada.\n");
            }
            ll_free(preview);

        }else if (strcmp(user_choice, "3") == 0) {
            printf("\n[Sistema]\nDigite o ID do registro a ser removido:\n[Usuario]\n");
            int id;
            scanf("%d%*c", &id);
            if (id < 1 || id > dv_size(BDPaciente)) {
                printf("[Sistema]\nID inválido.\n");
                continue;
            }
            struct LinkedList *row = dv_get(BDPaciente, id - 1);
            printf("[Sistema]\nTem certeza de que deseja excluir o registro abaixo? (S/N)\n");
            printf("ID CPF Nome Idade Data_Cadastro\n");
            ll_print(row);
            fgets(user_choice, sizeof(user_choice), stdin);
            if (strcasecmp(user_choice, "S\n") == 0 || strcasecmp(user_choice, "S") == 0) {
                dv_remove(BDPaciente, id - 1);
                printf("[Sistema]\nRegistro removido com sucesso.\n");
            } else {
                printf("[Sistema]\nRemoção cancelada.\n");
            }
        }
        else if (strcmp(user_choice, "4") == 0) {
            printf("\n[Sistema]\nPara inserir um novo registro, digite os valores para os campos CPF, Nome, Idade e Data_Cadastro:\n");
            
            
            // Clear the input buffer first
            while(getchar() != '\n');
            
            // Read each field separately with clear prompts using fgets
            printf("CPF: ");
            fgets(cpf, sizeof(cpf), stdin);
            cpf[strcspn(cpf, "\n")] = 0; // Remove newline
            
            // Format CPF if user entered only digits
            format_cpf(cpf);
            
            printf("Nome: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = 0; // Remove newline
            
            printf("Idade: ");
            fgets(idade, sizeof(idade), stdin);
            idade[strcspn(idade, "\n")] = 0; // Remove newline
            
            printf("Data de Cadastro (YYYY-MM-DD): ");
            fgets(data, sizeof(data), stdin);
            data[strcspn(data, "\n")] = 0; // Remove newline
            format_date(data);

            int id = dv_size(BDPaciente) + 1;
            int idade_int = atoi(idade);
            struct LinkedList *new_row = ll_create_from_fields(id, cpf, nome, idade_int, data);

            printf("\n[Sistema]\nConfirma a inserção do registro abaixo? (S/N)\n");
            printf("ID CPF Nome Idade Data_Cadastro\n");
            ll_print(new_row);
            
            printf("\n[Usuario]\n");
            fgets(user_choice, sizeof(user_choice), stdin);
            user_choice[strcspn(user_choice, "\n")] = 0; // Remove newline
            
            if (strcasecmp(user_choice, "S") == 0) {
                dv_insert(BDPaciente, new_row);
                printf("[Sistema]\nO registro foi inserido com sucesso.\n");
            } else {
                ll_free(new_row);
                printf("[Sistema]\nInserção cancelada.\n");
            }

        } else if (strcmp(user_choice, "5") == 0) {
            printf("\nImprimindo todos os pacientes...\n");
            dv_print_all(BDPaciente);  // This will print all rows
        } else if (strcasecmp(user_choice, "6") == 0) {
            system("clear"); // Hopefully it works on linux
        } else if (strcasecmp(user_choice, "Q") == 0) {
            printf("\nSaindo do sistema...\n");
            // Save data to CSV before exiting
            if (dv_write_to_csv(BDPaciente, filename) != 0) {
                printf("Erro ao salvar dados no arquivo.\n");
            } else {
                printf("Dados salvos com sucesso.\n");
            }
        } else {
            printf("Opção inválida, tente novamente.\n");
        }
        
        printf("\n");
        print_menu(); // Print the menu again after each operation
    }
    
    /* Step 4: Free each LinkedList inside patient_db, then free patient_db itself */
    dv_free_all(BDPaciente);

    return 0;
}