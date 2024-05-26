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

void generateNeighbor(std::vector<Agent>& agents, std::mt19937& gen, std::uniform_int_distribution<>& agentDist, std::uniform_int_distribution<>& directionDist);
void simulatedAnnealing(std::vector<Agent>& agents);
int calculateCost(const std::vector<Agent>& agents);

#endif //MAPF_SIMULATEDANNEALING_H
