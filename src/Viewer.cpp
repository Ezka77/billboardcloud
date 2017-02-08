
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#include "Viewer.h"
//#include "Density.h"
#include "OpenGL.h"
#include "Color.h"
#include "Scene.h"
#include <iostream>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QEvent>
#include <QGLWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTimer>



Viewer::Viewer(QWidget *parent)
  : QGLWidget(parent,0,0), mTrackMode(TM_NO_TRACK), mWireFrame(false), mpScene(0), pDensity(NULL), DW_AXES(false), modeSplit(false), fondBlanc(false), modeFusion(false), numViewPort(1), border(true),isRotatePossible(true)
{
	mCamera = new Camera();
}

Viewer::~Viewer()
{
}

void Viewer::attachScene(Scene* pScene, Scene* pScene2)
{
    mpScene = pScene;
		mpScene2 = pScene2;
}

void Viewer::attachDensity(Density * _pDensity)
{
  pDensity = _pDensity ;
}

void Viewer::freeDensity()
{
  if (pDensity!=NULL)
	{
	 	delete pDensity;
	}
   pDensity=NULL;
}

void Viewer::initializeGL()
{
	  // set defaults parameters
		//met le fond en noir transparent
    glClearColor(0., 0., 0., 0.0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glDisable(GL_BLEND);
		//activation et test de transparence
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    emit glInitialized();
}


void Viewer::setWireframeEnabled(bool on)
{
    mWireFrame = on;
		updateGL();
}
bool Viewer::isWireframeEnabled(void) const
{
    return mWireFrame;
}

void Viewer::resizeGL(int width, int height)
{
    mWidth = width;
    mHeight = height;
}

void Viewer::paintGL()
{
	int vp1X, vp1Y, vp1W, vp1H,
			vp2X, vp2Y, vp2W, vp2H;
// Choix du mode Split Screen
	if (modeSplit)
	{
		vp1X=0; vp1Y=0;
		vp1W=mWidth/2; vp1H=mHeight;
		
		vp2X=mWidth/2; vp2Y=0;
		vp2W=mWidth/2; vp2H=mHeight;
	}
// Choix du mode Fusion
	else if (modeFusion)
		{
			vp1X=0; vp1Y=0;
			vp1W=mWidth; vp1H=mHeight;
			
			vp2X=0; vp2Y=0;
			vp2W=mWidth; vp2H=mHeight;
		}
// Choix du mode Normal
	else
	{
		vp1X=0; vp1Y=0;
		vp1W=mWidth; vp1H=mHeight;
		
		vp2X=0; vp2Y=0;
		vp2W=mWidth; vp2H=mHeight;
	}
	mCamera->setViewportSize(vp1X,vp1Y,vp1W, vp1H);
	
  // Clear buffers
  glDisable(GL_COLOR_MATERIAL);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (modeFusion) glEnable (GL_SCISSOR_TEST);

  if(mWireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glDisable(GL_LIGHTING);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glEnable(GL_LIGHTING);
  }

  Timer t;
  t.start();

	mCamera->renderGLStates();
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);
  glColor4f(1., 1., 1. ,1.);

	if ((modeSplit| modeFusion) | numViewPort==1)
	{
		if (glIsEnabled(GL_SCISSOR_TEST)) glScissor (0, 0, mWidth/2, mHeight);
		mpScene->render();
	}


  // Draw world axis
  if (DW_AXES) drawAxes(1.0);
	
	if ((modeSplit| modeFusion) & numViewPort==1) drawSelectionVP();

  if (pDensity!=NULL)
    pDensity->drawDensity(mCamera);

	if ((modeSplit| modeFusion) | numViewPort==2)
	{
		if (glIsEnabled(GL_SCISSOR_TEST)) glScissor (mWidth/2, 0, mWidth/2, mHeight);
		glViewport(vp2X,vp2Y,vp2W, vp2H);
		mpScene2->render();
		if (DW_AXES) drawAxes(1.0);
		if ((modeSplit| modeFusion) & numViewPort==2) drawSelectionVP();
	}

  glFinish();
	
	t.stop();
	float fps=1./t.value();
	QString thetext = QString("FPS : %1").arg(fps);
	emit fpsChanged(thetext);
	if (modeFusion) glDisable (GL_SCISSOR_TEST);
}

void Viewer::mousePressEvent ( QMouseEvent * e )
{
    switch(e->button())
    {
      case Qt::LeftButton:
                mTrackMode = TM_QUAKE_MOVE_XY;//deplacement vertical/horizontal
            break;
      case Qt::MidButton:
                mTrackMode = TM_QUAKE_WALK;//zoom
            break;
      case Qt::RightButton:
	    			mTrackMode = TM_QUAKE_ROTATE;
            break;
        default:
            break;
    }
    mMouseX = e->x();
    mMouseY = e->y();
}

void Viewer::mouseReleaseEvent ( QMouseEvent *  )
{
    mTrackMode = TM_NO_TRACK;
}

void Viewer::mouseMoveEvent ( QMouseEvent * e )
{
		
    if(mTrackMode != TM_NO_TRACK)
    {
        float dx = (float)(e->x() - mMouseX) / (float)mCamera->getWidth();
        float dy = - ((float)(e->y() - mMouseY) / (float)mCamera->getHeight());

        switch(mTrackMode)
        {
            case TM_QUAKE_WALK :
                //mCamera->localTranslate(Vector3(0, 0, dy*10));
		mCamera->polarZoom( dy*10);
                break;
            case TM_QUAKE_MOVE_XY :
                //mCamera->localTranslate(Vector3(dx*10, dy*10, 0));
		mCamera->polarRotate(dx*M_PI, dy*M_PI);
                break;
            case TM_QUAKE_ROTATE :
                //mCamera->localRotate(dx*M_PI, dy*M_PI);
						if(isRotatePossible){
							mpScene->rotateObjet( dx*M_PI,dy*M_PI,mCamera->getValCam());
							mpScene2->rotateObjet( dx*M_PI,dy*M_PI,mCamera->getValCam());
						}
                break;
            default:
                break;
        }

        mMouseX = e->x();
        mMouseY = e->y();
		
    }
        updateGL();
}
void Viewer::keyPressEvent ( QKeyEvent * e )
{
	switch( e->key() )
	{
		default:;	
   }
	updateGL();
}



void Viewer::drawAxes(float length)
{
    static GLUquadricObj *cylindre = gluNewQuadric();

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    //glEnable(GL_LIGHTING);
    //mpScene->activateLights();
    
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Color::WHITE);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,80.0);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    // X axis
    glColor3fv(Color::RED);
    glPushMatrix();
    glRotatef(90,0.0,1.0,0.0);
    gluCylinder(cylindre, length/50.0, length/50.0, 0.8*length, 10, 10);
    glTranslatef(0.0,0.0,0.8*length);
    gluCylinder(cylindre, length/30.0, 0.0, 0.2*length, 10, 10);
    glPopMatrix();

    // Y axis
    glColor3fv(Color::GREEN);
    glPushMatrix();
    glRotatef(-90,1.0,0.0,0.0);
    gluCylinder(cylindre, length/50.0, length/50.0, 0.8*length, 10, 10);
    glTranslatef(0.0,0.0,0.8*length);
    gluCylinder(cylindre, length/30.0, 0.0, 0.2*length, 10, 10);
    glPopMatrix();

    // Z axis
    glColor3fv(Color::BLUE);
    glPushMatrix();
    gluCylinder(cylindre, length/50.0, length/50.0, 0.8*length, 10, 10);
    glTranslatef(0.0,0.0,0.8*length);
    gluCylinder(cylindre, length/30.0, 0.0, 0.2*length, 10, 10);
    glPopMatrix();
    
    glPopAttrib();
}

void Viewer::setAxes()
{
  DW_AXES= !DW_AXES ;
}

Camera *Viewer::getCamera(void) const{
    return mCamera;
}

void Viewer::setCamera(Camera *camera){
    mCamera=camera;
    updateGL();
}

void Viewer::maj(){
	mCamera->reinit(mpScene->getCenter()[0],mpScene->getCenter()[1],mpScene->getCenter()[2]);
	updateGL();
}

void Viewer::changeMode(int mode){
	switch(mode)
	{
		case 0:
			modeSplit=false;
			modeFusion=false;
		break;
		case 1:
			modeSplit=true;
			modeFusion=false;
		break;
		case 2:
			modeSplit=false;
			modeFusion=true;
		break;
		default:;	
  }
}

void Viewer::modifAxes( bool b){
	DW_AXES = b;
}

void Viewer::selectViewPort(int num){
	numViewPort=num;
}

void Viewer::drawSelectionVP(){
	if (border) {
		float decalage =0.;
		if (modeFusion & numViewPort==1) decalage =-0.518;
		if (modeFusion & numViewPort==2) decalage =0.515;
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_LIGHTING);
		glBegin(GL_LINE_LOOP);
			glColor3ub(255,0,0);
			glVertex3d(-0.515+decalage,-0.69,-1.);
			glColor3ub(255,255,0);
			glVertex3d(-0.515+decalage,0.69,-1.);
			glColor3ub(0,0,255);
			glVertex3d(0.515+decalage,0.69,-1.);
			glColor3ub(255,255,0);
			glVertex3d(0.515+decalage,-0.69,-1.);
		glEnd();
		glPopMatrix();
		glPopAttrib();
	}
}

void Viewer::borderChange()
{
	border=!border;
}

int Viewer::getViewport()
{
	return numViewPort;
}

void Viewer::setRotatePossible(bool b){
	isRotatePossible = b;
}

