/***************************************************************************
 *   Copyright (C) 2007 by GangsTER   *
 *   coban31@hotamil.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "Billboard.h"
#include "Density.h"
#include "Camera.h"
#include "Viewer.h"
#include "Image.h"
#include <stdio.h>

/** 
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

class BillboardCloud : public Entity
{
public:	
	/** 
	@name BillboardCloud
	@brief Construction d'un BillboardCloud 
	@param entite l'objet a simuler
	@param vect les plans des Billboards
	@param ep la distance entre le plan et les sommets qui vont etre pris en compte pour la copie des textures
	@param v le Viewer dans lequel on travaille
	@param reso la resolution maximum des billboards
	*/
	BillboardCloud(Entity* entite,std::vector<float*> vect,Viewer *v,float ep,int reso);
	/** 
	@name BillboardCloud
	@brief Chargement d'un BillboardCloud a partir d'un fichier texte
	@param filename le fichier
	@param v le Viewer dans lequel on travaille
	*/
	BillboardCloud(const QString& filename,Viewer *v);	
	/**
	@name saveBillboardCloud
	@return les informations a sauvegarder	
	*/
  QString saveBillboardCloud(const QString &filename);

	virtual ~BillboardCloud();

	/** @name setMode
	* @brief Permet de définir le mode d'affichage en cour pour placer correctement les informations
	* @param mode : le mode choisi
	*/
	void setMode(int mode, bool info);

	/** @name setTime
	* @brief Affecte le temps passé à la création et au chargement du Billboard
	* @param t : le temps passé
	*/
	void setTime(double t);
protected:

  virtual void drawGeometry(void);
	virtual Vector3 getCenterM(void);
	virtual std::vector<Vector3> getVertexArray();
	virtual void deleteVBO();
	virtual void afficheInfoM(int mode);
	virtual void fillDensity(int coverage, Density *pDensity);
	
	/**
	l'objet associe au billboard
	*/
	Entity* entity;

	/**
	la liste des Billboard
	*/
	std::vector<Billboard*> listBillboard;
	/**
	la camera de la scene
	*/
	Camera *cam;
	/**
	le Viewer associe
	*/
	Viewer *viewer;

private :
	/// Fonction et variables utilisé pour l'affichage des informations
	void drawText(void);
	bool Binfo;
	Vector3 position;
	bool isLoad;
	double time;
	bool error;
};



