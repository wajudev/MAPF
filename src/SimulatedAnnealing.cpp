#include "../include/SimulatedAnnealing.h"
#include "../include/Constants.h"
#include "../include/Utils.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

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

    // Ensure the move is within grid bounds and not on an obstacle
    if (newPos.first >= 0 && newPos.first < GRID_SIZE && newPos.second >= 0 && newPos.second < GRID_SIZE && obstacles.find(newPos) == obstacles.end()) {
        // Ensure the new position is adjacent to the previous and next positions in the path
        if (pathIdx > 0 && newPos != agents[agentIdx].path[pathIdx - 1]) return;
        if (pathIdx < agents[agentIdx].path.size() - 1 && newPos != agents[agentIdx].path[pathIdx + 1]) return;

        // Check for collision with other agents
        for (const auto& agent : agents) {
            if (agentIdx != agent.id) {
                if (!agent.path.empty() && newPos == agent.goal && agent.path.back() == agent.goal) return; // Do not move into the position of an agent's goal if agent is there
                for (const auto& pos : agent.path) {
                    if (newPos == pos) return;
                }
            }
        }
        // If all constraints are satisfied, update the position
        agents[agentIdx].path[pathIdx] = newPos;
    }
}

vector<string> simulatedAnnealing(vector<Agent>& agents, const unordered_set<pair<int, int>, pair_hash>& obstacles, double initialTemp, double coolingRate, int maxIterations) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> agentDist(0, (int) agents.size() - 1);
    uniform_int_distribution<> directionDist(0, 3);
    uniform_real_distribution<> acceptanceDist(0.0, 1.0);

    double T = initialTemp;
    int currentCost = calculateCost(agents, obstacles);
    vector<Agent> bestSolution = agents;
    int bestCost = currentCost;

    unordered_set<pair<int, int>, pair_hash> goals;
    for (const auto& agent : agents) {
        goals.insert(agent.goal);
    }

    vector<string> iterationLogs;

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

        // Collect iteration logs
        iterationLogs.push_back("Iteration " + to_string(iter) + ", Temperature: " + to_string(T) + 
                                ", Current Cost: " + to_string(currentCost) + 
                                ", Best Cost: " + to_string(bestCost) + 
                                ", New Cost: " + to_string(newCost));
    }

    agents = bestSolution;
    return iterationLogs;
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

    // Open the file in append mode
    std::string filename = "./logs/output_7.txt";
    std::ofstream outFile(filename, std::ios_base::app);

    if (!outFile) {
        std::cerr << "Unable to open file " << filename << std::endl;
        return bestConfig;
    }

    for (const auto& conf : configs) {
        auto start = std::chrono::high_resolution_clock::now();
        vector<string> iterationLogs = simulatedAnnealing(agents, obstacles, conf.initialTemp, conf.coolingRate, conf.maxIterations);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        int currentCost = calculateCost(agents, obstacles);
        int makespan = 0;
        int sumOfCosts = 0;
        for (const auto& agent : agents) {
            makespan = std::max(makespan, static_cast<int>(agent.path.size()));
            sumOfCosts += static_cast<int>(agent.path.size());
        }

        // Write iteration logs to the file
        for (const auto& log : iterationLogs) {
            outFile << log << std::endl;
        }

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

    outFile.close();

    std::cout << "Best Config: T=" << bestConfig.initialTemp << ", CR=" << bestConfig.coolingRate << ", Iter=" << bestConfig.maxIterations
              << " | Cost: " << bestCost << ", Makespan: " << bestMakespan << ", Sum of Costs: " << bestSumOfCosts << ", Duration: " << bestDuration << "ms" << std::endl;

    return bestConfig;
}
