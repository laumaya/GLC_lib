/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9, packaged on Novemeber, 2005.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*****************************************************************************/

//! \file glc_geometrie.h Interface for the GLC_Geometrie class.

#ifndef GLC_GEOMETRIE_H_
#define GLC_GEOMETRIE_H_

#include "glc_object.h"
#include "glc_material.h"

//////////////////////////////////////////////////////////////////////
//! \class GLC_Geometrie
/*! \brief GLC_Geometrie : parent class for all GLC class which contain
 *  geometrical data*/

/*! GLC_Geometrie is a abstract class. \n \n
 *  Main attributes of GLC_Geometrie:
 *		- Material : 	GLC_Material
 * 		- Graphic properties
 * 		- Transformation Matrix
 * 
 * GLC_Geometrie provide :
 * 		- Function to create OpenGL list : GLC_Geometrie::CreationListe
 * 		- Function to draw Geometrie : GLC_Geometrie::GlExecute
 * 		- Virtual function to overload for visual property: GLC_Geometrie::GlPropGeom
 * 		- Virtual function to overload for Object topologie: GLC_Geometrie::GlDraw
 *
 * \todo Add link to a sample inherit class.
 */
//////////////////////////////////////////////////////////////////////

class GLC_Geometrie :
	public GLC_Object
{
//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////
public:
	GLC_Geometrie(const char *pName, const GLfloat *pColor);
	virtual ~GLC_Geometrie(void);

//////////////////////////////////////////////////////////////////////
// Get Function
//////////////////////////////////////////////////////////////////////
public:

	//! Return Visibility state of geometrie
	const bool GetVisible(void) const
	{
		return m_bVisible;
	}

	//! Return an array of 4 GLfloat which represent the color
	const GLfloat *GetfRGBA(void) const
	{
		return m_fColor;
	}	
	//! Return Color Red component
	GLfloat GetfRouge(void) const
	{
		return m_fColor[0];
	}
	//! Return Color Green component
	GLfloat GetfVert(void) const
	{
		return m_fColor[1];
	}
	//! Return Color blue component
	GLfloat GetfBleu(void) const
	{
		return m_fColor[2];
	}
	//! Return Color Alpha component
	GLfloat GetfAlpha(void) const
	{
		return m_fColor[3];
	}
	//! Return transfomation 4x4Matrix
	const GLC_Matrix4x4 GetMatrice(void) const
	{
		return m_MatPos;
	}
	//! Return thickness
	const float GetEpaisseur(void) const
	{
		return m_Epaisseur;
	}
	//! Return associated OpenGL list ID
	GLuint GetListeID(void)
	{
		return m_ListeID;
	}

	//! Return Validity of associated OpenGL list
	bool GetListeValidite(void) const
	{
		return m_bListeIsValid;
	}

	//! Return Validity of geometry
	bool GetValidite(void) const
	{
		return (m_bValidite && m_bListeIsValid);
	}

	//! Return material of geometry
	GLC_Material* GetMat(void)
	{
		return m_pMatiere;
	}

	//! Return true if blending is enable
	bool GetBlending(void) const
	{
		return m_bBlending;
	}

/////////////////////////////////////////////////////////////////////
// Set function
//////////////////////////////////////////////////////////////////////
public:
	//! Set visibility statement
	void SetVisible(bool v)
	{
		m_bVisible= v;

		m_bValidite= false;
	}

	//! Set Color RGBA component
	void SetfRGBA(GLfloat Rouge, GLfloat Vert, GLfloat Bleu, GLfloat Alpha= 1)
	{
		m_fColor[0]= Rouge;
		m_fColor[1]= Vert;
		m_fColor[2]= Bleu;
		m_fColor[3]= Alpha;

		m_bValidite= false;		
	}

	//! Set Color RGBA component with an array of 4 GLfloat
	void SetfRGBA(const GLfloat* SetCol)	// SetCol[4]
	{
		m_fColor[0]= SetCol[0];
		m_fColor[1]= SetCol[1];
		m_fColor[2]= SetCol[2];
		m_fColor[3]= SetCol[3];

		m_bValidite= false;		
	}


// Set Position	

	//! Translate Geometry
	void Translate(double Tx, double Ty, double Tz);

	//! Move Geometry with à 4x4Matrix
	void MultMatrice(const GLC_Matrix4x4 &MultMat)
	{
		m_MatPos= MultMat * m_MatPos;

		m_bValidite= false;
	}
	
	//! Replace the Geometry Matrix
	void SetMatrice(const GLC_Matrix4x4 &SetMat)
	{
		m_MatPos= SetMat;

		m_bValidite= false;
	}
	
	//! Reset the Geometry Matrix
	void ResetMatrice(void)
	{
		m_MatPos.SetIdentite();

		m_bValidite= false;
	}
	
	//! Set Wire thickness
	void SetEpaisseur(float SetEp)
	{
		SetEp= fabs(SetEp);		
		m_Epaisseur= SetEp;

		m_bValidite= false;
	}

	//! Set Blending
	void SetBlending(bool Blending)
	{
		m_bBlending= Blending;

		m_bValidite= false;

	}

	// Material
	//! Set the Geometry material
	void SetMatiere(GLC_Material* pMat);
	
	//! Remove Geometry from the material without update material usage table
	/*! This function is call by GLC_Material, do not call it manualy ! */
	void DelMatiere(GLC_Geometrie* pGeom);


//////////////////////////////////////////////////////////////////////
// OpenGL Function
//////////////////////////////////////////////////////////////////////
public:
	//! Virtual interface for OpenGL execution from GLC_Object.
	/*! This Virtual function is implemented here.\n
	 * At the first call, this function call virtual function
	 * GLC_Geometrie::GlPropGeom and set up :
	 * 		- Geometry
	 * 		- OpenGL list
	 * .
	 * AfterWard this function
	 *		- Call virtual function GLC_Geometrie::GlPropGeom
	 * 		- If Display list exist or is Valid Call Display List
	 * 		- If Display list doesn't exist try to create IT by calling
	 *        virtual function GLC_Geometrie::GlDraw
	 */
	virtual bool GlExecute(GLenum Mode= GL_COMPILE_AND_EXECUTE);

	//! OpenGL list creation
	//! \todo Make it private
	bool CreationListe(GLenum Mode= GL_COMPILE_AND_EXECUTE);

protected:
	//! Virtual interface for OpenGL Geometry set up.
	/*! This Virtual function have to be implemented in concrete class.*/
	virtual void GlDraw(void) = 0;

	//! Virtual interface for OpenGL Geomtry properties.
	/*! This Virtual function have to be implemented in concrete class.*/
	virtual void GlPropGeom(void) = 0;

//////////////////////////////////////////////////////////////////////
// Protected services functions
//////////////////////////////////////////////////////////////////////
protected:

	//! Delete OpenGL list
	/*! Call by GLC_Geometrie::~GLC_Geometrie*/
	void DeleteListe()
	{
		// Si la liste d'affichage est valide, elle est supprimé
		if (glIsList(m_ListeID))
		{
			glDeleteLists(m_ListeID, 1);
		}
	//! \todo Add error handler
	}

//////////////////////////////////////////////////////////////////////
// Membres protégés
//////////////////////////////////////////////////////////////////////
protected:
	GLC_Matrix4x4	m_MatPos;

	// Identifiant de la liste d'affichage
	GLuint m_ListeID;

	// Validité de la liste
	bool m_bListeIsValid;

	// Validite de la geometrie
	bool m_bValidite;

	// Matiere
	GLC_Material* m_pMatiere;

	// Transparence
	bool m_bBlending;

//////////////////////////////////////////////////////////////////////
// Membres privés
//////////////////////////////////////////////////////////////////////

private:

	// Couleur de géométrie RGBA
	GLfloat	m_fColor[4];

	// Epaisseur des edges de la géométrie
	float m_Epaisseur;
	
	// Visibilité de la géométrie
	bool m_bVisible;



};
#endif //GLC_GEOMETRIE_H_
