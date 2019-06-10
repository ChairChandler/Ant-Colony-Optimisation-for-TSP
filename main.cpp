#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <float.h>
#include <string>
#include <ctime>
#include "point.h"
#include "ant.h"
#include "acmatrix.h"

static void main_init(int argc, char *argv[]);
static std::vector<Point> loadData();
static void loadData_loadVertices(std::vector<Point> &points, size_t numPoints, std::ifstream &in);
static void antColony(std::vector<Point> &points);
static void antColony_init(size_t startVertex, ACMatrix &matrix, std::vector<Ant> &ants);
static double antColony_findMin(ACMatrix &matrix, std::vector<Ant> &ants);
static double antColony_findMin_releaseAnts(std::vector<Ant> &ants);
static std::vector<size_t> antColony_findRoad(double min, std::vector<Ant> &ants);
static void antColony_printRoad(std::vector<size_t> &road);
static void antColony_saveResult(double min, std::vector<size_t> &road);

static size_t ITERATIONS = 5;
static size_t ANTS = 50;
static double COEFFICIENT_EVAPORATION = 0.2;
static double ALFA_PARAMETER = 0.03;
static double BETA_PARAMETER = 0.03;
static bool IS_SAVING_TO_FILE = false;
const double INF = DBL_MAX;


int main(int argc, char *argv[])
{
    main_init(argc, argv);

    std::vector<Point> points;
    points=loadData();
    antColony(points);

    return 0;
}

static void main_init(int argc, char *argv[])
{
    if(argc==6)
    {
        IS_SAVING_TO_FILE = true;

        ITERATIONS = static_cast<size_t>(atoi(argv[1]));
        ANTS = static_cast<size_t>(atoi(argv[2]));
        COEFFICIENT_EVAPORATION = atof(argv[3]);
        ALFA_PARAMETER = atof(argv[4]);
        BETA_PARAMETER = atof(argv[5]);
    }
}

static std::vector<Point> loadData()
{
    std::ifstream in("data.txt");

    if (!in.is_open())
    {
        std::cout<<"Could not open a file with tsp data!\n";
        exit(EXIT_FAILURE);
    }
    else
    {
        size_t numPoints;
        in>>numPoints;

        std::vector<Point> points;
        points.resize(numPoints);

        loadData_loadVertices(points, numPoints, in);

        return points;
    }
}

static void loadData_loadVertices(std::vector<Point> &points, size_t numPoints, std::ifstream &in)
{
    size_t index;

    for(size_t i=0; i<numPoints; ++i)
    {
        in>>index;
        in>>points[index - 1].x>>points[index - 1].y;
    }
}

static void antColony(std::vector<Point> &points)
{
    std::vector<Ant> ants;
    std::vector<size_t> road;
    ACMatrix matrix(points, COEFFICIENT_EVAPORATION);
    double min(INF);
    size_t startVertex;

    srand(_time64(NULL));
    startVertex=static_cast<size_t>(rand())%points.size();

    antColony_init(startVertex, matrix, ants);
    min=antColony_findMin(matrix, ants);
    road=antColony_findRoad(min, ants);

    std::cout<<"Min: "<<min<<std::endl;

    antColony_printRoad(road);

    if(IS_SAVING_TO_FILE == true)
        antColony_saveResult(min, road);
}

static void antColony_init(size_t startVertex, ACMatrix &matrix, std::vector<Ant> &ants)
{
    for(size_t i=0; i<ANTS; ++i)
    {
        ants.push_back(Ant(startVertex, matrix, ALFA_PARAMETER, BETA_PARAMETER));
    }
}

static double antColony_findMin(ACMatrix &matrix, std::vector<Ant> &ants)
{
    double min(INF), actualMin(INF);

    for(size_t i=0; i<ITERATIONS; i++)
    {
        actualMin=antColony_findMin_releaseAnts(ants);

        if(actualMin<min)
        {
            min=actualMin;
        }

        matrix.actualiseAllPheromones(ants);
    }

    return min;
}

static double antColony_findMin_releaseAnts(std::vector<Ant> &ants)
{
    double antRoadLength, min;
    min=INF;

    for(auto &itr: ants)
    {
        itr.reset();
        itr.findSolution();

        antRoadLength=itr.getReachedDistance();

        if(antRoadLength<min)
        {
            min=antRoadLength;
        }
    }

    return min;
}

static std::vector<size_t> antColony_findRoad(double min, std::vector<Ant> &ants)
{
    double epsilon(0.1);

    while(true)
    {
        for(auto &itr: ants)
        {
            if(fabs(min - itr.getReachedDistance()) < epsilon)
                return itr.getVisitedVertex();
        }

        epsilon += 0.1;
    }
}

static void antColony_printRoad(std::vector<size_t> &road)
{
    for(auto itr: road)
    {
        std::cout<<itr<<" ";
    }

    std::cout<<std::endl;
}

static void antColony_saveResult(double min, std::vector<size_t> &road)
{

    std::fstream file("results.txt", std::ios::app|std::ios::out);

    if(!file.is_open())
    {
        std::cout<< "Couldn't write result to file!"<< std::endl;
    }
    else
    {
        file<< ITERATIONS<< " ";
        file<< ANTS<< " ";
        file<< COEFFICIENT_EVAPORATION<< " ";
        file<< ALFA_PARAMETER<< " ";
        file<< BETA_PARAMETER<< " ";
        file<< min<< " ";

        for(auto &itr: road)
        {
            file<< itr<< " ";
        }

        file<< std::endl;

        file.close();
    }
}
