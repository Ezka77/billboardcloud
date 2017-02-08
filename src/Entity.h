#ifndef _Entity_h_
#define _Entity_h_


#include <GL/glew.h>
#include "Transform.h"
#include <qstring.h>
#include <vector>
#include "Vector3.h"
#include "Density.h"

class Material;

/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/


/** Classe abstraite repr�sentant un objet dans la sc�ne.
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

class Entity
{
public:

	Entity(const QString& name = "");
	
	virtual ~Entity() {};

	inline const QString& getName(void) const
	{
			return mName;
	}
	float thetaX,thetaY,thetaZ;
	Material* getMaterial(void) const;
	void setMaterial(Material* mat);
	
	void setTransform(const Transform& t);
	const Transform& getTransform(void);
	void applyTransform(void) const;
	
	void Rotation(float dTheta, float dPhi, std::vector<Vector3> camParam);
	/** m�thode � appeler pour tracer l'objet
	*/
	virtual void render(bool activateMaterial = true);
	virtual Vector3 getCenter(void);
	virtual std::vector<Vector3> getVertexArrayE();
	virtual void deleteMesh(void);
	void afficheInfo(int split);
	void fDensity(int coverage, Density *pDensity);
protected:

	/** M�thode charg�e du trac�e de la g�om�trie de l'objet.
	*/
	virtual void drawGeometry(void) = 0;
	virtual Vector3 getCenterM(void)=0;
	virtual std::vector<Vector3> getVertexArray()=0;
	virtual void deleteVBO() = 0;
	virtual void afficheInfoM(int split) = 0;
	virtual void fillDensity(int coverage, Density *pDensity)=0;
	QString mName;
	
	/** Stock le mat�riau de l'objet.
			Un m�re materiau peut �tre partag�e entre plusieurs objets.
	*/
	Material* mpMaterial;
	
	/** Position, orientation et mise � l'echelle de l'objet.
	*/
	Transform mTransform;

	static uint msEntitiesCount;
	
};


#endif
