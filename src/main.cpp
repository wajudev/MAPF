#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include "../include/Agent.h"
#include "../include/SimulatedAnnealing.h"
#include "../include/Visualization.h"
#include "../include/Constants.h"
#include "../include/Utils.h"

using namespace std::chrono;

int GRID_SIZE;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    }

    std::string filename = argv[1];
    std::unordered_set<std::pair<int, int>, pair_hash> obstacles;
    std::vector<Agent> agents;

    // Load data from file
    loadFromFile(filename, GRID_SIZE, obstacles, agents);

    std::cout << "Finished loading data from file.  Grid Size: " << GRID_SIZE << std::endl; // Debug
    sf::RenderWindow window(sf::VideoMode(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE), "MAPF Simulation");

    auto start = high_resolution_clock::now();

    int makespan, sumOfCosts;
    initializePaths(agents, obstacles, makespan, sumOfCosts);
    simulatedAnnealing(agents, obstacles);

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
