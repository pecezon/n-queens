import random
import time

# Genera la población inicial 
def generate_initial_population(population_size: int, n_queens: int) -> list:
    population = []
    for _ in range(population_size):
        individual = [random.randint(0, n_queens - 1) for _ in range(n_queens)]
        population.append(individual)
    
    return population

# No va a haber colisiones verticales porque el individuo tiene un valor por columna.
def calculate_fitness(individual: list) -> float:
    n = len(individual)
    max_fitness = (n * (n - 1)) / 2
    
    # Colisiones horizontales
    horizontal_collisions = sum([individual.count(queen) - 1 for queen in individual]) / 2
    
    # Colisiones diagonales
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
    
    fitness_val = int(max_fitness - (horizontal_collisions + diagonal_collisions))
    fitness_score = fitness_val / max_fitness
    
    return fitness_score

# Selecciona los k individuos mas aptos para la siguiente generación
def select_fitted_population(population: list, fitness_scores: list) -> list:
    return random.choices(population, weights=fitness_scores, k=len(population))

# Realiza el cruce de un solo punto entre dos padres para producir dos descendientes.
def crossover(parent1: list, parent2: list) -> tuple:
    n = len(parent1)
    # Punto aleatorio de cruce
    crossover_point = random.randint(1, n - 1)
    
    offspring1 = parent1[:crossover_point] + parent2[crossover_point:]
    offspring2 = parent2[:crossover_point] + parent1[crossover_point:]
    
    return offspring1, offspring2

# Mutación de un individuo basada en una probabilidad por gen.
def mutate(individual: list, mutation_rate: float) -> list:
    n = len(individual)
    for i in range(n):
        if random.random() <= mutation_rate:
            individual[i] = random.randint(0, n - 1)
            
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

def run_genetic_algorithm(n_queens: int = 8, population_size: int = 100, mutation_rate: float = 0.05):
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
        if max_fitness == 1.0:
            end_time = time.time()
            best_idx = fitness_scores.index(max_fitness)
            best_individual = population[best_idx]
            
            print(f"\n¡Solución perfecta encontrada en la generación {generation}!")
            print(f"Tiempo de ejecución: {end_time - start_time:.4f} segundos")
            visualize_board(best_individual)
            return best_individual
        
        # Selección de los individuos mas aptos
        parents = select_fitted_population(population, fitness_scores)
        
        next_generation = []
        
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
        
        # Si no hay solución después de un múltiplo de 1000 generaciones, inyectamos diversidad genética
        if generation % 1000 == 0:
            print(f"Generación {generation} alcanzada, mejor aptitud hasta ahora: {max_fitness:.4f}. Inyectando nuevo material genético...")
            # Se mantiene la mitad superior y se reemplaza la mitad inferior con nuevos individuos aleatorios
            sorted_population = [x for _, x in sorted(zip(fitness_scores, population), key=lambda pair: pair[0], reverse=True)]
            fresh_population = generate_initial_population(population_size // 2, n_queens)
            population = sorted_population[:(population_size - len(fresh_population))] + fresh_population

if __name__ == "__main__":
    run_genetic_algorithm()
