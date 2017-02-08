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


#ifndef _Billboard_h_
#define _Billboard_h_

#include "Entity.h"
#include <GL/glut.h>
#include "Camera.h"
#include "Viewer.h"
#include "Image.h"
#include <GL/glu.h>
#include <QTextStream>
#include <malloc.h>
#include <stdlib.h>
#include <QFile>
#include <png.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <string.h>
#include <vector>
#include "Matrix3.h"
#include "Matrix4.h"
#include <time.h>
#include <QMessageBox>
#include <stdio.h>
#include <stdlib.h>

/** 
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

class Billboard : public Entity
{
public:
	/** 
	@brief Construction d'un Billboard 
	@name Billboard
	@param vect le plan du Billboard
	@param entite l'objet à simuler
	@param ep la distance entre le plan et les sommets qui vont être pris en compte pour la copie des textures
	@param view le Viewer dans lequel on travaille
	@param res la resolution maximum du billboard
	*/
	Billboard(float *vect, Entity* entite,float ep,Viewer *view,int res);
	
	/** 
	@brief Chargement d'un Billboard à partir d'un fichier par le biais de BillboardCloud.
	@name Billboard
	@param im contient la texture
	@param v le Viewer dans lequel on travaille
	@param sommets les coordonnees des sommets
	@param texCoord les coordonnees de textures
	@param norm la normale au plan
	*/
	Billboard(Image im,Viewer *v,float *sommets,float *texCoord,Vector3 norm);
	/** 
	@brief Chargement d'un Billboard a partir d'un fichier texte.
	@name Billboard
	@param filename le fichier
	@param v le Viewer dans lequel on travaille
	*/
	Billboard(const QString& filename,Viewer *v);
	
		/** 
	@brief Sert juste à recuperer la boite englobante de l'objet
	@name Billboard
	@param entite l'objet 
	@param view le Viewer dans lequel on travaille
	*/
	Billboard(Entity* entite, Viewer*view);
	/**
	@brief Calcul de la boite englobante de l'objet
	@name createBox
	*/
	void createBox();
	/**
	appele par BillboardCloud pour l'affichage
	@name drawBillboard
	*/
	void drawBillboard();
	/**
	@brief  projection de points sur le plan du billboard
	@name projection
	@param vect les points à projetter
	*/
	void projection(std::vector<Vector3> vect);
	/**
	@brief Analyse en composante principales pour calculer le repere du plan
	@name acp
	@param vect les points projettes sur le plan
	@param frame le repere du plan a calculer
	@return le centre du Billboard	
	*/
	Vector3 acp(const std::vector<Vector3> &vect, std::vector<Vector3> &frame);

	/**
	@brief Calcule la distance entre deux sommets
	@name distVertices
	@param vect1 un sommet
	@param vect2 un sommet
	@return la distance entre ces deux sommets	
	*/
	float distVertices(Vector3 vect1,Vector3 vect2);
	/**
	@brief Recherche des coordonnees du Billboard par rapport aux points projettés sur le plan
	@name findCoord
	@param repere le repere du plan
	@param vertices les sommets
	@return la position de la camera pour le placage des textures
	*/
	Vector3* findCoord(Matrix4* repere ,std::vector<Vector3> vertices);
	/**
	@brief Recupère la texture pour le Billboard
	@name generateTexture	
	*/
	void generateTexture();
	/**
	@brief Création globale d'un billboard
	@name createBillboard
	*/
	void createBillboard();
	/**
	@brief Donne la resolution de texture par rapport à la longueur
	@name resolution
	@param a la distance entre 2 sommets
	@return la resolution
	*/
	int resolution(float a);
	/**
	@name getPixels
	@return renvoie la texture du Billboard	
	*/
	unsigned char *getPixels();
	/**
	@name getnX
	@return renvoie la resolution du Billboard ppar rapport a sa largeur	
	*/
	int getnX();
	/**
	@name getnY
	@return renvoie la resolution du Billboard ppar rapport a sa hauteur	
	*/
	int getnY();
	/**
	@name saveBillboard
	@return les informations a sauvegarder	
	*/
	QString saveBillboard(const QString &filename);
		/**
	@name getCoord
	@return les cooordonnees du Billboard	
	*/
	QString getCoord();
		/**
	@name getBox
	@return la boite englobante
	*/
	std::vector<Vector3> getBox();

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

protected :

	virtual void drawGeometry(void);
	virtual Vector3 getCenterM(void);
	virtual std::vector<Vector3> getVertexArray();
	virtual void deleteVBO();
	virtual void afficheInfoM(int split);
	virtual void fillDensity(int coverage, Density *pDensity);

	/**
	equation du plan
	*/
	float plan[4];
	/**
	sommets du billboard a tracer
	*/
	float verticesBB[12];
	/**
	distance qui sert de reference pour le calcul de la resolution de texture
	*/
	float distMaxBox;
	/**
	l'objet associe au billboard
	*/
	Entity* entity;
	/**
	la camera de la scene
	*/
  Camera *cam;
	/**
	identifiant de texture
	*/
 	unsigned int _textureID[1];
	/**
	la distance entre le plan et les sommets qui seront pris en compte lors du placage de la texture
	*/
  float epsilon;
	/**
	dimensions de la texture
	*/
	int vX,vY;
	/**
	le Viewer associe
	*/
	Viewer *viewer;
	/**
	la texture
	*/
	unsigned char *pixels;
	/**
	les sommets de la boite englobante projettes sur le plan
	*/
	std::vector<Vector3> projections;
	/**
	les coordonnees de textures
	*/
	float tCoord[8];
	/**
	les normales du billboard
	*/
	Vector3 normals;
	/**
	la boite englobante
	*/
	std::vector<Vector3> box;
	/**
	resolution maximum du billboard
	*/
	int resMax;
private :
	/// Fonction et variables utilisé pour l'affichage des informations
	void drawText(void);
	bool Binfo;
	Vector3 position;
	bool isLoad;
	double time;
	bool error;
};

#endif
