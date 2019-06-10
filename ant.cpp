#include "ant.h"
#include "acmatrix.h"
#include <cmath>
#include <iostream>
#include <float.h>
#include <ctime>

Ant::Ant(size_t startVertex, ACMatrix &matrix, double parameterAlfa, double parameterBeta):
    matrix(matrix), START_VERTEX(startVertex),ALFA_PARAMETER(parameterAlfa), BETA_PARAMETER(parameterBeta)
{
    srand(_time64(NULL));
    this->isVertexVisited.resize(this->matrix.getMatrixSize());
    reset();
}

void Ant::findSolution()
{
    size_t nextVertex;

    while(true)
    {
        try
        {
            nextVertex=findNextVertex();
            moveToVertex(nextVertex);
        }
        catch (ExcNoNextVertex)
        {
            if(isSolution()==false)
            {
                moveToVertex(randVertex());
            }
            else
            {
                moveToVertex(START_VERTEX);
                break;
            }
        }
    }
}

bool Ant::isSolution()
{
    for(auto vertex: isVertexVisited)
    {
        if(vertex==false)
            return false;
    }

    return true;
}

size_t Ant::freeVertices()
{
    size_t free(0);

    for(auto vertex: isVertexVisited)
    {
        if(vertex==false)
            free++;
    }

    return free;
}

size_t Ant::randVertex()
{
    /*size_t selectedVertex(0);
    std::vector<size_t> unvisitedVertex;

    for(size_t vertex=0; vertex < isVertexVisited.size(); vertex++)
    {
        if(isVertexVisited[vertex]==true)
        {
            unvisitedVertex.push_back(vertex);
        }
    }

    if(unvisitedVertex.size() > 1)
    selectedVertex=unvisitedVertex[static_cast<size_t>(rand())%unvisitedVertex.size()-1];

    return selectedVertex;*/

    double min(DBL_MAX);
    ACMatrix::Edge edge;
    size_t selectedVertex(START_VERTEX), randomNotSelectedVertex(START_VERTEX), worstNotSelectedVerted(START_VERTEX);

    for(size_t vertex=0; vertex < isVertexVisited.size(); vertex++)
    {
        if(isVertexVisited[vertex]==true)
            continue;
        else
        {
            edge=matrix.getEdge(actualVertex, vertex);
            if(edge.distance < min && (static_cast<size_t>(rand()) % freeVertices() == 0))
            {
                if(static_cast<size_t>(rand()) % freeVertices() == 0)
                {
                    min=edge.distance;
                    selectedVertex=vertex;
                }
                else
                {
                    randomNotSelectedVertex=vertex;
                }
            }
            else
            {
                worstNotSelectedVerted=vertex;
            }
        }
    }

    if(selectedVertex != START_VERTEX)
        return selectedVertex;
    else if(randomNotSelectedVertex != START_VERTEX)
        return randomNotSelectedVertex;
    else
        return worstNotSelectedVerted;
}

size_t Ant::findNextVertex()
{
    double probabilityToSelect, maxProbability(0);
    VertexProbability probability(*this);
    size_t vertexToSelect(START_VERTEX);

    for(size_t vertex=0; vertex<isVertexVisited.size(); vertex++)
    {
        if(isVertexVisited[vertex]==true)
            continue;
        else
        {
            probabilityToSelect=probability.calculate(vertex);
            if(probabilityToSelect > maxProbability)
            {
                maxProbability=probabilityToSelect;
                vertexToSelect=vertex;
            }
        }
    }

    if(maxProbability == 0.0)
        throw ExcNoNextVertex();
    else
        return vertexToSelect;
}

void Ant::moveToVertex(size_t vertex)
{
    try
    {
        reachedDistance += matrix.getEdge(actualVertex, vertex).distance;

        isVertexVisited[vertex]=true;
        actualVertex=vertex;
        VisitedVertexList.push_back(vertex);
    }
    catch (ACMatrix::ExcEqualIndexes &exc)
    {
        std::cout<<__FILE__<<" "<<__LINE__<<" "<<exc.what();
        exit(EXIT_FAILURE);
    }

}

double Ant::getReachedDistance()
{
    return reachedDistance;
}

std::vector<size_t> Ant::getVisitedVertex() const
{
    return VisitedVertexList;
}

void Ant::reset()
{
    for(auto vertex: isVertexVisited)
    {
        vertex=false;
    }

    this->actualVertex=START_VERTEX;
    this->reachedDistance=0;

    this->isVertexVisited[START_VERTEX]=true;
    this->VisitedVertexList.clear();
    this->VisitedVertexList.push_back(0);
}

bool Ant::isEdge(size_t pointA, size_t pointB)
{
    size_t size(VisitedVertexList.size() == 0? 0 : VisitedVertexList.size()-1);

    for(size_t vertex=0; vertex < size; vertex++)
    {
        if(VisitedVertexList[vertex]==pointA && VisitedVertexList[vertex+1]==pointB)
            return true;
    }

    return false;
}

Ant::VertexProbability::VertexProbability(Ant &upper): upper(upper) {}

double Ant::VertexProbability::calculate(size_t vertex)
{
    double dividend, divider;

    dividend=calculateNumerator(vertex);
    divider=calculateDenominator();

    if(dividend==0.0 || divider==0.0)
        return 0;
    else
    {
        return dividend / divider;
    }
}

double Ant::VertexProbability::calculateNumerator(size_t vertex)
{
    ACMatrix::Edge edge;
    double numerator;

    try
    {
        edge=upper.matrix.getEdge(upper.actualVertex, vertex);
        numerator=pow(edge.pheromones, upper.ALFA_PARAMETER) * pow( (1/edge.distance), 2);
    }
    catch (ACMatrix::ExcEqualIndexes &exc)
    {
        std::cout<<__FILE__<<" "<<__LINE__<<" "<<exc.what();
        exit(EXIT_FAILURE);
    }


    return numerator;
}

double Ant::VertexProbability::calculateDenominator()
{
    ACMatrix::Edge edge;
    double denominator(0);

    for(size_t unvisitedVertex=0; unvisitedVertex<upper.isVertexVisited.size(); unvisitedVertex++)
    {
        if(upper.isVertexVisited[unvisitedVertex]==false)
        {
            try
            {
                edge=upper.matrix.getEdge(upper.actualVertex, unvisitedVertex);
                denominator += pow(edge.pheromones, upper.ALFA_PARAMETER) * pow( (1/edge.distance), 2);
            }
            catch (ACMatrix::ExcEqualIndexes &exc)
            {
                std::cout<<__FILE__<<" "<<__LINE__<<" "<<exc.what();
                exit(EXIT_FAILURE);
            }
        }
    }

    return denominator;
}
