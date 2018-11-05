/*
 *  glc_lathemesh.h
 *
 *  Created on: 30/10/2018
 *      Author: Laurent Ribon
 */
#ifndef GLC_LATHEMESH_H
#define GLC_LATHEMESH_H

#include "../glc_config.h"

#include "../maths/glc_vector3d.h"

#include "glc_mesh.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_LatheMesh
/*! \brief GLC_LatheMesh : A lathe (Z axis) mesh defined by a profile,
 *  start angle, end angle*/
//////////////////////////////////////////////////////////////////////

class GLC_LIB_EXPORT GLC_LatheMesh : public GLC_Mesh
{
//////////////////////////////////////////////////////////////////////
/*! @name Constructor / Destructor */
//@{
//////////////////////////////////////////////////////////////////////

public:
    GLC_LatheMesh(const QList<GLC_Point3d>& profile);
    GLC_LatheMesh(const GLC_LatheMesh& other);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Get Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    //! Return the class Chunk ID
    static quint32 chunckID();

    QList<GLC_Point3d> profile() const
    {return m_Profile;}

    int discretion() const
    {return m_Discret;}

    //! Return the extruded mesh bounding box
    const GLC_BoundingBox& boundingBox(void) override;

//@}

//////////////////////////////////////////////////////////////////////
/*! \name Set Functions*/
//@{
//////////////////////////////////////////////////////////////////////
public:
    bool update() override;

    GLC_LatheMesh& operator=(const GLC_LatheMesh& other);

    void setGeometry(const QList<GLC_Point3d>& profile, double phiStart, double phiLength);

    void setProfile(const QList<GLC_Point3d>& profile);

    void setDiscret(double discret);

    void setSmoothingIndex(const QList<int>& smothingIndex);

    void setInvisibleEdgeIndex(const QList<int>& invisibleEdgeIndex);

//@}

//////////////////////////////////////////////////////////////////////
/*! \name OpenGL Functions*/
//@{
//////////////////////////////////////////////////////////////////////
protected:
    void glDraw(const GLC_RenderProperties& renderProperties) override;

//@}


//////////////////////////////////////////////////////////////////////
/*! \name Private services Functions*/
//@{
//////////////////////////////////////////////////////////////////////
private:
    void createMeshAndWire();

    void createMesh();

    void createWire();

    void createCircle(int index);

    QList<GLC_Vector3d> profileNormals() const;
    QList<GLC_Point3d> profileVertices() const;
    QList<double> profileV();

//@}

//////////////////////////////////////////////////////////////////////
// Private members
//////////////////////////////////////////////////////////////////////
private:
    QList<GLC_Point3d> m_Profile;

    QList<int> m_SmothingPoints;

    QList<int> m_InvisibleEdgeIndex;

    double m_PhiStart;

    double m_PhiLength;

    int m_Discret;

    int m_Step;

    static const quint32 m_ChunkId;
};

#endif // GLC_LATHEMESH_H
