# Atividade-Estrutura-de-Dados

O enunciado da atividade se mostrou um pouco confuso para dizer se deveria ser usado um vetor estatico ou dinamico, uma vez que ele nominalmente cita o uso de um vetor estatico e logo em seguida diz que um dos pontos cobrados na atividade é a alocação dinamica. Em todo caso, simplesmente optei por usar alocação dinamica já que é a opção mais tecnicamente desafiadora.

Como o array deve ser de um unico tipo de dados eu tive a ideia de fazer o array principal com ponteiros que apontam para listas, cada uma dessas listas sendo uma das linhas do originario csv. Desta forma os arquivos podem ter os seus diferentes tipos e além de serem mais acessiveis.