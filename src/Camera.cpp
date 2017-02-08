
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/
 
#include "Camera.h"

Camera::Camera()
    :   mCamX(1., 0., 0.), mCamY(0., 1., 0.),  mCamZ(0., 0., -1.),
       mPosition(0., 0., 2.), mDist(2.0), mFov(70.),mVectorUp(0., 1.0, 0.),mIsPerspective(true)
{
	mCamX.normalise();
	mCamY.normalise();
	mCamZ.normalise();
}


Camera::Camera(const Vector3& viewDir, const Vector3& upDir, bool isPerspective) 
    :   mCamY(upDir), mCamZ(viewDir), mDist(10.0), mFov(60.),mVectorUp(upDir),mIsPerspective(isPerspective)
{
	mCamZ.normalise();
	mCamX = mCamZ.crossProduct(mCamY);
	mCamX.normalise();
	mCamY = mCamX.crossProduct(mCamZ);
	mCamY.normalise();
	
	mPosition = mCamZ*-10.0;
}


Camera::~Camera(){
}


void Camera::renderGLStates()
{

	// set the viewport
	glViewport(mX, mY, mVpWidth, mVpHeight);
	// set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float ratio = (float)mVpWidth/(float)mVpHeight;

   if (mIsPerspective){
   	
		gluPerspective(mFov, ratio, 0.05, 10000.);
		mCamX.normalise();
		mCamY.normalise();
		mCamZ.normalise();
	}
   else{
	//initialise le viewport et le glOrtho pour recuperer la texture
		glViewport(mX, mY, nX, nY);
 		glOrtho(-pX/2.,pX/2.,-pY/2., pY/2.,0., epsilon*2.);

	}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Vector3 target(mPosition);
		target += mDist * mCamZ;
		gluLookAt(
       mPosition.x, mPosition.y, mPosition.z,
       target.x, target.y, target.z,
       mCamY.x, mCamY.y, mCamY.z);
}

void Camera::setViewportSize(int posX, int posY, int width, int height)
{
    mVpWidth = width;
    mVpHeight = height;
		mX=posX;
		mY=posY;
}
int Camera::getWidth(void)
{
    return mVpWidth;
}
int Camera::getHeight(void)
{
    return mVpHeight;
}

Vector3& Camera::getPosition(void)
{
	return mPosition;
}

void Camera::setPosition(const Vector3& v)
{
    mPosition = v;
}

void Camera::setDist(const float v)
{
  mDist = v;
}

void Camera::polarRotate(float dTheta, float dPhi)
{
    if (mIsPerspective) {

        Vector3 diff = - mDist * mCamZ;
        Vector3 target = mPosition - diff;
    
        Quaternion q0, q1;
        q0.FromAngleAxis(dTheta, mCamY);
        q1.FromAngleAxis(-dPhi, mCamX);
        diff = (q1 * q0) * diff;
    
        mPosition = target + diff;
    
        mCamZ = -diff;
        mCamZ.normalise();
        mCamX = mCamZ.crossProduct(mCamY);
        // projection de mCamX sur le plan horizontal
        //mCamX -= mVectorUp * mCamX.dotProduct(mVectorUp);
        mCamX.normalise();
        mCamY = mCamX.crossProduct(mCamZ);
        mCamY.normalise();
        mCamX = mCamZ.crossProduct(mCamY);
        mCamX.normalise();
    }
}

void Camera::polarZoom(float d)
{
				if(mDist>d)
        {
                mDist -= d;
                mPosition += mCamZ * d;
        }
}

void Camera::localTranslate(const Vector3& t)
{
    mPosition += mCamX * t.x;
    mPosition += mCamY * t.y;
    mPosition += mCamZ * t.z;
    if (!mIsPerspective)
        if(mDist>t.z)
            mDist -= t.z; 

}

void Camera::localRotate(float dTheta, float dPhi)
{
    if (mIsPerspective) {
        Quaternion q0, q1;
        q0.FromAngleAxis(-dTheta, mCamY);
        q1.FromAngleAxis(dPhi, mCamX);
        mCamZ = (q0*q1) * mCamZ;
    
        mCamZ.normalise();
        mCamX = mCamZ.crossProduct(mCamY);
        // projection de mCamX sur le plan horizontal
        mCamX -= mVectorUp * mCamX.dotProduct(mVectorUp);
        mCamX.normalise();
        mCamY = mCamX.crossProduct(mCamZ);
        mCamY.normalise();
        mCamX = mCamZ.crossProduct(mCamY);
        mCamX.normalise();
    }
}

void Camera::reinit(float tx, float ty, float tz)

{
  mCamX=Vector3(1., 0., 0.);
  mCamY=Vector3(0., 1., 0.);
  mCamZ=Vector3(0., 0., -1.);
  mPosition=Vector3(0., 0., 2.);
  mPosition+=Vector3(tx, ty, tz);
  mDist=2.0;
  mFov=70.;
  mVectorUp=Vector3(0., 1.0, 0.);
}

void Camera::setPerspective( bool b){
	mIsPerspective=b;
}


/** Nom: getValCam
	* Utilit? : Retourne les param?tres de la cam?ra
	* Param?tres : Aucun
	* Sortie : un vector de Vector3, contenant les param?tre de la cam?ra
	* 						- mCamX
	* 						- mCamY
	* 						- mCamZ
	* 						- mPosition
	* 						- mDistance
	*/
std::vector<Vector3> Camera::getValCam(void)
{
	std::vector<Vector3> res;
	res.push_back(mCamX);
	res.push_back(mCamY);
	res.push_back(mCamZ);
	res.push_back(mPosition);
	res.push_back(Vector3(mDist,0.,0.));
	return res;
}

void Camera::setX(const Vector3& v){
	mCamX=v;
}

void Camera::setY(const Vector3& v){
	mCamY=v;
}

void Camera::setZ(const Vector3& v){
	mCamZ=v;
}


void Camera::setViewport2(int x, int y, float a, float b, float c){
	nX=x;
	nY=y;
	pX=a;
	pY=b; 
	epsilon=c;
}
