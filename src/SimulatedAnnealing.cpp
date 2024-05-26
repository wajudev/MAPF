//
// Created by Lanre Ajibua on 26.05.24.
//

#include "../include/SimulatedAnnealing.h"
#include "../include/Constants.h"


using namespace std;

int calculateCost(const vector<Agent>& agents) {
    int cost = 0;
    unordered_map<pair<int, int>, int, pair_hash> positionCount;

    for (const auto& agent : agents) {
        cost += agent.path.size();
        for (const auto& pos : agent.path) {
            positionCount[pos]++;
        }
    }

    // Add conflict penalties (simple example: conflicts at same position)
    for (const auto& entry : positionCount) {
        if (entry.second > 1) {
            cost += 100 * (entry.second - 1); // Conflict penalty
        }
    }

    return cost;
}

void generateNeighbor(vector<Agent>& agents, mt19937& gen, uniform_int_distribution<>& agentDist, uniform_int_distribution<>& directionDist) {
    int agentIdx = agentDist(gen);
    if (agents[agentIdx].path.empty()) return;

    uniform_int_distribution<> pathDist(0, agents[agentIdx].path.size() - 1);
    int pathIdx = pathDist(gen);
    int direction = directionDist(gen);
    pair<int, int> newPos = agents[agentIdx].path[pathIdx];
    switch (direction) {
        case 0: newPos.first++; break; // Move right
        case 1: newPos.first--; break; // Move left
        case 2: newPos.second++; break; // Move down
        case 3: newPos.second--; break; // Move up
    }
    if (newPos.first >= 0 && newPos.first < GRID_SIZE && newPos.second >= 0 && newPos.second < GRID_SIZE) {
        agents[agentIdx].path[pathIdx] = newPos;
    }
}

void simulatedAnnealing(vector<Agent>& agents) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> agentDist(0, agents.size() - 1);
    uniform_int_distribution<> directionDist(0, 3);

    double T = 1000.0;
    double coolingRate = 0.999; // Adjusted cooling rate
    int maxIterations = 5000; // Reduced number of iterations

    int currentCost = calculateCost(agents);
    vector<Agent> bestSolution = agents;
    int bestCost = currentCost;

    for (int iter = 0; iter < maxIterations; ++iter) {
        vector<Agent> newSolution = agents;
        generateNeighbor(newSolution, gen, agentDist, directionDist);
        int newCost = calculateCost(newSolution);

        if (newCost < currentCost || exp((currentCost - newCost) / T) > ((double) rand() / RAND_MAX)) {
            agents = newSolution;
            currentCost = newCost;

            if (newCost < bestCost) {
                bestSolution = newSolution;
                bestCost = newCost;
            }
        }

        T *= coolingRate;

        if (T < 1e-5) break;
    }

    agents = bestSolution;
}

