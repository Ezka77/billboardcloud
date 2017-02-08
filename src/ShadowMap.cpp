
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

//#include "OpenGL.h"

#include <qstring.h>

#define INT32 _INT32
#include "OffScreenRendering.h"
#undef INT32

#include "ShadowMap.h"
#include "Light.h"


/*******************************************************************/
/* Début code à faire/completer par l'étudiant                     */
/* shadow map                                                      */
/*******************************************************************/

Matrix4 ShadowMap::scaleBiasMatrix(
    0.5, 0,   0,   0.5,
    0,   0.5, 0,   0.5,
    0,   0,   0.5, 0.5,
    0,   0,   0,   1.0
);

float ShadowMap::configEpsilon = 4.;

ShadowMap::ShadowMap(int width, int height, int stage)
    : mpLight(0)
{
    mTextureUnit = GL_TEXTURE0 + stage;
    mPBuffer = new OffScreenRendering(width,height,true);

    // à faire : créer la texture de profondeur.
    glGenTextures(1, &mTexId);
    glBindTexture(GL_TEXTURE_2D, mTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, Color::WHITE);
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24_ARB,width,height,0, GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,0); GL_TEST_ERR;
}

ShadowMap::~ShadowMap()
{
    // à faire : détruire la texture OpenGL.
    glDeleteTextures(1, &mTexId);
    delete mPBuffer;
}

void ShadowMap::setLight(Light* pLight)
{
    if(pLight->getType()==Light::LT_SPOT)
    {
        mpLight = pLight;
    }
}

void ShadowMap::beginUpdate(void)
{
    // à faire : activer le PBuffer
    // à faire : positionner la caméra OpenGL au niveau du spot
    // à faire : décalage des polygones

    if(!mpLight)
    {
        std::cerr << "No valid light was setted\n";
        exit(1);
    }

    mPBuffer->begin();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    Matrix4 lightview, lightproj;

    // render the scene from light source
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective( mpLight->getAperture()*2., 1.0, 3., 10e6);
    glGetFloatv(GL_PROJECTION_MATRIX, (lightproj[0]));
    lightproj = lightproj.transpose();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
   // Vector3 camPos = mpLight->getPosition();
  //  Vector3 lightDir = mpLight->getDirection();
   // Vector3 camTarget = camPos + lightDir;
   // Vector3 camUp = lightDir.perpendicular();
    //gluLookAt(camPos.x, camPos.y, camPos.z, camTarget.x, camTarget.y, camTarget.z, camUp.x, camUp.y, camUp.z);
    glGetFloatv(GL_MODELVIEW_MATRIX, (lightview[0]));
    lightview = lightview.transpose();

    mSceneToLightMatrix = scaleBiasMatrix * lightproj * lightview;

    glPolygonOffset(configEpsilon, 1.1f);
    glEnable(GL_POLYGON_OFFSET_FILL);
}

void ShadowMap::endUpdate(void)
{
    // à faire : restaurer les états OpenGL
    // à faire : copier le depth buffer dans la texture de profondeur
    // à faire : désactiver le PBuffer

    glDisable(GL_POLYGON_OFFSET_FILL);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D,mTexId);
    mPBuffer->copyToTexture(&mTexId);

    mPBuffer->end();
}

void ShadowMap::activate(void)
{
    // à faire : activer la matrice de passage de la scène à la shadow map via la génération automatique des coordonnées de texture ou la matrice de transformation des coordonnées de texture

    // à faire : activer la texture de profondeur

    glActiveTexture(mTextureUnit);
    glBindTexture(GL_TEXTURE_2D,mTexId);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_S, GL_EYE_PLANE, mSceneToLightMatrix[0]);
    glEnable(GL_TEXTURE_GEN_S);

    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_T, GL_EYE_PLANE, mSceneToLightMatrix[1]);
    glEnable(GL_TEXTURE_GEN_T);

    glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_R, GL_EYE_PLANE, mSceneToLightMatrix[2]);
    glEnable(GL_TEXTURE_GEN_R);

    glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glTexGenfv(GL_Q, GL_EYE_PLANE, mSceneToLightMatrix[3]);
    glEnable(GL_TEXTURE_GEN_Q);

    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);
}


void ShadowMap::release(void)
{
    // à faire : restaurer les états OpenGL

    glActiveTexture(mTextureUnit);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glDisable(GL_TEXTURE_GEN_R);
    glDisable(GL_TEXTURE_GEN_Q);
    glActiveTexture(GL_TEXTURE0);
}

/*******************************************************************/
/* Fin code à faire/completer par l'étudiant                       */
/* shadow map                                                      */
/*******************************************************************/

