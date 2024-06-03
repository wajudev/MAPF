//
// Created by Lanre Ajibua on 26.05.24.
//

#include "../include/Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>

void loadFromFile(const std::string &filename, int &gridSize, std::unordered_set<std::pair<int, int>, pair_hash> &obstacles,
             std::vector<Agent> &agents) {
    std::ifstream inputFile(filename);
    if (!inputFile) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        exit(1);
    }

    std::string line;
    int numObstacles, numAgents;

    // Read grid size
    std::getline(inputFile, line);
    gridSize = std::stoi(line);
    std::cout << "Grid Size: " << gridSize << std::endl; // Debug

    // Read number of obstacles
    std::getline(inputFile, line);
    numObstacles = std::stoi(line);
    std::cout << "Number of Obstacles: " << numObstacles << std::endl; // Debug

    // Read obstacles
    for (int i = 0; i < numObstacles; ++i) {
        getline(inputFile, line);
        std::istringstream iss(line);
        int x, y;
        iss >> x >> y;
        obstacles.insert({x, y});
        std::cout << "Obstacle: (" << x << ", " << y << ")" << std::endl; // Debug
    }

    // Read number of agents
    getline(inputFile, line);
    numAgents = std::stoi(line);
    std::cout << "Number of Agents: " << numAgents << std::endl; // Debug

    // Read agents
    std::vector<sf::Color> colors = {
            sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow,
            sf::Color::Cyan, sf::Color::Magenta, sf::Color::White, sf::Color::Black,
            sf::Color(255, 165, 0), // Orange
            sf::Color(128, 0, 128)  // Purple
    };

    for (int i = 0; i < numAgents; ++i) {
        getline(inputFile, line);
        std::istringstream iss(line);
        int agentId, startX, startY, goalX, goalY;
        iss >> agentId >> startX >> startY >> goalX >> goalY;
        std::cout << "Agent " << agentId << " Start: (" << startX << ", " << startY << "), Goal: (" << goalX << ", " << goalY << ")" << std::endl; // Debug

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
        agents.emplace_back(Agent{agentId, {startX, startY}, {goalX, goalY}, {}, colors[i % colors.size()], shape});
    }
}

std::unordered_map<std::string, std::string> readConfigFile(const std::string& filename) {
    std::unordered_map<std::string, std::string> config;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Unable to open config file: " << filename << std::endl;
        return config;
    }

    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string key;
        if (std::getline(lineStream, key, '=')) {
            std::string value;
            if (std::getline(lineStream, value)) {
                config[key] = value;
            }
        }
    }
    file.close();
    return config;
}

void resetAgents(std::vector<Agent>& agents) {
    for (auto& agent : agents) {
        agent.path.clear();
        agent.cameFrom.clear();
        agent.gScore.clear();
        agent.fScore.clear();
        while (!agent.openSet.empty()) agent.openSet.pop();
    }
}