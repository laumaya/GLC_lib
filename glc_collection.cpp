/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2006 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 0.9.5, packaged on February, 2006.

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_collection.cpp implementation of the GLC_Collection class.

#include <QtDebug>

#include "glc_collection.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLC_Collection::GLC_Collection()
: m_ListID(0)
, m_ListIsValid(false)
, m_pBoundingBox(NULL)
{
}

GLC_Collection::~GLC_Collection()
{
	Erase();
}
//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////
// Ajoute une géométrie à la collection
bool GLC_Collection::AddGLC_Geom(GLC_Geometry* pGeom)
{
	CGeomMap::iterator iGeom= m_TheMap.find(pGeom->GetID());
	
	if (iGeom == m_TheMap.end())
	{	// Ok, la clé n'est pas prise
		// Create an GLC_CollectionNode
		GLC_CollectionNode* p_CollectionNode= new GLC_CollectionNode(pGeom);
		// Add the collection Node
		m_TheMap.insert(pGeom->GetID(), p_CollectionNode);
		
		qDebug("GLC_Collection::AddGLC_Geom : Element Ajouté avec succès");
		
		// Validité de la liste
		m_ListIsValid= false;
		return true;
		
	}
	else
	{	// KO, la clé est prise
		qDebug("GLC_Collection::AddGLC_Geom : Element already in collection");
		return false;
	}
	
}

// Supprime une géométrie de la collection et la géométrie
bool GLC_Collection::DelGLC_Geom(GLC_uint Key)
{

	CGeomMap::iterator iGeom= m_TheMap.find(Key);
		
	if (iGeom != m_TheMap.end())
	{	// Ok, the key exist
		delete iGeom.value();		// delete the collection Node
		m_TheMap.remove(Key);		// Delete the conteneur
		// Search the list
			
		// Validité de la liste
		m_ListIsValid= false;
		
		qDebug("GLC_Collection::DelGLC_Geom : Element succesfuly deleted");
		return true;
		
	}
	else
	{	// KO, key doesn't exist
		qDebug("GLC_Collection::DelGLC_Geom : Element not deleted");
		return false;
	}
	
}

// Supprime une géométrie de la collection
bool GLC_Collection::RemGLC_Geom(GLC_uint Key)
{
	CGeomMap::iterator iGeom= m_TheMap.find(Key);
		
	if (iGeom != m_TheMap.end())
	{	// Ok, la clé existe
		// On ne Supprime pas la géométrie
		m_TheMap.remove(Key);		// Supprime le conteneur
			
		// Validité de la liste
		m_ListIsValid= false;
		
		//qDebug("GLC_Collection::RemGLC_Geom : Element Supprimé avec succès");
		return true;
		
	}
	else
	{	// KO, key doesn't exist
		qDebug("GLC_Collection::RemGLC_Geom : Element not deleted");
		return false;
	}
	
}


// Vide la collection
void GLC_Collection::Erase(void)
{
	// Suppression des géométries
	CGeomMap::iterator iEntry= m_TheMap.begin();
	
    while (iEntry != m_TheMap.constEnd())
    {
        // Supprime l'objet        
        delete iEntry.value();
        ++iEntry;
    }
    // Vide la table de hachage principale
    m_TheMap.clear();
	
	// Fin de la Suppression des géométries

	// Supprime la liste d'affichage
	DeleteList();
	// Fin des Suppressions des sous listes d'affichages
	
	// delete the boundingBox
	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}
	
}

// Retourne le pointeur d'un élément de la collection
GLC_Geometry* GLC_Collection::GetElement(GLC_uint Key)
{
	CGeomMap::iterator iGeom= m_TheMap.find(Key);
	
	if (iGeom != m_TheMap.end())
	{	// Ok, la clé est trouvé
		return iGeom.value()->getGeometry();
	}
	else
	{	// KO, la clé n'est pas trouvé
		return NULL;
	}
}

// Retourne le pointeur d'un élément de la collection
GLC_Geometry* GLC_Collection::GetElement(int Index)
{
	// Warning, performance will be poor
	int CurrentPos= 0;
	CGeomMap::iterator iEntry= m_TheMap.begin();
	GLC_Geometry* pGeom= NULL;
	
    while ((iEntry != m_TheMap.constEnd()) && (CurrentPos <= Index ))
    {
        // retrieve the object        
        if(CurrentPos == Index) pGeom= iEntry.value()->getGeometry();
        ++iEntry;
        ++CurrentPos;
    }
 	
 	return pGeom;
}

//! return the collection Bounding Box
GLC_BoundingBox GLC_Collection::getBoundingBox(void) const
{
	if (m_pBoundingBox != NULL)
	{
		return *m_pBoundingBox;
	}
	else
	{
		GLC_BoundingBox boundingBox;
		return boundingBox;
	}
}


//////////////////////////////////////////////////////////////////////
// Fonctions OpenGL
//////////////////////////////////////////////////////////////////////
void GLC_Collection::GlExecute(void)
{
	//qDebug() << "GLC_Collection::GlExecute";
	if (GetNumber() > 0)
	{
		CreateMemberLists();		// Si nécessaire

		//CreateSubLists();		// Si nécessaire

		if (m_ListIsValid)
		{	// La liste de la collection OK
			glCallList(m_ListID);
		}
		else
		{
			if(MemberIsUpToDate())
			{
				CreateList();
			}
			else
			{
				m_ListIsValid= false;
				qDebug("GLC_Collection::GlExecute : CreatMemberList KO -> display list not use");
			}
		}

		// Gestion erreur OpenGL
		GLenum errCode;
		if ((errCode= glGetError()) != GL_NO_ERROR)
		{
			const GLubyte* errString;
			errString = gluErrorString(errCode);
			qDebug("GLC_Collection::GlExecute OPENGL ERROR %s", errString);
		}
	}
}

// Affiche les éléments de la collection
void GLC_Collection::GlDraw(void)
{
	CGeomMap::iterator iEntry= m_TheMap.begin();
	if (m_pBoundingBox != NULL)
	{
		delete m_pBoundingBox;
		m_pBoundingBox= NULL;
	}
	m_pBoundingBox= new GLC_BoundingBox();
	
    while (iEntry != m_TheMap.constEnd())
    {
        iEntry.value()->GlExecute();
        // Combine Collection BoundingBox with element Bounding Box
        m_pBoundingBox->combine(iEntry.value()->getBoundingBox());
        
        ++iEntry;
    }
	

	// Gestion erreur OpenGL
	GLenum errCode;
	if ((errCode= glGetError()) != GL_NO_ERROR)
	{
		const GLubyte* errString;
		errString = gluErrorString(errCode);
		qDebug("GLC_Collection::GlDraw OPENGL ERROR %s", errString);
	}

}

// Création des listes d'affichages des membres
void GLC_Collection::CreateMemberLists(void)
{
	CGeomMap::iterator iEntry= m_TheMap.begin();
	//qDebug("GLC_Collection::CreateMemberList ENTER");
	
    while (iEntry != m_TheMap.constEnd())
    {
    	if(!iEntry.value()->getListValidity())
    	{
     		iEntry.value()->GlExecute(GL_COMPILE);
    	}
    	// Passe au Suivant
    	iEntry++;
    }

	// Gestion erreur OpenGL
	if (glGetError() != GL_NO_ERROR)
	{
		qDebug("GLC_Collection::CreateMemberList OPENGL ERROR");
	}


}
/*
// Création des sous listes d'affichages
void GLC_Collection::CreateSubLists(void)
{
	//qDebug() << "GLC_Collection::CreateSubLists";
	CGeomMap::iterator iEntry= m_TheMap.begin();
	CListeMap::iterator iListEntry;
	
	GLuint ListeID= 0;
    while (iEntry != m_TheMap.constEnd())
    {
    	if(!iEntry.value()->GetValidity())
    	{
    		iListEntry= m_ListMap.find(iEntry.key());
    		assert(iListEntry != m_ListMap.constEnd());
    		if (iListEntry.value() == 0)
    		{// Numéro non généré
   				qDebug() << "GLC_Collection::CreateSubLists: List not found";
    			ListeID= glGenLists(1);
    			m_ListMap[iListEntry.key()]= ListeID;
    			m_ListIsValid= false;    			
    		}
    		else
    		{
    			ListeID= iListEntry.value();
    		}
    		// Création de la liste
    		glNewList(ListeID, GL_COMPILE);
    			iEntry.value()->GlExecute(GL_COMPILE);
    		glEndList();
    		qDebug("GLC_Collection::CreateSubLists : Display list %u created", ListeID);
     	}
    	// Passe au Suivant
    	iEntry++;
    }
	

	// Gestion erreur OpenGL
	if (glGetError() != GL_NO_ERROR)
	{
		qDebug("GLC_Collection::CreateSubLists ERREUR OPENGL");
	}

}
*/
//////////////////////////////////////////////////////////////////////
// Fonctions de services privées
//////////////////////////////////////////////////////////////////////
// Verifie si les listes membres sont à jour
bool GLC_Collection::MemberIsUpToDate(void)
{
	CGeomMap::iterator iEntry= m_TheMap.begin();
	
    while (iEntry != m_TheMap.constEnd())
    {
    	if(iEntry.value()->getListValidity() || !iEntry.value()->getGeometry()->GetIsVisible())
    	{	// Géométrie valide ou non visible.
    		iEntry++;   		
    	}
    	else
    	{
 			qDebug("GLC_Collection::MemberListIsUpToDate : Child display list not updated");
			return false;
    	}
     }
		
	return true;	// Toutes les listes sont à jour

}
/*
// Verifie si les membres sont à jour
bool GLC_Collection::MemberIsUpToDate(void)
{
		
	CGeomMap::iterator iEntry= m_TheMap.begin();
	
    while (iEntry != m_TheMap.constEnd())
    {
    	if(iEntry.value()->GetValidity() || !iEntry.value()->GetIsVisible())
    	{	// Membre valide ou non visible.
    		iEntry++;   		
    	}
    	else
    	{
 			//qDebug("GLC_Collection::MemberIsUpToDate : Prop Geom d'un enfant non à jour");
			return false;
    	}
     }
	
	return true;	// Toutes les Membres sont à jour

}
*/

// Création de la liste d'affichage de la collection
bool GLC_Collection::CreateList(void)
{
	qDebug("GLC_Collection::CreateList");
	
	if(!m_ListID)		// La liste n'a jamais été créé
	{
		m_ListID= glGenLists(1);

		if (!m_ListID)	// ID de liste non obtenu
		{
			GlDraw();
			qDebug("GLC_Collection::CreateList : Display list nor created");
			return false;	// Géométrie affiché mais pas de liste de créé
		}
	}

	// Création de la liste
	glNewList(m_ListID, GL_COMPILE_AND_EXECUTE);				
		// Affichage des éléments de la collection
		GlDraw();
	glEndList();
	
	// Validité de la liste
	m_ListIsValid= true;

	//qDebug("GLC_Collection::CreateList : Liste d'affichage %u créé", m_ListID);	

	// Gestion erreur OpenGL
	GLenum errCode;
	if ((errCode= glGetError()) != GL_NO_ERROR)
	{
		const GLubyte* errString;
		errString = gluErrorString(errCode);
		qDebug("GLC_Collection::CreateList OPENGL ERROR %s\n", errString);
	}

	return true;	// Géométrie affiché et liste créé

}
