//Ilayda Bozan
//22102837
//CS201- SECTION_3

#include "PandemicSimulator.h"
#include <iostream>
#include <fstream> //Read and write on file
using namespace std;

//I will use link-based queue
struct Point {
    int x, y;
};

struct Node{
    Point data;
    Node* next;
    Node(Point newPoint) {
        data = newPoint;
        next = nullptr;
    }
};

struct Queue {

    Node* front;
    Node* rear;
    Queue() {
        front = nullptr;
        rear = nullptr;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void enqueue(Point newPoint) {
        Node* newNode = new Node(newPoint);
        if(isEmpty()) {
            front = newNode;
            rear = newNode;
        }
        else {
            rear->next = newNode;
            rear = newNode;
        }
    }

    Point dequeue() {
        if(isEmpty()) {
            cout << "Queue is empty." << endl;
        }
        Node* temp = front;
        Point dequeuedPoint = temp->data;
        front = front->next;
        if(front == nullptr) {
            rear = nullptr;
        }
        delete temp;
        return dequeuedPoint;
    }

    Point getFront() {
        if(isEmpty()) {
            cout << "Queue is empty." << endl;
        }
        return front->data;
    }

    Point getRear() {
        if(isEmpty()) {
            cout << "Queue is empty." << endl;
        }
        return rear->data;
    }

};
//------------------------------------------------------------------------

PandemicSimulator::PandemicSimulator(const string cityGridFile) {

    ifstream file(cityGridFile);

    if (!file.is_open()) {
        cerr << "Failed to open the file: " << cityGridFile << endl;
        return;
    }
    //cout << "File opened successfully. Reading dimensions..." << endl;

    if (!(file >> rows >> cols)) {
        cerr << "Failed to read dimensions from the file." << endl;
        file.close();
        return;
    }

    /*
    cout << "Rows: " << rows << ", Cols: " << cols << endl;

    if (rows <= 0 || cols <= 0) {
        cerr << "Invalid dimensions: Rows and cols must be positive integers." << endl;
        file.close();
        return;
    }
    */

    matrix = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        matrix[i] = new int[cols];
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            char ch;
            if (!(file >> ch)) {
                cerr << "Insufficient data in file at position (" << i << ", " << j << ")." << endl;
                file.close();
                return;
            }
            if (!isdigit(ch)) {
                cerr << "Invalid character in file at position (" << i << ", " << j << ")." << endl;
                file.close();
                return;
            }
            matrix[i][j] = ch - '0';
        }
    }
    file.close();
}


PandemicSimulator::~PandemicSimulator() {
    for (int i = 0; i < rows; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}


void PandemicSimulator::displayCityState(const int time) {

    cout << "City state at day " << time << ":\n";

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int day = numOfDay(i, j);
            if (matrix[i][j] == 0) {
                cout << "0";
            }
            else if (matrix[i][j] == 1 && day == -1) {
                cout << "1";
            }
            else if (matrix[i][j] == 2 || day <= time) {
                cout << "2";
            }
            else {
                cout << "1";
            }
        }
        cout << endl;
    }
}


int PandemicSimulator::numOfDay(const int r, const int c) {
    if (matrix[r][c] == 0) {
        //Cell is empty
        return -1;
    }
    if (matrix[r][c] == 2) {
        //Cell is already infected
        return 0;
    }

    int** visitedPoints = performBFS();
    int result = visitedPoints[r][c];

    for (int i = 0; i < rows; ++i) {
        delete[] visitedPoints[i];
    }
    delete[] visitedPoints;
    return result;
}


void PandemicSimulator::simulateBlock(const int r, const int c) {
    if (matrix[r][c] == 0) {
        //Cell is empty
        cout << "Time for block (" << r <<", "<< c <<") to be infected: " << -1 <<" days."<<endl;
        return;
    }
    if (matrix[r][c] == 2) {
        //cell is already infected
        cout << "Time for block (" << r <<", "<< c <<") to be infected: " << 0 <<" days."<<endl;
        return;
    }

    int** visitedPoints = performBFS();
    int result = visitedPoints[r][c];
    cout << "Time for block (" << r <<", "<< c <<") to be infected: " << visitedPoints[r][c] <<" days."<<endl;

    for (int i = 0; i < rows; ++i) {
        delete[] visitedPoints[i];
    }
    delete[] visitedPoints;
}

void PandemicSimulator::simulatePandemic() {

    int** visitedPoints = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        visitedPoints[i] = new int[cols];
        for (int j = 0; j < cols; ++j) {
            visitedPoints[i][j] = -1;
        }
    }
    Queue q;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (matrix[i][j] == 2) {
                q.enqueue({i, j});
                visitedPoints[i][j] = 0;
            }
        }
    }

    int maxDays = 0;
    int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    int dir[2];

    while (!q.isEmpty()) {
        Point current = q.dequeue();
        int x = current.x;
        int y = current.y;

        for (int i = 0; i < 4; ++i) {
            dir[0] = directions[i][0];
            dir[1] = directions[i][1];
            int nx = x + dir[0];
            int ny = y + dir[1];

            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && visitedPoints[nx][ny] == -1 && matrix[nx][ny] == 1) {
                visitedPoints[nx][ny] = visitedPoints[x][y] + 1;
                maxDays = max(maxDays, visitedPoints[nx][ny]);
                q.enqueue({nx, ny});
            }
        }
    }

    // last check for unreachable blocks
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (matrix[i][j] == 1 && visitedPoints[i][j] == -1) {
                cout << "Pandemic cannot spread to all blocks." << endl;
                for (int k = 0; k < rows; ++k) {
                    delete[] visitedPoints[k];
                }
                delete[] visitedPoints;
                return;
            }
        }
    }

    cout << "Minimum time for pandemic to spread to all blocks: " << maxDays << " days." << endl;

    for (int i = 0; i < rows; ++i) {
        delete[] visitedPoints[i];
    }
    delete[] visitedPoints;
}
int** PandemicSimulator::performBFS() {

    int** visitedPoints = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        visitedPoints[i] = new int[cols];
        for (int j = 0; j < cols; ++j) {
            visitedPoints[i][j] = -1;
        }
    }

    Queue visitedQ;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (matrix[i][j] == 2) {
                visitedQ.enqueue({i, j});
                visitedPoints[i][j] = 0;
            }
        }
    }

    int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    while (!visitedQ.isEmpty()) {
        Point current = visitedQ.dequeue();
        int x = current.x;
        int y = current.y;

        for (int i = 0; i < 4; ++i) {
            int nx = x + directions[i][0];
            int ny = y + directions[i][1];

            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && visitedPoints[nx][ny] == -1 && matrix[nx][ny] == 1) {
                visitedPoints[nx][ny] = visitedPoints[x][y] + 1;
                visitedQ.enqueue({nx, ny});
            }
        }
    }
    return visitedPoints;
}
