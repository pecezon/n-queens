#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N_QUEENS 8
#define POP_SIZE 100
#define MUTATION_RATE 0.05

// Función auxiliar para simular índices de arreglos negativos como en Python
int get_python_index(int idx, int max_size) {
    if (idx < 0) return max_size + idx;
    return idx;
}

// Genera la población inicial
void generate_initial_population(int pop[POP_SIZE][N_QUEENS]) {
    for (int i = 0; i < POP_SIZE; i++) {
        for (int j = 0; j < N_QUEENS; j++) {
            pop[i][j] = rand() % N_QUEENS;
        }
    }
}

// Calcula el puntaje de aptitud (fitness score)
double calculate_fitness(int individual[N_QUEENS]) {
    double max_fitness = (N_QUEENS * (N_QUEENS - 1)) / 2.0;
    
    // Colisiones horizontales
    int counts[N_QUEENS] = {0};
    for (int i = 0; i < N_QUEENS; i++) {
        counts[individual[i]]++;
    }
    
    double horizontal_collisions = 0;
    for (int i = 0; i < N_QUEENS; i++) {
        if (counts[i] > 1) {
            // Cada reina en la fila suma (counts[i] - 1)
            horizontal_collisions += counts[i] * (counts[i] - 1);
        }
    }
    horizontal_collisions /= 2.0;
    
    // Colisiones diagonales
    int left_diagonal[2 * N_QUEENS];
    int right_diagonal[2 * N_QUEENS];
    for(int i = 0; i < 2 * N_QUEENS; i++) { 
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
    
    double fitness_val = max_fitness - (horizontal_collisions + diagonal_collisions);
    return fitness_val / max_fitness;
}

// Selecciona los individuos más aptos usando Selección por Ruleta (Roulette Wheel Selection)
void select_fitted_population(int pop[POP_SIZE][N_QUEENS], double fitness[POP_SIZE], int parents[POP_SIZE][N_QUEENS]) {
    double total_fitness = 0;
    for (int i = 0; i < POP_SIZE; i++) {
        total_fitness += fitness[i];
    }
    
    for (int i = 0; i < POP_SIZE; i++) {
        double r = ((double)rand() / RAND_MAX) * total_fitness;
        double current_sum = 0;
        int selected_idx = POP_SIZE - 1;
        for (int j = 0; j < POP_SIZE; j++) {
            current_sum += fitness[j];
            if (current_sum >= r) {
                selected_idx = j;
                break;
            }
        }
        for (int k = 0; k < N_QUEENS; k++) {
            parents[i][k] = pop[selected_idx][k];
        }
    }
}

// Realiza el cruce de un solo punto entre dos padres
void crossover(int parent1[N_QUEENS], int parent2[N_QUEENS], int off1[N_QUEENS], int off2[N_QUEENS]) {
    int cp = 1 + rand() % (N_QUEENS - 1);
    for (int i = 0; i < N_QUEENS; i++) {
        if (i < cp) {
            off1[i] = parent1[i];
            off2[i] = parent2[i];
        } else {
            off1[i] = parent2[i];
            off2[i] = parent1[i];
        }
    }
}

// Aplica mutaciones aleatorias a un individuo
void mutate(int ind[N_QUEENS]) {
    for (int i = 0; i < N_QUEENS; i++) {
        double r = (double)rand() / RAND_MAX;
        if (r <= MUTATION_RATE) {
            ind[i] = rand() % N_QUEENS;
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
    // Inicializar la semilla para los números aleatorios
    srand(time(NULL));
    
    int population[POP_SIZE][N_QUEENS];
    double fitness_scores[POP_SIZE];
    int parents[POP_SIZE][N_QUEENS];
    int next_generation[POP_SIZE][N_QUEENS];
    
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
        
        for (int i = 0; i < POP_SIZE; i += 2) {
            int p1_idx = rand() % POP_SIZE;
            int p2_idx = rand() % POP_SIZE;
            
            crossover(parents[p1_idx], parents[p2_idx], next_generation[i], next_generation[i+1]);
            
            mutate(next_generation[i]);
            mutate(next_generation[i+1]);
        }
        
        for (int i = 0; i < POP_SIZE; i++) {
            for (int j = 0; j < N_QUEENS; j++) {
                population[i][j] = next_generation[i][j];
            }
        }
        
        generation++;
        
        // Si no hay solución después de un múltiplo de 1000 generaciones, inyectamos diversidad
        if (generation % 1000 == 0) {
            printf("Generación %d alcanzada, mejor aptitud hasta ahora: %.4f. Inyectando nuevo material genético...\n", generation, max_fitness);
            
            // Ordenar por fitness (Selection Sort)
            int indices[POP_SIZE];
            for (int i = 0; i < POP_SIZE; i++) indices[i] = i;
            
            for (int i = 0; i < POP_SIZE - 1; i++) {
                int max_idx = i;
                for (int j = i + 1; j < POP_SIZE; j++) {
                    if (fitness_scores[indices[j]] > fitness_scores[indices[max_idx]]) {
                        max_idx = j;
                    }
                }
                int temp = indices[i];
                indices[i] = indices[max_idx];
                indices[max_idx] = temp;
            }
            
            // Mantener la mitad superior, reemplazar la inferior con individuos aleatorios
            int temp_pop[POP_SIZE][N_QUEENS];
            for (int i = 0; i < POP_SIZE / 2; i++) {
                for (int j = 0; j < N_QUEENS; j++) {
                    temp_pop[i][j] = population[indices[i]][j];
                }
            }
            for (int i = POP_SIZE / 2; i < POP_SIZE; i++) {
                for (int j = 0; j < N_QUEENS; j++) {
                    temp_pop[i][j] = rand() % N_QUEENS;
                }
            }
            
            // Copiar la nueva población
            for (int i = 0; i < POP_SIZE; i++) {
                for (int j = 0; j < N_QUEENS; j++) {
                    population[i][j] = temp_pop[i][j];
                }
            }
        }
    }
    
    return 0;
}
