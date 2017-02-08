/***************************************************************************
 *   Copyright (C) 2007 by GangsTER   *
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
#ifndef DENSITYVIEWER_H
#define DENSITYVIEWER_H

#include <GL/glew.h>

#include <QtGui>
#include "Light.h"
#include "Vector3.h"
#include "Entity.h"
#include "Material.h"
#include "TextureLayer.h"
#include "Mesh.h"
#include "Density.h"
#include "ui_W_Density.h"
#include "Viewer.h"
#include "Scene.h"


/** 
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */


class DensityViewer :public QMainWindow
{
  Q_OBJECT

  public:
    DensityViewer(int nbRowDensity, bool isT3D, Scene *parentScene);
    ~DensityViewer();
		void attacheDensity(Density * pDensity);

  protected:
    void closeEvent(QCloseEvent *event);
  private slots:
    void createScene(void);
    void fileCleanScene();
    void showDensity();
    void removeDensity();
    void alphaChange(double d);
		void exit();
  private:
    Ui::DensityWindow ui;
    bool mShowFps;
    Viewer *mViewer;
    QDockWidget *mDensityWidget;
    Scene *mScene, *pScene;
    Entity *bbc ;
		int numScene;
		Density * pDensity;
		bool isTex3D;
};

#endif
