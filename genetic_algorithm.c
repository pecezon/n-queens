#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N_QUEENS 32
#define POP_SIZE 2000
#define MUTATION_RATE 0.5

// Función auxiliar para simular índices de arreglos negativos como en Python
int get_python_index(int idx, int max_size) {
    if (idx < 0) return max_size + idx;
    return idx;
}

// Mezcla un arreglo aleatoriamente (Fisher-Yates) para generar permutaciones válidas
void shuffle(int *array, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

// Genera la población inicial usando PERMUTACIONES exclusivas
void generate_initial_population(int pop[POP_SIZE][N_QUEENS]) {
    for (int i = 0; i < POP_SIZE; i++) {
        for (int j = 0; j < N_QUEENS; j++) {
            pop[i][j] = j;
        }
        shuffle(pop[i], N_QUEENS);
    }
}

// Calcula el puntaje de aptitud (fitness score)
// Optimizada: Ya no calcula colisiones horizontales (que son 0 por defecto).
double calculate_fitness(int individual[N_QUEENS]) {
    double max_fitness = (N_QUEENS * (N_QUEENS - 1)) / 2.0;
    
    int left_diagonal[2 * N_QUEENS];
    int right_diagonal[2 * N_QUEENS];
    for (int i = 0; i < 2 * N_QUEENS; i++) { 
        left_diagonal[i] = 0; 
        right_diagonal[i] = 0; 
    }
    
    for (int i = 0; i < N_QUEENS; i++) {
        int l_idx = get_python_index(i + individual[i] - 1, 2 * N_QUEENS);
        int r_idx = get_python_index(N_QUEENS - i + individual[i] - 2, 2 * N_QUEENS);
        left_diagonal[l_idx]++;
        right_diagonal[r_idx]++;
    }
    
    double diagonal_collisions = 0;
    for (int i = 0; i < 2 * N_QUEENS - 1; i++) {
        double counter = 0;
        if (left_diagonal[i] > 1) counter += left_diagonal[i] - 1;
        if (right_diagonal[i] > 1) counter += right_diagonal[i] - 1;
        
        int denom = N_QUEENS - abs(i - N_QUEENS + 1);
        if (denom != 0) {
            diagonal_collisions += counter / denom;
        }
    }
    
    double fitness_val = max_fitness - diagonal_collisions;
    return fitness_val / max_fitness;
}

// Selecciona los individuos usando Torneo (Tournament Selection)
void select_fitted_population(int pop[POP_SIZE][N_QUEENS], double fitness[POP_SIZE], int parents[POP_SIZE][N_QUEENS]) {
    int tournament_size = 5;
    for (int i = 0; i < POP_SIZE; i++) {
        int best_idx = rand() % POP_SIZE;
        for (int j = 0; j < tournament_size - 1; j++) {
            int idx = rand() % POP_SIZE;
            if (fitness[idx] > fitness[best_idx]) {
                best_idx = idx;
            }
        }
        for (int k = 0; k < N_QUEENS; k++) {
            parents[i][k] = pop[best_idx][k];
        }
    }
}

// Realiza el Cruce de Orden (Order Crossover - OX1)
void crossover(int parent1[N_QUEENS], int parent2[N_QUEENS], int off1[N_QUEENS], int off2[N_QUEENS]) {
    // Puntos de corte aleatorios
    int p1 = rand() % N_QUEENS;
    int p2 = rand() % N_QUEENS;
    int start = p1 < p2 ? p1 : p2;
    int end = p1 > p2 ? p1 : p2;
    
    // Arrays para seguimiento (valores de 0 a N-1)
    int in_off1[N_QUEENS] = {0};
    int in_off2[N_QUEENS] = {0};
    
    for(int i=0; i<N_QUEENS; i++) {
        off1[i] = -1;
        off2[i] = -1;
    }
    
    // Paso 1: Copiar sub-secuencia del padre respectivo
    for (int i = start; i <= end; i++) {
        off1[i] = parent1[i];
        in_off1[parent1[i]] = 1;
        
        off2[i] = parent2[i];
        in_off2[parent2[i]] = 1;
    }
    
    // Paso 2: Rellenar circularmente desde el final del corte
    int p2_idx1 = (end + 1) % N_QUEENS;
    int off_idx1 = (end + 1) % N_QUEENS;
    
    while (1) {
        int done = 1;
        for(int i=0; i<N_QUEENS; i++) {
            if(off1[i] == -1) { done = 0; break; }
        }
        if(done) break;
        
        int gene = parent2[p2_idx1];
        if (!in_off1[gene]) {
            off1[off_idx1] = gene;
            in_off1[gene] = 1;
            off_idx1 = (off_idx1 + 1) % N_QUEENS;
        }
        p2_idx1 = (p2_idx1 + 1) % N_QUEENS;
    }
    
    int p2_idx2 = (end + 1) % N_QUEENS;
    int off_idx2 = (end + 1) % N_QUEENS;
    
    while (1) {
        int done = 1;
        for(int i=0; i<N_QUEENS; i++) {
            if(off2[i] == -1) { done = 0; break; }
        }
        if(done) break;
        
        int gene = parent1[p2_idx2];
        if (!in_off2[gene]) {
            off2[off_idx2] = gene;
            in_off2[gene] = 1;
            off_idx2 = (off_idx2 + 1) % N_QUEENS;
        }
        p2_idx2 = (p2_idx2 + 1) % N_QUEENS;
    }
}

// Mutación por Inversión (Inversion Mutation)
void mutate(int ind[N_QUEENS]) {
    double r = (double)rand() / RAND_MAX;
    if (r <= MUTATION_RATE) {
        int p1 = rand() % N_QUEENS;
        int p2 = rand() % N_QUEENS;
        int start = p1 < p2 ? p1 : p2;
        int end = p1 > p2 ? p1 : p2;
        
        while (start < end) {
            int temp = ind[start];
            ind[start] = ind[end];
            ind[end] = temp;
            start++;
            end--;
        }
    }
}

// Visualiza el tablero
void visualize_board(int ind[N_QUEENS]) {
    printf("\nSolución encontrada: [");
    for(int i = 0; i < N_QUEENS; i++) {
        printf("%d%s", ind[i], i == N_QUEENS - 1 ? "]\n" : ", ");
    }
    
    for (int row = 0; row < N_QUEENS; row++) {
        for (int col = 0; col < N_QUEENS; col++) {
            if (ind[col] == row) printf("Q ");
            else printf(". ");
        }
        printf("\n");
    }
    printf("\n");
}

int main() {
    srand(time(NULL));
    
    // static para evitar Stack Overflow con poblaciones grandes
    static int population[POP_SIZE][N_QUEENS];
    static double fitness_scores[POP_SIZE];
    static int parents[POP_SIZE][N_QUEENS];
    static int next_generation[POP_SIZE][N_QUEENS];
    
    printf("Iniciando Algoritmo Genético para %d-Reinas...\n", N_QUEENS);
    clock_t start_time = clock();
    
    generate_initial_population(population);
    
    int generation = 0;
    while (1) {
        double max_fitness = -1.0;
        int best_idx = -1;
        
        for (int i = 0; i < POP_SIZE; i++) {
            fitness_scores[i] = calculate_fitness(population[i]);
            if (fitness_scores[i] > max_fitness) {
                max_fitness = fitness_scores[i];
                best_idx = i;
            }
        }
        
        if (max_fitness >= 1.0) {
            clock_t end_time = clock();
            double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
            
            printf("\n¡Solución perfecta encontrada en la generación %d!\n", generation);
            printf("Tiempo de ejecución: %.4f segundos\n", time_spent);
            visualize_board(population[best_idx]);
            break;
        }
        
        select_fitted_population(population, fitness_scores, parents);
        
        // Elitismo: Mantener al mejor individuo en la pos 0
        for (int k = 0; k < N_QUEENS; k++) {
            next_generation[0][k] = population[best_idx][k];
        }
        
        int next_idx = 1;
        while (next_idx < POP_SIZE) {
            int p1_idx = rand() % POP_SIZE;
            int p2_idx = rand() % POP_SIZE;
            
            int off1[N_QUEENS], off2[N_QUEENS];
            crossover(parents[p1_idx], parents[p2_idx], off1, off2);
            
            mutate(off1);
            mutate(off2);
            
            for (int k = 0; k < N_QUEENS; k++) {
                next_generation[next_idx][k] = off1[k];
            }
            next_idx++;
            
            if (next_idx < POP_SIZE) {
                for (int k = 0; k < N_QUEENS; k++) {
                    next_generation[next_idx][k] = off2[k];
                }
                next_idx++;
            }
        }
        
        for (int i = 0; i < POP_SIZE; i++) {
            for (int j = 0; j < N_QUEENS; j++) {
                population[i][j] = next_generation[i][j];
            }
        }
        
        generation++;
        
        // Extinción Masiva: Prevenir estancamiento en óptimos locales
        if (generation % 500 == 0) {
            printf("Generación %d alcanzada, mejor aptitud hasta ahora: %.4f. Inyectando nuevo material genético...\n", generation, max_fitness);
            
            int best_ind[N_QUEENS];
            for (int k = 0; k < N_QUEENS; k++) {
                best_ind[k] = population[best_idx][k];
            }
            
            generate_initial_population(population);
            
            for (int k = 0; k < N_QUEENS; k++) {
                population[0][k] = best_ind[k];
            }
        }
    }
    
    return 0;
}
