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
#ifndef BBWindow_H
#define BBWindow_H

#include <GL/glew.h>

#include <QtGui>

#include "Light.h"
#include "Vector3.h"
#include "Entity.h"
#include "Material.h"
#include "TextureLayer.h"
#include "Mesh.h"
#include "Density.h"
#include "DensityViewer.h"
#include "Billboard.h"
#include "BillboardCloud.h"
#include "ui_W_Main.h"
#include "Viewer.h"
#include "Scene.h"
#include <time.h>


/** 
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

 class BBWindow :public QMainWindow
{
  Q_OBJECT

  public:
/** @name : BBWindow
 *  @brief : Constructeur
 */
    BBWindow();
/** @name : ~BBWindow
 *  @brief : Destructeur
 */
    ~BBWindow();
  protected:
    void closeEvent(QCloseEvent *event);
/** @name : loadObject
 *  @brief : chargement d'un objet maill�
 *  @param fileName : Nom du fichier
 */
    void loadObject( const QString &fileName);

/** @name : loadTexture
 *  @brief : chargement d'une texture sur l'objet courant
 *  @param textureName : Nom du fichier
 */
    void loadTexture(const QString &textureName);
  private slots:
/** @name : createScene
 *  @brief : Initialisation de la sc�ne
 */
    void createScene(void);

/** @name : loadObjectDialog
 *  @brief : Affichage d'une boite de dialogue permettant le chargement des objets
 */
    void loadObjectDialog();

/** @name : loadTextureDialog
 *  @brief : Affichage d'une boite de dialogue permettant le chargement des textures pour les objets maill�
 */
    void loadTextureDialog();

/** @name : viewerAxis
 *  @brief : Active et d�sactive l'affichage du rep�re
 */
    void viewerAxis();

/** @name : viewerWireMode
 *  @brief : Active/D�sactive le mode filaire
 */
    void viewerWireMode();

/** @name : fileCleanScene
 *  @brief : Efface la sc�ne
 */
    void fileCleanScene();

    void toolCamReset();
/** @name : showDensity
 *  @brief : Appel la fen�tre pour afficher la densit�
 */
    void showDensity();

/** @name : BB
 *  @brief : Cr�e un billboard
 */
    void BB();

/** @name : BBC
 *  @brief : Cr�e un billboard cloud
 */
		void BBC();

/** @name : BBCBox
 *  @brief : Cr�e un billboard cloud par rapport a la boite englobante de l'objet
 */
	void BBCBox();

/** @name : statusChanged
 *  @brief : Affiche un message dans la barre de status
 */
		void statusChanged(const QString &message);

/** @name : toggleShowFps
 *  @brief : Active/D�sactive l'affichage des FPS
 */
		void toggleShowFps(void);

/** @name : toggleNormal
 *  @brief : Active le mode Normal 
 */
		void toggleNormal(void);

/** @name : toggleSplitScreen
 *  @brief : Active/D�sactive le mode Split Screen(partage en 2 de l'�cran)
 *					Cette fonction permet aussi de placer correctement l'affichage des 
 *					informations en fonction du mode
 */
		void toggleSplitScreen(void);

/** @name : toggleFusion
 *  @brief : Active/D�sactive le mode fusion
 *					Les 2 viewport sont l'un sur l'autre, l'un affiche sa partie droite,
 *					l'autre sa partie gauche.
 *					Cette fonction permet aussi de placer correctement l'affichage des 
 *					informations en fonction du mode
 */
		void toggleFusion(void);

/** @name : selectViewPortLeft	
 *  @brief : Selectionne le viewport de gauche
 */
		void selectViewPortLeft();

/** @name : selectViewPortRight
 *  @brief : Selectionne le viewport de droite
 */
		void selectViewPortRight();

/** @name : toggleInfo
 *  @brief : Active/D�sactive l'affichage des informations � l'�cran
 */
		void toggleInfo();

/** @name : toogleBorder
 *  @brief : Active/D�sactive l'affichage du cadre entourant le viewport
 */
		void toogleBorder();
		
/** @name : SaveAs
 *  @brief : Sauvegarde d'un Billboard ou d'un Billboard cloud
 *  Si la variable save = true, l'objet a sauvegarder sera un Billboard
 *  Sinon �a sauvegardera un Billboard Cloud
 */
		void SaveAs();
  private:
/** @name : reinitViewPort
 *  @brief : R�initialise les viewport
 */
		void reinitViewPort(void);

/** @name : ObjIsAMesh
 *  @brief : Test si l'objet contenu dans le viewport courant est de type bb ou bbc
 */
		bool ObjIsAMesh(void);

    Ui::MyWindow ui;
    bool mShowFps;
    Viewer *mViewer;
    QDockWidget *mDensityWidget;
    Scene *mScene;
		Scene *mScene2;
    BillboardCloud *bbc ;
		int numScene;
		Billboard * billbo;
		bool isBCC;

		void timer(bool start);
		double debut, fin;
		double time;
};

#endif
