#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>  //for uint32_t
#include <ctype.h>   //for input validations
#include "huffman.h"
#include "pqueue_heap.h"

#define BUFFER_SIZE 1024

int main() {
    int option;
    
    printf("====== Compactador Huffman ======\n");
    printf("Escolha uma opcao:\n");
    printf("1 - Compactar arquivo\n");
    printf("2 - Descompactar arquivo\n");
    printf("Opcao: ");
    scanf("%d", &option);
    getchar(); //clear the ENTER buffer

    if (option == 1) {
        printf("\nInsira o nome do arquivo a ser compactado, com a extensao:\n");

        char filename[BUFFER_SIZE];
        scanf("%s", filename);

        FILE* original_file = fopen(filename, "rb");
        if (original_file == NULL) {
            perror("Erro ao abrir o arquivo");
            return 1;
        }

        //Remove file name extension
        char base_name[BUFFER_SIZE];
        strcpy(base_name, filename);

        char* dot = strrchr(base_name, '.');
        if (dot != NULL) {
            *dot = '\0';    //ends the string before the dot
        }

        //Create a new name with .huff extension
        char new_file_name[BUFFER_SIZE];
        snprintf(new_file_name, sizeof(new_file_name), "%.*s.huff",
        (int)(sizeof(new_file_name) - strlen(".huff") - 1),base_name);

        FILE *new_file = fopen(new_file_name, "wb");
        if (new_file == NULL) {
            perror("Erro ao criar o arquivo de saída");
            fclose(original_file);
            return 1;
        }

        //Creates the two priority queues
        PRIORITY_QUEUE* huff_queue1 = create_queue();
        PRIORITY_QUEUE* huff_queue2 = create_queue();

        //Fills the rows with the character frequencies from the file
        create_huff_queue(original_file, &huff_queue1, &huff_queue2);

        //Build the Huffman tree
        NODE* root = build_huffman_tree(huff_queue1);

        //Creates the Huffman code table
        HuffmanCode huff_table[256] = {0};      
        uint32_t code = 0;                      
        create_huffman_table(root, code, 0, huff_table);   

        //Writes the header and tree to the new file
        write_header(huff_queue2, huff_table, new_file, root);


        free_huffman_tree(root);
        free_priority_queue(huff_queue1);
        free_priority_queue(huff_queue2);

        rewind(original_file);

        //Compresses the original file data using the Huffman table
        compactor(original_file, new_file, huff_table);

    
        fclose(original_file);
        fclose(new_file);

        printf("Arquivo compactado com sucesso: %s\n", new_file_name);

    } else if (option == 2) {
        char compressed_filename[BUFFER_SIZE];
        char final_format[10];
        printf("\nInsira o nome do arquivo compactado (.huff):\n");
        scanf("%s", compressed_filename);

        printf("\nInsira o formato da extensao final (ex: jpg, txt, etc):\n");
        scanf("%s", final_format);

        decompact(compressed_filename, final_format);

    } else {
        printf("Opcao invalida.\n");
    }

    return 0;
}