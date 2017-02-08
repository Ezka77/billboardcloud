
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _Light_h_
#define _Light_h_

#include <GL/glew.h>
#include "Vector3.h"
#include "Material.h"
#include "Entity.h"

/** 
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

class Light : public Entity
{
public:
	enum LightType {LT_POINT, LT_DIRECTIONAL, LT_SPOT};

	Light(LightType lt, const QString& name = "");

	~Light();

	LightType getType(void) const;

	/** Met à  jour les états OpenGL et active la source lumineuse.
			Le numéro de la source lumineuse OpenGL utilisé est GL_LIGHTnum, cà d GL_LIGHT0 + num.
	*/
	void activate(int num);

	/** Désactive la source lumimeuse.
	*/
	void release(void);

	/** Position de la source lumineuse.
			Ignorée pour une source directionnelle.
	*/
	void setPosition(const Vector3& pos);
	const Vector3& getPosition(void) const;

	/** Direction de la source lumineuse.
			Ignorée pour une source ponctuelle.
	*/
	void setDirection(const Vector3& pos);
	Vector3 getDirection(void) const;

	/** Couleur de la source lumineuse
	*/
	void setColor(const Color& c);

	/** Angle d'ouverture du spot.
	*/
	float getAperture(void) const;
	void setAperture(float a);

	/** Atténuation du faisceau.
	*/
	float getExponent(void) const;
	void setExponent(float e);

protected:

	virtual void drawGeometry(void);
	virtual Vector3 getCenterM(void);
	virtual std::vector<Vector3> getVertexArray();
	virtual void deleteVBO();
	virtual void afficheInfoM(int split);
	virtual void fillDensity(int coverage, Density *pDensity);
	Color mColor;
	int mGlLightId;
	LightType mType;
	float mAperture;
	float mExponent;
};

#endif
