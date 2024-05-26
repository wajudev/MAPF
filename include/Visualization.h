//
// Created by Lanre Ajibua on 26.05.24.
//

#ifndef MAPF_VISUALIZATION_H
#define MAPF_VISUALIZATION_H

#include "Agent.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>
#include <unordered_set>
#include <utility>

void simulateAgents(const std::vector<Agent>& agents, sf::RenderWindow& window, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles);


#endif //MAPF_VISUALIZATION_H
