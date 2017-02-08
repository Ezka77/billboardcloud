
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/
 
#include "Scene.h"
#include "ShadowMap.h"


Scene::Scene()
    : mAmbient(0.4,0.4,0.4), mShadowMap(0), mShadowEnabled(false),textureEnable(false)
{
}
//--------------------------------------------------------------------------------
Scene::~Scene()
{
    for(EntityMap::iterator it=mAllEntities.begin() ; it!=mAllEntities.end() ; ++it)
    {
        delete it->second;
    }
}
//--------------------------------------------------------------------------------
bool Scene::isShadowEnabled(void) const
{
    return mShadowEnabled;
}
void Scene::setShadowEnabled(bool on)
{
    mShadowEnabled = on;
    if(mShadowEnabled && ! mShadowMap)
    {
        mShadowMap = new ShadowMap;
    }
}
//--------------------------------------------------------------------------------
void Scene::setAmbient(const Color& c)
{
    mAmbient = c;
}
const Color& Scene::getAmbient(void) const
{
    return mAmbient;
}
//--------------------------------------------------------------------------------
void Scene::add(Entity* pObj, bool castShadow /*=true*/)
{
    mAllEntities[pObj->getName()] = pObj;
    mEntities.push_back(EntityElement(pObj,castShadow));
}

void Scene::remove(bool delMesh){
	if (mEntities.size()!=0)
	{
		if(delMesh) delete mEntities[0].pEntity;
		while(mEntities.size()!=0) mEntities.pop_back();
	}
}
//--------------------------------------------------------------------------------
void Scene::add(Light* pLight, bool castShadow /*=false*/)
{
	mAllEntities[pLight->getName()] = pLight;
	mLights.push_back(LightElement(pLight,castShadow));
}
//--------------------------------------------------------------------------------
Entity* Scene::getByName(const QString& name)
{
	EntityMap::const_iterator entity_iter = mAllEntities.find(name);
	if(entity_iter!=mAllEntities.end())
	{
			return entity_iter->second;
	}

	return 0;
}
//--------------------------------------------------------------------------------
void Scene::activateLights(void)
{
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, mAmbient);

    int lightNum = 0;
    for(LightList::iterator it=mLights.begin() ; it!=mLights.end() ; ++it)
    {
        it->pLight->activate(lightNum++);
    }
}
//--------------------------------------------------------------------------------
void Scene::releaseLights(void)
{
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Color::BLACK);
    for(LightList::iterator it=mLights.begin() ; it!=mLights.end() ; ++it)
    {
        it->pLight->release();
    }
}
//--------------------------------------------------------------------------------
void Scene::renderObjects(bool activateMaterial /*= true*/)
{
    for(EntityList::iterator it=mEntities.begin() ; it!=mEntities.end() ; ++it)
    {
				it->pEntity->render(activateMaterial);
    }
}
//--------------------------------------------------------------------------------
void Scene::renderOccluders(bool activateMaterial /*= false*/)
{
    for(EntityList::iterator it=mEntities.begin() ; it!=mEntities.end() ; ++it)
    {
        if(it->castShadow)
            it->pEntity->render(activateMaterial);
    }
}
//--------------------------------------------------------------------------------
void Scene::render(void)
{
    if(mShadowEnabled)
    {
        // recherche de la premiere source devant generer des ombres portees
        Light* pLight4Shadow = 0;
        for(LightList::iterator it=mLights.begin() ; it!=mLights.end() ; ++it)
        {
            if(it->castShadow)
            {
                pLight4Shadow = it->pLight;
                it = mLights.end();
                it--;
            }
        }
        if(pLight4Shadow)
        {
            /*******************************************************************/
            /* Debut code a faire/completer par l'etudiant                     */
            /* shadow map                                                      */
            /*******************************************************************/

            // a faire : mise a jour de la shadow map et rendu de la scene avec la shadow map
            
            // mise a jour de la shadow map
            mShadowMap->setLight(pLight4Shadow);
            mShadowMap->beginUpdate();
            renderOccluders();
            mShadowMap->endUpdate();

            // rendu de la scene avec la shadow map
            activateLights();
            mShadowMap->activate();
            renderObjects();
            mShadowMap->release();
            releaseLights();
            /*******************************************************************/
            /* Fin code a faire/completer par l'etudiant                       */
            /* shadow map                                                      */
            /*******************************************************************/
        }
        else
        {
            activateLights();
            renderObjects();
            releaseLights();
        }
    }
    else
    {
        activateLights();
        renderObjects();
        releaseLights();
    }

    for(LightList::iterator it=mLights.begin() ; it!=mLights.end() ; ++it)
    {
        it->pLight->render();
    }

}
//--------------------------------------------------------------------------------


Vector3 Scene::getCenter(){
		Vector3 center;
		if(mAllEntities.size() ==1)
		{
			center.x = 0.;
			center.y = 0.;
			center.z = 0.;
		}
		else
		{
			center = (mEntities[0].pEntity)->getCenter();
		}
		return center;
}

void Scene::rotateObjet(float dx, float dy, std::vector<Vector3> camParam){
	if(mAllEntities.size() !=3)
		{
			(mEntities[0].pEntity)->Rotation(dx,dy,camParam);
		}
}

Entity* Scene::getEntity(){

return mEntities[0].pEntity;
}

void Scene::afficheInfo(int split)
{
     for(EntityList::iterator it=mEntities.begin() ; it!=mEntities.end() ; ++it)
     {
 				it->pEntity->afficheInfo(split);
     }
}

void Scene::fDensity(int coverage, Density *pDensity)
{
	 for(EntityList::iterator it=mEntities.begin() ; it!=mEntities.end() ; ++it)
     {
 				it->pEntity->fDensity(coverage,pDensity);
     }
}

bool Scene::isTextureEnable(){
	return textureEnable;
}

void Scene::setTextureEnable(bool b){
	 textureEnable = b;
}

bool Scene::isEmpty(){
	if(mEntities.size()==0) return true;
	else return false;
}
