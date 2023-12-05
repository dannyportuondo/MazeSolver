#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <queue>

// 320 x 320 maze -> 102,400 cells
const int SIZE = 320;

enum CellType {
    WALL,
    PATH,
    START,
    END,
};

struct Cell {
    CellType type;
    bool isPartOfPath; // Indicates whether this cell is part of the shortest path
    int distance;      // Distance from the start node
    int prevX, prevY;  // Coordinates of the previous node in the path
    int heuristic;     // Heuristic value for A* Search

};

class Maze {
public:
    Maze(int size);
    void display();
    void findShortestPathDijkstra();
    void findShortestPathAStar();

    sf::RenderWindow window;

private:
    std::vector<std::vector<Cell>> cells;
};

Maze::Maze(int size) {
    // Initialize the maze with all paths
    cells = std::vector<std::vector<Cell>>(size, std::vector<Cell>(size, {PATH, false, INT_MAX, -1, -1}));

    // Set random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));


    // Set some random walls
    for (int i = 0; i < size * size / 3; ++i) {
        int x = std::rand() % size;
        int y = std::rand() % size;
        cells[x][y].type = WALL;
    }

    // Set start and end points
    cells[0][0].type = START;
    cells[size - 1][size - 1].type = END;

    // SFML window setup
    window.create(sf::VideoMode(1500, 1500), "Maze Visualization");

    // Set distance for start node to 0
    cells[0][0].distance = 0;
}

// Helper structure to represent a node in the maze for Dijkstra's algorithm
struct DijkstraNode {
    int x, y; // Coordinates of the node
    int distance; // Distance from the start node
    bool operator>(const DijkstraNode& other) const {
        return distance > other.distance;
    }
};

void Maze::findShortestPathDijkstra() {
    // Initialize isPartOfPath for all cells to false
    for (auto& row : cells) {
        for (auto& cell : row) {
            cell.isPartOfPath = false;
        }
    }

    // Dijkstra's algorithm
    std::priority_queue<DijkstraNode, std::vector<DijkstraNode>, std::greater<DijkstraNode>> pq;
    pq.push({0, 0, 0});

    while (!pq.empty()) {
        auto currentNode = pq.top();
        pq.pop();

        int x = currentNode.x;
        int y = currentNode.y;
        int distance = currentNode.distance;

        // Check if this node has already been processed
        if (cells[x][y].isPartOfPath) {
            continue;
        }

        // Mark the node as part of the shortest path
        cells[x][y].isPartOfPath = true;

        // Check if we reached the end
        if (x == cells.size() - 1 && y == cells[0].size() - 1) {
            break; // Found the shortest path
        }

        // Add adjacent nodes to the priority queue
        const std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
        for (const auto& dir : directions) {
            int newX = x + dir.first;
            int newY = y + dir.second;

            // Check if the new position is within bounds and not a wall
            if (newX >= 0 && newX < cells.size() && newY >= 0 && newY < cells[0].size() && cells[newX][newY].type != WALL) {
                int newDistance = distance + 1;

                // If the new distance is shorter, update the distance and add to the priority queue
                if (newDistance < cells[newX][newY].distance) {
                    cells[newX][newY].distance = newDistance;
                    pq.push({newX, newY, newDistance});

                    // Store the previous node for backtracking
                    cells[newX][newY].prevX = x;
                    cells[newX][newY].prevY = y;
                }
            }
        }
    }
}

// Helper structure to represent a node in the maze for A* Search's algorithm
struct AStarNode {
    int x, y; // Coordinates of the node
    int distance; // Distance from the start node
    int heuristic; // Heuristic value for A* Search
    bool operator>(const AStarNode& other) const {
        return (distance + heuristic) > (other.distance + other.heuristic);
    }
};

void Maze::findShortestPathAStar() {
    // Initialize isPartOfPath for all cells to false
    for (auto& row : cells) {
        for (auto& cell : row) {
            cell.isPartOfPath = false;
        }
    }

    // A* Search algorithm
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> pq;
    pq.push({0, 0, 0, cells[0][0].heuristic});

    while (!pq.empty()) {
        auto currentNode = pq.top();
        pq.pop();

        int x = currentNode.x;
        int y = currentNode.y;
        int distance = currentNode.distance;

        // Check if this node has already been processed
        if (cells[x][y].isPartOfPath) {
            continue;
        }

        // Mark the node as part of the shortest path
        cells[x][y].isPartOfPath = true;

        // Check if we reached the end
        if (x == cells.size() - 1 && y == cells[0].size() - 1) {
            break; // Found the shortest path
        }

        // Add adjacent nodes to the priority queue
        const std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
        for (const auto& dir : directions) {
            int newX = x + dir.first;
            int newY = y + dir.second;

            // Check if the new position is within bounds and not a wall
            if (newX >= 0 && newX < cells.size() && newY >= 0 && newY < cells[0].size() && cells[newX][newY].type != WALL) {
                int newDistance = distance + 1;

                // If the new distance is shorter, update the distance and add to the priority queue
                if (newDistance < cells[newX][newY].distance) {
                    cells[newX][newY].distance = newDistance;
                    pq.push({newX, newY, newDistance, std::abs(int(newX - (cells.size() - 1))) + std::abs(int(newY - (cells[0].size() - 1)))});

                    // Store the previous node for backtracking
                    cells[newX][newY].prevX = x;
                    cells[newX][newY].prevY = y;
                }
            }
        }
    }
}


void Maze::display() {
    window.clear(sf::Color(200, 200, 200)); // Set a light gray background

    // Adjust the cell size based on the window size and maze size
    float cellSize = static_cast<float>(window.getSize().x) / cells.size();

    // loop through cells
    for (int i = 0; i < cells.size(); ++i) {
        for (int j = 0; j < cells[i].size(); ++j) {
            sf::RectangleShape cellRect(sf::Vector2f(cellSize, cellSize));
            cellRect.setPosition(i * cellSize, j * cellSize);

            // Set color based on cell type
            switch (cells[i][j].type) {
                case WALL:
                    // Solid color for walls - dark grey
                    cellRect.setFillColor(sf::Color(100, 100, 100));
                    break;
                case PATH:
                    // Gradient for the path cells - light grey
                    cellRect.setFillColor(sf::Color(255, 255, 255, 150 + 50 * i / cells.size()));
                    break;
                case START:
                    // top left (0,0) - green
                    cellRect.setFillColor(sf::Color::Green);
                    break;
                case END:
                    // bottom right (size-1, size-1) - red
                    cellRect.setFillColor(sf::Color::Red);
                    break;

            }

            // Draw black border around each cell
            cellRect.setOutlineColor(sf::Color::Black);
            cellRect.setOutlineThickness(1.0f);

            // Add a shadow effect
            sf::ConvexShape shadow(4);
            shadow.setPoint(0, sf::Vector2f(i * cellSize, j * cellSize + cellSize));
            shadow.setPoint(1, sf::Vector2f(i * cellSize + cellSize, j * cellSize + cellSize));
            shadow.setPoint(2, sf::Vector2f(i * cellSize + cellSize, j * cellSize + cellSize + 10));
            shadow.setPoint(3, sf::Vector2f(i * cellSize, j * cellSize + cellSize + 10));
            shadow.setFillColor(sf::Color(0, 0, 0, 100));
            window.draw(shadow);

            window.draw(cellRect);
        }
    }

    // Draw the shortest path using Dijkstra algorithm - BLUE
    sf::VertexArray dijkstraPath(sf::LinesStrip);
    int currentX = cells.size() - 1;
    int currentY = cells[0].size() - 1;

    while (currentX != -1 || currentY != -1) {
        dijkstraPath.append(sf::Vertex(sf::Vector2f(currentX * cellSize + cellSize / 2, currentY * cellSize + cellSize / 2), sf::Color::Blue, sf::Vector2f(2.0f, 2.0f)));

        int tempX = cells[currentX][currentY].prevX;
        currentY = cells[currentX][currentY].prevY;
        currentX = tempX;
    }
    // draw blue line
    window.draw(dijkstraPath, sf::BlendAlpha);


    // draw the shortest path using A* Search algorithm - RED
    findShortestPathAStar();

    sf::VertexArray aStarPath(sf::LinesStrip);
    int aStarCurrentX = cells.size() - 1;
    int aStarCurrentY = cells[0].size() - 1;

    while (aStarCurrentX != -1 || aStarCurrentY != -1) {
        aStarPath.append(sf::Vertex(sf::Vector2f(aStarCurrentX * cellSize + cellSize / 2, aStarCurrentY * cellSize + cellSize / 2), sf::Color::Red, sf::Vector2f(2.0f, 2.0f)));

        int aStarTempX = cells[aStarCurrentX][aStarCurrentY].prevX;
        aStarCurrentY = cells[aStarCurrentX][aStarCurrentY].prevY;
        aStarCurrentX = aStarTempX;
    }
    // draw red line
    window.draw(aStarPath, sf::BlendAlpha);

    // display window
    window.display();
}


int main() {
    Maze maze(SIZE);

    // Find and display the correct shortest path
    maze.findShortestPathDijkstra();

    while (maze.window.isOpen()) {
        sf::Event event;
        while (maze.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                maze.window.close();
            }
        }

        // display maze with algorithm paths
        // Dijkstra algorithm - blue
        // A* Search algorithm - red
        maze.display();
    }

    return 0;
}
