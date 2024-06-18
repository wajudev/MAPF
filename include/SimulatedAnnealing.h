//
// Created by Lanre Ajibua on 26.05.24.
//

#ifndef MAPF_SIMULATEDANNEALING_H
#define MAPF_SIMULATEDANNEALING_H

#include "Agent.h"
#include <vector>
#include <unordered_map>
#include <utility>
#include <random>

struct SAConfig {
    double initialTemp;
    double coolingRate;
    int maxIterations;
};


SAConfig gridSearchSAConfigs(std::vector<Agent>& agents, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles, const std::unordered_map<std::string, std::string>& config);
void generateNeighbor(std::vector<Agent>& agents, std::mt19937& gen, std::uniform_int_distribution<>& agentDist, std::uniform_int_distribution<>& directionDist);
std::vector<std::string> simulatedAnnealing(std::vector<Agent>& agents, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles, double initialTemp, double coolingRate, int maxIterations);
int calculateCost(const std::vector<Agent>& agents, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles);

#endif //MAPF_SIMULATEDANNEALING_H
