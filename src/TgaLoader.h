/***************************************************************************
 *   Copyright (C) 2007 by GangsTER   *
 *   coban31@hotmail.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef _TgaLoader_h_
#define _TgaLoader_h_
#include <GL/glew.h>

#include <QString>
/**
	@author GangsTER <coban31@hotmail.fr>
*/
class TgaLoader
{
public:
	TgaLoader(const QString& fileName);

	~TgaLoader();
	GLuint getWidth();
	GLuint getHeight();
	GLint getInternalFormat();
	GLenum getFormat();
	GLubyte *getTexels();

protected:
	/* OpenGL texture info */
	typedef struct
    {
    	GLuint width;           /* largeur */
			GLuint height;          /* hauteur */
		
			GLenum format;          /* RVB, RVBA, Luminance, Luminance Alpha */
			GLint internalFormat;  /* composantes d'un texel */

			GLubyte *texels;        /* données de l'image */
		}gl_texture_t;
	
	/* tga header */
	#pragma pack(push, 1)
	typedef struct 
	{
		GLubyte id_lenght;          /* size of image id */
		GLubyte colormap_type;      /* 1 is has a colormap */
		GLubyte image_type;         /* compression type */
	
		short	cm_first_entry;       /* colormap origin */
		short	cm_length;            /* colormap length */
		GLubyte cm_size;            /* colormap size */
	
		short	x_origin;             /* bottom left x coord origin */
		short	y_origin;             /* bottom left y coord origin */
	
		short	width;                /* picture width (in pixels) */
		short	height;               /* picture height (in pixels) */
	
		GLubyte pixel_depth;        /* bits per pixel: 8, 16, 24 or 32 */
		GLubyte image_descriptor;   /* 24 bits = 0x00; 32 bits = 0x80 */
	}tga_header_t;
	#pragma pack(pop)

	void GetTextureInfo (tga_header_t *header, gl_texture_t *texinfo);
	void ReadTGA24bits (FILE *fp, gl_texture_t *texinfo);
	void ReadTGA32bits (FILE *fp, gl_texture_t *texinfo);
	void ReadTGA16bits (FILE *fp, gl_texture_t *texinfo);
	void ReadTGA8bits (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo);
	void ReadTGAgray8bits (FILE *fp, gl_texture_t *texinfo);
	void ReadTGA24bitsRLE (FILE *fp, gl_texture_t *texinfo);
	void ReadTGAgray16bits (FILE *fp, gl_texture_t *texinfo);
	void ReadTGA8bitsRLE (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo);
	void ReadTGA16bitsRLE (FILE *fp, gl_texture_t *texinfo);
	void ReadTGA32bitsRLE (FILE *fp, gl_texture_t *texinfo);
	void ReadTGAgray8bitsRLE (FILE *fp, gl_texture_t *texinfo);
	void ReadTGAgray16bitsRLE (FILE *fp, gl_texture_t *texinfo);

private:
	gl_texture_t *texinfo;
  tga_header_t header;
  GLubyte *colormap;
};

#endif
