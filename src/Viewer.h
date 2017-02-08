
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _Viewer_h_
#define _Viewer_h_


#include "OpenGL.h"
#include "Camera.h"
#include "Density.h"
#include "Timer.h"

#include <QGLWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include <QTime>

class Scene;
class Light;
class Entity;

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;


/** Classe principale representant la fenetre OpenGL. C'est ici que l'on doit creer la scene, les objets, les lumieres, la camera ... C'est aussi ici que doivent etre gere les evenements souris et claviers
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

class Viewer : public QGLWidget
{
Q_OBJECT

public:

  Viewer(QWidget *parent = 0); 
    virtual ~Viewer();
    
    void setAxes();

    void attachScene(Scene* pScene, Scene* pScene2);
    void attachDensity(Density * _pDensity);
    void freeDensity();

    void setWireframeEnabled(bool on);
    bool isWireframeEnabled(void) const;

    Camera *getCamera(void) const;
    void setCamera(Camera *camera);
    void maj();
		void changeMode(int mode);
		void modifAxes(bool b);
		void selectViewPort(int num);
		void borderChange(void);
		int getViewport();
		void setRotatePossible(bool b);
signals:
    void glInitialized(void);
    void fpsChanged(const QString &);
protected:

    /** Methode appelee automatiquement par QT apres la creation du constext OpenGL.
        C'est ici que l'on peut mettre a jour les etats qui ne seront jamais modifies par la suite.
    */
    virtual void initializeGL ();

    /** Methode appelee automatiquement par QT lorsque le contexte OpenGL change de taille.
        Cad, lorsque la fenetre change de taille !
        Cette methode doit entre autre appeler la methode createScene.
    */
    virtual void resizeGL ( int width, int height );

    /** Methode appelee automatiquement par QT lorsque la fenetre OpenGL doit etre redessinee.
        Pour forcer l'appel de cette fonction par QT, utilisez la methode (qui est aussi un slot) QGLWidget::updateGL().
    */
    virtual void paintGL ();

    //--------------------------------------------------------------------------------
    // Les methodes suivantes permettent gerer les evenements recus par le widget.
    virtual void mousePressEvent ( QMouseEvent * e );
    virtual void mouseReleaseEvent ( QMouseEvent * e );
    virtual void mouseMoveEvent ( QMouseEvent * e );
    //virtual void mouseDoubleClickEvent ( QMouseEvent * e );
    //virtual void wheelEvent ( QWheelEvent * e );
    virtual void keyPressEvent ( QKeyEvent * e );
    //virtual void keyReleaseEvent ( QKeyEvent * e );
    //--------------------------------------------------------------------------------


private:

	/** Methode tracant un repere orthonorme au centre de la scene (en 0,0,0).
			@param length longeur des axes
	*/
	void drawAxes(float length = 1.0);
	void drawSelectionVP();
	/** Enum representant le mode de navigation courrant.
	*/
	enum TrackMode {TM_NO_TRACK=0, TM_POLAR_ROTATE, TM_POLAR_ZOOM, TM_QUAKE_ROTATE, TM_QUAKE_WALK, TM_QUAKE_MOVE_XY};

	/** Mode de navigation courrant.
	*/
	TrackMode mTrackMode;

	bool mWireFrame;


	/** Position precedente de la souris.
	*/
	int mMouseX, mMouseY;

	Camera *mCamera;
	Scene* mpScene;
	Scene* mpScene2;
	Density * pDensity;

	int mWidth, mHeight;
	bool DW_AXES ;
	bool modeSplit;
	bool fondBlanc;
	bool modeFusion;
	int numViewPort;
	bool border;
  bool isRotatePossible;
};

#endif
