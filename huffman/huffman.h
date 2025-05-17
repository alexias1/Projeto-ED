#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "pqueue_heap.h"

#define BUFFER_SIZE 1024

/*
    FUNÇÕES PARA COMPACTAR O ARQUIVO
*/


//Creates priority queue with frequency of each character in the file
void create_huff_queue(FILE *input_file, PRIORITY_QUEUE** pq1, PRIORITY_QUEUE** pq2) {
    int freq[256] = {0};
    unsigned char c;

    //Count character frequencies
    while (fread(&c, 1, 1, input_file) == 1) {
        freq[c]++;
    }

    //Create nodes for characters with non-zero frequency and insert them into both queues
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            NODE* node = create_node(i, freq[i], NULL, NULL);
            insert(*pq1, node);
            insert(*pq2, node);
        }
    }
}

//Constructs Huffman tree from priority queue
NODE* build_huffman_tree(PRIORITY_QUEUE* pq) {
    while (pq->size > 1) {
        NODE* left = remove_lower(pq);
        NODE* right = remove_lower(pq);

        //Create a parent node with the left and right children
        NODE* parent = create_node('\0', left->frequency + right->frequency, left, right);

        //Insert the parent node back into the priority queue
        insert(pq, parent);
    }

    //The last remaining node is the root of the Huffman tree
    return remove_lower(pq);
}


typedef struct {
    uint32_t code;
    int length;
} HuffmanCode;


void create_huffman_table(NODE* root, uint32_t code, int depth, HuffmanCode huff_table[256]) {
    if (!root) return;

    if (!root->left && !root->right) {
        huff_table[root->character].code = code;
        huff_table[root->character].length = depth;
        return;
    }

    create_huffman_table(root->left, (code << 1), depth + 1, huff_table);
    create_huffman_table(root->right, (code << 1) | 1, depth + 1, huff_table);
}

//Calculates how many total bits will be written to the compressed body
int calculate_bits_trashed(PRIORITY_QUEUE* pq, HuffmanCode huff_table[256]) {
    int bit_amount = 0;

    while (pq->size > 0) {
        NODE* node = remove_lower(pq);
        if (node && huff_table[node->character].length > 0) {
            bit_amount += node->frequency * huff_table[node->character].length;
        }
    }

    return bit_amount;
}

//Counts the number of nodes in the tree (for the tree size at the head)
int count_tree_size(NODE* root) {
    if (!root) return 0;
    return count_tree_size(root->left) + count_tree_size(root->right) + 1;
}

int is_leaf(NODE* node) {
    return node && !node->left && !node->right;
}


//Writes the Huffman tree to the output file
void write_tree(NODE* root, FILE* output_file) {
    if (is_leaf(root)) {
        fputc('1', output_file);
        if (root->character == '*' || root->character == '\\') {
            fputc('\\', output_file);
        }
        fputc(root->character, output_file);
    } else {
        fputc('0', output_file);
        write_tree(root->left, output_file);
        write_tree(root->right, output_file);
    }
}

//Write header to new file (trash, treesize, tree)
void write_header(PRIORITY_QUEUE* pq, HuffmanCode huff_table[256], FILE *output_file, NODE* root) {

    int total_bits = calculate_bits_trashed(pq, huff_table);
    int trash = ((8 - (total_bits % 8)) % 8);
    int tree_size = count_tree_size(root);

    unsigned short header = (trash << 13) | tree_size;
    unsigned char byte1 = header >> 8;
    unsigned char byte2 = header & 0xFF;

    fwrite(&byte1, 1, 1, output_file);
    fwrite(&byte2, 1, 1, output_file);

    write_tree(root, output_file);
}

//Structure to store bits until a byte is completed
typedef struct {
    unsigned char byte;
    int bits_used;
} BitBuffer;


void bit_buffer_add(BitBuffer *bit_buffer, int bit) {
    bit_buffer->byte <<= 1;
    bit_buffer->byte |= (bit & 1);
    bit_buffer->bits_used++;
}


void write_buffer(FILE *f, BitBuffer *bit_buffer) {
    if (bit_buffer->bits_used == 0) return;

    unsigned char temp_byte = bit_buffer->byte << (8 - bit_buffer->bits_used);
    fwrite(&temp_byte, 1, 1, f);

    bit_buffer->byte = 0;
    bit_buffer->bits_used = 0;
}

//Writes the compressed data to the new file
void compactor(FILE *input_file, FILE *output_file, HuffmanCode huff_table[256]) {
    unsigned char c;
    BitBuffer bit_buffer = {0, 0}; //Initialize the bit buffer

    //Read each character from the input file
    while (fread(&c, 1, 1, input_file) == 1) {
        HuffmanCode code = huff_table[c];

        //Add each bit of the Huffman code to the bit buffer
        for (int i = code.length - 1; i >= 0; i--) {
            bit_buffer_add(&bit_buffer, (code.code >> i) & 1);

            //Write the buffer to the file when it is full
            if (bit_buffer.bits_used == 8) {
                write_buffer(output_file, &bit_buffer);
            }
        }
    }

    //Write any remaining bits in the buffer
    write_buffer(output_file, &bit_buffer);
}


void free_huffman_tree(NODE* root) {
    if (root == NULL) return;

    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root);
}


/*
    FUNÇÕES PARA DESCOMPACTAR O ARQUIVO
*/


//Reads the first two bytes of the header and extracts garbage and tree size
void read_header(FILE *file, int *trash, int *tree_size) {
    unsigned char byte1, byte2;
    fread(&byte1, 1, 1, file);
    fread(&byte2, 1, 1, file);

    unsigned short header = (byte1 << 8) | byte2;

    *trash = header >> 13;
    *tree_size = header & 0x1FFF;
}

//Reads the encoded tree from the compressed file and reconstructs the Huffman tree
NODE* read_tree(FILE *file, int *bytes_read) {
    int c = fgetc(file);
    (*bytes_read)++;

    if (c == '1') {
        int next = fgetc(file);
        (*bytes_read)++;

        if (next == '\\') {
            next = fgetc(file);
            (*bytes_read)++;
        }

        return create_node((unsigned char)next, 0, NULL, NULL);
    } else if (c == '0') {
        NODE *left = read_tree(file, bytes_read);
        NODE *right = read_tree(file, bytes_read);
        return create_node('\0', 0, left, right);
    }

    return NULL;
}

//Iterates through the bits of the compressed body and writes the characters to the output file
void decompress(FILE *input, FILE *output, NODE* root, int trash_size, int header_bytes) {
    fseek(input, 0, SEEK_END);
    long file_size = ftell(input);
    long data_size = file_size - header_bytes;
    fseek(input, header_bytes, SEEK_SET);

    NODE* current = root;
    unsigned char byte;

    for (long i = 0; i < data_size; i++) {
        fread(&byte, 1, 1, input);

        for (int bit = 7; bit >= 0; bit--) {
            int current_bit = (byte >> bit) & 1;
            if (current_bit == 0)
                current = current->left;
            else
                current = current->right;

            if (is_leaf(current)) {
                fputc(current->character, output);
                current = root;
            }

            //Avoid processing "garbage" bits in the last byte
            if (i == data_size - 1 && bit == trash_size - 1) {
                return;
            }
        }
    }
}


void decompact(const char* compressed_filename, char final_format[]) {
    FILE *input_file = fopen(compressed_filename, "rb");
    if (!input_file) {
        perror("Erro ao abrir o arquivo compactado");
        return;
    }

    //Create name for output file
    char output_filename[BUFFER_SIZE]; 
    strcpy(output_filename, compressed_filename);
    char* dot = strrchr(output_filename, '.');
    if (dot) *dot = '\0'; 

    char base_name[50];
    strcpy(base_name, output_filename);
    snprintf(output_filename, sizeof(output_filename), "%s_descompactado.%s", base_name, final_format);

    FILE *output_file = fopen(output_filename, "wb");
    if (!output_file) {
        perror("Erro ao criar arquivo de saida");
        fclose(input_file);
        return;
    }

    int trash_size = 0, tree_size = 0, bytes_read = 0;
    read_header(input_file, &trash_size, &tree_size);
    NODE* root = read_tree(input_file, &bytes_read);

    decompress(input_file, output_file, root, trash_size, 2 + bytes_read);

    printf("Arquivo descompactado com sucesso: %s\n", output_filename);

    free_huffman_tree(root);
    fclose(input_file);
    fclose(output_file);
}


#endif // HUFFMAN_H