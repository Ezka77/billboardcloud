/***************************************************************************
 *   Copyright (C) 2007 by GangsTER   *
 ***************************************************************************/
#include "ObjectView.h"

/* Ajout du BE */
#include "Viewer.h"
#include "Scene.h"
#include "Light.h"
#include "Entity.h"
#include "Material.h"
#include "TextureLayer.h"
#include "Mesh.h"

#include <GL/glut.h>

ObjectView::ObjectView()
    : Q3MainWindow( 0, "Billboard Clouds - For Extreme Model Simplification", Qt::WDestructiveClose )
{
    Q3PopupMenu * file = new Q3PopupMenu( this );
    menuBar()->insertItem( tr("&Fichier"), file );

    file->insertItem( tr("&Quit"), qApp, SLOT( closeAllWindows() ), Qt::CTRL+Qt::Key_Q );

    menuBar()->insertSeparator();

   	QGLFormat format;
    format.setDoubleBuffer(true);
    format.setDepth(true);
    format.setRgba(true);
    format.setAlpha(true);
    format.setStencil(true);
    format.setStereo(false);
    QGLFormat::setDefaultFormat(format);
   // glutInit(0, 0);

    mViewer = new Viewer( this, "glviewer" );
    mViewer->setFocus();
    setCentralWidget( mViewer );
		statusBar()->message(tr("Ready"), 2000);


    connect(mViewer, SIGNAL(glInitialized()), this, SLOT(createScene()));
   
    mScene = new Scene();
    mViewer->attachScene(mScene);
		
}


ObjectView::~ObjectView()
{
}

void ObjectView::createScene(void)
{
    Light *pLight;
    pLight = new Light(Light::LT_POINT);
    pLight->setPosition(Vector3(0., 0.,20.));
    pLight->setColor(Color(1.,1.,1.));
    mScene->add(pLight, false);
}


void ObjectView::load( const QString &fileName )
{
	Entity *pEntity;
	Transform t;
	Material *pMat;

	QFile f( fileName );
	if ( !f.open( QIODevice::ReadOnly ) )
	return;

	pEntity = new Mesh(fileName, "obj");
	t.setIdentity();
	t.setPosition(0., 0., 0.);
	//t.setOrientation(-90.,1.,0.,0.);
	pEntity->setTransform(t);
	pMat = new Material();
	pMat->setBaseColor(Color(0.8,0.6,0.5), 0.4);
	pMat->setSpecular(Color::WHITE, 1);
	pEntity->setMaterial(pMat);
	mScene->remove();
	mScene->add(pEntity);
    statusBar()->message( tr("Chargement de l'objet %1").arg(fileName), 2000 );
	mViewer->maj();
}

void ObjectView::load( const QString &fileName, const QString &textureName)
{
    Entity *pEntity;
    Transform t;
    Material *pMat;
	TextureLayer* pLayer;

    QFile f( fileName );
    if ( !f.open( QIODevice::ReadOnly ) )
	return;

	pEntity = new Mesh(fileName, "obj");
	t.setIdentity();
	t.setPosition(0., 0., 0.);
	//t.setOrientation(-90.,1.,0.,0.);
	pEntity->setTransform(t);
	pMat = new Material();
	pMat->setBaseColor(Color(0.8,0.6,0.5), 0.4);
	pMat->setSpecular(Color::WHITE, 1);

	pLayer = new TextureLayer(textureName);
    pMat->pushTextureLayer(pLayer);

	pEntity->setMaterial(pMat);
	mScene->remove();
	mScene->add(pEntity);
    statusBar()->message( tr("Chargement de l'objet %1").arg(fileName), 2000 );
	mViewer->maj();
}
