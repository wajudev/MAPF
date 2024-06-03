//
// Created by Lanre Ajibua on 26.05.24.
//

#include "../include/SimulatedAnnealing.h"
#include "../include/Constants.h"
#include "../include/Utils.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>


using namespace std;

int calculateCost(const vector<Agent>& agents, const unordered_set<pair<int, int>, pair_hash>& obstacles) {
    int cost = 0;
    unordered_map<pair<int, int>, int, pair_hash> positionCount;

    for (const auto& agent : agents) {
        cost += (int) agent.path.size();
        for (const auto& pos : agent.path) {
            positionCount[pos]++;
            if (obstacles.find(pos) != obstacles.end()) {
                cost += 1000; // High penalty for being on an obstacle
            }
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

void generateNeighbor(vector<Agent>& agents, mt19937& gen, uniform_int_distribution<>& agentDist, uniform_int_distribution<>& directionDist, const unordered_set<pair<int, int>, pair_hash>& obstacles) {
    int agentIdx = agentDist(gen);
    if (agents[agentIdx].path.empty()) return;

    uniform_int_distribution<> pathDist(0, (int) agents[agentIdx].path.size() - 1);
    int pathIdx = pathDist(gen);
    int direction = directionDist(gen);
    pair<int, int> newPos = agents[agentIdx].path[pathIdx];
    switch (direction) {
        case 0: newPos.first++; break; // Move right
        case 1: newPos.first--; break; // Move left
        case 2: newPos.second++; break; // Move down
        case 3: newPos.second--; break; // Move up
        default: break;
    }

    if (newPos.first >= 0 && newPos.first < GRID_SIZE && newPos.second >= 0 && newPos.second < GRID_SIZE && obstacles.find(newPos) == obstacles.end()) {
        agents[agentIdx].path[pathIdx] = newPos;
    }
}

void simulatedAnnealing(vector<Agent>& agents, const unordered_set<pair<int, int>, pair_hash>& obstacles, double initialTemp, double coolingRate, int maxIterations) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> agentDist(0, (int) agents.size() - 1);
    uniform_int_distribution<> directionDist(0, 3);
    uniform_real_distribution<> acceptanceDist(0.0, 1.0);

    double T = initialTemp;
    int currentCost = calculateCost(agents, obstacles);
    vector<Agent> bestSolution = agents;
    int bestCost = currentCost;

    for (int iter = 0; iter < maxIterations && T > 1e-5; ++iter) {
        vector<Agent> newSolution = agents;
        generateNeighbor(newSolution, gen, agentDist, directionDist, obstacles);
        int newCost = calculateCost(newSolution, obstacles);

        if (newCost < currentCost || exp((currentCost - newCost) / T) > acceptanceDist(gen)) {
            agents = newSolution;
            currentCost = newCost;

            if (newCost < bestCost) {
                bestSolution = newSolution;
                bestCost = newCost;
            }
        }

        T *= coolingRate;

        // Debug output
        cout << "Iteration " << iter << ", Temperature: " << T << ", Current Cost: " << currentCost << endl;
    }

    agents = bestSolution;
}

SAConfig gridSearchSAConfigs(std::vector<Agent>& agents, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles, const std::unordered_map<std::string, std::string>& config) {
    double initialTempMin = std::stod(config.at("initialTempMin"));
    double initialTempMax = std::stod(config.at("initialTempMax"));
    double coolingRateMin = std::stod(config.at("coolingRateMin"));
    double coolingRateMax = std::stod(config.at("coolingRateMax"));
    int maxIterationsMin = std::stoi(config.at("maxIterationsMin"));
    int maxIterationsMax = std::stoi(config.at("maxIterationsMax"));
    int steps = std::stoi(config.at("steps"));

    std::vector<SAConfig> configs;

    for (int i = 0; i <= steps; ++i) {
        for (int j = 0; j <= steps; ++j) {
            for (int k = 0; k <= steps; ++k) {
                double initialTemp = initialTempMin + i * (initialTempMax - initialTempMin) / steps;
                double coolingRate = coolingRateMin + j * (coolingRateMax - coolingRateMin) / steps;
                int maxIterations = maxIterationsMin + k * (maxIterationsMax - maxIterationsMin) / steps;
                configs.push_back({initialTemp, coolingRate, maxIterations});
            }
        }
    }

    SAConfig bestConfig = configs[0];
    int bestCost = std::numeric_limits<int>::max();
    int bestMakespan = std::numeric_limits<int>::max();
    int bestSumOfCosts = std::numeric_limits<int>::max();
    unsigned long bestDuration = std::numeric_limits<int>::max();


    for (const auto& conf : configs) {
        auto start = std::chrono::high_resolution_clock::now();
        simulatedAnnealing(agents, obstacles, conf.initialTemp, conf.coolingRate, conf.maxIterations);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        int currentCost = calculateCost(agents, obstacles);
        int makespan = 0;
        int sumOfCosts = 0;
        for (const auto& agent : agents) {
            makespan = std::max(makespan, static_cast<int>(agent.path.size()));
            sumOfCosts += static_cast<int>(agent.path.size());
        }

        std::cout << "Config: T=" << conf.initialTemp << ", CR=" << conf.coolingRate << ", Iter=" << conf.maxIterations
                  << " | Cost: " << currentCost << ", Makespan: " << makespan << ", Sum of Costs: " << sumOfCosts << ", Duration: " << duration.count() << "ms" << std::endl;

        if (currentCost < bestCost) {
            bestConfig = conf;
            bestCost = currentCost;
            bestMakespan = makespan;
            bestSumOfCosts = sumOfCosts;
            bestDuration = duration.count();
        }
        // Reset agents after each run
        resetAgents(agents);
    }

    std::cout << "Best Config: T=" << bestConfig.initialTemp << ", CR=" << bestConfig.coolingRate << ", Iter=" << bestConfig.maxIterations
              << " | Cost: " << bestCost << ", Makespan: " << bestMakespan << ", Sum of Costs: " << bestSumOfCosts << ", Duration: " << bestDuration << "ms" << std::endl;


    return bestConfig;
}