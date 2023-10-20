/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2013 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_extrudedmesh.h Interface for the GLC_ExtrudedMesh class.

#ifndef GLC_EXTRUDEDMESH_H
#define GLC_EXTRUDEDMESH_H

#include <QList>
#include <QMap>

#include "glc_mesh.h"
#include "../maths/glc_vector3d.h"
#include "../maths/glc_polygon.h"

#include "../glc_config.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_ExtrudedMesh
/*! \brief GLC_ExtrudedMesh : An Extruded mesh defined by a list of points,
 *a direction and a distance*/
//////////////////////////////////////////////////////////////////////

class GLC_LIB_EXPORT GLC_ExtrudedMesh : public GLC_Mesh
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
    //! Default constructor
    GLC_ExtrudedMesh(const QList<GLC_Point3d>& points, const GLC_Vector3d& dir, double length, bool mirroredExtend= false);

    //! Copy constructor
    GLC_ExtrudedMesh(const GLC_ExtrudedMesh& other);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Return the class Chunk ID
    static quint32 chunckID();

    //! Return the list of points which defined the face to extrude
    QList<GLC_Point3d> facePoints() const
    {return m_Points;}

    //! Return the extrusion vector
    GLC_Vector3d extrusionVector() const
    {return m_ExtrusionVector;}

    //! Return the extrusion length
    double extrusionLength() const
    {return m_ExtrusionLength;}

    //! Return a copy of the extruded mesh
    GLC_Geometry* clone() const override;

    //! Return the extruded mesh bounding box
    const GLC_BoundingBox& boundingBox(void) override;

    GLC_uint masterMaterialId() const
    {return m_MasterMaterialId;}

    GLC_Material* masterMaterial() const;

    float textureRangeFactor() const
    {return m_TextureRangeFactor;}

    QMap<int, GLC_Polygon> polygons() const;

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    bool update() override;

    //! Assignement operator overload
    GLC_ExtrudedMesh& operator=(const GLC_ExtrudedMesh& other);

    //! Set the mesh from the given points, vector and length
    void setGeometry(const QList<GLC_Point3d>& points, const GLC_Vector3d& extrudedVector, double length);

    //! Set the mesh points list
    void setPoints(const QList<GLC_Point3d>& points);

    //! Set the mesh extruded vector
    void setExtrudedVector(const GLC_Vector3d& extrudedVector);

    //! Set the mesh extruded length
    void setExtrudedLength(double length);

    //! Set Smothing index
    void setSmoothingIndex(const QList<int>& smothingIndex);

    void setInvisibleEdgeIndex(const QList<int>& invisibleEdgeIndex);

    void setMasterMaterial(const GLC_Material& material);

    void setFaceMappingAndMaterial(const QMap<int, int>& mapping);

    QMap<int, GLC_uint> edgeMaterialId() const
    {return m_FaceToMaterialId;}

    void setTextureRangeFactor(double value);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
protected:
    //! Virtual interface for OpenGL Geometry set up.
    /*! This Virtual function have to be implemented in concrete class.*/
    void glDraw(const GLC_RenderProperties& renderProperties) override;

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
    //! Create the extruded mesh mesh and wire
    void createMeshAndWire();

    //! Create the extruded mesh mesh
    void createMesh();

    //! Create the extruded mesh wire
    void createWire();

    //! compute the given face normal
    void computeGivenFaceNormal();

    //! Return true if the list of points lie on a plane
    bool pointsLieOnAPlane() const;

    //! Return base outline normmals
    GLfloatVector baseOutlineNormals() const;

    //! Return created outline normmals
    GLfloatVector createdOutlineNormals() const;

    //! Return base face vertices
    GLfloatVector baseFaceVertices() const;

    //! Return base face texel
    GLfloatVector baseFaceTexels() const;

    //! Return base outline faces vertices
    GLfloatVector baseOutlineFacesVertices() const;

    //! Return base outline faces texels
    GLfloatVector basedOutlineFacesTexels() const;

    //! Return the base face normals
    GLfloatVector baseFaceNormals() const;

    //! Return the list of points of the base face
    QList<GLC_Point3d> baseFacePoints() const;

    //! Return the list of points of the created face
    QList<GLC_Point3d> createdFacePoints() const;

    //! Return the created face normals
    GLfloatVector createdFaceNormals() const;

    //! Return created face vertices
    GLfloatVector createdFaceVertices() const;

    //! Return created face texels
    GLfloatVector createdFaceTexels() const;

    //! Return created outline faces vertices
    GLfloatVector createdOutlineFacesVertices() const;

    //! Return created outline faces texels
    GLfloatVector createdOutlineFacesTexels() const;

    void copyEdgeMaterialId(const GLC_ExtrudedMesh& other);

    GLC_Material* faceOutlineMaterial(int face) const;

    void createPolygons();

    int face(int i);

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
    //! The list of point which define the face to extrude
    QList<GLC_Point3d> m_Points;

    //! The list of index of smoothing points
    QList<int> m_SmothingPoints;

    QList<int> m_InvisibleEdgeIndex;

    //! The direction of extrusion
    GLC_Vector3d m_ExtrusionVector;

    //! The extrusion length
    double m_ExtrusionLength;

    //! The normal of the given face
    GLC_Vector3d m_GivenFaceNormal;

    bool m_MirroredExtend;

    //! Class chunk id
    static quint32 m_ChunkId;

    GLC_uint m_MasterMaterialId;

    QMap<int, GLC_uint> m_FaceToMaterialId;

    QMap<int, int> m_FaceMapping;

    float m_TextureRangeFactor;

    QMap<int, GLC_Polygon> m_FaceToPolygons;

};

#endif // GLC_EXTRUDEDMESH_H
