
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _TextureLayer_h_
#define _TextureLayer_h_

#include <GL/glew.h>
#include "OpenGL.h"
#include <QString>

#include "Vector3.h"
#include "Transform.h"

#include "TgaLoader.h"
class TextureLayer
{
public:

    /** Constructeur d'une texture
        \param fileName nom du fichier contenant l'image
        \param semantic sémantique de la texture  (DiffuseMap, AlphaMap, EnvironementMap, ...)
        \note à  faire par l'étudiant
    */
    TextureLayer(const QString& fileName, const QString& semantic = "DiffuseMap");

    /** retourne la sémentique de la texture
    */
    const QString& getSemantic(void) const;

    /** Active les états OpenGL de la texture.
        \note à  faire par l'étudiant
    */
    void activate(void);

    /** Désactive les états OpenGL de la texture.
        \note à  faire par l'étudiant
    */
    void release(void);

    /** Spécifie le mode de filtrage pour la réduction et l'agrandissement (GL_LINEAR, GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR, ...).
    */
    void setFilteringMode(GLenum minMode, GLenum magMode);

    /** Spécifie le mode de mélange entre la texture et la couleur du fragment (GL_MODULATE, GL_REPLACE, GL_ADD, ...)
    */
    void setBlendMode(GLenum mode);

    /** Spécifie le mode de répétition de la texture (GL_REPEAT ou GL_CLAMP).
    */
    void setWrapMode(GLenum mode);

    /** Spécifie les transformations des coordonnées de textures.
    */
    void setTransform(const Transform& t);
    const Transform& getTransform(void);

protected:

    QString mSemantic;
    
    GLenum mBlendMode;
    GLenum mWrapMode;
    GLenum mFilteringModes[2];

    /// numéro OpenGL de la texture.
    GLuint mTexId;

    Transform mTransform;
		TgaLoader *tga;
};

#endif
