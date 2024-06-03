//
// Created by Lanre Ajibua on 26.05.24.
//

#include "../include/Visualization.h"
#include "../include/Constants.h"
#include "../include/SimulatedAnnealing.h"
#include <thread>
#include <iostream>

std::string FONT_COURIER = "resources/Courier_New.ttf";

void simulateAgents(const std::vector<Agent>& agents, sf::RenderWindow& window, const std::unordered_set<std::pair<int, int>, pair_hash>& obstacles, const SAConfig& bestConfig, int bestMakespan, int bestCost) {
    size_t maxSteps = 0;
    for (const auto& agent : agents) {
        if (agent.path.size() > maxSteps) {
            maxSteps = agent.path.size();
        }
    }

    sf::Font font;
    if(!font.loadFromFile(FONT_COURIER)) {
        std::cerr << "Error loading fonts" << std::endl;
        return;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::Black);

    while (window.isOpen()) {
        for (size_t t = 0; t <= maxSteps; ++t) {
            window.clear(sf::Color::White);

            std::unordered_map<std::pair<int, int>, int, pair_hash> positionCount;
            for (const auto& agent : agents) {
                if (t < agent.path.size()) {
                    positionCount[agent.path[t]]++;
                }
            }

            // Draw the grid and obstacles
            for (int i = 0; i < GRID_SIZE; ++i) {
                for (int j = 0; j < GRID_SIZE; ++j) {
                    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
                    cell.setPosition(i * CELL_SIZE, j * CELL_SIZE);
                    if (obstacles.find({i, j}) != obstacles.end()) {
                        cell.setFillColor(sf::Color::Black);
                    } else {
                        cell.setFillColor(sf::Color::White);
                    }
                    cell.setOutlineThickness(1);
                    cell.setOutlineColor(sf::Color::Black);
                    window.draw(cell);
                }
            }

            // Draw agents
            for (const auto& agent : agents) {
                if (t < agent.path.size()) {
                    agent.shape->setPosition(agent.path[t].first * CELL_SIZE + 1, agent.path[t].second * CELL_SIZE + 1);
                } else {
                    // Draw agents at their final position if t exceeds path size
                    agent.shape->setPosition(agent.path.back().first * CELL_SIZE + 1, agent.path.back().second * CELL_SIZE + 1);
                }
                window.draw(*agent.shape);
            }

            // Check for collisions and highlight
            for (const auto& entry : positionCount) {
                if (entry.second > 1) {
                    sf::RectangleShape collisionCell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
                    collisionCell.setPosition(entry.first.first * CELL_SIZE, entry.first.second * CELL_SIZE);
                    collisionCell.setFillColor(sf::Color::Red);
                    window.draw(collisionCell);
                }
            }

            // Display best configuration details
            std::ostringstream ss;
            ss << "Best Config: T=" << bestConfig.initialTemp << ", CR=" << bestConfig.coolingRate << ", Iter=" << bestConfig.maxIterations
               << "\n Best Makespan: " << bestMakespan << ", Best Cost: " << bestCost;

            text.setString(ss.str());
            text.setPosition(10, GRID_SIZE * CELL_SIZE + 10);
            window.draw(text);

            window.display();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Slow down the simulation for visualization purposes

            // Handle events to keep the window open
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            if (!window.isOpen()) {
                break;
            }
        }
    }
}
