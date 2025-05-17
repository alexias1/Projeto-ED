#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAMANHO_DADOS 10000
#define NUM_BUSCAS 1000


typedef struct no {
    int item;
    struct no* prox;
} no;


no* criar_no_lista(int valor) {
    no* novo_no = (no*)malloc(sizeof(no));
    novo_no->item = valor;
    novo_no->prox = NULL;
    return novo_no;
}


void inserir_lista(no** head, int valor) {
    no* novo_no = criar_no_lista(valor);
    if (*head == NULL) {
        *head = novo_no;
        return;
    }
    no* temp = *head;
    while (temp->prox != NULL)
        temp = temp->prox;
        temp->prox = novo_no;
}


int buscar_lista(no* head, int valor, int* comparacoes) {
    *comparacoes = 0;
    no* temp = head;
    while (temp != NULL) {
        (*comparacoes)++;
        if (temp->item == valor)
            return 1; 
        temp = temp->prox;
    }
    return 0;
}


void liberar_lista(no* head) {
    no* temp;
    while (head != NULL) {
        temp = head;
        head = head->prox;
        free(temp);
    }
}

/////

typedef struct arvore {
    int item;
    struct arvore *esq, *dir;
} arvore;


arvore* criar_no_arvore(int valor) {
    arvore* novo_no = (arvore*)malloc(sizeof(arvore));
    novo_no->item = valor;
    novo_no->esq = novo_no->dir = NULL;
    return novo_no;
}


arvore* inserir_arvore (arvore* raiz, int valor){
    if (raiz == NULL) return criar_no_arvore(valor);
    if (valor < raiz->item){
        raiz->esq = inserir_arvore(raiz->esq, valor);
    }else {
        raiz->dir = inserir_arvore(raiz->dir, valor);
    }
    return raiz;
}

int buscar_arvore (arvore* raiz, int valor, int* comparacoes){
    
    *comparacoes = 0;
    
    while (raiz != NULL){
        (*comparacoes)++;
        if (valor == raiz->item){
            return 1;
        }else if (valor < raiz->item){
            raiz = raiz->esq;
        }else {
            raiz = raiz->dir;
        }
        
    }
    
    return 0;
}


void liberar_arvore(arvore* raiz) {
    if (raiz != NULL) {
        liberar_arvore(raiz->esq);
        liberar_arvore(raiz->dir);
        free(raiz);
    }
}


int main()
{
    no* lista = NULL;
    arvore* tree = NULL;
    int dados[TAMANHO_DADOS];
    int sorteado;
    
    srand(time(NULL));
    
    for (int i = 0; i < TAMANHO_DADOS; i++) {
        dados[i] = rand() % (TAMANHO_DADOS * 2);
        inserir_lista(&lista, dados[i]);
        tree = inserir_arvore(tree, dados[i]);
    }
    
    FILE* arquivo = fopen("resultados.csv", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return 1;
    }
    
    fprintf(arquivo, "numero_sorteado,comparacoes_abb,comparacoes_lista\n");
    
    for (int i = 0; i < NUM_BUSCAS; i++) {
        sorteado = dados[rand() % 10000];
        int comparacoes_abb = 0;
        int comparacoes_lista = 0;

        buscar_arvore(tree, sorteado, &comparacoes_abb);
        buscar_lista(lista, sorteado, &comparacoes_lista);

        fprintf(arquivo, "%d,%d,%d\n", sorteado, comparacoes_abb, comparacoes_lista);
    }
    

    fclose(arquivo);
    printf("Resultados salvos no arquivo resultados.csv.\n");
    
   
    liberar_lista(lista);
    liberar_arvore(tree);
    

    return 0;
}