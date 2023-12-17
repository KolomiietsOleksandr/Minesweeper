#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

using namespace std;
using namespace sf;

static bool timerStarted;
Clock gameClock;

class Cell {
private:
    bool isMine;
    mutable bool isRevealed;
    bool isMarked;
    bool isExploded;

public:
    Cell() : isMine(false), isRevealed(false), isMarked(false), isExploded(false) {}

    void reveal() const {
        isRevealed = true;
    }

    void explode() {
        isExploded = true;
    }

    void mark() {
        isMarked = !isMarked;
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
    int mineCount = 10;

    Grid(int rows, int columns) : rows(rows), columns(columns) {
        initializeGrid();
    }

    void initializeGrid() {
        cells.resize(rows, vector<Cell>(columns));

        srand(static_cast<unsigned>(time(nullptr)));

        int count = mineCount;
        while (count > 0) {
            int x = rand() % rows;
            int y = rand() % columns;

            if (!cells[x][y].getIsMine()) {
                cells[x][y].changeIsMine(true);
                count--;
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
                if (cells[i][j].getIsMine() && cells[i][j].getMarked() == false) {
                    cells[i][j].reveal();
                }
            }
        }
    }

    int countMarkedCells() const {
        int count = 0;
        for (size_t i = 0; i < cells.size(); ++i) {
            for (size_t j = 0; j < cells[i].size(); ++j) {
                if (cells[i][j].getMarked()) {
                    ++count;
                }
            }
        }
        return count;
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

    void markCell(int x, int y) {
        if (x >= 0 && x < rows && y >= 0 && y < columns) {
            if (!cells[x][y].getMarked() && countMarkedCells() < mineCount) {
                cells[x][y].mark();
            }
            else if (cells[x][y].getMarked() && countMarkedCells() > 0) {
                cells[x][y].mark();
            }
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
    bool revealMode;
public:
    Minesweeper(int rows, int columns) : rows(rows), columns(columns), mineGrid(rows, columns), normalGrid(rows, columns), gameOver(false), revealMode(true), win(false) {}

    bool win = false;

    void StartGame() {
        gameOver = false;
        win = false;
        timerStarted = false;
        gameClock.restart();
        normalGrid = Grid(rows, columns);
    }


    void Click(int x, int y) {
        if (gameOver) {
            return;
        }

        Cell& clickedCell = const_cast<Cell&>(normalGrid.getCells()[x][y]);

        if (!clickedCell.getRevealed() && !clickedCell.getMarked()) {
            clickedCell.reveal();

            if (clickedCell.getIsMine()) {
                clickedCell.explode();
                Defeat();
            }
            else {
                if (normalGrid.countAdjacentBombs(x, y) == 0) {
                    revealEmptyCells(x, y);
                }

                bool allNonMineCellsRevealed = true;
                for (size_t i = 0; i < normalGrid.getCells().size(); ++i) {
                    for (size_t j = 0; j < normalGrid.getCells()[i].size(); ++j) {
                        if (!normalGrid.getCells()[i][j].getIsMine() && !normalGrid.getCells()[i][j].getRevealed()) {
                            allNonMineCellsRevealed = false;
                            break;
                        }
                    }
                }
                if (allNonMineCellsRevealed && normalGrid.countMarkedCells() == normalGrid.mineCount) {
                    Win();
                }
                cout << allNonMineCellsRevealed << endl;
                cout << normalGrid.countMarkedCells() << endl;
            }
        }
    }

    void revealEmptyCells(int x, int y) {
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int newX = x + i;
                int newY = y + j;

                if (newX >= 0 && newX < rows && newY >= 0 && newY < columns && !normalGrid.getCells()[newX][newY].getRevealed()) {
                    normalGrid.getCells()[newX][newY].reveal();

                    if (normalGrid.countAdjacentBombs(newX, newY) == 0) {
                        revealEmptyCells(newX, newY);
                    }
                }
            }
        }
    }

    void markCell(int x, int y) {
        normalGrid.markCell(x, y);

        bool allNonMineCellsRevealed = true;
        for (size_t i = 0; i < normalGrid.getCells().size(); ++i) {
            for (size_t j = 0; j < normalGrid.getCells()[i].size(); ++j) {
                if (!normalGrid.getCells()[i][j].getIsMine() && !normalGrid.getCells()[i][j].getRevealed()) {
                    allNonMineCellsRevealed = false;
                    break;
                }
            }
        }
        if (allNonMineCellsRevealed && normalGrid.countMarkedCells() == normalGrid.mineCount) {
            Win();
        }
    }

    void Defeat() {
        normalGrid.revealAllBombs();
        gameOver = true;
        timerStarted = false;
    }

    void Win() {
        gameOver = true;
        win = true;
        timerStarted = false;
    }

    bool isGameOver() const {
        return gameOver;
    }

    const Grid& getNormalGrid() const {
        return normalGrid;
    }

    void toggleRevealMode() {
        revealMode = !revealMode;
    }

    bool isRevealMode() const {
        return revealMode;
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
    vector<Texture> numberTextures;

    Texture restartButtonTexture;
    Sprite restartButton;

    Minesweeper game;

    vector<Texture> digitTextures;
    Texture emptyDigitTexture;

    Texture ModeTextureReveal;
    Texture ModeTextureMark;
    Sprite ModeSprite;

    Texture timerDigitTextures[11];
    Sprite timerSprite;

    float elapsedTime;

    void loadDigitTextures() {
        for (int i = 0; i <= 9; ++i) {
            digitTextures.push_back(Texture());
            digitTextures.back().loadFromFile("Sprites/Counts/" + to_string(i) + "s.png");
            emptyDigitTexture.loadFromFile("Sprites/Counts/s.png");
        }
    }

    void drawNumber(int number, float x, float y, float scale) {
        string numberString;

        if (number >= 0 && number < 10) {
            numberString = "#";
            numberString += "#";
            numberString += to_string(number);
        } else if (number >= 10 && number < 100) {
            numberString = "#";
            numberString += to_string(number);
        } else if (number >= 100) {
            numberString = to_string(number);
        } else {
            numberString = "#";
        }

        float digitWidth = 13.0f * scale;

        for (size_t i = 0; i < numberString.size(); ++i) {
            char currentChar = numberString[i];
            Sprite digitSprite;

            if (currentChar >= '0' && currentChar <= '9') {
                int digit = currentChar - '0';
                digitSprite.setTexture(digitTextures[digit]);
            } else {
                digitSprite.setTexture(emptyDigitTexture);
            }

            digitSprite.setPosition(x + i * digitWidth, y);
            digitSprite.setScale(scale, scale);
            window.draw(digitSprite);
        }
    }

void loadRevealModeTextures() {
        ModeTextureReveal.loadFromFile("Sprites/R.png");
        ModeTextureMark.loadFromFile("Sprites/M.png");
    }

    void drawRevealMode() {
        if (game.isRevealMode()) {
            ModeSprite.setTexture(ModeTextureReveal);
        }
        else {
            ModeSprite.setTexture(ModeTextureMark);
        }

        ModeSprite.setPosition(115, 20);
        ModeSprite.setScale(2.5f, 2.5f);

        window.draw(ModeSprite);
    }

    void drawTimerDigits(float x, float y, float scale) {
        string elapsedTimeString = to_string(static_cast<int>(elapsedTime));

        while (elapsedTimeString.length() < 3) {
            elapsedTimeString = "0" + elapsedTimeString;
        }

        float digitWidth = 13.0f * scale;

        for (size_t i = 0; i < 3; ++i) {
            char currentChar = elapsedTimeString[i];
            Sprite digitSprite;

            if (currentChar >= '0' && currentChar <= '9') {
                int digit = currentChar - '0';
                digitSprite.setTexture(timerDigitTextures[digit]);
            }
            else {
                digitSprite.setTexture(timerDigitTextures[10]);
            }

            digitSprite.setPosition(x + i * digitWidth, y);
            digitSprite.setScale(scale, scale);
            window.draw(digitSprite);
        }
    }

    void loadTimerDigitTextures() {
        for (int i = 0; i <= 9; ++i) {
            timerDigitTextures[i].loadFromFile("Sprites/Counts/" + to_string(i) + "s.png");
        }
        timerDigitTextures[10].loadFromFile("Sprites/Counts/s.png");
    }


public:

    Render(int rows, int columns, Minesweeper& game) : game(game), window(VideoMode(columns * 60, rows * 72), "Minesweeper"), elapsedTime(0.0f) {
        timerStarted = false;
        emptyTexture.loadFromFile("Sprites/0.png");
        revealedTexture.loadFromFile("Sprites/Check.png");
        markedTexture.loadFromFile("Sprites/Flag.png");
        unmarkedTexture.loadFromFile("Sprites/Cell.png");
        bombTexture.loadFromFile("Sprites/Bomb.png");
        bombBoomTexture.loadFromFile("Sprites/BombLose.png");

        for (int i = 1; i <= 6; ++i) {
            numberTextures.push_back(Texture());
            numberTextures.back().loadFromFile("Sprites/" + to_string(i) + ".png");
        }

        restartButtonTexture.loadFromFile("Sprites/Game.png");
        restartButton.setTexture(restartButtonTexture);
        restartButton.setScale(60 / restartButton.getLocalBounds().width, 60 / restartButton.getLocalBounds().height);
        restartButton.setPosition(window.getSize().x / 2.0f - restartButton.getLocalBounds().width / 0.75f, 20.0f);

        loadDigitTextures();
        loadRevealModeTextures();
        loadTimerDigitTextures();
    }

    void handleButtonClick() {
        if (Mouse::isButtonPressed(Mouse::Left)) {
            Vector2i mousePosition = Mouse::getPosition(window);

            if (restartButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) {
                game.StartGame();
                timerStarted = false;
                elapsedTime = 0.0f;
            }
        }
    }


    void updateButtonTexture() {
        if (game.isGameOver()) {
            if (game.getNormalGrid().getColumns() > 0 && game.win == false) {
                restartButtonTexture.loadFromFile("Sprites/Lose.png");
            }
            else if (game.win == true) {
                restartButtonTexture.loadFromFile("Sprites/Win.png");
            }
        }
        else {
            restartButtonTexture.loadFromFile("Sprites/Game.png");
        }

        restartButton.setTexture(restartButtonTexture);
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
                        }
                        else {
                            cellSprite.setTexture(bombTexture);
                        }
                    }
                    else {
                        int adjacentBombs = grid.countAdjacentBombs(i, j);
                        if (adjacentBombs == 0) {
                            cellSprite.setTexture(emptyTexture);
                        }
                        else {
                            cellSprite.setTexture(numberTextures[adjacentBombs - 1]);
                        }
                    }
                }
                else if (cells[i][j].getMarked()) {
                    cellSprite.setTexture(markedTexture);
                }
                else {
                    cellSprite.setTexture(unmarkedTexture);
                }

                cellSprite.setPosition(i * cellSize, j * cellSize + topMargin);
                cellSprite.setScale(cellSize / cellSprite.getLocalBounds().width, cellSize / cellSprite.getLocalBounds().height);

                window.draw(cellSprite);
            }
        }

        drawNumber(game.getNormalGrid().mineCount - game.getNormalGrid().countMarkedCells(), 10.0f, 20.0f, 2.5f);
        drawRevealMode();

        drawTimerDigits(373.0f, 20.0f, 2.5f);

        window.draw(restartButton);
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
                    Vector2i mousePosition = Mouse::getPosition(window);

                    int x = mousePosition.x / 60;
                    int y = (mousePosition.y - 95) / 60;

                    if (!timerStarted) {
                        timerStarted = true;
                        gameClock.restart();
                    }

                    if (game.isRevealMode()) {
                        game.Click(x, y);
                    }
                    else {
                        game.markCell(x, y);
                    }
                    handleButtonClick();
                }
            }
            else if (event.type == Event::KeyPressed && !game.isGameOver()) {
                if (event.key.code == Keyboard::F) {
                    game.toggleRevealMode();
                }
            }
        }
        if (timerStarted) {
            elapsedTime = gameClock.getElapsedTime().asSeconds();
        }

        if (timerStarted && !game.isGameOver()) {
            elapsedTime = gameClock.getElapsedTime().asSeconds();
        }
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
        renderer.updateButtonTexture();
        renderer.renderWindow();
    }

    return 0;
}