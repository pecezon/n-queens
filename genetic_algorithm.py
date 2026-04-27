import random
import time

# Genera la población inicial usando PERMUTACIONES exclusivas
def generate_initial_population(population_size: int, n_queens: int) -> list:
    population = []
    for _ in range(population_size):
        # Al usar una permutación aseguramos que no haya colisiones horizontales desde el día 1
        individual = list(range(n_queens))
        random.shuffle(individual)
        population.append(individual)
    
    return population

# No va a haber colisiones verticales porque el individuo tiene un valor por columna.
def calculate_fitness(individual: list) -> float:
    n = len(individual)
    max_fitness = (n * (n - 1)) / 2
    
    # Ya no hay colisiones horizontales (ni verticales) gracias al uso estricto de permutaciones.
    # Eliminar este cálculo reduce enormemente el tiempo de procesamiento por generación.
    left_diagonal = [0] * (2 * n)
    right_diagonal = [0] * (2 * n)
    for i in range(n):
        left_diagonal[i + individual[i] - 1] += 1
        right_diagonal[n - i + individual[i] - 2] += 1

    diagonal_collisions = 0
    for i in range(2 * n - 1):
        counter = 0
        if left_diagonal[i] > 1:
            counter += left_diagonal[i] - 1
        if right_diagonal[i] > 1:
            counter += right_diagonal[i] - 1
        diagonal_collisions += counter / (n - abs(i - n + 1))
    
    fitness_val = int(max_fitness - diagonal_collisions)
    fitness_score = fitness_val / max_fitness
    
    return fitness_score

# Selecciona los individuos usando Torneo (Tournament Selection) para mantener la presión selectiva
def select_fitted_population(population: list, fitness_scores: list) -> list:
    parents = []
    pop_size = len(population)
    tournament_size = 5 # Seleccionamos al mejor de 5 individuos al azar
    for _ in range(pop_size):
        best_idx = random.randint(0, pop_size - 1)
        for _ in range(tournament_size - 1):
            idx = random.randint(0, pop_size - 1)
            if fitness_scores[idx] > fitness_scores[best_idx]:
                best_idx = idx
        parents.append(population[best_idx])
    return parents

# Realiza el Cruce de Orden (Order Crossover - OX1) para preservar las permutaciones.
def crossover(parent1: list, parent2: list) -> tuple:
    n = len(parent1)
    # Elegir dos puntos de corte aleatorios
    start, end = sorted(random.sample(range(n), 2))
    
    def apply_ox(p1, p2):
        offspring = [-1] * n
        # Copiar segmento del padre 1
        offspring[start:end+1] = p1[start:end+1]
        
        # Rellenar con los del padre 2 omitiendo los repetidos
        p2_idx = (end + 1) % n
        off_idx = (end + 1) % n
        
        while -1 in offspring:
            gene = p2[p2_idx]
            if gene not in offspring:
                offspring[off_idx] = gene
                off_idx = (off_idx + 1) % n
            p2_idx = (p2_idx + 1) % n
            
        return offspring

    return apply_ox(parent1, parent2), apply_ox(parent2, parent1)

# Mutación por Intercambio (Swap Mutation) para no romper las permutaciones.
def mutate(individual: list, mutation_rate: float) -> list:
    n = len(individual)
    for i in range(n):
        if random.random() <= mutation_rate:
            # En lugar de asignar un valor al azar (que crearía duplicados), intercambiamos
            swap_idx = random.randint(0, n - 1)
            individual[i], individual[swap_idx] = individual[swap_idx], individual[i]
            
    return individual

# Visualiza el tablero.
def visualize_board(individual: list):
    n = len(individual)
    print(f"\nSolución encontrada: {individual}")
    for row in range(n):
        row_str = ""
        for col in range(n):
            if individual[col] == row:
                row_str += "Q "
            else:
                row_str += ". "
        print(row_str)
    print()

def run_genetic_algorithm(n_queens: int = 32, population_size: int = 500, mutation_rate: float = 0.1):
    print(f"Iniciando Algoritmo Genético para {n_queens}-Reinas...")
    start_time = time.time()
    
    # Generación de la población inicial
    population = generate_initial_population(population_size, n_queens)
    
    generation = 0
    while True:
        # Cálculo de la función de fitness
        fitness_scores = [calculate_fitness(ind) for ind in population]
        
        # Se verifica si se encontró una solución perfecta
        max_fitness = max(fitness_scores)
        best_idx = fitness_scores.index(max_fitness)
        best_individual = population[best_idx]
        
        if max_fitness == 1.0:
            end_time = time.time()
            
            print(f"\n¡Solución perfecta encontrada en la generación {generation}!")
            print(f"Tiempo de ejecución: {end_time - start_time:.4f} segundos")
            visualize_board(best_individual)
            return best_individual
        
        # Selección de los individuos mas aptos
        parents = select_fitted_population(population, fitness_scores)
        
        next_generation = [best_individual] # Elitismo: Preservar siempre al mejor de la generación
        
        # Generación de la siguiente generación
        while len(next_generation) < population_size:
            # Selección de dos padres aleatorios del pool de padres seleccionados
            parent1, parent2 = random.sample(parents, 2)
            
            # Cruce
            offspring1, offspring2 = crossover(parent1, parent2)
            
            # Mutación
            offspring1 = mutate(offspring1, mutation_rate)
            offspring2 = mutate(offspring2, mutation_rate)
            
            next_generation.extend([offspring1, offspring2])
            
        # Para asegurar el tamaño exacto de la población
        population = next_generation[:population_size]
        generation += 1
        
        # Si no hay solución después de un múltiplo de 500 generaciones, inyectamos diversidad genética
        if generation % 500 == 0:
            print(f"Generación {generation} alcanzada, mejor aptitud hasta ahora: {max_fitness:.4f}. Inyectando nuevo material genético...")
            # Extinción Masiva: Para evitar convergencia prematura por clones, mantenemos SOLO al mejor individuo
            # y reemplazamos a todos los demás con sangre completamente nueva.
            best_idx = fitness_scores.index(max_fitness)
            best_ind = population[best_idx]
            fresh_population = generate_initial_population(population_size - 1, n_queens)
            population = [best_ind] + fresh_population

if __name__ == "__main__":
    run_genetic_algorithm()
