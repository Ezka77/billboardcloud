/***************************************************************************
 *   Copyright (C) 2007 by GangsTER   *
 *   coban31@hotamil.fr   *
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
#include "TgaLoader.h"
#include <iostream>
#include <stdio.h>

TgaLoader::TgaLoader(const QString& fileName)
{
  FILE *fp;
  colormap=NULL;

  fp = fopen (fileName.toAscii(), "rb");
  if (!fp)
    {
      std::cerr<<"error: couldn't open "<<fileName.toStdString ()<<" !"<<std::endl;
      return ;
    }

  /* lecture de l'en-tête du fichier */
  fread (&header, sizeof (tga_header_t), 1, fp);

  texinfo = (gl_texture_t *)malloc (sizeof (gl_texture_t));
  GetTextureInfo (&header, texinfo);
  fseek (fp, header.id_lenght, SEEK_CUR);

	/* read color map */
	if (header.colormap_type)
  {
    /* NOTE: color map is stored in BGR format */
    colormap = (GLubyte *)malloc (sizeof (GLubyte)
      * header.cm_length * (header.cm_size >> 3));
    fread (colormap, sizeof (GLubyte), header.cm_length
      * (header.cm_size >> 3), fp);
  }

	/* memory allocation */
	texinfo->texels = (GLubyte *)malloc (sizeof (GLubyte) *texinfo->width * texinfo->height * texinfo->internalFormat);
	if (!texinfo->texels)
  {
    free (texinfo);
    return ;
  }

  /* lecture des données pixels de l'image... */
  switch (header.image_type)
    {
    case 0:
      /* no data */
      break;

    case 1:
      /* uncompressed 8 bits color index */
      ReadTGA8bits (fp, colormap, texinfo);
      break;

    case 2:
      /* uncompressed 16-24-32 bits */
      switch (header.pixel_depth)
			{
			case 16:
				ReadTGA16bits (fp, texinfo);
				break;
		
			case 24:
				ReadTGA24bits (fp, texinfo);
				break;
		
			case 32:
				ReadTGA32bits (fp, texinfo);
				break;
			}

      break;

    case 3:
      /* uncompressed 8 or 16 bits grayscale */
      if (header.pixel_depth == 8)
				ReadTGAgray8bits (fp, texinfo);
      else /* 16 */
				ReadTGAgray16bits (fp, texinfo);

      break;

    case 9:
      /* RLE compressed 8 bits color index */
      ReadTGA8bitsRLE (fp, colormap, texinfo);
      break;

    case 10:
      /* RLE compressed 16-24-32 bits */
      switch (header.pixel_depth)
			{
			case 16:
				ReadTGA16bitsRLE (fp, texinfo);
				break;
		
			case 24:
				ReadTGA24bitsRLE (fp, texinfo);
				break;
		
			case 32:
				ReadTGA32bitsRLE (fp, texinfo);
				break;
			}

      break;

    case 11:
      /* RLE compressed 8 or 16 bits grayscale */
      if (header.pixel_depth == 8)
				ReadTGAgray8bitsRLE (fp, texinfo);
      else /* 16 */
				ReadTGAgray16bitsRLE (fp, texinfo);

      break;

    default:
      /* image type is not correct */
      std::cerr<<"error: unknown TGA image typ"<<header.image_type<<std::endl;
      free (texinfo->texels);
      free (texinfo);
      texinfo = NULL;
      break;
    }
  /* désallocations mémoire nécessaires... */
	if (colormap)
    free (colormap);

  fclose (fp);
}


TgaLoader::~TgaLoader()
{
}

void
TgaLoader::GetTextureInfo (tga_header_t *header, gl_texture_t *texinfo)
{
  texinfo->width = header->width;
  texinfo->height = header->height;

  switch (header->image_type)
    {
    case 3:  /* grayscale 8 bits */
    case 11: /* grayscale 8 bits (RLE) */
      {
	if (header->pixel_depth == 8)
	  {
	    texinfo->format = GL_LUMINANCE;
	    texinfo->internalFormat = 1;
	  }
	else /* 16 bits */
	  {
	    texinfo->format = GL_LUMINANCE_ALPHA;
	    texinfo->internalFormat = 2;
	  }

	break;
      }

    case 1:  /* 8 bits color index */
    case 2:  /* BGR 16-24-32 bits */
    case 9:  /* 8 bits color index (RLE) */
    case 10: /* BGR 16-24-32 bits (RLE) */
      {
	/* 8 bits and 16 bits images will be converted to 24 bits */
	if (header->pixel_depth <= 24)
	  {
	    texinfo->format = GL_RGB;
	    texinfo->internalFormat = 3;
	  }
	else /* 32 bits */
	  {
	    texinfo->format = GL_RGBA;
	    texinfo->internalFormat = 4;
	  }

	break;
      }
    }
}

void
TgaLoader::ReadTGA24bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;
	int taille = texinfo->width * texinfo->height;
  for (i = 0; i < taille; ++i)
    {
      /* read and convert BGR to RGB */
      texinfo->texels[(i * 3) + 2] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 3) + 1] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 3) + 0] = (GLubyte)fgetc (fp);
    }
}

void
TgaLoader::ReadTGA32bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;
	int taille = texinfo->width * texinfo->height;
  for (i = 0; i < taille; ++i)
    {
      /* read and convert BGRA to RGBA */
      texinfo->texels[(i * 4) + 2] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 4) + 1] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 4) + 0] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 4) + 3] = (GLubyte)fgetc (fp);
    }
}

void
TgaLoader::ReadTGA16bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;
  int taille = texinfo->width * texinfo->height;
  unsigned short color;

  for (i = 0; i < taille; ++i)
    {
      /* read color word */
      color = fgetc (fp) + (fgetc (fp) << 8);

      /* convert BGR to RGB */
      texinfo->texels[(i * 3) + 0] = (GLubyte)(((color & 0x7C00) >> 10) << 3);
      texinfo->texels[(i * 3) + 1] = (GLubyte)(((color & 0x03E0) >>  5) << 3);
      texinfo->texels[(i * 3) + 2] = (GLubyte)(((color & 0x001F) >>  0) << 3);
    }
}

void
TgaLoader::ReadTGA8bits (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo)
{
  int i;
  GLubyte color;
	int taille = texinfo->width * texinfo->height;
  for (i = 0; i < taille; ++i)
    {
      /* read index color byte */
      color = (GLubyte)fgetc (fp);

      /* convert to RGB 24 bits */
      texinfo->texels[(i * 3) + 2] = colormap[(color * 3) + 0];
      texinfo->texels[(i * 3) + 1] = colormap[(color * 3) + 1];
      texinfo->texels[(i * 3) + 0] = colormap[(color * 3) + 2];
    }
}

void
TgaLoader::ReadTGAgray8bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;
	int taille = texinfo->width * texinfo->height;
  for (i = 0; i < taille; ++i)
    {
      /* read grayscale color byte */
      texinfo->texels[i] = (GLubyte)fgetc (fp);
    }
}

void
TgaLoader::ReadTGA24bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte rgb[3];
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 3)
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
			{
				/* run-length packet */
				fread (rgb, sizeof (GLubyte), 3, fp);
		
				for (i = 0; i < size; ++i, ptr += 3)
					{
						ptr[0] = rgb[2];
						ptr[1] = rgb[1];
						ptr[2] = rgb[0];
					}
			}
      else
			{
				/* non run-length packet */
				for (i = 0; i < size; ++i, ptr += 3)
					{
						ptr[2] = (GLubyte)fgetc (fp);
						ptr[1] = (GLubyte)fgetc (fp);
						ptr[0] = (GLubyte)fgetc (fp);
					}
			}
    }
}

void
TgaLoader::ReadTGAgray16bits (FILE *fp, gl_texture_t *texinfo)
{
  int i;
	int taille = texinfo->width * texinfo->height;
  for (i = 0; i < taille; ++i)
    {
      /* read grayscale color + alpha channel bytes */
      texinfo->texels[(i * 2) + 0] = (GLubyte)fgetc (fp);
      texinfo->texels[(i * 2) + 1] = (GLubyte)fgetc (fp);
    }
}

void
TgaLoader::ReadTGA8bitsRLE (FILE *fp, GLubyte *colormap, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte color;
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 3)
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
			{
				/* run-length packet */
				color = (GLubyte)fgetc (fp);
		
				for (i = 0; i < size; ++i, ptr += 3)
					{
						ptr[0] = colormap[(color * 3) + 2];
						ptr[1] = colormap[(color * 3) + 1];
						ptr[2] = colormap[(color * 3) + 0];
					}
			}
      else
			{
				/* non run-length packet */
				for (i = 0; i < size; ++i, ptr += 3)
					{
						color = (GLubyte)fgetc (fp);
		
						ptr[0] = colormap[(color * 3) + 2];
						ptr[1] = colormap[(color * 3) + 1];
						ptr[2] = colormap[(color * 3) + 0];
					}
			}
    }
}

void
TgaLoader::ReadTGA16bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  unsigned short color;
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 3)
    {
      /* read first byte */
      packet_header = fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
			{
				/* run-length packet */
				color = fgetc (fp) + (fgetc (fp) << 8);
		
				for (i = 0; i < size; ++i, ptr += 3)
					{
						ptr[0] = (GLubyte)(((color & 0x7C00) >> 10) << 3);
						ptr[1] = (GLubyte)(((color & 0x03E0) >>  5) << 3);
						ptr[2] = (GLubyte)(((color & 0x001F) >>  0) << 3);
					}
			}
      else
			{
				/* non run-length packet */
				for (i = 0; i < size; ++i, ptr += 3)
					{
						color = fgetc (fp) + (fgetc (fp) << 8);
		
						ptr[0] = (GLubyte)(((color & 0x7C00) >> 10) << 3);
						ptr[1] = (GLubyte)(((color & 0x03E0) >>  5) << 3);
						ptr[2] = (GLubyte)(((color & 0x001F) >>  0) << 3);
					}
			}
    }
}

void
TgaLoader::ReadTGA32bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte rgba[4];
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 4)
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
			{
				/* run-length packet */
				fread (rgba, sizeof (GLubyte), 4, fp);
		
				for (i = 0; i < size; ++i, ptr += 4)
					{
						ptr[0] = rgba[2];
						ptr[1] = rgba[1];
						ptr[2] = rgba[0];
						ptr[3] = rgba[3];
					}
			}
      else
			{
				/* non run-length packet */
				for (i = 0; i < size; ++i, ptr += 4)
					{
						ptr[2] = (GLubyte)fgetc (fp);
						ptr[1] = (GLubyte)fgetc (fp);
						ptr[0] = (GLubyte)fgetc (fp);
						ptr[3] = (GLubyte)fgetc (fp);
					}
			}
    }
}


void
TgaLoader::ReadTGAgray8bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte color;
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height))
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
			{
				/* run-length packet */
				color = (GLubyte)fgetc (fp);
		
				for (i = 0; i < size; ++i, ptr++)
					*ptr = color;
			}
      else
			{
				/* non run-length packet */
				for (i = 0; i < size; ++i, ptr++)
					*ptr = (GLubyte)fgetc (fp);
			}
    }
}


void
TgaLoader::ReadTGAgray16bitsRLE (FILE *fp, gl_texture_t *texinfo)
{
  int i, size;
  GLubyte color, alpha;
  GLubyte packet_header;
  GLubyte *ptr = texinfo->texels;

  while (ptr < texinfo->texels + (texinfo->width * texinfo->height) * 2)
    {
      /* read first byte */
      packet_header = (GLubyte)fgetc (fp);
      size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
			{
				/* run-length packet */
				color = (GLubyte)fgetc (fp);
				alpha = (GLubyte)fgetc (fp);
		
				for (i = 0; i < size; ++i, ptr += 2)
					{
						ptr[0] = color;
						ptr[1] = alpha;
					}
			}
      else
			{
				/* non run-length packet */
				for (i = 0; i < size; ++i, ptr += 2)
					{
						ptr[0] = (GLubyte)fgetc (fp);
						ptr[1] = (GLubyte)fgetc (fp);
					}
			}
    }
}

GLuint TgaLoader::getWidth()
{
	return texinfo->width;
}

GLuint TgaLoader::getHeight()
{
	return texinfo->height;
}

GLint TgaLoader::getInternalFormat()
{
	return texinfo->internalFormat;
}

GLenum TgaLoader::getFormat()
{
	return texinfo->format;
}

GLubyte *TgaLoader::getTexels()
{
	return texinfo->texels;
}
