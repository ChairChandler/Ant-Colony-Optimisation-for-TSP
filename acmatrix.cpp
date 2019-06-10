#include "acmatrix.h"
#include "ant.h"
#include <cmath>
#include <algorithm>
#include <iostream>

ACMatrix::ACMatrix(std::vector<Point> &points, double coefficientEvaporation):
    MATRIX_SIZE(points.size()), COEFFICIENT_EVAPORATION(coefficientEvaporation)
{
    matrix.resize(MATRIX_SIZE);

    for(size_t i=0; i<MATRIX_SIZE; i++)
    {
        matrix[i].resize(MATRIX_SIZE);

        for(size_t j=i+1; j<MATRIX_SIZE; j++)
        {
            matrix[i][j]=distance(points[i], points[j]);
        }
    }

    resetPheromones();
}

double ACMatrix::distance(Point &a, Point &b)
{
    return sqrt(pow( (a.x-b.x), 2) + pow((a.y-b.y), 2));
}

void ACMatrix::resetPheromones()
{
    for(size_t i=0; i<MATRIX_SIZE; i++)
    {
        for(size_t j=0; j<i; j++)
        {
            matrix[i][j]=0;
        }
    }
}

size_t ACMatrix::getMatrixSize() const
{
    return MATRIX_SIZE;
}

ACMatrix::Edge ACMatrix::getEdge(size_t pointA, size_t pointB)
{
    if(pointA==pointB)
        throw ACMatrix::ExcEqualIndexes();
    else
    {
        size_t min, max;
        Edge edge;

        min=std::min(pointA, pointB);
        max=std::max(pointA, pointB);

        edge.distance=matrix[min][max];
        edge.pheromones=matrix[max][min];

        return edge;
    }
}

void ACMatrix::actualiseAllPheromones(std::vector<Ant> &ants)
{
    for(size_t vertex=0; vertex<MATRIX_SIZE; vertex++)
    {
        for(size_t nextVertex=vertex+1; nextVertex<MATRIX_SIZE; nextVertex++)
        {
            try
            {
                actualiseEdgePheromones(ants, vertex, nextVertex);
            }
            catch (ExcEqualIndexes &exc)
            {
                std::cout<<__FILE__<<" "<<__LINE__<<" "<<exc.what();
                exit(EXIT_FAILURE);
            }
        }
    }
}

void ACMatrix::actualiseEdgePheromones(std::vector<Ant> &ants, size_t pointA, size_t pointB)
{
    if(pointA==pointB)
        throw ACMatrix::ExcEqualIndexes();
    else
        calculatePheromones(ants, pointA, pointB);
}

void ACMatrix::calculatePheromones(std::vector<Ant> &ants, size_t pointA, size_t pointB)
{
    size_t min, max;

    min=std::min(pointA, pointB);
    max=std::max(pointA, pointB);

    matrix[max][min]=(1-COEFFICIENT_EVAPORATION) * matrix[max][min] + reciprocalDistanceSum(ants, pointA, pointB);
}

double ACMatrix::reciprocalDistanceSum(std::vector<Ant> &ants, size_t pointA, size_t pointB)
{
    double sum(0), antDistance;

    for(size_t i=0; i<ants.size(); i++)
    {
        if(ants[i].isEdge(pointA, pointB)==true)
        {
            antDistance=ants[i].getReachedDistance();

            if(antDistance > 0.0)
            {
                sum += 1/antDistance;
            }
        }
    }

    return sum;
}
