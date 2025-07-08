# Sistema de Gerenciamento de Pacientes Hospitalares - Estruturas de Dados

Este projeto implementa um sistema de gerenciamento de pacientes hospitalares em C usando **vetor dinâmico** cujos elementos são **listas duplamente encadeadas** de campos heterogêneos. O sistema carrega o arquivo `bd_paciente.csv` em memória e oferece operações CRUD completas (Create, Read, Update, Delete) com persistência automática de dados.

## Como Executar

### 1. Clone o repositório
```bash
git clone https://github.com/Dudubl120/Estrutura-de-Dados-parte-2.git
cd Estrutura-de-Dados-parte-2
```

### 2. Compile e execute
```bash
make
```
Ou alternativamente:
```bash
gcc -Wall -o Hospital_Patients_Management_System main.c linkedlist.c dinamic_vector.c
./Hospital_Patients_Management_System
```

### 3. Menu interativo
Ao iniciar, o CSV é carregado automaticamente e aparece o menu:

```
HealthSys Log in!

Bem Vindo ao sistema de gerenciamento de clientes!
[Sistema]
Como gostaria de proceder?
1 - Consultar pacientes
2 - Atualizar pacientes  
3 - Remover pacientes
4 - Adicionar pacientes
5 - Imprimir todos os pacientes
6 - Limpar terminal
Q - Sair do sistema
```

- **1 – Consultar**: submenu para buscar por Nome ou CPF (busca parcial)
- **2 – Atualizar**: permite modificar dados de pacientes existentes
- **3 – Remover**: remove pacientes com reatribuição automática de IDs
- **4 – Adicionar**: adiciona novos pacientes com formatação automática de CPF
- **5 – Imprimir todos**: exibe todas as linhas carregadas
- **6 – Limpar terminal**: limpa a tela
- **Q – Sair**: salva automaticamente e encerra o programa

### 4. Limpar compilação
```bash
make clean
```

## Principais TADs

### 1. Lista Encadeada Heterogênea (linkedlist.h/c)
**Objetivo**: Representar cada linha do CSV como sequência ordenada de campos de tipos diferentes.

**Estruturas**:
```c
enum FieldType {
    FIELD_INT,      // ID ou Idade
    FIELD_STRING,   // CPF, Nome, Data
    FIELD_NULL      // Campos vazios
};

struct Field {
    enum FieldType type;
    int i;          // valor inteiro  
    char *s;        // ponteiro para string
};

struct LinkedList {
    int count;
    struct ListNode *first;
    struct ListNode *last;
};
```

**Operações principais**:
- `ll_create()` – cria lista vazia (exit(1) em falha)
- `ll_append_field(l, f)` – insere campo no fim
- `ll_print(l)` – imprime todos os campos não-nulos
- `ll_update_fields()` – atualiza múltiplos campos de uma vez
- `ll_copy()` – cria cópia profunda para preview de alterações
- `ll_create_from_fields()` – cria lista a partir de dados de paciente

### 2. Vetor Dinâmico de Listas (dinamic_vector.h/c)
**Objetivo**: Armazenar ponteiros para LinkedList, onde cada lista representa uma linha completa do CSV.

**Estrutura**:
```c
struct Dinamic_Vector {
    int n;           // número de elementos
    int n_max;       // capacidade atual
    struct LinkedList **v;  // array de ponteiros para LinkedList
};
```

**Operações principais**:
- `dv_create()` – cria vetor vazio com capacidade inicial (4)
- `dv_insert(dv, list_ptr)` – insere lista no final; dobra capacidade se necessário
- `dv_read_from_csv()` – carrega dados do CSV na inicialização
- `dv_write_to_csv()` – salva dados automaticamente ao sair
- `dv_consult_by_field()` – busca por prefixo case-insensitive
- `dv_remove()` – remove registros com reorganização automática
- `dv_reassign_ids()` – reatribui IDs sequenciais após remoções
- `dv_free_all()` – liberação completa de memória

## Principais Decisões de Implementação

### Modelo de Dados
Cada registro contém 5 campos fixos:
- **Coluna 0**: ID (FIELD_INT) - gerado automaticamente
- **Coluna 1**: CPF (FIELD_STRING) - formatado automaticamente para XXX.XXX.XXX-XX  
- **Coluna 2**: Nome (FIELD_STRING) - suporta espaços
- **Coluna 3**: Idade (FIELD_INT)
- **Coluna 4**: Data_Cadastro (FIELD_STRING) - formato YYYY-MM-DD

### Vetor de Ponteiros para Listas
Escolhido para permitir que cada registro mantenha campos de tipos diferentes usando estrutura unificada em memória, com redimensionamento automático quando necessário.

### Campo Heterogêneo com Union
Cada nó da lista armazena um `Field` com enum que indica se é inteiro, string ou nulo. Isso permite otimização de memória e tratamento uniforme de tipos diferentes.

### Splitting que Preserva Campos Vazios
A função `split_csv_line()` gera sempre array de 5 strings, mesmo com campos vazios entre delimitadores. Garante que cada índice corresponde sempre à mesma coluna.

### Interface Robusta
- **Entrada com `fgets()`**: Suporta nomes com espaços ao invés de `scanf()`
- **Formatação Automática de CPF**: Aceita apenas dígitos ou já formatado
- **Confirmações**: Operações críticas requerem confirmação do usuário
- **Preview de Alterações**: Mostra dados antes de confirmar mudanças
- **Limpeza de Buffer**: Tratamento adequado para evitar bugs de entrada

### Persistência e Gerenciamento de Memória
- **Carregamento Automático**: CSV carregado na inicialização
- **Salvamento Automático**: Dados persistidos ao sair com 'Q'
- **Liberação Completa**: Toda memória alocada é liberada adequadamente
- **Verificação de Erros**: `exit(1)` em falhas críticas de alocação

### Abstração Modular
O `main.c` apenas chama funções da interface, sem acessar detalhes internos das estruturas. Cada módulo tem responsabilidade específica e bem definida, facilitando manutenção e extensão.