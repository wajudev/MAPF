#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <unordered_set>
#include "../include/Agent.h"
#include "../include/SimulatedAnnealing.h"
#include "../include/Visualization.h"
#include "../include/Constants.h"
#include "../include/Utils.h"

using namespace std::chrono;

int GRID_SIZE;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename> [configFile]" << std::endl;
    }

    std::string filename = argv[1];
    std::string configFile = (argc > 2) ? argv[2] : "config.txt";
    std::unordered_map<std::string, std::string> config = readConfigFile(configFile);

    std::unordered_set<std::pair<int, int>, pair_hash> obstacles;
    std::vector<Agent> agents;

    // Load data from file
    loadFromFile(filename, GRID_SIZE, obstacles, agents);

    std::cout << "Finished loading data from file.  Grid Size: " << GRID_SIZE << std::endl; // Debug
    // Create a window for visualization
    sf::RenderWindow window(sf::VideoMode(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE), "MAPF Simulation");

    int makespan, sumOfCosts;
    initializePaths(agents, obstacles, makespan, sumOfCosts);

    // Find the best configuration
    SAConfig bestConfig = gridSearchSAConfigs(agents, obstacles, config);

    // Use the best configuration for visualization
    resetAgents(agents);
    initializePaths(agents, obstacles, makespan, sumOfCosts);
    simulatedAnnealing(agents, obstacles, bestConfig.initialTemp, bestConfig.coolingRate, bestConfig.maxIterations);

    // Run the simulation and visualization
    simulateAgents(agents, window, obstacles);

    return 0;
}
