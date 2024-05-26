#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include "../include/Agent.h"
#include "../include/SimulatedAnnealing.h"
#include "../include/Visualization.h"
#include "../include/Constants.h"

using namespace std::chrono;

int main() {
    int numAgents;
    std::cout << "Enter the number of agents: ";
    std::cin >> numAgents;

    int numObstacles = 10; // Adjust as needed
    auto obstacles = generateObstacles(numObstacles);

    std::vector<Agent> agents = initializeAgents(numAgents, obstacles);
    sf::RenderWindow window(sf::VideoMode(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE), "MAPF Simulation");

    auto start = high_resolution_clock::now();

    int makespan, sumOfCosts;
    initializePaths(agents, obstacles, makespan, sumOfCosts);
    simulatedAnnealing(agents);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << "Time taken by simulated annealing: " << duration.count() << " milliseconds" << std::endl;
    std::cout << "Makespan: " << makespan << std::endl;
    std::cout << "Sum of Costs: " << sumOfCosts << std::endl;

    // Run the simulation and visualization
    simulateAgents(agents, window, obstacles);

    // Clean up shapes
    for (auto& agent : agents) {
        delete agent.shape;
    }

    return 0;
}
