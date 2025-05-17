# Projeto de Estrutura de Dados

Trabalho desenvolvido para a disciplina de **Estrutura de Dados** do curso de **Ciência da Computação (UFAL)**. O projeto é composto por quatro entregas principais, cada uma envolvendo uma aplicação prática de estruturas de dados.

---

## Entregas

### 1. Compactador Huffman

Implementa um algoritmo de compressão de arquivos baseado na codificação de Huffman.

- **Principais arquivos**:
  - `pqueue_heap.h`: Fila de prioridade com heap binário
  - `huffman.h`: Construção da árvore de Huffman, geração da tabela de códigos, compressão e descompressão
  - `main.c`: Interface principal que permite compactar e descompactar arquivos usando a tabela de Huffman

---

### 2. SAT Solver

SAT Solver em C que verifica a satisfatibilidade de fórmulas booleanas no formato DIMACS (`.cnf`).

- **Funcionalidades**:
  - Leitura do arquivo `.cnf`
  - Verificação de cláusulas
  - Atribuição de variáveis com backtracking
  - Impressão do resultado (SAT ou UNSAT) e interpretação final

---

### 3. Seminário – Agendamento de Processos

Simula um escalonador de processos baseado em prioridade.

- Utiliza uma **heap máxima** para garantir que o processo com maior prioridade seja executado primeiro
- Implementa **aging** para evitar inanição
- Permite **inserção dinâmica** de novos processos durante a simulação

---

### 4. Contagem e Plotagem de Dados

Compara o desempenho de busca entre **lista encadeada** e **árvore binária de busca (ABB)**.

- **Arquivos**:
- `contagem.c`: Código em C responsável pela contagem de comparações
- `resultados.csv`: Exemplo de saída gerada pelo código
- `grafico.R`: Script em R para geração de gráficos a partir do `.csv`

---
