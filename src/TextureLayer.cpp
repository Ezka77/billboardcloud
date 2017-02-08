
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/
 
#include "TextureLayer.h"
//#include <qimage.h>
#include <QImage>
#include <iostream>

/*******************************************************************/
/* Début code à  faire/completer par l'étudiant                     */
/* Texture                                                         */
/*******************************************************************/
TextureLayer::TextureLayer(const QString& fileName, const QString& semantic)
    : mSemantic(semantic), mBlendMode(GL_MODULATE), mWrapMode(GL_REPEAT)
{
    QImage img = QImage(fileName).rgbSwapped(); // swapRGB : pour être conforme avec OpenGL

    if(!img.isNull())
    {
        img = img.mirrored(); // pour être conforme avec OpenGL
        GLenum fmt = GL_RGB;

        switch(img.depth())
        {
            case 8:
                fmt = GL_LUMINANCE;
                break;
            case 16:
                fmt = GL_LUMINANCE_ALPHA;
                break;
            case 24:
                fmt = GL_RGB;
                break;
            case 32:
                fmt = GL_RGBA;
                break;
            default:
                std::cerr << "Error in TextureLayer, bpp = " << img.depth() << " !" << std::endl;
                exit(1);
        }


        glGenTextures(1, &mTexId); GL_TEST_ERR

        glBindTexture(GL_TEXTURE_2D, mTexId);

				gluBuild2DMipmaps(GL_TEXTURE_2D, fmt, img.width(), img.height(), fmt, GL_UNSIGNED_BYTE, img.bits()); GL_TEST_ERR;

				glBindTexture(GL_TEXTURE_2D,0);

        mFilteringModes[1] = GL_LINEAR;
        mFilteringModes[0] = GL_LINEAR_MIPMAP_LINEAR;
    }
		else if (fileName.contains(".tga", Qt::CaseInsensitive))
		{
			tga=new TgaLoader(fileName);
			glGenTextures(1, &mTexId);
			glBindTexture(GL_TEXTURE_2D, mTexId);
			gluBuild2DMipmaps (	GL_TEXTURE_2D, tga->getInternalFormat(),
			 										tga->getWidth(), tga->getHeight(),
			 										tga->getFormat(), GL_UNSIGNED_BYTE, tga->getTexels());
			glBindTexture(GL_TEXTURE_2D,0);

			mFilteringModes[1] = GL_LINEAR;
			mFilteringModes[0] = GL_LINEAR_MIPMAP_LINEAR;										
		}
		else 
			std::cerr<<"Invalid format image"<<std::endl;
}

void TextureLayer::activate(void)
{
		glMatrixMode (GL_TEXTURE);
		glLoadIdentity();
		glMatrixMode (GL_MODELVIEW); 
	
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,mTexId);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
}

void TextureLayer::release(void)
{
    glBindTexture(GL_TEXTURE_2D,0);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_COLOR_MATERIAL); 
}

/*******************************************************************/
/* Fin code à  faire/completer par l'étudiant                       */
/* Texture                                                         */
/*******************************************************************/

const QString& TextureLayer::getSemantic(void) const
{
    return mSemantic;
}

void TextureLayer::setTransform(const Transform& t)
{
    mTransform = t;
}
const Transform& TextureLayer::getTransform(void)
{
    return mTransform;
}

void TextureLayer::setFilteringMode(GLenum minMode, GLenum magMode)
{
    mFilteringModes[0] = minMode;
    mFilteringModes[1] = magMode;
}
void TextureLayer::setBlendMode(GLenum mode)
{
    mBlendMode = mode;
}
void TextureLayer::setWrapMode(GLenum mode)
{
    mWrapMode = mode;
}




