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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_collection.cpp implementation of the GLC_Collection class.

#include <QtDebug>

#include "glc_collection.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLC_Collection::GLC_Collection()
: m_ListeID(0)
, m_bListeIsValid(false)
{
}

GLC_Collection::~GLC_Collection()
{
	Erase();
}
//////////////////////////////////////////////////////////////////////
// Fonctions Set
//////////////////////////////////////////////////////////////////////
// Ajoute une géométrie à la collection
bool GLC_Collection::AddGLC_Geom(GLC_Geometry* pGeom)
{
	CGeomMap::iterator iGeom= m_TheMap.find(pGeom->GetID());
	
	if (iGeom == m_TheMap.end())
	{	// Ok, la clé n'est pas prise
		// Ajoute la géométrie
		m_TheMap[pGeom->GetID()]= pGeom;
		// Ajoute la sous liste
		m_ListeMap[pGeom->GetID()] = 0;
		//TRACE("GLC_Collection::AddGLC_Geom : Element Ajouté avec succès\n");
		
		// Validité de la liste
		m_bListeIsValid= false;
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
	{	// Ok, la clé existe
		delete iGeom.value();		// Supprime la géométrie
		m_TheMap.remove(Key);		// Supprime le conteneur
		// Recherche la liste
		CListeMap::iterator iListe= m_ListeMap.find(Key);
		// \todo Vérifier que la liste d'affichage est trouvée
		if (!!iListe.value())
		{
			glDeleteLists(iListe.value(),1);
			//qDebug("GLC_Collection::DelGLC_Geom : Sous liste %u Supprimée\n", RemListe);
		}
		m_ListeMap.remove(Key);		// Supprime le conteneur
			
		// Validité de la liste
		m_bListeIsValid= false;
		
		//qDebug("GLC_Collection::DelGLC_Geom : Element succesfuly deleted);
		return true;
		
	}
	else
	{	// KO, la clé n'existe pas
		qDebug("GLC_Collection::DelGLC_Geom : Elément non Supprimé");
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
		// Recherche la liste
		CListeMap::iterator iListe= m_ListeMap.find(Key);
		// TODO Vérifier que la liste d'affichage est trouvée
		if (!!iListe.value())
		{
			glDeleteLists(iListe.value(),1);
			//qDebug("GLC_Collection::RemGLC_Geom : Sous liste %u Supprimée", RemListe);
		}
		m_ListeMap.remove(Key);		// Supprime le conteneur
			
		// Validité de la liste
		m_bListeIsValid= false;
		
		//qDebug("GLC_Collection::RemGLC_Geom : Element Supprimé avec succès");
		return true;
		
	}
	else
	{	// KO, la clé n'existe pas
		qDebug("GLC_Collection::RemGLC_Geom : Elément non Supprimé");
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

	// Suppression des sous listes d'affichages
	CListeMap::iterator iListEntry= m_ListeMap.begin();
	
    while (iListEntry != m_ListeMap.constEnd())
    {
        // Supprime l'objet
        if (!!iListEntry.value()) glDeleteLists(iListEntry.value(), 1);
        ++iListEntry;
    }
    // Vide la table de hachage de liste
    m_ListeMap.clear();

	// Supprime la liste d'affichage
	DeleteListe();
	// Fin des Suppressions des sous listes d'affichages

}

// Retourne le pointeur d'un élément de la collection
GLC_Geometry* GLC_Collection::GetElement(GLC_uint Key)
{
	CGeomMap::iterator iGeom= m_TheMap.find(Key);
	
	if (iGeom != m_TheMap.end())
	{	// Ok, la clé est trouvé
		return iGeom.value();
	}
	else
	{	// KO, la clé n'est pas trouvé
		return NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// Fonctions OpenGL
//////////////////////////////////////////////////////////////////////
void GLC_Collection::GlExecute(void)
{
	if (GetNumber() > 0)
	{
		CreateMemberList();		// Si nécessaire

		CreateSousList();		// Si nécessaire

		if (m_bListeIsValid)
		{	// La liste de la collection OK
			glCallList(m_ListeID);
		}
		else
		{
			if(MemberIsUpToDate())
			{
				CreationListe();
			}
			else
			{
				m_bListeIsValid= false;
				qDebug("GLC_Collection::DrawGl : CreatMemberList KO -> Affichage éléments\n");
			}
		}

		// Gestion erreur OpenGL
		GLenum errCode;
		if ((errCode= glGetError()) != GL_NO_ERROR)
		{
			const GLubyte* errString;
			errString = gluErrorString(errCode);
			qDebug("GLC_Collection::DrawGl ERREUR OPENGL %s\n", errString);
		}
	}
}

// Affiche les éléments de la collection
void GLC_Collection::GlDraw(void)
{
	CListeMap::iterator iEntry= m_ListeMap.begin();
	
    while (iEntry != m_ListeMap.constEnd())
    {
        glCallList(iEntry.value());
        ++iEntry;
    }
	

	// Gestion erreur OpenGL
	GLenum errCode;
	if ((errCode= glGetError()) != GL_NO_ERROR)
	{
		const GLubyte* errString;
		errString = gluErrorString(errCode);
		qDebug("GLC_Collection::GlDraw ERREUR OPENGL %s\n", errString);
	}

}

// Création des listes d'affichages des membres
void GLC_Collection::CreateMemberList(void)
{
	CGeomMap::iterator iEntry= m_TheMap.begin();
	
    while (iEntry != m_TheMap.constEnd())
    {
    	if(!iEntry.value()->GetListIsValid())
    	{
    		iEntry.value()->CreateList(GL_COMPILE);    		
    	}
    	// Passe au Suivant
    	iEntry++;
    }

	// Gestion erreur OpenGL
	if (glGetError() != GL_NO_ERROR)
	{
		qDebug("GLC_Collection::CreateMemberList ERREUR OPENGL");
	}

}
// Création des sous listes d'affichages
void GLC_Collection::CreateSousList(void)
{
	CGeomMap::iterator iEntry= m_TheMap.begin();
	CListeMap::iterator iListEntry;
	
	GLuint ListeID= 0;
    while (iEntry != m_TheMap.constEnd())
    {
    	if(!iEntry.value()->GetValidity())
    	{
    		iListEntry= m_ListeMap.find(iEntry.key());
    		if(iListEntry != m_ListeMap.constEnd())
    		{	// Numéro non généré
    			ListeID= glGenLists(1);
    			m_ListeMap[iEntry.key()]= ListeID;
    			m_bListeIsValid= false;
    		}
    		// Création de la liste
    		glNewList(ListeID, GL_COMPILE);
    			iEntry.value()->GlExecute(GL_COMPILE);
    		glEndList();
    		//qDebug("GLC_Collection::CreateSousList : Liste d'affichage %u créé", ListeID);
     	}
    	// Passe au Suivant
    	iEntry++;
    }
	

	// Gestion erreur OpenGL
	if (glGetError() != GL_NO_ERROR)
	{
		qDebug("GLC_Collection::CreateSousList ERREUR OPENGL\n");
	}

}

//////////////////////////////////////////////////////////////////////
// Fonctions de services privées
//////////////////////////////////////////////////////////////////////
// Verifie si les listes d'affichage des membres sont à jour
bool GLC_Collection::MemberListIsUpToDate(void)
{
	CGeomMap::iterator iEntry= m_TheMap.begin();
	
    while (iEntry != m_TheMap.constEnd())
    {
    	if(iEntry.value()->GetListIsValid() || !iEntry.value()->GetIsVisible())
    	{	// Géométrie valide ou non visible.
    		iEntry++;   		
    	}
    	else
    	{
 			//qDebug("GLC_Collection::MemberListIsUpToDate : Liste d'affichage d'un enfant non à jour");
			return false;
    	}
     }
		
	return true;	// Toutes les listes sont à jour

}

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

// Création de la liste d'affichage de la collection
bool GLC_Collection::CreationListe(void)
{
	
	if(!m_ListeID)		// La liste n'a jamais été créé
	{
		m_ListeID= glGenLists(1);

		if (!m_ListeID)	// ID de liste non obtenu
		{
			GlDraw();
			//qDebug("GLC_Collection::CreateList : ERREUR Liste d'affichage NON créé");
			return false;	// Géométrie affiché mais pas de liste de créé
		}
	}

	// Création de la liste
	glNewList(m_ListeID, GL_COMPILE_AND_EXECUTE);				
		// Affichage des éléments de la collection
		GlDraw();
	glEndList();
	
	// Validité de la liste
	m_bListeIsValid= true;

	//qDebug("GLC_Collection::CreateList : Liste d'affichage %u créé", m_ListID);	

	// Gestion erreur OpenGL
	GLenum errCode;
	if ((errCode= glGetError()) != GL_NO_ERROR)
	{
		const GLubyte* errString;
		errString = gluErrorString(errCode);
		qDebug("GLC_Collection::CreationListe ERREUR OPENGL %s\n", errString);
	}

	return true;	// Géométrie affiché et liste créé

}
