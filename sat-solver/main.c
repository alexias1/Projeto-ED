#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

//Representa uma cláusula como uma lista de literais
typedef struct {
    int* literais;
    int num_literais;
} Clausula;


//Uma formula cnf eh composta de cláusulas, e cada cláusula eh composta por literais
typedef struct {
    Clausula* clausulas;     
    int n_clausulas;     
    int n_variaveis;      
} Formula;


//A interpretacao parcial atribui valores as clausulas
typedef struct {
    int* valores;     
    int n_variaveis;       
} Interpretacao;


//Função para ler a fórmula CNF do arquivo
Formula ler_formula(const char* nome_arquivo) {
    Formula F;
    char texto[1000];
    FILE* arquivo_entrada = fopen(nome_arquivo, "r");
    if (arquivo_entrada == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", nome_arquivo);
        exit(1);
    }

    while (fgets(texto, sizeof(texto), arquivo_entrada)) {
        if (texto[0] == 'c') continue;
        if (texto[0] == 'p') {
            sscanf(texto, "p cnf %d %d", &F.n_variaveis, &F.n_clausulas);
            F.clausulas = (Clausula*)malloc(F.n_clausulas * sizeof(Clausula));
            break;
        }
    }

    int clausula_idx = 0;

    while (fgets(texto, sizeof(texto), arquivo_entrada)) {
        if (texto[0] == 'c' || texto[0] == 'p') continue;

        int literais_aux[200];
        int lits_lidos = 0;
        char* ptr = texto;

        while (*ptr != '\0') {
            int literal_atual;
            sscanf(ptr, "%d", &literal_atual);
            if (literal_atual == 0) break;
            literais_aux[lits_lidos++] = literal_atual;
            while (*ptr != ' ' && *ptr != '\n' && *ptr != '\0') ptr++;
            while (*ptr == ' ') ptr++;
        }

        F.clausulas[clausula_idx].num_literais = lits_lidos;
        F.clausulas[clausula_idx].literais = (int*)malloc(lits_lidos * sizeof(int));
        for (int i = 0; i < lits_lidos; i++) {
            F.clausulas[clausula_idx].literais[i] = literais_aux[i];
        }
        clausula_idx++;
    }

    fclose(arquivo_entrada);
    return F;
}

//Função para verificar se uma cláusula é satisfeita pela interpretação atual
bool clausula_sat(const Clausula* clausula, const Interpretacao* interpretacao) {
    for (int i = 0; i < clausula->num_literais; i++) {
        int literal = clausula->literais[i];
        int variavel_idx = abs(literal);
        int valor = interpretacao->valores[variavel_idx];
        if ((literal > 0 && valor == 1) || (literal < 0 && valor == 0)) {
            return true;
        }
    }
    return false;
}

//Função para verificar se todas as variáveis em uma cláusula foram atribuídas
bool todas_var_atribuidas(const Clausula* clausula, const Interpretacao* interpretacao) {
    for (int i = 0; i < clausula->num_literais; i++) {
        int variavel_idx = abs(clausula->literais[i]);
        if (interpretacao->valores[variavel_idx] == -1) {
            return false;
        }
    }
    return true;
}

//Função para encontrar a primeira variável livre (não atribuída)
int encontrar_var_livre(const Interpretacao* interpretacao) {
    for (int i = 1; i <= interpretacao->n_variaveis; i++) {
        if (interpretacao->valores[i] == -1) {
            return i;
        }
    }
    return -1; //Retorna -1 se todas as variáveis estiverem atribuídas
}

//Função principal para determinar se a fórmula F é satisfatível
bool sat(Formula* formula, Interpretacao* interpretacao) {
    for (int i = 0; i < formula->n_clausulas; i++) {
        if (!clausula_sat(&formula->clausulas[i], interpretacao)) {
            if (todas_var_atribuidas(&formula->clausulas[i], interpretacao)) {
                return false;
            }
        }
    }

    if (encontrar_var_livre(interpretacao) == -1) {
        return true; //Todas as cláusulas foram satisfeitas com a atribuição atual
    }

    int var_nao_atribuida = encontrar_var_livre(interpretacao);

    //Tenta atribuir TRUE
    interpretacao->valores[var_nao_atribuida] = 1;
    if (sat(formula, interpretacao)) return true;

    //Tenta atribuir FALSE
    interpretacao->valores[var_nao_atribuida] = 0;
    if (sat(formula, interpretacao)) return true;

    //Backtracking
    interpretacao->valores[var_nao_atribuida] = -1;
    return false;
}

int main() {
    Formula F = ler_formula("arquivo_entrada.cnf");

    Interpretacao I;
    I.n_variaveis = F.n_variaveis;
    I.valores = (int*)malloc((I.n_variaveis + 1) * sizeof(int));
    for (int i = 1; i <= I.n_variaveis; i++) {
        I.valores[i] = -1;
    }

    if (sat(&F, &I)) {
        printf("\nFormula e SAT!\n");
        printf("\nInterpretacao final:\n");
        for (int i = 1; i <= I.n_variaveis; i++) {
            printf("x%d = %s\n", i, I.valores[i] == 1 ? "1" : "0");
        }
    } else {
        printf("\nFormula e UNSAT!\n");
    }


    for (int i = 0; i < F.n_clausulas; i++) {
        free(F.clausulas[i].literais);
    }
    free(F.clausulas);
    free(I.valores);

    return 0;
}