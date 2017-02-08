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

#include "BBWindow.h"

BBWindow::BBWindow()
  :QMainWindow(), mShowFps(false), mDensityWidget(NULL), numScene(1),isBCC(false)
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
	mScene2 = new Scene();

  mViewer->attachScene(mScene, mScene2);

  // /!\ Ne Pas Modifier la fenètre ICI ! Utiliser QtDesigner et le .ui !
  // Action de la fenêtre Main
  connect(mViewer, SIGNAL(glInitialized()), this, SLOT(createScene()));

	// Fichier
	connect(ui.actionCharger_un_Maillage, SIGNAL(triggered()), this, SLOT(loadObjectDialog()));
	connect(ui.actionChargerTexture,SIGNAL(triggered()), this, SLOT(loadTextureDialog()));
	connect(ui.actionSauvegarder,SIGNAL(triggered()), this, SLOT(SaveAs()));
  connect(ui.actionQuit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

  // Affichage
	connect(ui.actionGauche,SIGNAL(triggered()), this, SLOT(selectViewPortLeft()));
	connect(ui.actionDroite,SIGNAL(triggered()), this, SLOT(selectViewPortRight()));

	connect(ui.actionMode_Normal,SIGNAL(triggered()), this, SLOT(toggleNormal()));
	connect(ui.actionMode_SplitScreen,SIGNAL(triggered()), this, SLOT(toggleSplitScreen()));
	connect(ui.actionMode_Fusion,SIGNAL(triggered()), this, SLOT(toggleFusion()));

  connect(ui.actionAfficher_Cacher_le_Rep_re,SIGNAL(triggered()), this, SLOT(viewerAxis()));
  connect(ui.actionWireMode,SIGNAL(triggered()), this, SLOT(viewerWireMode()));
  connect(ui.actionVider_la_Sc_ne, SIGNAL(triggered()), this, SLOT(fileCleanScene()));
  connect(ui.actionReset_Camera, SIGNAL(triggered()), this, SLOT(toolCamReset()));
	connect(ui.actionSelecteur, SIGNAL(triggered()), this, SLOT(toogleBorder()));

  connect(ui.actionToggleFPS,SIGNAL(triggered()), this, SLOT(toggleShowFps()));
	connect(ui.actionInformation_Objet,SIGNAL(triggered()), this, SLOT(toggleInfo()));

	// Densité
  connect(ui.actionCharger_la_Densite,SIGNAL(triggered()), this, SLOT(showDensity()));

	// Billboard
	connect(ui.actionBillboard,SIGNAL(triggered()), this, SLOT(BB()));
	connect(ui.actionBillboardCloud,SIGNAL(triggered()), this, SLOT(BBC()));

};

void BBWindow::closeEvent(QCloseEvent *event)
{
  event->accept();
#ifdef DEBUG
  std::cout << "Quitter" << std::endl ;
#endif
}

void BBWindow::createScene (void)
{
  Light *pLight;
  Light *pLight2;
  Light *pLight3;

 GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    std::cout << "Erreur !! GlewInit : " << glewGetErrorString(err) << std::endl;
  } else {
	// On lance la suite ...
	std::cout << "Utilisation de GLEW Version: " << glewGetString (GLEW_VERSION) << std::endl;
	pLight = new Light (Light::LT_POINT);
	pLight->setPosition (Vector3 (3., 5.,5.));
	pLight->setColor (Color (1.,1.,1.));

	pLight2 = new Light (Light::LT_POINT);
	pLight2->setPosition (Vector3 (3., -5.,-5.));
	pLight2->setColor (Color (1.,1.,1.));

	pLight3 = new Light (Light::LT_POINT);
	pLight3->setPosition (Vector3 (-5., 0.,0.));
	pLight3->setColor (Color (1.,1.,1.));

	mScene->add (pLight, false);
	mScene->add (pLight2, false);
	mScene->add (pLight3, false);

	//// Scene deuxieme viewport /////
	mScene2->add (pLight, false);
	mScene2->add (pLight2, false);
	mScene2->add (pLight3, false);
	//////////////////////////////////
#ifdef DEBUG
    std::cout << "Creation de la Scene : OK !" << std::endl ;
#endif
  }
}

void BBWindow::loadObjectDialog()
{
  QString fileName = QFileDialog::getOpenFileName(this,
					    "Choisissez un Objet à Charger",
              "./../../branches/Objets",
					    tr("Tous les fichiers objet (*.3ds *.gts *.obj *.off *.ply *.bb *.bbc);;3D Studio files (*.3ds);;GNU Triangulated Surface (*.gts);;WaveFront model file (*.obj);;DEC Object File Format (*.off);;Stanford triangle format (*.ply);; ;;Tous les types Billboard (*.bb *.bbc);;Billboard (*.bb);;Billboard Cloud (*.bbc)"), 0,0);
  if ( !fileName.isEmpty() )
    loadObject( fileName );
  else
    ui.statusbar->showMessage( tr("Chargement annulé"), 0 );
}

void BBWindow::loadTextureDialog()
{
	bool ok= true;
	if(numScene==1 && !mScene->isTextureEnable()) ok = false;
	if(numScene==2 && !mScene2->isTextureEnable()) ok = false;

	if(ok){
  QString textureName = QFileDialog::getOpenFileName(this,
      "Choisissez une texture à charger",
      "../../branches/Objets/",
      tr("Tous les fichiers image (*.bmp *.jpeg *.jpg *.png *.tga);; ;;Joint Photographic Experts Group (*.jpg *.jpeg);;PNG (*.png);; Windows BITMAP(*.bmp) ;; Truevision Targa (*.tga)"), 0,0);
  if ( !textureName.isEmpty() )
    loadTexture( textureName );
  else
    ui.statusbar->showMessage( tr("Chargement annulé"), 0 );
	}
	else ui.statusbar->showMessage( tr("Chargement Impossible : Pas d'objet valide à texturer dans la scène."), 2000 );
}

void BBWindow::loadObject( const QString &fileName)
{
	timer(true);
  Entity *pEntity;
  Transform t;
  Material *pMat;
	numScene = mViewer->getViewport();

	if(numScene==2 && mScene->isEmpty()) {
		selectViewPortLeft();
		numScene=1;	
	}

	int mode =1;
	if (ui.actionMode_Normal->isChecked() & numScene==1) mode =0;
	if (ui.actionMode_Fusion->isChecked()  & numScene==1) mode =0;
	if (ui.actionMode_Fusion->isChecked()  & numScene==2) mode =2;

  QFile f( fileName );
  if ( !f.open( QIODevice::ReadOnly ) )
	{
		ui.statusbar->showMessage( tr("Chargement annulé"), 2000 );
    return;
  }
	// Test si l'objet à charger est un billboard cloud
	if(fileName.contains(".bbc"))
	{
		bbc = new BillboardCloud(fileName,mViewer);
		switch(numScene)
		{
			case 1:
				mScene->remove();
				mScene->add(bbc);
				mScene->setTextureEnable(false);
			break;
			case 2:
				mScene2->remove();
				mScene2->add(bbc);
				mScene2->setTextureEnable(false);
			break;
		}
		bbc->setMode(mode,ui.actionInformation_Objet->isChecked());
		timer(false);
		bbc->setTime(time);
	}
	// Test si l'objet à charger est un billboard
	else if (fileName.contains(".bb"))
	{
		billbo = new Billboard(fileName,mViewer);
		switch(numScene)
		{
			case 1:
				mScene->remove();
				mScene->add(billbo);
				mScene->setTextureEnable(false);
			break;
			case 2:
				mScene2->remove();
				mScene2->add(billbo);
				mScene2->setTextureEnable(false);
			break;
		}
		billbo->setMode(mode,ui.actionInformation_Objet->isChecked());
		timer(false);
		billbo->setTime(time);
	}
	// Chargement des autres objets
	else
	{
		pEntity = new Mesh(fileName, "obj", ui.actionActiver_Desactiver_VBO->isChecked()
																			, mode);
		t.setIdentity();
		t.setPosition(0., 0., 0.);
		pEntity->setTransform(t);
		pMat = new Material();
		pMat->setBaseColor(Color(0.8,0.6,0.5), 0.4);
		pMat->setSpecular(Color::WHITE, 1);
		pEntity->setMaterial(pMat);
		if (numScene==1)
		{
			mScene->remove();
			mScene->add(pEntity);
			mScene->setTextureEnable(true);
		}
		else
		{
			mScene2->remove();
			mScene2->add(pEntity);
			mScene2->setTextureEnable(true);
		}
		ui.statusbar->showMessage( tr("Objet Chargé : ")+fileName, 0 );
		ui.menuBillboard->setEnabled(true);
		ui.actionBillboard->setEnabled(true);
		ui.actionBillboardCloud->setEnabled(true);
		ui.actionSauvegarder->setEnabled(false);
	}
	ui.actionMode_SplitScreen->setEnabled(true);
	ui.actionMode_Fusion->setEnabled(true);

	if (ObjIsAMesh()) ui.actionChargerTexture->setEnabled(true);
	else ui.actionChargerTexture->setEnabled(false);
	ui.menuViewport->setEnabled(true);
	mViewer->maj();
}

void BBWindow::loadTexture(const QString &textureName)
{
  Entity *pEntity;
  TextureLayer* pLayer;
  Material *pMat;
	numScene=mViewer->getViewport();

	if (numScene==1)
	{
		pEntity=mScene->getByName("obj");
	}
	else
	{
		pEntity=mScene2->getByName("obj");
	}

  pMat=pEntity->getMaterial();
  pLayer = new TextureLayer(textureName);
  if (pMat->getNumberOfLayers()!=0) pMat->popTextureLayer();
  pMat->setBaseColor(Color::WHITE, 0.3);
  pMat->pushTextureLayer(pLayer);
  pMat->setAlphaRejection(0.5);

  pEntity->setMaterial(pMat);
	if (numScene==1)
	{
  	mScene->remove(false);
  	mScene->add(pEntity);
	}
	else
	{
		mScene2->remove(false);
  	mScene2->add(pEntity);
	}

  ui.statusbar->showMessage( tr("Chargement de la Texture ")+textureName+" sur l'objet de la scène", 0 );
  mViewer->updateGL();
}



void BBWindow::viewerAxis()
{
  mViewer->setAxes();
  mViewer->updateGL();
}

void BBWindow::fileCleanScene()
{
  mScene->remove();
	mScene2->remove();
	mScene->setTextureEnable(false);
	mScene2->setTextureEnable(false);
	ui.actionChargerTexture->setEnabled(false);

	ui.actionMode_Normal->setChecked(true);
	ui.actionMode_SplitScreen->setChecked(false);
	ui.actionMode_Fusion->setChecked(false);
	ui.actionMode_SplitScreen->setEnabled(false);
	ui.actionMode_Fusion->setEnabled(false);

	ui.menuViewport->setEnabled(false);
	ui.actionGauche->setChecked(true);
	ui.actionDroite->setChecked(false);

	ui.menuBillboard->setEnabled(false);
	ui.actionBillboard->setEnabled(false);
	ui.actionBillboardCloud->setEnabled(false);
	ui.actionSauvegarder->setEnabled(false);

  Camera *mpCamera = mViewer->getCamera() ;
  mpCamera->reinit(0,0,0);
	reinitViewPort();
  mViewer->updateGL();

}

void BBWindow::toolCamReset()
{
  Camera *mpCamera = mViewer->getCamera() ;
	//si la scene est vide on centre la camera sur le centre du repere
	if(mScene->isEmpty()) mpCamera->reinit(0,0,0);
	//sinon sur le centre de l'objet
  else mViewer->maj();
	mViewer->updateGL();
}

void BBWindow::showDensity()
{
	int nbRowDensity;
  bool ok;
  int res = QInputDialog::getInteger(this, "Densité", "Taille de la matrice de densité",64, 4, 1024, 2,&ok,0);
  if ( ok ) {
    nbRowDensity=res;
  } else {
    nbRowDensity=0;
  }

  if (nbRowDensity != 0 )
  {
		DensityViewer * mm = new DensityViewer(nbRowDensity, ui.actionDensiteTex3D->isChecked(), mScene);
		mm->show();
	}
}

BBWindow::~BBWindow()
{
}


void BBWindow::BB()
{
	//si l'objet de la scene peut etre texture alors il a un objet sur lequel on peur creer un Billboard 
	if(mScene->isTextureEnable()){
		bool ok=false;
		int res = QInputDialog::getInteger(this, "Resolution", "resolution du billboard",512, 16, 512, 2,&ok,0);
		if(ok){
			timer(true);
			reinitViewPort();
			mScene->afficheInfo(4);
  		mViewer->modifAxes(false); 
  		Density *densite = new Density(64);
			Entity* pEntity = mScene->getEntity();
			int reso=2 ;
			while ( reso < res) reso *= 2;
	
			std::vector<float*> vecto2;
			densite->reset();
			mScene->fDensity( 4,densite);
			densite->extractAllPlane(0.3,1,vecto2);
			billbo = new Billboard(vecto2[0],pEntity,1.,mViewer,reso);

			timer(false);
			billbo->setTime(time);
			int mode =1;
			if (ui.actionMode_Normal->isChecked() & numScene==1) mode =0;
			if (ui.actionMode_Fusion->isChecked()  & numScene==1) mode =0;
			if (ui.actionMode_Fusion->isChecked()  & numScene==2) mode =2;
			billbo->setMode(mode,ui.actionInformation_Objet->isChecked());

			numScene=1;
			mViewer->selectViewPort(numScene);
			mViewer->changeMode(1);
			mScene2->remove();
			mScene2->add(billbo);

			ui.actionSauvegarder->setEnabled(true);
			toggleSplitScreen();
			mScene->afficheInfo(5);
			mViewer->maj();
			isBCC=false;
			ui.actionChargerTexture->setEnabled(true);
		}
	}
	else 	statusBar()->showMessage( tr("Pas d'objet valide dans la scène pour la construction"), 2000 );
}


void BBWindow::BBC(){
	if(ui.actionBoite_Englobante->isChecked())
	{
		BBCBox();
		return;
	}
	if(mScene->isTextureEnable()){
		timer(true);
		bool ok=false;
		int res = QInputDialog::getInteger(this, "Choix de résolution maximale", "résolution du billboardCloud",512, 16, 512, 2,&ok,0);
		if(ok){
			ok = false;
			int nbBB = QInputDialog::getInteger(this, "Choix du nombre de Billboards", "Combien de Billboards ?",50, 1, 500, 1,&ok,0);
			if(ok){
				reinitViewPort();
				mScene->afficheInfo(4);
  			mViewer->modifAxes(false); 
  			Density *densite = new Density(128);
				Entity* pEntity = mScene->getEntity();
				int reso=2 ;
				while ( reso < res) reso *= 2;
	
				std::vector<float*> vecto2;
				densite->reset();
				int coverage = QInputDialog::getInteger(this, "Choix du coverage", "Quel coverage?",5, 1, 20, 1,&ok,0);
				mScene->fDensity( coverage,densite);
				densite->extractAllPlane(0.0001,nbBB,vecto2);
  			bbc = new BillboardCloud(pEntity,vecto2,mViewer,0.025,reso);

				timer(false);
				bbc->setTime(time);
				numScene=1;
				mViewer->selectViewPort(numScene);
				mViewer->changeMode(1);
				mScene2->remove();
				mScene2->add(bbc);
				int mode =1;
				if (ui.actionMode_Normal->isChecked() & numScene==1) mode =0;
				if (ui.actionMode_Fusion->isChecked()  & numScene==1) mode =0;
				if (ui.actionMode_Fusion->isChecked()  & numScene==2) mode =2;
				bbc->setMode(mode,ui.actionInformation_Objet->isChecked());
			
		
				ui.actionSauvegarder->setEnabled(true);
				toggleSplitScreen();
				mScene->afficheInfo(5);
				ui.actionChargerTexture->setEnabled(true);
				isBCC=true;
				mViewer->maj();
			}
		}
	}
	else 	statusBar()->showMessage( tr("Pas d'objet valide dans la scène pour la construction"), 2000 );
}

void BBWindow::BBCBox(){
	if(mScene->isTextureEnable()){
		bool ok=false;
		int res = QInputDialog::getInteger(this, "Choix de résolution maximale", "résolution du billboard",512, 16, 512, 2,&ok,0);
		if(ok){
			timer(true);
			reinitViewPort();
			mScene->afficheInfo(4);

			mViewer->modifAxes(false);

			std::vector<float*> vecto;
			Entity* pEntity = mScene->getEntity();
			//pour la creation de la boite englobante
			Billboard *bb = new Billboard(pEntity,mViewer);


			float a,b,c,d,e,f,g,h,i,ap,bp,cp,dp;
			// plan 1
			a=(bb->getBox()[0]).x;
			b=(bb->getBox()[0]).y;
			c=(bb->getBox()[0]).z;
			d=(bb->getBox()[1]).x;
			e=(bb->getBox()[1]).y;
			f=(bb->getBox()[1]).z;
			g=(bb->getBox()[2]).x;
			h=(bb->getBox()[2]).y;
			i=(bb->getBox()[2]).z;

			ap = b*f + e*i + h*c - f*h - i*b - c*e ;
			bp = -(a*f + d*i + g*c - f*g - i*a - c*d);
			cp = a*e + d*h + g*b - e*g - h*a - b*d;
			dp = -(a*e*i + d*h*c + g*b*f - c*e*g - f*h*a - i*b*d);

			float fl[4] = {ap,bp,cp,dp};
			vecto.push_back(fl);

			// plan 2
			a=(bb->getBox()[0]).x;
			b=(bb->getBox()[0]).y;
			c=(bb->getBox()[0]).z;
			d=(bb->getBox()[1]).x;
			e=(bb->getBox()[1]).y;
			f=(bb->getBox()[1]).z;
			g=(bb->getBox()[7]).x;
			h=(bb->getBox()[7]).y;
			i=(bb->getBox()[7]).z;

			ap = b*f + e*i + h*c - f*h - i*b - c*e ;
			bp = -(a*f + d*i + g*c - f*g - i*a - c*d);
			cp = a*e + d*h + g*b - e*g - h*a - b*d;
			dp = -(a*e*i + d*h*c + g*b*f - c*e*g - f*h*a - i*b*d);

			float fl2[4] = {ap,bp,cp,dp};
			vecto.push_back(fl2);

			//plan 3
			a=(bb->getBox()[3]).x;
			b=(bb->getBox()[3]).y;
			c=(bb->getBox()[3]).z;
			d=(bb->getBox()[1]).x;
			e=(bb->getBox()[1]).y;
			f=(bb->getBox()[1]).z;
			g=(bb->getBox()[7]).x;
			h=(bb->getBox()[7]).y;
			i=(bb->getBox()[7]).z;

			ap = b*f + e*i + h*c - f*h - i*b - c*e ;
			bp = -(a*f + d*i + g*c - f*g - i*a - c*d);
			cp = a*e + d*h + g*b - e*g - h*a - b*d;
			dp = -(a*e*i + d*h*c + g*b*f - c*e*g - f*h*a - i*b*d);

			float fl3[4] = {ap,bp,cp,dp};
			vecto.push_back(fl3);

			//plan 4
			a=(bb->getBox()[4]).x;
			b=(bb->getBox()[4]).y;
			c=(bb->getBox()[4]).z;
			d=(bb->getBox()[5]).x;
			e=(bb->getBox()[5]).y;
			f=(bb->getBox()[5]).z;
			g=(bb->getBox()[7]).x;
			h=(bb->getBox()[7]).y;
			i=(bb->getBox()[7]).z;

			ap = b*f + e*i + h*c - f*h - i*b - c*e ;
			bp = -(a*f + d*i + g*c - f*g - i*a - c*d);
			cp = a*e + d*h + g*b - e*g - h*a - b*d;
			dp = -(a*e*i + d*h*c + g*b*f - c*e*g - f*h*a - i*b*d);

			float fl4[4] = {ap,bp,cp,dp};
			vecto.push_back(fl4);

			//plan 5
			a=(bb->getBox()[0]).x;
			b=(bb->getBox()[0]).y;
			c=(bb->getBox()[0]).z;
			d=(bb->getBox()[2]).x;
			e=(bb->getBox()[2]).y;
			f=(bb->getBox()[2]).z;
			g=(bb->getBox()[6]).x;
			h=(bb->getBox()[6]).y;
			i=(bb->getBox()[6]).z;

			ap = b*f + e*i + h*c - f*h - i*b - c*e ;
			bp = -(a*f + d*i + g*c - f*g - i*a - c*d);
			cp = a*e + d*h + g*b - e*g - h*a - b*d;
			dp = -(a*e*i + d*h*c + g*b*f - c*e*g - f*h*a - i*b*d);

			float fl5[4] = {ap,bp,cp,dp};
			vecto.push_back(fl5);

			//plan 6
			a=(bb->getBox()[2]).x;
			b=(bb->getBox()[2]).y;
			c=(bb->getBox()[2]).z;
			d=(bb->getBox()[3]).x;
			e=(bb->getBox()[3]).y;
			f=(bb->getBox()[3]).z;
			g=(bb->getBox()[4]).x;
			h=(bb->getBox()[4]).y;
			i=(bb->getBox()[4]).z;

			ap = b*f + e*i + h*c - f*h - i*b - c*e ;
			bp = -(a*f + d*i + g*c - f*g - i*a - c*d);
			cp = a*e + d*h + g*b - e*g - h*a - b*d;
			dp = -(a*e*i + d*h*c + g*b*f - c*e*g - f*h*a - i*b*d);

			float fl6[4] = {ap,bp,cp,dp};
			vecto.push_back(fl6);

			int reso=2 ;
			while ( reso < res) reso *= 2;

			bbc = new BillboardCloud(pEntity,vecto,mViewer,0.1,reso);

			timer(false);
			bbc->setTime(time);
			numScene=1;
			mViewer->selectViewPort(numScene);
			mViewer->changeMode(1);
			mScene2->remove();
			mScene2->add(bbc);
			int mode =1;
			if (ui.actionMode_Normal->isChecked() & numScene==1) mode =0;
			if (ui.actionMode_Fusion->isChecked()  & numScene==1) mode =0;
			if (ui.actionMode_Fusion->isChecked()  & numScene==2) mode =2;
			bbc->setMode(mode,ui.actionInformation_Objet->isChecked());
	
			ui.actionSauvegarder->setEnabled(true);
			toggleSplitScreen();
			mScene->afficheInfo(5);
			ui.actionChargerTexture->setEnabled(true);
			isBCC=true;
			mViewer->maj();
		}
	}
	else 	statusBar()->showMessage( tr("Pas d'objet valide dans la scène pour la construction"), 2000 );
}

void BBWindow::viewerWireMode(){

  if (mViewer->isWireframeEnabled())
    mViewer->setWireframeEnabled(false);
  else
    mViewer->setWireframeEnabled(true);
}

void BBWindow::statusChanged(const QString &message)
{
	ui.statusbar->showMessage( message, 0 );
}

void BBWindow::toggleShowFps(void)
{
    mShowFps = !mShowFps;
    if(mShowFps)
    {
        connect(mViewer, SIGNAL(fpsChanged(const QString &)), this, SLOT(statusChanged(const QString &)));
    }
    else
    {
        disconnect(mViewer, SIGNAL(fpsChanged(const QString &)), this, SLOT(statusChanged(const QString &)));
				ui.statusbar->showMessage( tr(" "), 0 );
    }
    mViewer->updateGL();
}

void BBWindow::toogleBorder(void)
{
	mViewer->borderChange();
	mViewer->updateGL();
}

void BBWindow::toggleNormal(void)
{
	ui.actionMode_Normal->setChecked(true);
	ui.actionMode_SplitScreen->setChecked(false);
	ui.actionMode_Fusion->setChecked(false);
	mViewer->changeMode(0);
	
	if(numScene==1){
		ui.actionGauche->setChecked(true);
		ui.actionDroite->setChecked(false);
		mScene->afficheInfo(2);
	}
	else{
		ui.actionGauche->setChecked(false);
		ui.actionDroite->setChecked(true);
		mScene2->afficheInfo(2);
	}
	mViewer->updateGL();
}

void BBWindow::toggleSplitScreen(void)
{
	if(ui.actionMode_SplitScreen->isEnabled())
	{
		ui.actionMode_Normal->setChecked(false);
		ui.actionMode_SplitScreen->setChecked(true);
		ui.actionMode_Fusion->setChecked(false);

		ui.menuViewport->setEnabled(true);

		mViewer->changeMode(1);

		mScene->afficheInfo(1);
		mScene2->afficheInfo(1);
		mViewer->updateGL();
	}
}

void BBWindow::toggleFusion()
{
	if(ui.actionMode_Fusion->isEnabled())
	{
		ui.actionMode_Normal->setChecked(false);
		ui.actionMode_SplitScreen->setChecked(false);
		ui.actionMode_Fusion->setChecked(true);

		ui.menuViewport->setEnabled(true);

		mViewer->changeMode(2);

		mScene->afficheInfo(2);
		mScene2->afficheInfo(3);
		mViewer->updateGL();
	}
}

void BBWindow::selectViewPortLeft(void)
{
	numScene=1;
	mViewer->selectViewPort(numScene);
	ui.actionDroite->setChecked(false);
	if (ObjIsAMesh()) ui.actionChargerTexture->setEnabled(true);
	else ui.actionChargerTexture->setEnabled(false);
	mViewer->updateGL();
}

void BBWindow::selectViewPortRight(void)
{
	if(ui.menuViewport->isEnabled())
	{
		numScene=2;
		mViewer->selectViewPort(numScene);
		ui.actionGauche->setChecked(false);
		if (ObjIsAMesh()) ui.actionChargerTexture->setEnabled(true);
		else ui.actionChargerTexture->setEnabled(false);
		mViewer->updateGL();
	}
}

void BBWindow::toggleInfo()
{
	mScene->afficheInfo(0);
	mScene2->afficheInfo(0);
	mViewer->updateGL();
}

void BBWindow::reinitViewPort(void)
{
	numScene=1;
	mViewer->selectViewPort(numScene);
	mViewer->changeMode(0);
	ui.actionMode_SplitScreen->setChecked(false);
	ui.actionMode_Fusion->setChecked(false);
	ui.actionGauche->setChecked(true);
	ui.actionDroite->setChecked(false);
}

void BBWindow::SaveAs(void)
{
	if(!isBCC){
		QString filename;
		QString fn = QFileDialog::getSaveFileName(this, "Enregistrer un Billboard","./../../branches/Objets/",
								tr("Billboard (*.bb)"), 0,0);
		if ( !fn.isEmpty() ) {
			if(fn.contains(".bb")) filename=fn.section('.',0,0);
			else filename = fn;
			
			QFile BBFile(filename.toAscii()+".png");
			if(BBFile.open(QIODevice::ReadOnly))
			{
				QMessageBox msgBox;
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.setWindowTitle("Conflit : Texture déjà présente");
				msgBox.setText("Attention, la texture liée à ce Billboard est déjà liée à un autre Billboard, voulez-vous continuer?");
				msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				switch (msgBox.exec()) {
					case QMessageBox::Yes:
					break;
					case QMessageBox::No:
						return ;
					break;
				}
			}
			QFile f( filename+".bb" );
			if ( !f.open( QIODevice::WriteOnly ) ) {
				statusBar()->showMessage( tr("Pas de droit d'écriture").arg(fn),2000 );
				return;
			}
			QTextStream t( &f );
			t << billbo->saveBillboard(filename) ;
			f.close();
		} else {
			statusBar()->showMessage( tr("Sauvegarde annulée"), 2000 );
		}
	}
	else{
		QString filename;
		QString fn = QFileDialog::getSaveFileName(this, "Enregistrer un BillboardCloud","./../../branches/Objets/",
					    tr("BillboardCloud (*.bbc)"), 0,0);
		  if ( !fn.isEmpty() ) {
		if(fn.contains(".bbc")) filename=fn.section('.',0,0);
		else filename = fn;

		QFile BBFile(filename.toAscii()+".png");
			if(BBFile.open(QIODevice::ReadOnly))
			{
				QMessageBox msgBox;
				msgBox.setIcon(QMessageBox::Warning);
				msgBox.setWindowTitle("Conflit : Texture déjà présente");
				msgBox.setText("Attention, la texture liée à ce Billboard Cloud est déjà liée à un autre Billboard ou Billboard Cloud, voulez-vous continuer?");
				msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				switch (msgBox.exec()) {
					case QMessageBox::Yes:
					break;
					case QMessageBox::No:
						return ;
					break;
				}
			}

		QFile f( filename+".bbc" );
		if ( !f.open( QIODevice::WriteOnly ) ) {
			statusBar()->showMessage( tr("Pas de droit d'écriture").arg(fn),2000 );
			return;
		}
		QTextStream t( &f );
    t << bbc->saveBillboardCloud(filename) ;
    f.close();
		} else {
			statusBar()->showMessage( tr("Sauvegarde annulée"), 2000 );
		}
	}
}

bool BBWindow::ObjIsAMesh(void)
{
	Entity *pEntity;
	if (numScene==1)
	{
		if(mScene->isEmpty()) return false;
		pEntity=mScene->getEntity();
	}
	else
	{
		if(mScene2->isEmpty()) return false;
		pEntity=mScene2->getEntity();
	}

	if(pEntity->getName() == "obj" ) return true;
	else return false;
}

void BBWindow::timer(bool start)
{
	if(start)
	{
		debut = clock();
		fin = clock();
	}
	else
	{
		fin = clock();
		time = (fin - debut) / CLOCKS_PER_SEC;
	}
}

