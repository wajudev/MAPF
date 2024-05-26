//
// Created by Lanre Ajibua on 26.05.24.
//

#ifndef MAPF_AGENT_H
#define MAPF_AGENT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include <queue>
#include <unordered_set>
#include <unordered_map>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

struct Agent {
    int id;
    std::pair<int, int> start;
    std::pair<int, int> goal;
    std::vector<std::pair<int, int>> path;
    sf::Color color;
    sf::Shape* shape;
    std::priority_queue<std::pair<int, std::pair<int, int>>,
            std::vector<std::pair<int, std::pair<int, int>>>,
            std::greater<>> openSet;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> cameFrom;
    std::unordered_map<std::pair<int, int>, int, pair_hash> gScore;
    std::unordered_map<std::pair<int, int>, int, pair_hash> fScore;
};

std::vector<Agent> initializeAgents(int numAgents, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles);
void initializePaths(std::vector<Agent>& agents, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles, int& makeSpan, int& sumOfCosts);
int manhattanDistance(std::pair<int, int> a, std::pair<int, int> b);
void aStar(Agent& agent, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles, const std::vector<Agent>& agents);
std::unordered_set<std::pair<int, int>, pair_hash> generateObstacles(int numObstacles);

#endif //MAPF_AGENT_H
