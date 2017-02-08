
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _Camera_h_
#define _Camera_h_

#include <GL/glew.h>
#include <vector>
#include "Vector3.h"


/** 
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */


class Camera
{
public:

	Camera();
	virtual ~Camera();
	Camera(const Vector3& viewDir, const Vector3& upDir, bool isPerspective = true);
	virtual void renderGLStates();
	void setViewportSize(int posX, int posY, int width, int height);
	int getWidth(void);
	int getHeight(void);
	void setPosition(const Vector3& v);
	void setDist(const float v);
	Vector3& getPosition(void);
	std::vector<Vector3> getValCam(void);
	void reinit(float tx, float ty, float tz);
	void setPerspective(bool b);
	/**
	@name setX
	@param v le nouveau vecteur X du repere camera
	*/
	void setX(const Vector3& v);
	/**
	@name setY
	@param v le nouveau vecteur Y du repere camera
	*/
	void setY(const Vector3& v);
	/**
	@name setZ
	@param v le nouveau vecteur Z du repere camera
	*/	
	void setZ(const Vector3& v);
	/**
	@name setViewport2
	@param x largeur du viewport 
	@param y hauteur du viewport
	@param a parametre pour la largeur du glOrtho 
	@param b parametre pour la hauteur du glOrtho
	@param c parametre pour la profondeur du glOrtho
	*/
	void setViewport2(int x,int y, float a, float b,float c);
	
	virtual void polarRotate(float dTheta, float dPhi);
	virtual void polarZoom(float d);
	virtual void localTranslate(const Vector3& t);
	virtual void localRotate(float dTheta, float dPhi);	
	protected:
	Vector3 mCamX, mCamY, mCamZ;
	Vector3 mPosition;
	float mDist;
	float mFov;
	/**
	les parametres du glOrtho
	*/
	float pX,pY,epsilon;
	/**
	les parametres du viewport
	*/
	int mVpWidth, mVpHeight,mX, mY,nX,nY;
	Vector3 mVectorUp;
	bool mIsPerspective; 

};

#endif

