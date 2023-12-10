
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <algorithm>
#include <optional>
//debug options
#include <chrono>
#include <thread>

// base lattice dimensions
#define WIDTH 8
#define HEIGHT 12

// global used for extending the lattice in extended road network
int Prediction = 0; 

// TRAFFIC POINTS
//     LT---RT
//     #######
//     LB---RB


// Cell (Point class) types
enum Type{
    ROAD,
    CAR, 
    EMPTY
};

// the types of roads in the system
enum RoadType{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    UNASSIGNED
};

// Left, Right, Top, Bottom  combinations
enum PositionType{
    LB,
    LT,
    RT,
    RB
};


// statistics class
class Intensity{
public:
    int LT;
    int LB;
    int RT;
    int RB;
    // LT -> LB
    int avgIterationsA;
    // LT -> RT
    int avgIterationsB;
    // LT -> RB
    int avgIterationsC;
    // LB -> LT
    int avgIterationsD;
    // LB -> RT
    int avgIterationsE;
    // LB -> RB
    int avgIterationsF;
    // RT -> LT
    int avgIterationsG;
    // RT -> LB
    int avgIterationsH;
    // RB -> LT
    int avgIterationsI;
    // RB -> LB
    int avgIterationsJ;
    int totalA;
    int totalB;
    int totalC;
    int totalD;
    int totalE;
    int totalF;
    int totalG;
    int totalH;
    int totalI;
    int totalJ;

    // printed lines can be used to debug the model further outside the graphing bounds
    void print(){
        //std::cout << "---Intensity---" << std::endl;
        //std::cout << "LT: " << LT << std::endl;
        //std::cout << "LB: " << LB << std::endl;
        //std::cout << "RT: " << RT << std::endl;
        //std::cout << "RB: " << RB << std::endl;
        //std::cout << "Overall: " << LT + LB + RT + RB << std::endl;
        //std::cout << "Total cars for path A: " << totalA <<std::endl;
        std::cout << "Average Iteration delay for path A: " << (totalA == 0 ? 0 : (avgIterationsA/(float)totalA-12 - Prediction)) << std::endl;
        //std::cout << "Total cars for path B: " << totalB <<std::endl;
        std::cout << "Average Iteration delay for path B: " << (totalB == 0 ? 0 : (avgIterationsB/(float)totalB-17 - (Prediction*2))) << std::endl;
        //std::cout << "Total cars for path C: " << totalC <<std::endl;
        std::cout << "Average Iteration delay for path C: " << (totalC == 0 ? 0 : (avgIterationsC/(float)totalC-15 - Prediction)) << std::endl;
        //std::cout << "Total cars for path D: " << totalD <<std::endl;
        std::cout << "Average Iteration delay for path D: " << (totalD == 0 ? 0 : (avgIterationsD/(float)totalD-12 - Prediction)) << std::endl;
        //std::cout << "Total cars for path E: " << totalE <<std::endl;
        std::cout << "Average Iteration delay for path E: " << (totalE == 0 ? 0 : (avgIterationsE/(float)totalE-15 - Prediction)) << std::endl;
        //std::cout << "Total cars for path F: " << totalF <<std::endl;
        std::cout << "Average Iteration delay for path F: " << (totalF == 0 ? 0 : (avgIterationsF/(float)totalF-13)) << std::endl;
        //std::cout << "Total cars for path G: " << totalG <<std::endl;
        std::cout << "Average Iteration delay for path G: " << (totalG == 0 ? 0 : (avgIterationsG/(float)totalG-13)) << std::endl;
        //std::cout << "Total cars for path H: " << totalH <<std::endl;
        std::cout << "Average Iteration delay for path H: " << (totalH == 0 ? 0 : (avgIterationsH/(float)totalH-16 - Prediction)) << std::endl;
        //std::cout << "Total cars for path I: " << totalI <<std::endl;
        std::cout << "Average Iteration delay for path I: " << (totalI == 0 ? 0 : (avgIterationsI/(float)totalI-15 - Prediction)) << std::endl;
        //std::cout << "Total cars for path J: " << totalJ <<std::endl;
        std::cout << "Average Iteration delay for path J: " << (totalJ == 0 ? 0 : (avgIterationsJ/(float)totalJ-18)) << std::endl;
    }
};


// public accessable structure for statistics
Intensity stats;

// class for car, containing the direction information and iteration information used for statistics
class Car{
public:
    PositionType start;
    PositionType end;
    int startingIteration;
};

// class used as a cell in lattice
class Point{
public:
    Type pointType;
    std::optional<Car> car;
    RoadType road;
    int iteration = -1; // prevents moving vehicle multiple times in iteration

    Point(Type type, RoadType roadType, const std::optional<Car>& carValue = {}) 
        : pointType(type), car(carValue), road(roadType) {}

    bool isCellBlocked(){
        if(pointType == ROAD){
            return false;
        }
        return true;
    }
    void createCar(PositionType start, PositionType end, int iteration){
        car = Car{start, end, iteration};
        pointType = CAR;
    }
};

void moveCarUp(Point trafficStytem[][WIDTH], int x, int y, int iteration, Car car){
    trafficStytem[y-1][x].car = car;
    trafficStytem[y][x].car.reset();
    trafficStytem[y-1][x].pointType = CAR;
    trafficStytem[y][x].pointType = ROAD;
    trafficStytem[y-1][x].iteration = iteration;
}
void moveCarUpRight(Point trafficStytem[][WIDTH], int x, int y, int iteration, Car car){
    trafficStytem[y-1][x+1].car = car;
    trafficStytem[y][x].car.reset();
    trafficStytem[y-1][x+1].pointType = CAR;
    trafficStytem[y][x].pointType = ROAD;
    trafficStytem[y-1][x+1].iteration = iteration;
}
void moveCarUpLeft(Point trafficStytem[][WIDTH], int x, int y, int iteration, Car car){
    trafficStytem[y-1][x-1].car = car;
    trafficStytem[y][x].car.reset();
    trafficStytem[y-1][x-1].pointType = CAR;
    trafficStytem[y][x].pointType = ROAD;
    trafficStytem[y-1][x-1].iteration = iteration;
}
void moveCarDown(Point trafficStytem[][WIDTH], int x, int y, int iteration, Car car){
    trafficStytem[y+1][x].car = car;
    trafficStytem[y][x].car.reset();
    trafficStytem[y+1][x].pointType = CAR;
    trafficStytem[y][x].pointType = ROAD;
    trafficStytem[y+1][x].iteration = iteration;
}
void moveCarDownRight(Point trafficStytem[][WIDTH], int x, int y, int iteration, Car car){
    trafficStytem[y+1][x+1].car = car;
    trafficStytem[y][x].car.reset();
    trafficStytem[y+1][x+1].pointType = CAR;
    trafficStytem[y][x].pointType = ROAD;
    trafficStytem[y+1][x+1].iteration = iteration;
}
void moveCarLeft(Point trafficStytem[][WIDTH], int x, int y, int iteration, Car car){
    trafficStytem[y][x-1].car = car;
    trafficStytem[y][x].car.reset();
    trafficStytem[y][x-1].pointType = CAR;
    trafficStytem[y][x].pointType = ROAD;
    trafficStytem[y][x-1].iteration = iteration;
}
void moveCarDownLeft(Point trafficStytem[][WIDTH], int x, int y, int iteration, Car car){
    trafficStytem[y+1][x-1].car = car;
    trafficStytem[y][x].car.reset();
    trafficStytem[y+1][x-1].pointType = CAR;
    trafficStytem[y][x].pointType = ROAD;
    trafficStytem[y+1][x-1].iteration = iteration;
}
void moveCarRight(Point trafficStytem[][WIDTH], int x, int y, int iteration, Car car){
    trafficStytem[y][x+1].car = car;
    trafficStytem[y][x].car.reset();
    trafficStytem[y][x+1].pointType = CAR;
    trafficStytem[y][x].pointType = ROAD;
    trafficStytem[y][x+1].iteration = iteration;
}

// apllies defined rules on specific cells based on the car information
void applyRule(Point trafficStytem[][WIDTH], int x, int y, int iteration){
    auto car = *std::move(trafficStytem[y][x].car);
    switch (car.start)
    {
    // Starting at left bottom
    case LB:
        switch (car.end)
        {
        // Ending at Left Top
        case LT:
            // leave system
            if(y == 0){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsD += car.startingIteration - iteration;
                stats.totalD++;
                break;
            } 
            if(!trafficStytem[y-1][x].isCellBlocked())
            {
                moveCarUp(trafficStytem, x, y, iteration, car);
            }
            break;
        // Ending at Right Top
        case RT:
            if(y == 0){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsE += car.startingIteration - iteration;
                stats.totalE++;
                break;
            }
            if(trafficStytem[y-1][x].road == C && trafficStytem[y-1][x+1].road != G && !trafficStytem[y-1][x].isCellBlocked()){
                moveCarUp(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y-1][x+1].road == G && !trafficStytem[y-1][x+1].isCellBlocked()){
                moveCarUpRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+1].road == G && !trafficStytem[y][x+1].isCellBlocked()){
                moveCarRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+2].road == E && !trafficStytem[y][x+1].isCellBlocked() && !trafficStytem[y+1][x+1].isCellBlocked() && !trafficStytem[y+1][x+2].isCellBlocked() && !trafficStytem[y-1][x+2].isCellBlocked()){
                moveCarRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+1].road == E && !trafficStytem[y][x+1].isCellBlocked() && !trafficStytem[y-1][x+1].isCellBlocked()){
                moveCarRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y-1][x].road == E && !trafficStytem[y-1][x].isCellBlocked()){
                moveCarUp(trafficStytem, x, y, iteration, car);
            }
            break;
        // Ending at Right Bottom
        case RB:
            if(y == HEIGHT-1 + Prediction && x == WIDTH-2){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsF += car.startingIteration - iteration;
                stats.totalF++;
                break;
            }
            if(trafficStytem[y-1][x].road == C && trafficStytem[y-1][x+1].road != G && !trafficStytem[y-1][x].isCellBlocked()){
                moveCarUp(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y-1][x+1].road == G && !trafficStytem[y-1][x+1].isCellBlocked()){
                moveCarUpRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+1].road == G && !trafficStytem[y][x+1].isCellBlocked()){
                moveCarRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+1].road == D && !trafficStytem[y][x+1].isCellBlocked()){
                moveCarDownRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y+1][x].road == D && !trafficStytem[y+1][x].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }
            break;
        default:
            break;
        }
        break;
    // Starting at Left Top
    case LT:
        switch (car.end)
        {
        // Ending at Left Bottom
        case LB:
            // leave system
            if(y == HEIGHT-1 + Prediction){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsA += car.startingIteration - iteration;
                stats.totalA++;
            }else if(!trafficStytem[y+1][x].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }
            break;
        // Ending at Right Top
        case RT:
            if(y == 0 && x == WIDTH-1){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsB += car.startingIteration - iteration;
                stats.totalB++;
                break;
            }
            if(trafficStytem[y+1][x].road == B && !trafficStytem[y+1][x].isCellBlocked() && trafficStytem[y+1][x+2].road != F && trafficStytem[y][x+2].road != F  && trafficStytem[y][x+2].road != H){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y+1][x+2].road == F && !trafficStytem[y+1][x].isCellBlocked() && !trafficStytem[y+4 + Prediction][x+1].isCellBlocked()  && !trafficStytem[y+3 + Prediction][x+1].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 1 && trafficStytem[y][x+2].road == F && !trafficStytem[y+1][x].isCellBlocked() && !trafficStytem[y+4][x+1].isCellBlocked()  && !trafficStytem[y+3][x+1].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 1 && trafficStytem[y][x+2].road == H && !trafficStytem[y+1][x+1].isCellBlocked() && !trafficStytem[y+2][x+1].isCellBlocked()){
                moveCarDownRight(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 0 && trafficStytem[y][x+2].road == F && !trafficStytem[y+1][x+1].isCellBlocked() && !trafficStytem[y+2][x+1].isCellBlocked()){
                moveCarDownRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+1].road == G && !trafficStytem[y][x+1].isCellBlocked()){
                moveCarRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+2].road == E && !trafficStytem[y][x+1].isCellBlocked() && !trafficStytem[y][x+2].isCellBlocked() && !trafficStytem[y+1][x+2].isCellBlocked()){
                moveCarRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+1].road == E && !trafficStytem[y][x+1].isCellBlocked()){
                moveCarRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y-1][x].road == E && !trafficStytem[y-1][x].isCellBlocked()){
                moveCarUp(trafficStytem, x, y, iteration, car);
            }
            break;
        // Ending at Right Bottom
        case RB:
            if(y == HEIGHT-1 + Prediction && x == WIDTH-2){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsC += car.startingIteration - iteration;
                stats.totalC++;
                break;
            }
            if(trafficStytem[y+1][x].road == B && !trafficStytem[y+1][x].isCellBlocked() && trafficStytem[y+1][x+2].road != F && trafficStytem[y][x+2].road != F  && trafficStytem[y][x+2].road != H){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y+1][x+2].road == F && !trafficStytem[y+1][x].isCellBlocked() && !trafficStytem[y+4 + Prediction][x+1].isCellBlocked()  && !trafficStytem[y+3 + Prediction][x+1].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 1 && trafficStytem[y][x+2].road == F && !trafficStytem[y+1][x].isCellBlocked() && !trafficStytem[y+4][x+1].isCellBlocked()  && !trafficStytem[y+3][x+1].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 1 && trafficStytem[y][x+2].road == H && !trafficStytem[y+1][x+1].isCellBlocked() && !trafficStytem[y+2][x+1].isCellBlocked()){
                moveCarDownRight(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 0 && trafficStytem[y][x+2].road == F && !trafficStytem[y+1][x+1].isCellBlocked() && !trafficStytem[y+2][x+1].isCellBlocked()){
                moveCarDownRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+1].road == G && !trafficStytem[y][x+1].isCellBlocked()){
                moveCarRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x+1].road == D && !trafficStytem[y][x+1].isCellBlocked()){
                moveCarRight(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y+1][x].road == D && !trafficStytem[y+1][x].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }
            break;
        default:
            break;
        }
        break;
    // Starting at Right Top
    case RT:
        switch (car.end)
        {
        // Ending at Left Bottom
        case LB:
            // leave system
            if(y == HEIGHT-1 + Prediction){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsH += car.startingIteration - iteration;
                stats.totalH++;
                break;
            }
            if(trafficStytem[y+1][x].road == D && trafficStytem[y+1][x-1].road != F && trafficStytem[y+1][x-1].road != H && !trafficStytem[y+1][x].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 1 && trafficStytem[y+1][x-1].road == F && !trafficStytem[y+2][x-1].isCellBlocked() && !trafficStytem[y+1][x].isCellBlocked()  && !trafficStytem[y+2][x].isCellBlocked()  && !trafficStytem[y+3][x].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 1 && trafficStytem[y+1][x-1].road == H && !trafficStytem[y+1][x-1].isCellBlocked()){
                moveCarDownLeft(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 1 && trafficStytem[y][x-1].road == H && !trafficStytem[y][x-1].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y+1][x-1].road == F && !trafficStytem[y+1][x-1].isCellBlocked() && !trafficStytem[y+2][x].isCellBlocked()){
                moveCarDownLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-1].road == F && !trafficStytem[y][x-1].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-3].road == A && !trafficStytem[y][x-1].isCellBlocked() && !trafficStytem[y+1][x-1].isCellBlocked() && !trafficStytem[y+2][x-1].isCellBlocked() &&  !trafficStytem[y-1][x-2].isCellBlocked() &&  !trafficStytem[y-1][x-3].isCellBlocked() && !trafficStytem[y-2][x-3].isCellBlocked() && !trafficStytem[y-2][x-3].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-2].road == A && !trafficStytem[y][x-1].isCellBlocked() && !trafficStytem[y][x-2].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-1].road == A && !trafficStytem[y+1][x-1].isCellBlocked()){
                moveCarDownLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y+1][x].road == A && !trafficStytem[y+1][x].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }
            break;
        // Ending at Left Top
        case LT:
            if(y == 0 && x == 2){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsG +=  car.startingIteration - iteration;
                stats.totalG++;
                break;
            }
            if(trafficStytem[y+1][x].road == D && trafficStytem[y+1][x-1].road != F && !trafficStytem[y+1][x].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y+1][x-1].road == F && !trafficStytem[y+1][x-1].isCellBlocked() && !trafficStytem[y+2][x].isCellBlocked()){
                moveCarDownLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-1].road == F && !trafficStytem[y][x-1].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-1].road == C && !trafficStytem[y][x-1].isCellBlocked() && !trafficStytem[y+1][x-1].isCellBlocked() && !trafficStytem[y-1][x-1].isCellBlocked()){
                moveCarUpLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y-1][x].road == C && !trafficStytem[y-1][x].isCellBlocked()){
                moveCarUp(trafficStytem, x, y, iteration, car);
            }
            break;
        default:
            break;
        }
        break;
    // Starting at Right Bottom
    case RB:
         switch (car.end)
        {
        // Ending at Left bottom
        case LB:
            // leave system
            if(y == HEIGHT-1 + Prediction && x == 0){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsJ += car.startingIteration - iteration;
                stats.totalJ++;
                break;
            }
            if(trafficStytem[y-1][x].road == E && trafficStytem[y-1][x-2].road != (Prediction == 1 ? H : F) && !trafficStytem[y-1][x].isCellBlocked()){
                moveCarUp(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 1 && trafficStytem[y-1][x-2].road == H && !trafficStytem[y-1][x-1].isCellBlocked()){
                moveCarUpLeft(trafficStytem, x, y, iteration, car);
            }else if(Prediction == 1 && trafficStytem[y][x-1].road == H && !trafficStytem[y][x-1].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y-1][x-2].road == F && !trafficStytem[y-1][x-1].isCellBlocked()){
                moveCarUpLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-1].road == F && !trafficStytem[y][x-1].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-3].road == A && !trafficStytem[y][x-1].isCellBlocked() && !trafficStytem[y+1][x-1].isCellBlocked() && !trafficStytem[y+2][x-1].isCellBlocked() &&  !trafficStytem[y-1][x-2].isCellBlocked() &&  !trafficStytem[y-1][x-3].isCellBlocked() && !trafficStytem[y-2][x-3].isCellBlocked() && !trafficStytem[y-2][x-3].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-2].road == A && !trafficStytem[y][x-1].isCellBlocked() && !trafficStytem[y][x-2].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-1].road == A && !trafficStytem[y+1][x-1].isCellBlocked()){
                moveCarDownLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y+1][x].road == A && !trafficStytem[y+1][x].isCellBlocked()){
                moveCarDown(trafficStytem, x, y, iteration, car);
            }
            break;
        // Ending at Left Top
        case LT:
            if(y == 0){
                trafficStytem[y][x].car.reset();
                trafficStytem[y][x].pointType = ROAD;
                stats.avgIterationsI += car.startingIteration - iteration;
                stats.totalI++;
                break;
            }
            // todo reworked
            if(trafficStytem[y-1][x].road == E && trafficStytem[y-1][x-2].road != F && !trafficStytem[y-1][x].isCellBlocked()){
                moveCarUp(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y-1][x-2].road == F && !trafficStytem[y-1][x-1].isCellBlocked()){
                moveCarUpLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-1].road == F && !trafficStytem[y][x-1].isCellBlocked()){
                moveCarLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y][x-1].road == C && !trafficStytem[y][x-1].isCellBlocked() && !trafficStytem[y+1][x-1].isCellBlocked() && !trafficStytem[y-1][x-1].isCellBlocked()){
                moveCarUpLeft(trafficStytem, x, y, iteration, car);
            }else if(trafficStytem[y-1][x].road == C && !trafficStytem[y-1][x].isCellBlocked()){
                moveCarUp(trafficStytem, x, y, iteration, car);
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

// check rules for each cell in lattice
void moveCars(Point trafficSystem[][WIDTH], int iteration) {
    for (int i = 0; i < HEIGHT + Prediction; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            if (trafficSystem[i][j].pointType == CAR && trafficSystem[i][j].iteration != iteration) {
                applyRule(trafficSystem, j, i, iteration);
            }
        }
    }
}

// debugging tool to control the active state of cars in iteration
void printLattice(const Point trafficSystem[][WIDTH]) {
    for (int i = 0; i < HEIGHT + Prediction; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            switch (trafficSystem[i][j].pointType) {
                case ROAD:
                    switch (trafficSystem[i][j].road) {
                    case A:
                        std::cout << "A";
                        break;
                    case B:
                        std::cout << "B";
                        break;
                    case C:
                        std::cout << "C";
                        break;
                    case D:
                        std::cout << "D";
                        break;
                    case E:
                        std::cout << "E";
                        break;
                    case F:
                        std::cout << "F";
                        break;
                    case G:
                        std::cout << "G";
                        break;
                    case H:
                        std::cout << "H";
                        break;
                    default:
                        break;
                    }
                    break;
                case CAR:
                    std::cout << "o";
                    break;
                case EMPTY:
                    std::cout << " ";
                    break;
                default:
                    break;
            }


            std::cout << " ";
        }
        std::cout << std::endl;
    }
}


// probabilty functions to determine car creation 
// ---
bool probabilty(float percentage){
    float randomNum =  (float)rand()/(float)(RAND_MAX);  
    return (randomNum < percentage);
}

bool createBasedOnTotal(int realTotal){
    float percentage = (realTotal/15.0)/60.0;
    return probabilty(percentage);
}

// ---

// determine if a car should be created
void createCars(Point trafficSystem[][WIDTH], int iteration, int *data){
    // LT-A constructor
    if(trafficSystem[0][0].pointType == ROAD && createBasedOnTotal((float)data[0])){
        stats.LT++;
        trafficSystem[0][0].createCar(LT,LB, iteration);
    }
    // LT-B constructor
    if(trafficSystem[0][1].pointType == ROAD && createBasedOnTotal((float)data[2])){
        stats.LT++;
        // decide on destination
        trafficSystem[0][1].createCar(LT, probabilty((float)data[3]/((float)data[2]+(float)data[4])) ? RT : RB, iteration);
    }
    // LB constructor
    if(trafficSystem[HEIGHT-1 + Prediction][2].pointType == ROAD && createBasedOnTotal((float)data[1] + (float)data[4])){
        stats.LB++;
        PositionType dest;
        if(probabilty((float)data[4]/(float)data[2])){
            dest =  probabilty((float)data[3]/((float)data[2]+(float)data[4])) ? RT : RB;
        }else{
            dest = LT;
        }
        trafficSystem[HEIGHT-1 + Prediction][2].createCar(LB,dest, iteration);
    }
    // RB constructor
    if(trafficSystem[HEIGHT-1 + Prediction][WIDTH-1].pointType == ROAD && createBasedOnTotal((float)data[5])){
        stats.RB++;
        trafficSystem[HEIGHT-1 + Prediction][WIDTH-1].createCar(RB,probabilty((float)data[7]/(float)data[6]) ? LB : LT, iteration);
    }
    // RT constructor
    if(trafficSystem[0][WIDTH-2].pointType == ROAD && createBasedOnTotal((float)data[3])){
        stats.RT++;
        trafficSystem[0][WIDTH-2].createCar(RT,probabilty((float)data[7]/(float)data[6]) ? LB : LT, iteration);
    }
}

// parses the program arguments
void parseArgs(int argc, char** argv, int* data){
    
    int option;
    while ((option = getopt(argc, argv, "a:b:c:d:e:f:g:h:i:p:")) != -1) {
        switch (option) {
            case 'a':
                data[0] = std::stof(optarg);
                break;
            case 'b':
                data[1] = std::stof(optarg);
                break;
            case 'c':
                data[2] = std::stof(optarg);
                break;
            case 'd':
                data[3] = std::stof(optarg);
                break;
            case 'e':
                data[4] = std::stof(optarg);
                break;
            case 'f':
                data[5] = std::stof(optarg);
                break;
            case 'g':
                data[6] = std::stof(optarg);
                break;
            case 'h':
                data[7] = std::stof(optarg);
                break;
            case 'i':
                data[8] = std::stof(optarg);
                break;
            case 'p':
                if(std::stof(optarg) == 1){
                    Prediction = 1;
                }
                break;
            case '?':
                if (optopt == 'a' || optopt == 'b' || optopt == 'c' || optopt == 'd' || optopt == 'e' || optopt == 'f' || optopt == 'g' || optopt == 'h' || optopt == 'p') {
                    std::cerr << "Option -" << static_cast<char>(optopt) << " requires an argument." << std::endl;
                } else {
                    std::cerr << "Unknown option: -" << static_cast<char>(optopt) << std::endl;
                }
                exit(0);
            default:
                exit(1);
        }
    }

    return;
}

// main program function
int main(int argc, char** argv){
    int data[9] = {0};
    parseArgs(argc, argv, data);

    // create a seed for rand
    srand(time(0));

    // extended lattice used for Solution tests
    Point trafficSystemRevised[HEIGHT+1][WIDTH] = {
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C},           {ROAD, F},           {ROAD, F},           {ROAD, F}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C},           {ROAD, H},           {ROAD, H},           {ROAD, H}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C},           {ROAD, G},           {ROAD, G},           {ROAD, G}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}}
    };

    // basic road network lattice used in normal simulation
    Point trafficSystem[HEIGHT][WIDTH] = {
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C},           {ROAD, F},           {ROAD, F},           {ROAD, F}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A},           {ROAD, B}, {ROAD, C},           {ROAD, G},           {ROAD, G},           {ROAD, G}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}},
    {{ROAD, A}, {EMPTY, UNASSIGNED}, {ROAD, C}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {EMPTY, UNASSIGNED}, {ROAD, D}, {ROAD, E}}
    };

    // number of iterations 
    // iteration simulates one second 
    // entered interval is in minutes
    int tempTimer = 60*data[8];

    // main simulation loop
    // debug functions are commented and include printing and waiting for a live overview of the system
    while(tempTimer){
        //std::this_thread::sleep_for(std::chrono::milliseconds(400));
        moveCars(Prediction == 1 ? trafficSystemRevised : trafficSystem, tempTimer);
        createCars(Prediction == 1 ? trafficSystemRevised : trafficSystem, tempTimer, data);
        //printLattice(Prediction == 1 ? trafficSystemRevised : trafficSystem);
        --tempTimer;
        //std::cout << std::endl;
    }

    // prints the final stats
    stats.print();
}