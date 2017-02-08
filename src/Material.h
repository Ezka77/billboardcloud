
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _Material_h_
#define _Material_h_

#include <GL/glew.h>
#include <vector>
#include "OpenGL.h"

#include "Color.h"


/** 
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

class TextureLayer;

class Material
{
public:

    Material(void);

    /** Active l'ensemble du materiau, c�d m�j des �tats OpenGL:
          - materiaux
          - blending
          - test alpha
        \note � completer par l'�tudiant (texture)
    */
    void activate(bool alphaOnly = false);

    /** D�sactive le blending, le test alpha
        \note � completer par l'�tudiant (texture)
    */
    void release(bool alphaOnly = false);


    /**
        \param c couleur de l'objet
        \param coeffAmbient coefficient d'ambiant entre 0 et 1
    */
    void setBaseColor(const Color& c, float coeffAmbient);

    /**
        \param c couleur des reflets sp�culaires
        \param shininess coefficient de brillance entre 0 et 1
    */
    void setSpecular(const Color& c, float shininess);

    /** Sp�cifie les facteurs de sources et de destination du blending.
        Par defaut le blending est d�sactiv�.
        Utiliser setBlendFunc(GL_ONE,GL_ZERO) pour d�sactiver le blending dans le mat�riau
    */
    void setBlendFunc(GLenum src, GLenum dst);

    /** Sp�cifie la valeur minimal de l'alpha des fragments.
        Utiliser setAlphaRejection(0) pour d�sactiver le test sur l'alpha
    */
    void setAlphaRejection(float value);
    bool hasAlphaRejection(void) const;

    /** Sp�cifie si le mat�riau peut �tre �clair� ou non.
        \note Ceci est utile pour les objets n'ayant pas de surface ou de normales.
    */
    void setLighted(bool on);
    bool isLighted(void) const;


    /** Ajoute une couche de texture
    */
    void pushTextureLayer(TextureLayer* pTex);
    /** Retire une couche de texture
     */
    void popTextureLayer();
    /** Nombre de couches de texture du materiau
    */
    int getNumberOfLayers(void);

public:

    /** Materiau par defaut.
    */
    static Material Default;

    //--------------------------------------------------------------------------------
    // configuration globale

    /** active/d�sactive l'utilisation des shaders
    */
    static bool configEnableShaders;
    //--------------------------------------------------------------------------------

protected:

    Color mColor;
    Color mAmbient;
    Color mDiffuse;
    Color mSpecular;
    float mShininess;

    bool mActiveBlending;
    GLenum mBlendSrcFunc;
    GLenum mBlendDstFunc;

    bool mAlphaRejection;
    float mAlphaRejectionValue;

    bool mIsLighted;
    bool mPreviousLightingState;

    typedef std::vector<TextureLayer*> TextureLayers;
    /// Stocke la liste des textures associ�es au mat�riau.
    TextureLayers mTextureLayers;
};

#endif
