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
    struct Dinamic_Vector *dv = dv_create();  // exit(1) on failure, but dv_create never returns NULL */
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
            printf("\n[Sistema]\nDigite o ID do registro a ser atualizado:\n[Usuario]\n");
            int id;
            scanf("%d%*c", &id); // %*c consome o \n
            if (id < 1 || id > dv_size(dv)) {
                printf("[Sistema]\nID inválido.\n");
                continue;
            }
            struct LinkedList *row = dv_get(dv, id - 1);
            printf("\n[Sistema]\nDigite o novo valor para os campos CPF (apenas dígitos), Nome, Idade e Data_Cadastro (para manter o valor atual de um campo, digite '-'): \n[Usuario]\n");
            char cpf[256], nome[256], idade[256], data[256];
            fgets(cpf, sizeof(cpf), stdin); cpf[strcspn(cpf, "\n")] = 0;
            fgets(nome, sizeof(nome), stdin); nome[strcspn(nome, "\n")] = 0;
            fgets(idade, sizeof(idade), stdin); idade[strcspn(idade, "\n")] = 0;
            fgets(data, sizeof(data), stdin); data[strcspn(data, "\n")] = 0;

            struct LinkedList *preview = ll_copy(row);
            ll_update_fields(preview, cpf, nome, idade, data);

            printf("[Sistema]\nConfirma os novos valores para o registro abaixo? (S/N)\n");
            printf("ID CPF Nome Idade Data_Cadastro\n");
            ll_print(preview);
            char confirm[10];
            fgets(confirm, sizeof(confirm), stdin);
            if (strcasecmp(confirm, "S\n") == 0 || strcasecmp(confirm, "S") == 0) {
                ll_update_fields(row, cpf, nome, idade, data);
                printf("[Sistema]\nRegistro atualizado com sucesso.\n");
            } else {
                printf("[Sistema]\nAtualização cancelada.\n");
            }
            ll_free(preview);

        }else if (strcmp(flag, "3") == 0) {
            printf("\n[Sistema]\nDigite o ID do registro a ser removido:\n[Usuario]\n");
            int id;
            scanf("%d%*c", &id);
            if (id < 1 || id > dv_size(dv)) {
                printf("[Sistema]\nID inválido.\n");
                continue;
            }
            struct LinkedList *row = dv_get(dv, id - 1);
            printf("[Sistema]\nTem certeza de que deseja excluir o registro abaixo? (S/N)\n");
            printf("ID CPF Nome Idade Data_Cadastro\n");
            ll_print(row);
            char confirm[10];
            fgets(confirm, sizeof(confirm), stdin);
            if (strcasecmp(confirm, "S\n") == 0 || strcasecmp(confirm, "S") == 0) {
                dv_remove(dv, id - 1);
                printf("[Sistema]\nRegistro removido com sucesso.\n");
            } else {
                printf("[Sistema]\nRemoção cancelada.\n");
            }
        }
        else if (strcmp(flag, "4") == 0) {
            printf("[Sistema]\nPara inserir um novo registro, digite os valores para os campos CPF (apenas dígitos), Nome, Idade e Data_Cadastro:\n[Usuario]\n");
            char cpf[256], nome[256], idade[256], data[256];
            fgets(cpf, sizeof(cpf), stdin); cpf[strcspn(cpf, "\n")] = 0;
            fgets(nome, sizeof(nome), stdin); nome[strcspn(nome, "\n")] = 0;
            fgets(idade, sizeof(idade), stdin); idade[strcspn(idade, "\n")] = 0;
            fgets(data, sizeof(data), stdin); data[strcspn(data, "\n")] = 0;
            int id = dv_size(dv) + 1;
            struct LinkedList *new_row = ll_create_from_fields(id, cpf, nome, atoi(idade), data);

            printf("[Sistema]\nConfirma a inserção do registro abaixo? (S/N)\n");
            printf("ID CPF Nome Idade Data_Cadastro\n");
            ll_print(new_row);
            char confirm[10];
            fgets(confirm, sizeof(confirm), stdin);
            if (strcasecmp(confirm, "S\n") == 0 || strcasecmp(confirm, "S") == 0) {
                dv_insert(dv, new_row);
                printf("[Sistema]\nO registro foi inserido com sucesso.\n");
            } else {
                ll_free(new_row);
                printf("[Sistema]\nInserção cancelada.\n");
            }

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
        struct LinkedList *row = dv_get(dv, i);  /* exit(1) if invalid */
        ll_free(row);
    }
    dv_free(dv);

    return 0;
}