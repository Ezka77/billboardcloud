
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
        \param semantic s�mantique de la texture  (DiffuseMap, AlphaMap, EnvironementMap, ...)
        \note � faire par l'�tudiant
    */
    TextureLayer(const QString& fileName, const QString& semantic = "DiffuseMap");

    /** retourne la s�mentique de la texture
    */
    const QString& getSemantic(void) const;

    /** Active les �tats OpenGL de la texture.
        \note � faire par l'�tudiant
    */
    void activate(void);

    /** D�sactive les �tats OpenGL de la texture.
        \note � faire par l'�tudiant
    */
    void release(void);

    /** Sp�cifie le mode de filtrage pour la r�duction et l'agrandissement (GL_LINEAR, GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR, ...).
    */
    void setFilteringMode(GLenum minMode, GLenum magMode);

    /** Sp�cifie le mode de m�lange entre la texture et la couleur du fragment (GL_MODULATE, GL_REPLACE, GL_ADD, ...)
    */
    void setBlendMode(GLenum mode);

    /** Sp�cifie le mode de r�p�tition de la texture (GL_REPEAT ou GL_CLAMP).
    */
    void setWrapMode(GLenum mode);

    /** Sp�cifie les transformations des coordonn�es de textures.
    */
    void setTransform(const Transform& t);
    const Transform& getTransform(void);

protected:

    QString mSemantic;
    
    GLenum mBlendMode;
    GLenum mWrapMode;
    GLenum mFilteringModes[2];

    /// num�ro OpenGL de la texture.
    GLuint mTexId;

    Transform mTransform;
		TgaLoader *tga;
};

#endif
