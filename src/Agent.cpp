#include "../include/Agent.h"
#include "../include/Constants.h"
#include <iostream>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>


std::vector<Agent> initializeAgents(int numAgents, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles) {
    std::vector<sf::Color> colors = {
            sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow,
            sf::Color::Cyan, sf::Color::Magenta, sf::Color::White, sf::Color::Black,
            sf::Color(255, 165, 0), // Orange
            sf::Color(128, 0, 128)  // Purple
    };

    std::vector<Agent> agents;
    for (int i = 0; i < numAgents; ++i) {
        int startX, startY, goalX, goalY;
        std::cout << "Enter start (x y) for agent " << i << ": ";
        std::cin >> startX >> startY;
        std::cout << "Enter goal (x y) for agent " << i << ": ";
        std::cin >> goalX >> goalY;

        // Ensure start and goal positions are not on obstacles
        while (obstacles.find({startX, startY}) != obstacles.end() || obstacles.find({goalX, goalY}) != obstacles.end()) {
            std::cout << "Start or goal position is on an obstacle. Please re-enter positions.\n";
            std::cout << "Enter start (x y) for agent " << i << ": ";
            std::cin >> startX >> startY;
            std::cout << "Enter goal (x y) for agent " << i << ": ";
            std::cin >> goalX >> goalY;
        }

        // Create a shape for the agent
        sf::Shape* shape;
        switch (i % 3) {
            case 0: // Circle
                shape = new sf::CircleShape(CELL_SIZE / 2 - 2);
                break;
            case 1: // Square
                shape = new sf::RectangleShape(sf::Vector2f(CELL_SIZE - 4, CELL_SIZE - 4));
                break;
            case 2: // Triangle
                shape = new sf::CircleShape(CELL_SIZE / 2 - 2, 3);
                break;
        }
        shape->setFillColor(colors[i % colors.size()]);
        agents.emplace_back(Agent{ i, {startX, startY}, {goalX, goalY}, {}, colors[i % colors.size()], shape });
    }
    return agents;
}

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
    agent.openSet.emplace(0, agent.start);
    agent.gScore[agent.start] = 0;
    agent.fScore[agent.start] = manhattanDistance(agent.start, agent.goal);

    while (!agent.openSet.empty()) {
        auto current = agent.openSet.top().second;
        agent.openSet.pop();

        if (current == agent.goal) {
            reconstructPath(agent, current);
            return;
        }

        for (auto neighbor : getNeighbors(current, obstacles)) {
            int tentative_gScore = agent.gScore[current] + 1;

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

        // Debug: Print the path for each agent
        std::cout << "Path for agent " << agent.id << ": ";
        for (const auto& step : agent.path) {
            std::cout << "(" << step.first << ", " << step.second << ") ";
        }
        std::cout << std::endl;
    }
}

int manhattanDistance(std::pair<int, int> a, std::pair<int, int> b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

std::unordered_set<std::pair<int, int>, pair_hash> generateObstacles(int numObstacles) {
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