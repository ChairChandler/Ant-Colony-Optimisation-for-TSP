#ifndef ANT_H
#define ANT_H
#include <vector>
#include <stdlib.h>

class ACMatrix;

class Ant
{
private:
    ACMatrix &matrix;
    std::vector<bool> isVertexVisited;
    std::vector<size_t> VisitedVertexList;
    size_t actualVertex;
    double reachedDistance;
    const size_t START_VERTEX;
    const double ALFA_PARAMETER, BETA_PARAMETER;

    void moveToVertex(size_t vertex);
    double denominatorSum();
    bool isSolution();
    size_t findNextVertex();

    class VertexProbability
    {
    private:
        double calculateNumerator(size_t vertex);
        double calculateDenominator();
        Ant &upper;

    public:
        VertexProbability(Ant &upper);
        double calculate(size_t vertex);
    };

    class ExcNoNextVertex: std::exception
    {
    public:
        const char* what() const noexcept
        {
            return "Next vertex not found";
        }
    };

    size_t freeVertices();
    size_t randVertex();

public:
    Ant(size_t startVertex, ACMatrix &matrix, double ALFA_PARAMETER, double BETA_PARAMETER);
    void findSolution();
    double getReachedDistance();
    std::vector<size_t> getVisitedVertex() const;
    bool isEdge(size_t pointA, size_t pointB);
    void reset();
};

#endif // ANT_H
