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

#include "DensityViewer.h"

DensityViewer::DensityViewer(int nbRowDensity, bool isT3D, Scene *parentScene)
  :QMainWindow(), mShowFps(false), mDensityWidget(NULL), pScene(parentScene), numScene(1), pDensity(NULL), isTex3D(isT3D)
{
  ui.setupUi(this);

  // Init QGL
  QGLFormat format;
  format.setDoubleBuffer(true);
  format.setDepth(true);
  format.setRgba(true);
  format.setAlpha(true);
  format.setStencil(true);
  format.setStereo(false);
  QGLFormat::setDefaultFormat(format);

  // Chargement viewer en fenètre Main
  mViewer = new Viewer(this);
  mViewer->setFocus();
  setCentralWidget(mViewer);
  // Attachement d'une scène au viewer
  mScene = new Scene();

  mViewer->attachScene(mScene, NULL);

	// Création de la densité
	Density * pDensity = new Density(nbRowDensity, isTex3D);
	// Création de la densité de l'objet
	pScene->fDensity(0,pDensity);
	attacheDensity(pDensity);
	mViewer->setRotatePossible( false);
  // /!\ Ne Pas Modifier la fenètre ICI ! Utiliser QtDesigner et le .ui !
  // Action de la fenêtre Main
  connect(mViewer, SIGNAL(glInitialized()), this, SLOT(createScene()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(exit()));
	connect(ui.actionDensityON,SIGNAL(triggered()), this, SLOT(showDensity()));
	connect(ui.actionDensityOFF,SIGNAL(triggered()), this, SLOT(removeDensity()));
}

void DensityViewer::closeEvent(QCloseEvent *event)
{
	(mDensityWidget->widget())->close();
	removeDockWidget(mDensityWidget);
	mViewer->setRotatePossible(true);
  this->removeDensity();
  event->accept();
}

void DensityViewer::exit()
{
  this->removeDensity();
	this->hide();
}

void DensityViewer::createScene (void)
{
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    std::cout << "Erreur !! GlewInit : " << glewGetErrorString(err) << std::endl;
  }

	showDensity();
}


/** Nom: fileCleanScene
 * Fct : efface la scène
 */
void DensityViewer::fileCleanScene()
{
  mScene->remove();
  Camera *mpCamera = mViewer->getCamera() ;
  mpCamera->reinit(0,0,0);
	numScene=1;
	mViewer->selectViewPort(numScene);
	mViewer->changeMode(0);
  mViewer->updateGL();
}


/** Nom: showDensity
  * Fct : Appel la fenêtre pour afficher la densité
 */
void DensityViewer::showDensity()
{

  if (pDensity != NULL )
  {
		//mScene->fDensity(0,pDensity);
    pDensity->initDraw();
    mViewer->freeDensity();
    mViewer->attachDensity(pDensity);

  //Ajout d'un widget
    mDensityWidget = new QDockWidget("Densité minimale", this,0);
    mDensityWidget->setFloating(true);
    QDoubleSpinBox *w = new QDoubleSpinBox(mDensityWidget);
    w->setValue(0);
    w->setRange(0,1);
    w->setSingleStep(0.01);
    mDensityWidget->setWidget(w);
    addDockWidget(Qt::RightDockWidgetArea, mDensityWidget);

    connect(w, SIGNAL(valueChanged(double)), this, SLOT(alphaChange(double)));
  }
}

void DensityViewer::alphaChange(double d)
{
  pDensity->setMinAlpha((float)d);
  mViewer->updateGL();
}


/** Nom : removeDensity
  * Fct : Efface la densité
  */
void DensityViewer::removeDensity()
{
//   mViewer->freeDensity();
//   mViewer->updateGL();
	if (pDensity!=NULL)
	{
	 	delete pDensity;
	}
  pDensity=NULL;

	if (mScene!=NULL)
	{
	 	delete mScene;
	}
  mScene=NULL;

	if (mViewer!=NULL)
	{
	 	delete mViewer;
	}
  mViewer=NULL;
}

DensityViewer::~DensityViewer()
{
  this->removeDensity();
}

void DensityViewer::attacheDensity(Density * mDensity)
{
	pDensity = mDensity;
}
