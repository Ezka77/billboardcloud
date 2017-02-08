
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _Scene_h_
#define _Scene_h_

#include <vector>
#include <map>

#include "Entity.h"
#include "Light.h"
#include "Density.h"

#include "Color.h"

class ShadowMap;

/** Classe permettant de gerer une scene complete.
*/

/** 
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

class Scene
{
public:
    Scene();
    virtual ~Scene();

    /** Ajoute un objet dans la scene
        @param pObj pointeur sur l'objet a ajouter.
        @param castShadow indique si l'objet genere des ombres projetees ou non.
    */
    void add(Entity* pObj, bool castShadow = true);
		void remove(bool delMesh = true);

    /** Ajoute une source lumineuse dans la scene
        @param pObj pointeur sur la lumiere a ajouter.
        @param castShadow indique si la lumiere projete des ombress ou non.
    */
    void add(Light* pLight, bool castShadow = false);

    Entity* getByName(const QString& name);

    template<class T> T* getByName(const QString& name)
    {
        EntityMap::const_iterator entity_iter = mAllEntities.find(name);
        if(entity_iter!=mAllEntities.end())
        {
            return dynamic_cast<T*>(entity_iter->second);
        }

        return 0;
    }

    void setAmbient(const Color& c);
    const Color& getAmbient(void) const;

    /** Trace la scene.
    */
    virtual void render(void);

    bool isShadowEnabled(void) const;
    void setShadowEnabled(bool on);

		Vector3 getCenter(void);
		void rotateObjet(float dx, float dy, std::vector<Vector3> camParam);
		void afficheInfo(int split);
		void fDensity(int coverage, Density *pDensity);
		Entity* getEntity();
		/**
		@name 	isTextureEnable
		@brief pour voir si l'objet peut etre textute ou non ou s'il y a un objet dans la scene
		*/
		bool isTextureEnable();
			/**
		@name 	setTextureEnable
		@brief pour authoriser ou non le placage de texture sur l'objet de la scene
		*/
		void setTextureEnable(bool b);

		bool isEmpty();
protected:

    typedef std::map<QString,Entity*> EntityMap;

    struct EntityElement
    {
        EntityElement(Entity* e, bool c)
            : pEntity(e), castShadow(c)
        {}
        Entity* pEntity;
        bool castShadow;
    };
    typedef std::vector<EntityElement> EntityList;

    struct LightElement
    {
        LightElement(Light* e, bool c)
            : pLight(e), castShadow(c)
        {}
        Light* pLight;
        bool castShadow;
    };
    typedef std::vector<LightElement> LightList;

    // liste les objets
    EntityList mEntities;

    // liste des sources lumineuses
    LightList mLights;

    EntityMap mAllEntities;

    Color mAmbient;

    ShadowMap* mShadowMap;
    bool mShadowEnabled;

protected:

    //--------------------------------------------------------------------------------
    // fonctions utiles pour les differents mode de rendu
    void activateLights(void);
    void releaseLights(void);
    void renderObjects(bool activateMaterial = true);
    void renderOccluders(bool activateMaterial = false);
    //--------------------------------------------------------------------------------
	 	
		/**
		texture applicable ou pas
		*/
		bool textureEnable;
		
};

#endif
