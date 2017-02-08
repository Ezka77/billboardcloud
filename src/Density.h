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
#ifndef _Density_h_
#define _Density_h_

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include "Color.h"
#include "Camera.h"
#include <vector>

/** Density class. Density
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */


class Density
{
  public:
    Density(const int nbRow, bool UseTex3D=false);
    ~Density();

  /** @name getElt
     * Renvoie la valeur d'un élément de la matrice
     * @param x : coordonné x de l'élément
     * @param y : coordonné y de l'élément
     * @param z : coordonné z de l'élément
     * @return float
   */
    float getElt(int x, int y, int z);
		
		 /** @name getElt
		 * Renvoie la valeur d'un élément de la matrice
		 * @param coord : coordonné x de l'élément Attention il faut parcourir la matrice de 4 en 4.
		 * @return float
			*/
		float getElt(int coord);
  /** @name setElt
		 * Attribue une valeur a un élément de la matrice
     * @param x : coordonné x de l'élément
     * @param y : coordonné y de l'élément
     * @param z : coordonné z de l'élément
		 * @param c : la valeur
   */
		void setElt(int x, int y, int z, float elt);

  /** @name setElt
		 * Attribue une valeur a un élément de la matrice
     * @param coord : coordonné de l'élément (ici la matrice est considérée comme un long tableau)
		 * @param c : la valeur
   */
		void setElt(int coord, float c);

  /** @name fill
     * Remplis un élément de la matrice en incrémentant l'alpha
     * @param x : coordonné x de l'élément
     * @param y : coordonné y de l'élément
     * @param z : coordonné z de l'élément
     * @param alphaAdd : intensité à ajouter à l'alpha
   */
    void fill(int x, int y, int z, float alphaAdd);

  /** @name setDistanceMax
   * Définit le plan de l'objet le plus éloigné du centre, cela est nécessaire pour remplir la matrice de densité
   * @param dmax : valeur de cette distance maximale
   */
  void setDistanceMax(float dmax);


  /** @name getDistanceMax
   * Restitue la valeur de la distance maximale d'un plan cela est nécessaire lors de l'extraction du maximum
   * afin de retrouver notre équation de plan
   * @return this->distMax : la valeur de la distance maximum
   */
  float getDistanceMax();

 /**
   * @name convertMaxPlane
   * @brief Récupère le plan d'intensité maximal
   * @return maxPlane : le plan d'intensité maximale
  */
 float* convertMaxPlane();

	/**
   * @name extractAllMaxPlane
   * @brief Récupère les plans d'intensité maximal
   * @param alpha : valeur limite pour extraire un plan en tant que plan max
   * @param billboard : nombre de plan max à retirer
   * @param listMaxPlan : la liste de tous les plans max (vide a l'origine)
  */
 	void extractAllPlane(float alpha,int billboard, std::vector<float*> &listMaxPlan);

    /** @name getRow
     * Retourne la taille de la Densité
     */
    int getRow();

  /**
     * @name reset
     * Toutes les valeurs de la densité sont mis a 0.0
   */
    void reset();

  /**
     * @name reset
     * Permet de remplir la densité avec une valeur par defaut
     * @param defVal : valeur de remplissage
   */
    void reset(float defVal);

  /** @name initDraw
     * Crée les textures necessaires pour le rendu.
   */
    void initDraw();

  /**
     * @name drawDensity
     * @brief Trace la densité
     * @param camera : La camera de la scéne -> permet de récupérer sa positon
   */
    void drawDensity(Camera *camera);

  /**
     * @name setMinAlpha
     * modifie la valeur alpha du blending lors de
     * l'affichage de la densité
   */
    void setMinAlpha(float minf);

    // Provisoire
    void InitTest();
		
		void removeShaders();
  private :
	
  /**
   * @name extractMaxPlane
   * @brief Récupère le plan d'intensité maximal de la matrice de densité 
   * @return MaxPlane : le plan d'intensité maximale 
  */
    float* extractMaxPlane();

  /**
   * @name drawSlicesTex2D
   * Trace des Slices de la matrice de densité en utilisant une texture 2D
   * Les slices sont ordonnées pour être tracé du plus loin au plus prés.
   */
    void drawSlicesTex2D(Camera *camera);

  /**
     * @name drawSlicesTex3D
     * Trace des slices dans le Volume de la texture 3D
     * @param c : camera.
   */
    void drawSlicesTex3D(Camera *camera);

  /**
     * @name drawBorder
     * Trace un cadre autout de la Densité
     * en jaune : le cotés des tranches
     * en rouge : quand on est en face
   */
    void drawBorder();
    void generatePolygon(int & n, Vector3 *Ptab, Vector3 *Ttab, Camera *cm, float pos);
    void makeConvexPolygon(int n, Vector3 *Ptab, Vector3 *Ttab);
		void coloring();
		void setShaders();
  protected:
    int DRow;
    bool DensityTex3D ;
    GLfloat minAlpha ;
    float * Data;
    unsigned int _textureID[1];
		GLuint mTexId;
		GLuint mVertShader, mFragShader, mProgShader;
		float distMax;
};
#endif
