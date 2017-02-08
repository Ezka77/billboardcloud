
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _ShadowMap_h_
#define _ShadowMap_h_

class OffScreenRendering;
class Light;
class Entity;


#include <GL/glew.h>
#include "OpenGL.h"
#include "Matrix4.h"


/** Classe permetant de gérer une shadow map.
    - Une seule instance d'une ShadowMap est généralement suffisante, même pour gérer plusieurs sources lumineuses.
    - Son utilisation doit être très simple :
        - Pour la mise à jour :
        \code
    setLight(light);
    beginUpdate();
    // tracer les occluders
    endUpdate();
        \endcode
        - Au moment du rendu de la scène :
        \code
    activate();
    // tracer les objets de la scène
    release();
        \endcode
        
\note classe à completer entièrement par l'étudiant
*/
class ShadowMap
{
public:

    /** Constructeur.
        \param width largeur de la texture de profondeur.
        \param height hauteur de la texture de profondeur.
        \param stage numéro de l'unité de texture à utiliser.
    */
    ShadowMap(int width=1024, int height=1024, int stage=3);

    ~ShadowMap();

    /** Définie la source lumineuse à utiliser (doit être de type "spot")
    */
    void setLight(Light* pLight);

    /** Méthode à appeler pour mettre à jour la shadow map.
    */
    void beginUpdate(void);

    /** Méthode à appeler à la fin de la mise à jour.
    */
    void endUpdate(void);

    /** Active les état OpenGL permettant le rendu avec la shadow map.
    */
    void activate(void);

    /** Désactive les état OpenGL.
    */
    void release(void);

public:

    /** Epsilon pour le décalage des polygones
    */
    static float configEpsilon;

protected:

    /** Contexte de rendu hors écran.
    */
    OffScreenRendering *mPBuffer;

    /** Source lumineuse courante.
    */
    Light* mpLight;

    static Matrix4 scaleBiasMatrix;

    /** Unité de texture à utiliser.
    */
    GLenum mTextureUnit;

    /** Numéro OpenGL de la texture.
    */
    GLuint mTexId;

    /** Matrice de passage de l'espace scene à l'espace de la shadow map.
    */
    Matrix4 mSceneToLightMatrix;
};

#endif
