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
     * Renvoie la valeur d'un �l�ment de la matrice
     * @param x : coordonn� x de l'�l�ment
     * @param y : coordonn� y de l'�l�ment
     * @param z : coordonn� z de l'�l�ment
     * @return float
   */
    float getElt(int x, int y, int z);
		
		 /** @name getElt
		 * Renvoie la valeur d'un �l�ment de la matrice
		 * @param coord : coordonn� x de l'�l�ment Attention il faut parcourir la matrice de 4 en 4.
		 * @return float
			*/
		float getElt(int coord);
  /** @name setElt
		 * Attribue une valeur a un �l�ment de la matrice
     * @param x : coordonn� x de l'�l�ment
     * @param y : coordonn� y de l'�l�ment
     * @param z : coordonn� z de l'�l�ment
		 * @param c : la valeur
   */
		void setElt(int x, int y, int z, float elt);

  /** @name setElt
		 * Attribue une valeur a un �l�ment de la matrice
     * @param coord : coordonn� de l'�l�ment (ici la matrice est consid�r�e comme un long tableau)
		 * @param c : la valeur
   */
		void setElt(int coord, float c);

  /** @name fill
     * Remplis un �l�ment de la matrice en incr�mentant l'alpha
     * @param x : coordonn� x de l'�l�ment
     * @param y : coordonn� y de l'�l�ment
     * @param z : coordonn� z de l'�l�ment
     * @param alphaAdd : intensit� � ajouter � l'alpha
   */
    void fill(int x, int y, int z, float alphaAdd);

  /** @name setDistanceMax
   * D�finit le plan de l'objet le plus �loign� du centre, cela est n�cessaire pour remplir la matrice de densit�
   * @param dmax : valeur de cette distance maximale
   */
  void setDistanceMax(float dmax);


  /** @name getDistanceMax
   * Restitue la valeur de la distance maximale d'un plan cela est n�cessaire lors de l'extraction du maximum
   * afin de retrouver notre �quation de plan
   * @return this->distMax : la valeur de la distance maximum
   */
  float getDistanceMax();

 /**
   * @name convertMaxPlane
   * @brief R�cup�re le plan d'intensit� maximal
   * @return maxPlane : le plan d'intensit� maximale
  */
 float* convertMaxPlane();

	/**
   * @name extractAllMaxPlane
   * @brief R�cup�re les plans d'intensit� maximal
   * @param alpha : valeur limite pour extraire un plan en tant que plan max
   * @param billboard : nombre de plan max � retirer
   * @param listMaxPlan : la liste de tous les plans max (vide a l'origine)
  */
 	void extractAllPlane(float alpha,int billboard, std::vector<float*> &listMaxPlan);

    /** @name getRow
     * Retourne la taille de la Densit�
     */
    int getRow();

  /**
     * @name reset
     * Toutes les valeurs de la densit� sont mis a 0.0
   */
    void reset();

  /**
     * @name reset
     * Permet de remplir la densit� avec une valeur par defaut
     * @param defVal : valeur de remplissage
   */
    void reset(float defVal);

  /** @name initDraw
     * Cr�e les textures necessaires pour le rendu.
   */
    void initDraw();

  /**
     * @name drawDensity
     * @brief Trace la densit�
     * @param camera : La camera de la sc�ne -> permet de r�cup�rer sa positon
   */
    void drawDensity(Camera *camera);

  /**
     * @name setMinAlpha
     * modifie la valeur alpha du blending lors de
     * l'affichage de la densit�
   */
    void setMinAlpha(float minf);

    // Provisoire
    void InitTest();
		
		void removeShaders();
  private :
	
  /**
   * @name extractMaxPlane
   * @brief R�cup�re le plan d'intensit� maximal de la matrice de densit� 
   * @return MaxPlane : le plan d'intensit� maximale 
  */
    float* extractMaxPlane();

  /**
   * @name drawSlicesTex2D
   * Trace des Slices de la matrice de densit� en utilisant une texture 2D
   * Les slices sont ordonn�es pour �tre trac� du plus loin au plus pr�s.
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
     * Trace un cadre autout de la Densit�
     * en jaune : le cot�s des tranches
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
