
#include "Entity.h"
#include "Material.h"


/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

uint Entity::msEntitiesCount = 0;

Entity::Entity(const QString& name)
    : mName(name), mpMaterial(&Material::Default)
{
    if(mName=="")
    {
        mName = QString("Object%1").arg(msEntitiesCount++);
    }
		thetaX=0.;
		thetaY=0.;
		thetaZ=0.;
}
//--------------------------------------------------------------------------------
Material* Entity::getMaterial(void) const
{
    return mpMaterial;
}
void Entity::setMaterial(Material* mat)
{
    mpMaterial = mat;
}
//--------------------------------------------------------------------------------
void Entity::setTransform(const Transform& t)
{
    mTransform = t;
}
const Transform& Entity::getTransform(void)
{
    return mTransform;
}
void Entity::applyTransform(void) const
{
    mTransform.multCurrentMatrix();
}
//--------------------------------------------------------------------------------
void Entity::render(bool activateMaterial /* = true */)
{
	float tmpX=getCenter()[0],tmpY=getCenter()[1],tmpZ=getCenter()[2];

	mpMaterial->activate(!activateMaterial);
	glPushMatrix();
	
	mTransform.multCurrentMatrix();

	glTranslatef(tmpX,tmpY,tmpZ);
	glRotatef (thetaX,1.,0.,0.);
	glRotatef (thetaY,0.,1.,0.);
	glRotatef (thetaZ,0.,0.,1.);
	glTranslatef(-tmpX,-tmpY,-tmpZ);

	drawGeometry();

	glPopMatrix();
	
	mpMaterial->release(!activateMaterial);
}

Vector3 Entity::getCenter(void)
{
		return getCenterM();
}

/** Nom: Rotation
	* Utilit� : Effectue une rotation a l'aide du clic droit de la souris
	* Param�tres : 	dx : mouvement horizontale de la souris
	*								dy : mouvement verticale de la souris
	*								camParam : Param�tres de la cam�ra :
	*										camParam[0] : mCamX
	*										camParam[1] : mCamY
	*										camParam[2] : mCamZ
	*										camParam[3] : mPosition
	*										camParam[4][camParam[3]0] : mDist
	* Sortie : l'objet � effectu� une rotation
	*/
void Entity::Rotation(float dx, float dy, std::vector<Vector3> camParam)
{
	if(camParam[3][0]*camParam[3][0] > camParam[3][2]*camParam[3][2])
 		thetaZ+=camParam[2][0]*dy*-30.;
	else
		thetaX+=camParam[0][0]*dy*-30.;

	thetaY+=dx*30.;

	if (thetaX>360.) thetaX=0.;
	if (thetaY>360.) thetaY=0.;
	if (thetaZ>360.) thetaZ=0.;
	if (thetaX<0.) thetaX=360.;
	if (thetaY<0.) thetaY=360.;
	if (thetaZ<0.) thetaZ=360.;
	
}

std::vector<Vector3> Entity::getVertexArrayE(){
	return getVertexArray();
}

/** Nom : deleteMesh
  * Fct : d�truit le VBO lors de la suppression d'une Mesh
  */
void Entity::deleteMesh(){ 
	deleteVBO();
}


/** Nom : afficheInfo
  * Fct : Appel la fonction pour afficher les informations � l'�cran
  * @param split: Indique quel mode est activ�e 
  * (normal=0, split screen=1, fusion(viewport 1)=2, fusion(viewport 2)=3)
  */
void Entity::afficheInfo(int split){
	afficheInfoM(split);
}

/** Nom : fDensity
  * Fct : Appel la fonction pour cr�er la matrice de densit� d'un objet
  * @param coverage : indice de couverture
 	* @param pDensity : matrice de densit�
  */
void Entity::fDensity(int coverage, Density *pDensity)
{
	fillDensity(coverage, pDensity);
}
