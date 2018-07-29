//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//    .--------------------------------------------------.
//    |  This file is part of NTGraphics                 |
//    |  Created 2018 by NT (https://ttnghia.github.io)  |
//    '--------------------------------------------------'
//                            \o/
//                             |
//                            / |
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma once

#include <array>

#include <LibOpenGL/MeshObjects/MeshObject.h>

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
class WireFrameBoxObject : public MeshObject
{
public:
    WireFrameBoxObject() { generateBox(); }

    void generateBox()
    {
        clearData();

        m_Vertices = {
            // top
            Vec3f(  -1.0, 1.0,    1.0),
            Vec3f(1.0,    1.0,    1.0),
            Vec3f(1.0,    1.0,      -1.0),
            Vec3f(  -1.0, 1.0,      -1.0),
            // bottom
            Vec3f(  -1.0,   -1.0, 1.0),
            Vec3f(1.0,      -1.0, 1.0),
            Vec3f(1.0,      -1.0,   -1.0),
            Vec3f(  -1.0,   -1.0,   -1.0)
        };

        m_IndexList = {
            // top
            0, 1,
            1, 2,
            2, 3,
            3, 0,
            // bottom
            4, 5,
            5, 6,
            6, 7,
            7, 4,
            // sides
            0, 4,
            1, 5,
            2, 6,
            3, 7
        };

        ////////////////////////////////////////////////////////////////////////////////
        m_NVertices      = 8;
        m_isDataReady    = true;
        m_hasIndexBuffer = true;
        m_DataTopology   = GL_LINES;
    }

    void setBox(const Vec3f& boxMin, const Vec3f& boxMax)
    {
        m_Vertices = {
            // top
            Vec3f(boxMin[0], boxMax[1], boxMax[2]),
            Vec3f(boxMax[0], boxMax[1], boxMax[2]),
            Vec3f(boxMax[0], boxMax[1], boxMin[2]),
            Vec3f(boxMin[0], boxMax[1], boxMin[2]),
            // bottom
            Vec3f(boxMin[0], boxMin[1], boxMax[2]),
            Vec3f(boxMax[0], boxMin[1], boxMax[2]),
            Vec3f(boxMax[0], boxMin[1], boxMin[2]),
            Vec3f(boxMin[0], boxMin[1], boxMin[2])
        };
    }
};