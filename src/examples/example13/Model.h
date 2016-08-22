/*
 *  Model.h
 *
 *  Created on: 22/08/2016
 *      Author: Laurent Ribon
 */
#ifndef MODEL_H
#define MODEL_H

#include <GLC_World>
#include <GLC_CsgNode>

class GLC_Matrix4x4;

class Model
{
public:
    Model();
    ~Model();

public:
    GLC_World world() const
    {return m_World;}

public:
    void updateChildMatrix(const GLC_Matrix4x4& matrix);
    void update();

private:
    void init();

private:
    GLC_World m_World;
    GLC_CsgNode* m_pRootCsgNode;
    GLC_CsgNode* m_pChild2Node;
};

#endif // MODEL_H
