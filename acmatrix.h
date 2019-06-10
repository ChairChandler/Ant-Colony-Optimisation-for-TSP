#ifndef ACMATRIX_H
#define ACMATRIX_H
#include "point.h"
#include <vector>
#include <stdlib.h>

class Ant;

class ACMatrix
{
private:
    std::vector<std::vector<double>> matrix;
    const size_t MATRIX_SIZE;
    const double COEFFICIENT_EVAPORATION;

    void calculatePheromones(std::vector<Ant> &ants, size_t pointA, size_t pointB);
    void actualiseEdgePheromones(std::vector<Ant> &ants, size_t pointA, size_t PointB);
    double distance(Point &a, Point &b);
    double reciprocalDistanceSum(std::vector<Ant> &ants, size_t pointA, size_t pointB);

public:

    class ExcEqualIndexes: public std::exception
    {
    public:
        const char* what() const noexcept
        {
            return "No exist edge to the same point";
        }
    };

    struct Edge
    {
        double distance;
        double pheromones;
    };

    ACMatrix(std::vector<Point> &points, double COEFFICIENT_EVAPORATION);

    void resetPheromones();
    size_t getMatrixSize() const;
    Edge getEdge(size_t pointA, size_t pointB);
    void actualiseAllPheromones(std::vector<Ant> &ants);
};

#endif // ACMATRIX_H
