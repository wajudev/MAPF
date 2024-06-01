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

    double initialTemp = config.count("initialTemp") ? std::stod(config["initialTemp"]) : 1000.0;
    double coolingRate = config.count("coolingRate") ? std::stod(config["coolingRate"]) : 0.999;
    int maxIterations = config.count("maxIterations") ? std::stod(config["maxIterations"]) : 5000;

    std::unordered_set<std::pair<int, int>, pair_hash> obstacles;
    std::vector<Agent> agents;

    // Load data from file
    loadFromFile(filename, GRID_SIZE, obstacles, agents);

    std::cout << "Finished loading data from file.  Grid Size: " << GRID_SIZE << std::endl; // Debug
    sf::RenderWindow window(sf::VideoMode(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE), "MAPF Simulation");

    auto start = high_resolution_clock::now();

    int makespan, sumOfCosts;
    initializePaths(agents, obstacles, makespan, sumOfCosts);

    simulatedAnnealing(agents, obstacles, initialTemp, coolingRate, maxIterations);

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
