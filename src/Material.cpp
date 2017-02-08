
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/
 
#include "Material.h"
#include "TextureLayer.h"


bool Material::configEnableShaders = false;

Material Material::Default;

//--------------------------------------------------------------------------------
Material::Material(void)
    :   mColor(0.8,0.8,0.8), mAmbient(0.3,0.3,0.3), mDiffuse(0.5,0.5,0.5),
        mSpecular(0.8,0.8,0.8), mShininess(10),
        mActiveBlending(false), mBlendSrcFunc(GL_ONE), mBlendDstFunc(GL_ZERO),
        mAlphaRejection(false), mAlphaRejectionValue(1.), mIsLighted(true)
{
}
//--------------------------------------------------------------------------------
void Material::activate(bool alphaOnly /*= false*/)
{
    if(!alphaOnly)
    {
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

        // si l'éclairage n'est pas activé
        glColor4fv(mColor);

        if(!mIsLighted)
        {
            mPreviousLightingState = glIsEnabled(GL_LIGHTING);
            glDisable(GL_LIGHTING);
        }

        if(mActiveBlending)
        {
            glEnable(GL_BLEND);
            glBlendFunc(mBlendSrcFunc, mBlendDstFunc);
        }
    }

    if(!alphaOnly || mAlphaRejection)
    {
				/*******************************************************************/
        /* Début code à  faire/completer par l'étudiant                     */
        /* Texture                                                         */
        /*******************************************************************/
        
        // à  faire : activer chaque texture dans une unité de texture différente
        GLuint unit = 0;
        for(TextureLayers::iterator it = mTextureLayers.begin() ;
            it!=mTextureLayers.end() ; ++unit, ++it)
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            (*it)->activate();
        }
        glActiveTexture(GL_TEXTURE0);


        /*******************************************************************/
        /* Fin code à  faire/completer par l'étudiant                       */
        /* Texture                                                         */
        /*******************************************************************/
    }

    if(mAlphaRejection)
    {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, mAlphaRejectionValue);
    }
}
//--------------------------------------------------------------------------------
void Material::release(bool alphaOnly /*= false*/)
{
    if(!alphaOnly)
    {
        if(mActiveBlending)
        {
            glDisable(GL_BLEND);
        }
        if(!mIsLighted && mPreviousLightingState)
        {
            glEnable(GL_LIGHTING);
        }
    }

    if(!alphaOnly || mAlphaRejection)
    {
        /*******************************************************************/
        /* Début code à  faire/completer par l'étudiant                     */
        /* Texture                                                         */
        /*******************************************************************/
        
        // à  faire : désactiver chaque texture de leur unité respective
        GLuint unit = GL_TEXTURE0;
        for(TextureLayers::iterator it = mTextureLayers.begin() ;
            it!=mTextureLayers.end() ; ++unit, ++it)
        {
            glActiveTexture(unit);
            (*it)->release();
        }
        glActiveTexture(GL_TEXTURE0);

        /*******************************************************************/
        /* Fin code à  faire/completer par l'étudiant                       */
        /* Texture                                                         */
        /*******************************************************************/
    }

    if(mAlphaRejection)
    {
        glDisable(GL_ALPHA_TEST);
    }
}
//--------------------------------------------------------------------------------
void Material::setBaseColor(const Color& c, float coeffAmbient)
{
    mAmbient = c * coeffAmbient * 2.0;

    mDiffuse = c * (1.-coeffAmbient);

    mColor = c;

    // l'alpha du sommet éclairé est égale à  l'alpha de la composante diffuse
    mDiffuse.a = c.a;
}

void Material::setSpecular(const Color& c, float shininess)
{
    mSpecular = c;
    // shininess entre 0 et 1
    // pour OpenGL : entre 0 et 128
    mShininess = shininess * 128;
}
//--------------------------------------------------------------------------------
void Material::setBlendFunc(GLenum src, GLenum dst)
{
    mBlendSrcFunc = src;
    mBlendDstFunc = dst;
    mActiveBlending = (src!=GL_ONE) || (dst!=GL_ZERO);
}
//--------------------------------------------------------------------------------
void Material::setAlphaRejection(float value)
{
    mAlphaRejectionValue = value;
    mAlphaRejection = (value>0.);
}

bool Material::hasAlphaRejection(void) const
{
    return mAlphaRejection;
}
//--------------------------------------------------------------------------------
void Material::setLighted(bool on)
{
    mIsLighted = on;
}
bool Material::isLighted(void) const
{
    return mIsLighted;
}
//--------------------------------------------------------------------------------
void Material::pushTextureLayer(TextureLayer* pTex)
{
    mTextureLayers.push_back(pTex);
}
//--------------------------------------------------------------------------------
int Material::getNumberOfLayers(void)
{
    return mTextureLayers.size();
}
//--------------------------------------------------------------------------------
void Material::popTextureLayer()
{
  mTextureLayers.pop_back();
}
