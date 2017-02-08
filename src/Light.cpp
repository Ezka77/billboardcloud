
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/
 
#include "Light.h"
#include "Material.h"
//#include "OpenGL.h"
#include <GL/glut.h>

//--------------------------------------------------------------------------------
Light::Light(LightType lt,const QString& name)
    : Entity(name), mColor(Color::WHITE), mGlLightId(0), mType(lt), mAperture(45.), mExponent(10.)
{
	setPosition(Vector3(1.,1.,1.));
	setDirection(Vector3(-1.,-1.,-1.));
	mpMaterial = new Material();
	mpMaterial->setLighted(false);
	mpMaterial->setBaseColor(mColor,0.5);
}
Light::~Light()
{
	delete mpMaterial;
}
//--------------------------------------------------------------------------------
Light::LightType Light::getType(void) const
{
	return mType;
}
//--------------------------------------------------------------------------------
void Light::activate(int num)
{
	float v4[4];

	mGlLightId = GL_LIGHT0 + num;

	glLightfv(mGlLightId, GL_AMBIENT, Color::BLACK);
	glLightfv(mGlLightId, GL_DIFFUSE, mColor);
	glLightfv(mGlLightId, GL_SPECULAR, mColor);

	if(mType != LT_DIRECTIONAL)
	{
		Vector3 pos = getPosition();
		v4[0] = pos.x; v4[1] = pos.y; v4[2] = pos.z;
		v4[3] = 1.;
	}
	else
	{
		Vector3 dir = getDirection();
		v4[0] = dir.x; v4[1] = dir.y; v4[2] = dir.z;
		v4[3] = 0.;
	}
	glLightfv(mGlLightId, GL_POSITION, v4);

	if(mType == LT_SPOT)
	{
		Vector3 dir = getDirection();
		v4[0] = dir.x; v4[1] = dir.y; v4[2] = dir.z;
		v4[3] = 1.;
		glLightfv(mGlLightId, GL_SPOT_DIRECTION, v4);
		glLightf(mGlLightId, GL_SPOT_CUTOFF, mAperture);
		glLightf(mGlLightId, GL_SPOT_EXPONENT, mExponent);
	}
	else
	{
		glLightf(mGlLightId, GL_SPOT_CUTOFF, 180.);
	}
	glEnable(mGlLightId);
}
//--------------------------------------------------------------------------------
void Light::release(void)
{
    glDisable(mGlLightId);
    glLightfv(mGlLightId, GL_AMBIENT, Color::BLACK);
    glLightfv(mGlLightId, GL_DIFFUSE, Color::BLACK);
    glLightfv(mGlLightId, GL_SPECULAR, Color::BLACK);
}
//--------------------------------------------------------------------------------
void Light::setPosition(const Vector3& pos)
{
    mTransform.setPosition(pos);
}
const Vector3& Light::getPosition(void) const
{
    return mTransform.getPosition();
}
//--------------------------------------------------------------------------------
void Light::setDirection(const Vector3& _dir)
{
    Vector3 dir = _dir;
    dir.normalise();
    Vector3 rotAxes = Vector3::UNIT_Z.crossProduct(dir);
    float rotAngle = acos(Vector3::UNIT_Z.dotProduct(dir));
    mTransform.setOrientation(rotAngle, rotAxes.x, rotAxes.y, rotAxes.z);
}
Vector3 Light::getDirection(void) const
{
    Vector3 x, y, z;
    mTransform.getOrientation().ToAxes(x, y, z);
    return z;
}
//--------------------------------------------------------------------------------
void Light::setColor(const Color& c)
{
    mColor = c;
    mpMaterial->setBaseColor(mColor,0.5);
}
//--------------------------------------------------------------------------------
float Light::getAperture(void) const
{
    return mAperture;
}
void Light::setAperture(float a)
{
    if(a<90.)
        mAperture = a;
}
float Light::getExponent(void) const
{
    return mExponent;
}
void Light::setExponent(float e)
{
    mExponent = e;
}
//--------------------------------------------------------------------------------
void Light::drawGeometry(void)
{
}

Vector3 Light::getCenterM(void)
{
		Vector3 center;
		center.x = 0.;
		center.y = 0.;
		center.z = 0.;
		return center;
}

std::vector<Vector3> Light::getVertexArray(){
	std::vector<Vector3> vect;
	return vect;
}


void Light::deleteVBO(){}

void Light::afficheInfoM(int split){}

void Light::fillDensity(int coverage, Density *pDensity){}
