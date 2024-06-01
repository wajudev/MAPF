#include "../include/Agent.h"
#include "../include/Constants.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <random>

std::vector<std::pair<int, int>> getNeighbors(std::pair<int, int> node, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles) {
    std::vector<std::pair<int, int>> neighbors;
    if (node.first > 0 && obstacles.find({node.first - 1, node.second}) == obstacles.end()) neighbors.emplace_back(node.first - 1, node.second);
    if (node.first < GRID_SIZE - 1 && obstacles.find({node.first + 1, node.second}) == obstacles.end()) neighbors.emplace_back(node.first + 1, node.second);
    if (node.second > 0 && obstacles.find({node.first, node.second - 1}) == obstacles.end()) neighbors.emplace_back(node.first, node.second - 1);
    if (node.second < GRID_SIZE - 1 && obstacles.find({node.first, node.second + 1}) == obstacles.end()) neighbors.emplace_back(node.first, node.second + 1);
    return neighbors;
}

void reconstructPath(Agent& agent, std::pair<int, int> current) {
    std::vector<std::pair<int, int>> totalPath = {current};
    while (agent.cameFrom.find(current) != agent.cameFrom.end()) {
        current = agent.cameFrom[current];
        totalPath.push_back(current);
    }
    reverse(totalPath.begin(), totalPath.end());
    agent.path = totalPath;
}

void aStar(Agent& agent, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles, const std::vector<Agent>& agents) {
    std::cout << "Running A* for agent " << agent.id << " from (" << agent.start.first << ", " << agent.start.second << ") to (" << agent.goal.first << ", " << agent.goal.second << ")" << std::endl;
    agent.openSet.emplace(0, agent.start);
    agent.gScore[agent.start] = 0;
    agent.fScore[agent.start] = manhattanDistance(agent.start, agent.goal);

    while (!agent.openSet.empty()) {
        auto current = agent.openSet.top().second;
        agent.openSet.pop();

        if (current == agent.goal) {
            reconstructPath(agent, current);
            std::cout << "Path found for agent " << agent.id << ": ";
            for (const auto& step : agent.path) {
                std::cout << "(" << step.first << ", " << step.second << ") ";
            }
            std::cout << std::endl;
            return;
        }

        for (auto neighbor : getNeighbors(current, obstacles)) {
            size_t tentative_gScore = agent.gScore[current] + 1;

            // Check for collisions with other agents
            bool collision = false;
            for (const auto& otherAgent : agents) {
                if (otherAgent.id != agent.id) {
                    if (!otherAgent.path.empty() && otherAgent.path.size() > tentative_gScore) {
                        if (neighbor == otherAgent.path[tentative_gScore] ||
                            (tentative_gScore > 0 && neighbor == otherAgent.path[tentative_gScore - 1] && current == otherAgent.path[tentative_gScore])) {
                            collision = true;
                            break;
                        }
                    }
                }
            }

            if (collision) continue;


            if (agent.gScore.find(neighbor) == agent.gScore.end() || tentative_gScore < agent.gScore[neighbor]) {
                agent.cameFrom[neighbor] = current;
                agent.gScore[neighbor] = tentative_gScore;
                agent.fScore[neighbor] = tentative_gScore + manhattanDistance(neighbor, agent.goal);
                agent.openSet.emplace(agent.fScore[neighbor], neighbor);
            }
        }
    }
    std::cout << "No path found for agent " << agent.id << std::endl;
}

void initializePaths(std::vector<Agent>& agents, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles, int& makespan, int& sumOfCosts) {
    makespan = 0;
    sumOfCosts = 0;
    for (auto& agent : agents) {
        agent.path.clear();
        agent.cameFrom.clear();
        agent.gScore.clear();
        agent.fScore.clear();
        while (!agent.openSet.empty()) agent.openSet.pop();
        aStar(agent, obstacles, agents);
        makespan = std::max(makespan, (int) agent.path.size());
        sumOfCosts += (int) agent.path.size();
    }
}

int manhattanDistance(std::pair<int, int> a, std::pair<int, int> b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

std::unordered_set<std::pair<int, int>, pair_hash> generateObstacles(size_t numObstacles) {
    std::unordered_set<std::pair<int, int>, pair_hash> obstacles;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, GRID_SIZE - 1);

    while (obstacles.size() < numObstacles){
        int x = dis(gen);
        int y = dis(gen);
        obstacles.insert({x, y});
    }

    return obstacles;
}