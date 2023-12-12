#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace sf;

class Cell {
private:
    bool isMine;
    bool isRevealed;
    bool isMarked;

public:
    Cell() : isMine(false), isRevealed(false), isMarked(false) {}

    void reveal() {
        isRevealed = true;
    }

    void mark() {
        isMarked = true;
    }

    bool getRevealed() const {
        return isRevealed;
    }

    bool getMarked() const {
        return isMarked;
    }

    bool getIsMine() const {
        return isMine;
    }

    bool changeIsMine(bool status) {
        isMine = status;
        return isMine;
    }
};

class Grid {
private:
    int rows;
    int columns;
    vector<vector<Cell>> cells;

public:
    Grid(int rows, int columns) : rows(rows), columns(columns) {
        initializeGrid();
    }

    void initializeGrid() {
        cells.resize(rows, vector<Cell>(columns));

        // Seed the random number generator
        srand(static_cast<unsigned>(time(nullptr)));

        int mineCount = 10;
        while (mineCount > 0) {
            int x = rand() % rows;
            int y = rand() % columns;

            if (!cells[x][y].getIsMine()) {
                cells[x][y].changeIsMine(true);
                mineCount--;
            }
        }
    }

    int countAdjacentBombs(int x, int y) const {
        int count = 0;

        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int newX = x + i;
                int newY = y + j;

                if (newX >= 0 && newX < rows && newY >= 0 && newY < columns && cells[newX][newY].getIsMine()) {
                    ++count;
                }
            }
        }

        return count;
    }

    void printGrid() const {
        for (size_t i = 0; i < cells.size(); ++i) {
            for (size_t j = 0; j < cells[i].size(); ++j) {
                if (cells[i][j].getIsMine()) {
                    cout << "B ";
                } else {
                    int adjacentBombs = countAdjacentBombs(i, j);
                    cout << (adjacentBombs > 0 ? to_string(adjacentBombs) : "▢") << " ";
                }
            }
            cout << endl;
        }
    }

    bool revealCell(int x, int y) {
        if (x >= 0 && x < rows && y >= 0 && y < columns) {
            cells[x][y].reveal();
            return true;
        }
        return false;
    }

    void markCell(int x, int y) {
        if (x >= 0 && x < rows && y >= 0 && y < columns) {
            cells[x][y].mark();
        }
    }

    const vector<vector<Cell>>& getCells() const {
        return cells;
    }

    int getColumns() const {
        return columns;
    }
};

class Minesweeper {
private:
    int rows;
    int columns;
    Grid mineGrid;
    Grid normalGrid;

public:
    Minesweeper(int rows, int columns) : rows(rows), columns(columns), mineGrid(rows, columns), normalGrid(rows, columns) {}

    void StartGame() {
        // Initialize or reset the game state
    }

    void Click(int x, int y) {
        // Process the click event, reveal cells, check for win/lose conditions
        normalGrid.revealCell(x, y);
        // ...
    }

    void markCell(int x, int y) {
        // Mark a cell with a flag
        normalGrid.markCell(x, y);
    }

    void Defeat() {
        // Handle defeat condition
    }

    void Win() {
        // Handle win condition
    }

    bool isGameOver() {
        // Check if the game is over (win or lose)
        return false;
    }

    const Grid& getNormalGrid() const {
        return normalGrid;
    }
};

class Render {
private:
    Texture revealedTexture;
    Texture markedTexture;
    Texture unmarkedTexture;

public:
    Render(int rows, int columns) : window(VideoMode(columns * 60, rows * 72), "Minesweeper") {
        revealedTexture.loadFromFile("Sprites/Check.png");
        markedTexture.loadFromFile("/Users/zakerden1234/Desktop/Minesweeper/Sprites/Flag.png");
        unmarkedTexture.loadFromFile("/Users/zakerden1234/Desktop/Minesweeper/Sprites/Cell.png");
    }

    void renderWindow(const Minesweeper& game) {
        window.clear(Color(192, 192, 192));

        const Grid& grid = game.getNormalGrid();
        const auto& cells = grid.getCells();

        float cellSize = 60.0f;
        float topMargin = 95.0f;

        for (size_t i = 0; i < cells.size(); ++i) {
            for (size_t j = 0; j < cells[i].size(); ++j) {
                Sprite cellSprite;

                if (cells[i][j].getRevealed()) {
                    cellSprite.setTexture(revealedTexture);
                } else if (cells[i][j].getMarked()) {
                    cellSprite.setTexture(markedTexture);
                } else {
                    cellSprite.setTexture(unmarkedTexture);
                }

                cellSprite.setPosition(i * cellSize, j * cellSize + topMargin);

                // Set the scale of the sprite to match the cell size
                cellSprite.setScale(cellSize / cellSprite.getLocalBounds().width, cellSize / cellSprite.getLocalBounds().height);

                window.draw(cellSprite);
            }
        }

        window.display();
    }

    void refresh() {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }
    }

    void WinText() {
        cout << "Congratulations! You won!" << endl;
    }

    void LoseText() {
        cout << "Game Over! You lost!" << endl;
    }

    RenderWindow window;
};

int main() {
    int rows = 8;
    int columns = 8;

    Render renderer(rows, columns);
    Minesweeper game(rows, columns);

    // Print the grid with bomb information
    cout << "Initial grid with bomb information:" << endl;
    game.getNormalGrid().printGrid();

    while (renderer.window.isOpen()) {
        renderer.refresh();
        renderer.renderWindow(game);
    }

    return 0;
}
