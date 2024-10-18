#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <Windows.h>
#include <conio.h>
using namespace std;

enum SquareStates { D, A };

class MultiStream {
private:
    ostream& console;
    ostream& file;
public:
    MultiStream(ostream& console, ostream& file) : console(console), file(file) {};
    template <typename T>
    MultiStream& operator<<(const T& data) {
        console << data;
        file << data;
        return *this;
    }
    MultiStream& operator<<(ostream& (*manip)(ostream&)) {
        console << manip;
        file << manip;
        return *this;
    }
};

class GameOfLife {
private:
    char liveChar = 'O';
    char deadChar = ' ';
    int gen;
    int totalSquares;
    int liveSquares;
    int initLiveSquares;
    int xDim;
    int yDim;
    int sleepMS = 200;
    int maxGens = 1;
    vector<vector<SquareStates>> grid;

public:
    GameOfLife();
    void loadGridFromFile(const string& fileName);
    void initialiseGrid();
    void printInitialGrid();
    void printGrid();
    int nextLiveSquares(int i, int j);
    void nextGen();
    int countLiveSquares();
    void start();
};

void moveCursor(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

GameOfLife::GameOfLife() {
    cout << "Load from Save? (Y/N): ";
    char load;
    cin >> load;
    if (load == 'Y' || load == 'y') {
        string fileName;
        cout << "Enter File Name (including .txt): ";
        cin >> fileName;
        loadGridFromFile(fileName);
    }
    else {
        int gsize;
        cout << "Enter Grid Size: ";
        cin >> gsize;
        xDim = gsize;
        yDim = gsize;
        totalSquares = xDim * yDim;
        cout << "Enter Number of Live Squares: ";
        cin >> initLiveSquares;
        cout << "Enter Number of Steps: ";
        cin >> maxGens;
        gen = 1;
        initialiseGrid();
    }
}

void GameOfLife::loadGridFromFile(const string& fileName) {
    ifstream loadFile(fileName);
    ofstream initialFile("initial.txt");
    MultiStream multistream(cout, initialFile);
    if (!loadFile) {
        cout << "Error" << endl;
        return;
    }
    cout << "Enter Number of Steps: ";
    cin >> maxGens;
    gen = 1;
    string line;
    vector<string> lines;
    while (getline(loadFile, line)) {
        lines.push_back(line);
    }
    yDim = lines.size();
    xDim = lines[0].size() - 1;
    grid.resize(yDim, vector<SquareStates>(xDim));
    for (int j = 0; j < yDim; j++) {
        cout << ". ";
    }
    cout << "." << endl;
    for (int i = 0; i < yDim; i++) {
        for (int j = 0; j < xDim; j++) {
            cout << ".";
            if (lines[i][j] == liveChar) {
                grid[i][j] = A;
                multistream << liveChar;
            }
            else {
                grid[i][j] = D;
                multistream << deadChar;
            }
        }
    multistream << "." << endl;
    }
    liveSquares = countLiveSquares();
    cout << "Live Squares: " << liveSquares << endl;
    loadFile.close();
    initialFile.close();
    return;
}

void GameOfLife::initialiseGrid() {
    srand(time(0));
    grid = vector<vector<SquareStates>>(xDim, vector<SquareStates>(yDim, SquareStates(D)));
    if (initLiveSquares > totalSquares) {
        initLiveSquares = totalSquares;
    }
    int liveSquaresPlaced = 0;
    while (liveSquaresPlaced < initLiveSquares) {
        int randSquare = rand() % totalSquares;
        int i = randSquare / yDim;
        int j = randSquare % yDim;
        if (grid[i][j] == D) {
            grid[i][j] = A;
            liveSquaresPlaced++;
        }
    }
    printInitialGrid();
    return;
}

int GameOfLife::nextLiveSquares(int i, int j) {
    int lives = 0;
    for (int k = -1; k <= 1; k++) {
        for (int h = -1; h <= 1; h++) {
            if (k == 0 && h == 0) {
                continue;
            }
            int x = k + i;
            int y = h + j;
            if (x >= 0 && x < xDim && y >= 0 && y < yDim) {
                lives += (int)grid[x][y];
            }
        }
    }
    return lives;
}

void GameOfLife::nextGen() {
    vector<vector<SquareStates>> tempGrid(xDim);
    for (int i = 0; i < xDim; i++) {
        tempGrid[i] = vector<SquareStates>(yDim);
        for (int j = 0; j < yDim; j++) {
            int liveSquares = nextLiveSquares(i, j);
            if (grid[i][j] == A && liveSquares == 2 || liveSquares == 3) {
                tempGrid[i][j] = A;
            }
            else if (grid[i][j] == D && liveSquares == 3) {
                tempGrid[i][j] = A;
            }
            else {
                tempGrid[i][j] = D;
            }
        }
    }
    grid = tempGrid;
    return;
}

void GameOfLife::printInitialGrid() {
    moveCursor(0, 0);
    ofstream initialFile("initial.txt");
    MultiStream multistream(cout, initialFile);

    for (int j = 0; j < yDim; j++) {
        cout << ". ";
    }
    cout << "." << endl;
    for (int i = 0; i < xDim; i++) {
        for (int j = 0; j < yDim; j++) {
            cout << ".";
            if (grid[i][j] == A) {
                multistream << liveChar;
            }
            else {
                multistream << deadChar;
            }
        }
        multistream << "." << endl;
    }
    liveSquares = countLiveSquares();
    cout << "Live Squares: " << liveSquares << endl;
    initialFile.close();
}

void GameOfLife::printGrid() {
    moveCursor(0, 0);
    ofstream saveFile("save.txt");
    MultiStream multistream(cout, saveFile);
    for (int j = 0; j < yDim; j++) {
        cout << ". ";
    }
    cout << "." << endl;
    for (int i = 0; i < xDim; i++) {
        for (int j = 0; j < yDim; j++) {
            cout << ".";
            if (grid[i][j] == A) {
                multistream << liveChar;
            }
            else {
                multistream << deadChar;
            }
        }
        multistream << "." << endl;
    }
    cout << "Live Squares: " << liveSquares << endl;
    cout << "Current Generation: " << gen << endl;
    saveFile.close();
}

int GameOfLife::countLiveSquares() {
    int liveSquares = 0;
    for (int i = 0; i < xDim; i++) {
        for (int j = 0; j < yDim; j++) {
            if (grid[i][j] == A) {
                liveSquares++;
            }
        }
    }
    return liveSquares;
}

void GameOfLife::start() {
    while (gen < maxGens) {
        nextGen();
        gen++;
        liveSquares = countLiveSquares();
        printGrid();
        if (_kbhit()) {
            char choice = _getch();
            if (choice == 'p' || choice == 'P') {
                cout << "Currently paused. Press 'c' to continue" << endl;
                while (true) {
                    char resume = _getch();
                    if (resume == 'c' || resume == 'C') {
                        break;
                    }
                }
            }
        }
        Sleep(sleepMS);
    }
    return;
}

int main() {
    GameOfLife game;
    game.start();
    return 0;
}