//
// Created by Lanre Ajibua on 26.05.24.
//

#ifndef MAPF_UTILS_H
#define MAPF_UTILS_H

#include <string>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "Constants.h"
#include "Agent.h"

void loadFromFile(const std::string& filename, int& gridSize, std::unordered_set<std::pair<int, int>, pair_hash>& obstacles, std::vector<Agent>& agents);
std::unordered_map<std::string, std::string> readConfigFile(const std::string& filename);
void resetAgents(std::vector<Agent>& agents);

#endif //MAPF_UTILS_H
