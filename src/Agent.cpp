#include "../include/Agent.h"
#include "../include/Constants.h"
#include <iostream>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

vector<Agent> initializeAgents(int numAgents) {
    vector<sf::Color> colors = {
            sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow,
            sf::Color::Cyan, sf::Color::Magenta, sf::Color::White, sf::Color::Black,
            sf::Color(255, 165, 0), // Orange
            sf::Color(128, 0, 128)  // Purple
    };

    vector<Agent> agents;
    for (int i = 0; i < numAgents; ++i) {
        int startX, startY, goalX, goalY;
        cout << "Enter start (x y) for agent " << i << ": ";
        cin >> startX >> startY;
        cout << "Enter goal (x y) for agent " << i << ": ";
        cin >> goalX >> goalY;

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
        agents.push_back(Agent{ i, {startX, startY}, {goalX, goalY}, {}, colors[i % colors.size()], shape });
    }
    return agents;
}

vector<pair<int, int>> getNeighbors(pair<int, int> node) {
    vector<pair<int, int>> neighbors;
    if (node.first > 0) neighbors.emplace_back(node.first - 1, node.second);
    if (node.first < GRID_SIZE - 1) neighbors.emplace_back(node.first + 1, node.second);
    if (node.second > 0) neighbors.emplace_back(node.first, node.second - 1);
    if (node.second < GRID_SIZE - 1) neighbors.emplace_back(node.first, node.second + 1);
    return neighbors;
}

void reconstructPath(Agent& agent, pair<int, int> current) {
    while (current != agent.start) {
        agent.path.push_back(current);
        current = agent.cameFrom[current];
    }
    reverse(agent.path.begin(), agent.path.end());
}

void aStar(Agent& agent) {
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

        for (auto neighbor : getNeighbors(current)) {
            int tentative_gScore = agent.gScore[current] + 1;

            if (agent.gScore.find(neighbor) == agent.gScore.end() || tentative_gScore < agent.gScore[neighbor]) {
                agent.cameFrom[neighbor] = current;
                agent.gScore[neighbor] = tentative_gScore;
                agent.fScore[neighbor] = tentative_gScore + manhattanDistance(neighbor, agent.goal);
                agent.openSet.emplace(agent.fScore[neighbor], neighbor);
            }
        }
    }
}

void initializePaths(vector<Agent>& agents) {
    for (auto& agent : agents) {
        agent.path.clear();
        agent.cameFrom.clear();
        agent.gScore.clear();
        agent.fScore.clear();
        while (!agent.openSet.empty()) agent.openSet.pop();
        aStar(agent);
    }
}

int manhattanDistance(pair<int, int> a, pair<int, int> b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}
