#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

const int SIZE = 10;  // You can adjust the size of the maze

enum CellType {
    WALL,
    PATH,
    START,
    END,
    CHECKPOINT
};

struct Cell {
    CellType type;
};

class Maze {
public:
    Maze(int size);
    void display();
    sf::RenderWindow window;

private:
    std::vector<std::vector<Cell>> cells;
};

Maze::Maze(int size) {
    // Initialize the maze with all paths
    cells = std::vector<std::vector<Cell>>(size, std::vector<Cell>(size, {PATH}));

    // Set random seed
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Set start and end points
    cells[0][0].type = START;
    cells[size - 1][size - 1].type = END;

    // Set some random walls
    for (int i = 0; i < size * size / 3; ++i) {
        int x = std::rand() % size;
        int y = std::rand() % size;
        cells[x][y].type = WALL;
    }

    // Set some random checkpoints
    for (int i = 0; i < size / 2; ++i) {
        int x = std::rand() % size;
        int y = std::rand() % size;
        cells[x][y].type = CHECKPOINT;
    }

    // SFML window setup
    window.create(sf::VideoMode(1500, 1500), "Maze Visualization");
}

void Maze::display() {
    window.clear(sf::Color(200, 200, 200)); // Set a light gray background

    // Adjust the cell size based on the window size and maze size
    float cellSize = static_cast<float>(window.getSize().x) / cells.size();

    for (int i = 0; i < cells.size(); ++i) {
        for (int j = 0; j < cells[i].size(); ++j) {
            sf::RectangleShape cellRect(sf::Vector2f(cellSize, cellSize));
            cellRect.setPosition(i * cellSize, j * cellSize);

            // Set color based on cell type
            switch (cells[i][j].type) {
                case WALL:
                    // Solid color for walls
                    cellRect.setFillColor(sf::Color(100, 100, 100));
                    break;
                case PATH:
                    // Gradient for the path cells
                    cellRect.setFillColor(sf::Color(255, 255, 255, 150 + 50 * i / cells.size()));
                    break;
                case START:
                    cellRect.setFillColor(sf::Color::Green);
                    break;
                case END:
                    cellRect.setFillColor(sf::Color::Red);
                    break;
                case CHECKPOINT:
                    // Different colors for checkpoints
                    cellRect.setFillColor(sf::Color::Blue);
                    break;
            }

            // Draws black border around each cell
            cellRect.setOutlineColor(sf::Color::Black);
            cellRect.setOutlineThickness(1.0f);

            // Adds a shadow effect
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

    window.display();
}

int main() {
    Maze maze(SIZE);

    while (maze.window.isOpen()) {
        sf::Event event;
        while (maze.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                maze.window.close();
            }
        }

        maze.display();
    }

    return 0;
}
