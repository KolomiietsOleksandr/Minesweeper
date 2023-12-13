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
    bool isExploded;

public:
    Cell() : isMine(false), isRevealed(false), isMarked(false), isExploded(false) {}

    void reveal() {
        isRevealed = true;
    }

    void explode() {
        isExploded = true;
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

    bool getIsExploded() const {
        return isExploded;
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

    void printGrid() const {
        for (size_t i = 0; i < cells.size(); ++i) {
            for (size_t j = 0; j < cells[i].size(); ++j) {
                cout << (cells[j][i].getIsMine() ? "B" : to_string(countAdjacentBombs(j, i))) << " ";
            }
            cout << endl;
        }
    }

    void revealAllBombs() {
        for (size_t i = 0; i < cells.size(); ++i) {
            for (size_t j = 0; j < cells[i].size(); ++j) {
                if (cells[i][j].getIsMine()) {
                    cells[i][j].reveal();
                }
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
    bool gameOver;

public:
    Minesweeper(int rows, int columns) : rows(rows), columns(columns), mineGrid(rows, columns), normalGrid(rows, columns), gameOver(false) {}

    void StartGame() {
        gameOver = false;
    }

    void Click(int x, int y) {
        if (gameOver) {
            return;
        }

        Cell& clickedCell = const_cast<Cell &>(normalGrid.getCells()[x][y]);

        if (!clickedCell.getRevealed() && !clickedCell.getMarked()) {
            clickedCell.reveal();

            if (clickedCell.getIsMine()) {
                clickedCell.explode();
                Defeat();
            } else {
                bool allNonMineCellsRevealed = true;
                for (size_t i = 0; i < normalGrid.getCells().size(); ++i) {
                    for (size_t j = 0; j < normalGrid.getCells()[i].size(); ++j) {
                        if (!normalGrid.getCells()[i][j].getIsMine() && !normalGrid.getCells()[i][j].getRevealed()) {
                            allNonMineCellsRevealed = false;
                            break;
                        }
                    }
                }
                if (allNonMineCellsRevealed) {
                    Win();
                }
            }
        }
    }


    void markCell(int x, int y) {
        // Mark a cell with a flag
        normalGrid.markCell(x, y);
    }

    void Defeat() {
        // Handle defeat condition
        normalGrid.revealAllBombs();
        gameOver == false;
    }

    void Win() {
        // Handle win condition
        gameOver = true;
    }

    bool isGameOver() const {
        // Check if the game is over (win or lose)
        return gameOver;
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
    Texture emptyTexture;
    Texture bombTexture;
    Texture bombBoomTexture;
    vector<Texture> numberTextures;  // Текстури для чисел від 1 до 8

    Minesweeper game;

public:
    Render(int rows, int columns, Minesweeper& game) : game(game), window(VideoMode(columns * 60, rows * 72), "Minesweeper") {
        emptyTexture.loadFromFile("/Users/zakerden1234/Desktop/Minesweeper/Sprites/0.png");
        revealedTexture.loadFromFile("/Users/zakerden1234/Desktop/Minesweeper/Sprites/Check.png");
        markedTexture.loadFromFile("/Users/zakerden1234/Desktop/Minesweeper/Sprites/Flag.png");
        unmarkedTexture.loadFromFile("/Users/zakerden1234/Desktop/Minesweeper/Sprites/Cell.png");
        bombTexture.loadFromFile("/Users/zakerden1234/Desktop/Minesweeper/Sprites/Bomb.png");
        bombBoomTexture.loadFromFile("/Users/zakerden1234/Desktop/Minesweeper/Sprites/BombLose.png");

        // Завантаження текстур для чисел від 1 до 8
        for (int i = 1; i <= 8; ++i) {
            numberTextures.push_back(Texture());
            numberTextures.back().loadFromFile("/Users/zakerden1234/Desktop/Minesweeper/Sprites/" + to_string(i) + ".png");
        }
    }

    void renderWindow() {
        window.clear(Color(192, 192, 192));

        const Grid& grid = game.getNormalGrid();
        const auto& cells = grid.getCells();

        float cellSize = 60.0f;
        float topMargin = 95.0f;

        for (size_t i = 0; i < cells.size(); ++i) {
            for (size_t j = 0; j < cells[i].size(); ++j) {
                Sprite cellSprite;

                if (cells[i][j].getRevealed()) {
                    if (cells[i][j].getIsMine()) {
                        if (cells[i][j].getIsExploded()) {
                            cellSprite.setTexture(bombBoomTexture);
                        } else {
                            cellSprite.setTexture(bombTexture);
                        }
                    } else {
                        int adjacentBombs = grid.countAdjacentBombs(i, j);
                        if (adjacentBombs == 0) {
                            cellSprite.setTexture(emptyTexture);
                        } else {
                            cellSprite.setTexture(numberTextures[adjacentBombs - 1]);
                        }
                    }
                } else if (cells[i][j].getMarked()) {
                    cellSprite.setTexture(markedTexture);
                } else {
                    cellSprite.setTexture(unmarkedTexture);
                }

                cellSprite.setPosition(i * cellSize, j * cellSize + topMargin);

                // Встановити масштаб спрайта для відповідності розміру клітинки
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
            else if (event.type == Event::MouseButtonPressed) {
                if (event.mouseButton.button == Mouse::Left) {
                    // Отримати координати миші відносно вікна
                    Vector2i mousePosition = Mouse::getPosition(window);

                    // Перетворити координати миші на координати клітинки в сітці
                    int x = mousePosition.x / 60; // 60 - розмір клітинки
                    int y = (mousePosition.y - 95) / 60; // 95 - відстань від верхнього краю до сітки

                    // Обробити клік мишею в грі
                    game.Click(x, y);
                }
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

    Minesweeper game(rows, columns);
    Render renderer(rows, columns, game);


    cout << "Initial grid with bomb information:" << endl;
    game.getNormalGrid().printGrid();

    while (renderer.window.isOpen()) {
        renderer.refresh();
        renderer.renderWindow();
    }

    return 0;
}
