#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include "../include/Agent.h"
#include "../include/SimulatedAnnealing.h"
#include "../include/Visualization.h"
#include "../include/Constants.h"

using namespace std;
using namespace std::chrono;

int main() {
    int numAgents;
    cout << "Enter the number of agents: ";
    cin >> numAgents;

    vector<Agent> agents = initializeAgents(numAgents);
    sf::RenderWindow window(sf::VideoMode(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE), "MAPF Simulation");

    auto start = high_resolution_clock::now();

    initializePaths(agents);
    simulatedAnnealing(agents);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken by simulated annealing: " << duration.count() << " milliseconds" << endl;

    // Run the simulation and visualization
    simulateAgents(agents, window);

    // Clean up shapes
    for (auto& agent : agents) {
        delete agent.shape;
    }

    return 0;
}
