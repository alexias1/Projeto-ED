# SAT Solver

Este é um **SAT Solver** em C que recebe uma fórmula no formato **DIMACS** (arquivo `.cnf`) e verifica sua satisfatibilidade.

## Descrição

O programa recebe um arquivo `.cnf` como entrada, lê e interpreta a fórmula fornecida e realiza a verificação de satisfatibilidade:

- **Leitura da fórmula**: A fórmula DIMACS é lida a partir de um arquivo de entrada.
- **Verificação de cláusulas**: Para cada cláusula, o solver verifica se ela é satisfeita.
- **Atribuição de variáveis**: Se uma variável não foi atribuída, o solver tenta atribuir os valores **True** e **False**, e faz backtracking caso não consiga satisfazer a fórmula.
- **Resultado**: O programa imprime se a fórmula é **SAT** ou **UNSAT**, e, em caso de sucesso, exibe a interpretação final das variáveis.

## Como usar

### Requisitos

- O arquivo de entrada no formato DIMACS `.cnf` deve estar na pasta `output/`.

### Exemplo de arquivo `.cnf`

```txt
c Este é um comentário
p cnf 3 3
-1 -2 0
1 -2 0
-1 -3 0
```
### Exemplo de saída
```
Formula eh SAT!

Interpretacao final:
x1 = 1
x2 = 0
x3 = 0
