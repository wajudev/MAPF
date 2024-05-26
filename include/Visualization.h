//
// Created by Lanre Ajibua on 26.05.24.
//

#ifndef MAPF_VISUALIZATION_H
#define MAPF_VISUALIZATION_H

#include "Agent.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <utility>

void simulateAgents(const std::vector<Agent>& agents, sf::RenderWindow& window);

#endif //MAPF_VISUALIZATION_H
