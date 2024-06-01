# MAPF

# Multi-Agent Path Finding (MAPF) Visualizer

This project is a Multi-Agent Path Finding (MAPF) visualizer that computes collision-free paths for a team of agents. The visualization ensures that two agents will not collide if and only if, during the same time step, they both move to the same cell or both move to the current cell of the other agent. The objective is to minimize the makespan, which is the number of time steps required for all agents to reach their target. Optionally, it also considers minimizing the sum of costs, which is the sum of time steps required by each agent to reach its target.

## Features

- Computes collision-free paths for agents.
- Visualizes agent paths and obstacles.
- Supports dynamic obstacle avoidance.
- Displays makespan and sum of costs.

## Requirements

- C++11 or later
- SFML (Simple and Fast Multimedia Library)

## Installation

### MacOS

1. **Install SFML**:
    - Using Homebrew:
      ```sh
      brew install sfml
      ```

2. **Clone the Repository**:
    ```sh
    git clone https://github.com/wajudev/MAPF.git
    cd MAPF
    ```

3. **Build the Project**:
    - Compile the project using the provided Makefile:
      ```sh
      make clean
      make
      ```

4. **Run the Application**:
    ```sh
    ./mapf_visualizer <filename> [configfile]
    ```

### Windows

1. **Install SFML**:
    - Download SFML from the official [SFML website](https://www.sfml-dev.org/download.php).
    - Extract the files to a known location.
    - Set the environment variables to point to the SFML installation.

2. **Clone the Repository**:
    ```sh
    git clone https://github.com/wajudev/MAPF.git
    cd MAPF
    ```

3. **Build the Project**:
    - Ensure you have a C++ compiler installed (e.g., MinGW).
    - Update the `Makefile` if necessary to point to the correct SFML include and library directories.
    - Compile the project using the provided Makefile:
      ```sh
      make clean
      make 
      ```

4. **Run the Application**:
    ```sh
    ./mapf_visualizer.exe <filename> [configfile]
    ```

## Usage

