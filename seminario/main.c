#include <stdio.h>
#include <stdlib.h>

#define MAX 1000

typedef struct {
    int id;
    int priority;
    int wait_cycles;      
    int executed_cycles;   
    int required_cycles;   
    char type;           
} Process;

typedef struct {
    Process heap[MAX];
    int size;
} PriorityQueue;

PriorityQueue create_priority_queue() {
    PriorityQueue queue;
    queue.size = 0;
    return queue;
}

void swap(Process *a, Process *b) {
    Process temp = *a;
    *a = *b;
    *b = temp;
}

void insert(PriorityQueue *queue, int id, int priority, char type, int required_cycles) {
    int i = queue->size;

    queue->heap[i].id = id;
    queue->heap[i].priority = priority;
    queue->heap[i].wait_cycles = 0;
    queue->heap[i].executed_cycles = 0;
    queue->heap[i].required_cycles = required_cycles;
    queue->heap[i].type = type;

    while (i != 0) {
        int parent = (i - 1) / 2;

        if (queue->heap[parent].priority > queue->heap[i].priority) break;

        if (queue->heap[parent].priority == queue->heap[i].priority &&
            queue->heap[parent].type == 'i') break;

        swap(&queue->heap[i], &queue->heap[parent]);
        i = parent;
    }
    queue->size++;
}

Process remove_max(PriorityQueue *queue) {
    Process max = queue->heap[0];
    queue->heap[0] = queue->heap[--queue->size];

    int i = 0;
    while (2 * i + 1 < queue->size) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (queue->heap[left].priority > queue->heap[largest].priority ||
            (queue->heap[left].priority == queue->heap[largest].priority && 
             queue->heap[left].type == 'i' && queue->heap[largest].type != 'i')) {
            largest = left;
        }

        if (right < queue->size && 
            (queue->heap[right].priority > queue->heap[largest].priority ||
             (queue->heap[right].priority == queue->heap[largest].priority && 
              queue->heap[right].type == 'i' && queue->heap[largest].type != 'i'))) {
            largest = right;
        }

        if (largest == i) break;

        swap(&queue->heap[i], &queue->heap[largest]);
        i = largest;
    }
    return max;
}

void aging(PriorityQueue *queue) {
    for (int i = 0; i < queue->size; i++) {
        queue->heap[i].wait_cycles++;

        if (queue->heap[i].type == 'i' && queue->heap[i].wait_cycles >= 2) {
            queue->heap[i].priority++;
            queue->heap[i].wait_cycles = 0;
        } 
        else if (queue->heap[i].type == 'b' && queue->heap[i].wait_cycles >= 5) {
            queue->heap[i].priority++;
            queue->heap[i].wait_cycles = 0;
        }
    }
}

void print_queue(PriorityQueue *queue) {
    printf("\n--- Estado Atual da Fila de Prioridade: ---\n");
    for (int i = 0; i < queue->size; i++) {
        printf("ID: %d | Prioridade: %d | Tipo: %s | Ciclos em espera: %d | Executado: %d/%d\n",
               queue->heap[i].id,
               queue->heap[i].priority,
               queue->heap[i].type == 'i' ? "Interativo" : "Background",
               queue->heap[i].wait_cycles,
               queue->heap[i].executed_cycles,
               queue->heap[i].required_cycles);
    }
    printf("-------------------------------------\n");
}

void execute_process(PriorityQueue *queue) {
    if (queue->size == 0) return;

    Process *p = &queue->heap[0];
    p->executed_cycles++;

    printf("\n Executando Processo ID #%d | Prioridade: %d | Ciclo %d/%d\n",
           p->id, p->priority, p->executed_cycles, p->required_cycles);

    if (p->required_cycles - p->executed_cycles <= 1) {
        if (p->type == 'i' && p->priority < 10) {
            printf("Processo Interativo ID #%d prestes a finalizar! Prioridade elevada para 10.\n", p->id);
            p->priority = 10;
        } else if (p->type == 'b' && p->priority < 9) {
            printf("Processo Background ID #%d prestes a finalizar! Prioridade elevada para 9.\n", p->id);
            p->priority = 9;
        }
    }

    if (p->executed_cycles >= p->required_cycles) {
        printf("Processo ID #%d concluido e removido da fila.\n", p->id);
        remove_max(queue);
    }
}
void generate_new_process(PriorityQueue *queue, int cycles) {
    if (cycles % 4 != 0) return;   

    int new_id = rand() + cycles;   
    int new_priority = rand() % 10 + 1;  
    char type = (rand() % 2 == 0) ? 'i' : 'b';  
    int required_cycles = (rand() % 5) + 1;    

    printf("\n Novo processo chegou! (ID #%d, Prioridade: %d, Ciclos necessarios: %d)\n",
           new_id, new_priority, required_cycles);

    insert(queue, new_id, new_priority, type, required_cycles);
}


int main() {
    PriorityQueue queue = create_priority_queue();
    int cycles = 0;

    insert(&queue, 1, 3, 'i', 4);
    insert(&queue, 2, 2, 'b', 5);
    insert(&queue, 3, 4, 'i', 2);

    print_queue(&queue);

    printf("\nIniciando Agendamento de Processos...\n");

    while (queue.size > 0) {
        printf("\n--- Ciclo numero: %d ---\n", cycles);

        generate_new_process(&queue, cycles);
        execute_process(&queue);
        cycles++;
        aging(&queue);
        print_queue(&queue);
    }

    printf("\nTodos os processos foram concluidos.\n");
    return 0;
}
