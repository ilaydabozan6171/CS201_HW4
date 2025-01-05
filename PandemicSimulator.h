//Ilayda Bozan
// 22102837
//CS201- SECTION_3

#ifndef PANDEMICSIMULATOR_H
#define PANDEMICSIMULATOR_H

#include <iostream>
using namespace std;

class PandemicSimulator{
public:
    PandemicSimulator(const string cityGridFile);
    ~PandemicSimulator();

    void displayCityState(const int time);
    void simulateBlock(const int row, const int col);
    void simulatePandemic();

    int numOfDay(const int r, const int c);
    int** performBFS();

private:
    int** matrix;
    int rows;
    int cols;

};
#endif //PANDEMICSIMULATOR_H