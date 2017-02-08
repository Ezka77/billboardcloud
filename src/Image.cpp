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
/*--------------------------------------------------------------------
 --             Introduction a OpenGL
 --             Master 1 Informatique
 --             UPS                                     
 --                                                                   
 --             Mathias Paulin 2006                             
 --                                                                   
 -------------------------------------------------------------------*/

#include "Image.h"

#include <GL/glu.h>

#include <malloc.h>
#include <stdlib.h>

#include <png.h>


#include <setjmp.h>
#include <string.h> 

/*------------------------------------------------------------------*/
/*					Definition du type complet						*/


typedef struct _image {
	int width;
	int height;
	int depth;
	unsigned char *pixels;
} internalImageRepresentation;

/*------------------------------------------------------------------*/
/*					fonctions de chargement d'une image				*/

/*------------------------------------------------------------------*/


/*------------------------------------------------------------------*/
/*								PNG support							*/
static double get_gamma_exp(void)
{
	static double default_exponent = 2.2;
	static int set = 0;
	if (! set)
    {
		#if defined(NeXT)
		default_exponent = 1.0; 
		#elif defined(sgi)
		FILE *infile;
		default_exponent = 1.3;   
		if (infile = fopen("/etc/config/system.glGammaVal", "r")) 
		{
			double sgi_gamma;
			char fooline[80];
			fgets(fooline, sizeof(fooline), infile);
			fclose(infile);
			sgi_gamma = atof(fooline);
			if (sgi_gamma > 0.0)
				default_exponent = 2.2 / sgi_gamma;
		}
		#elif defined(Macintosh)
		default_exponent = 1.5;
		#endif
		set = 1;
    }
	return default_exponent;
}

int loadPng(internalImageRepresentation *imageDest, const char *localFileName)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	int bytes_per_row, row;
	int gray_palette;
	unsigned char *pixels = 0, **rows = 0;
	unsigned int i;
	
	FILE *fp=fopen(localFileName,"rb");
	if (fp==NULL) return 0;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
 
	if (png_ptr == NULL) return 0;
 
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        return 0;
    }

	if (setjmp(png_ptr->jmpbuf))
    {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

		if (pixels) free(pixels);
		if (rows) free(rows);

		return 0;
    }
 
    png_init_io(png_ptr, fp);

    png_read_info(png_ptr, info_ptr);
 
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	             &interlace_type, NULL, NULL);
 
    imageDest->depth = png_get_channels(png_ptr, info_ptr);

    png_set_strip_16(png_ptr);

    png_set_packing(png_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
      png_set_expand(png_ptr);
      imageDest->depth = 3;
    }
	
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand(png_ptr);
 
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    {
		png_set_expand(png_ptr);
		++(imageDest->depth);
    }

	gray_palette = 0;
	if (color_type == PNG_COLOR_TYPE_PALETTE)
    {
		int n, num_palette;
		png_colorp palette;
		if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette))
		{
			gray_palette = 1;
			for (n=0; n<num_palette; ++n)
				if (palette[n].red != palette[n].green || palette[n].blue != palette[n].green)
				{
					gray_palette = 0;
					break;
				}
		}
    }

	{
		double file_gamma, default_exponent = get_gamma_exp();
		if (png_get_gAMA(png_ptr, info_ptr, &file_gamma))
			png_set_gamma(png_ptr, default_exponent, file_gamma);
		else
			png_set_gamma(png_ptr, default_exponent, 0.45455);
	}

	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
				 &interlace_type, NULL, NULL);
 
	bytes_per_row = imageDest->depth * width;

	pixels = (unsigned char*) malloc(bytes_per_row * height);
	rows = (unsigned char**) malloc(height * sizeof(char *));
	if (pixels == 0 || rows == 0)
    {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return 0;
    }

	for (row=0; row<(int)height; ++row)
		rows[row] = &pixels[row * bytes_per_row];

	png_read_image(png_ptr, rows);
 
	png_read_end(png_ptr, info_ptr);
 
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	free(rows);

	if (gray_palette)
    {
		int n, np = width * height;
		if (imageDest->depth == 3)
		{
			for (n=1; n<np; ++n)
				pixels[n] = pixels[3*n];
			imageDest->depth = 1;
		}
		else if (imageDest->depth == 4)
		{
			for (n=0; n<np; ++n) 
			{
				pixels[2*n] = pixels[4*n];
				pixels[2*n+1] = pixels[4*n+3];
			}
			imageDest->depth = 2;
		}
    }

	imageDest->width = width;
	imageDest->height = height;
 	imageDest->pixels = (unsigned char *)malloc(row * bytes_per_row);
//
	for (i=0;i<imageDest->height ;i++)
		#ifdef WIN32
			memcpy(&(imageDest->pixels[i*bytes_per_row]),&(pixels[bytes_per_row *(imageDest->height-i-1)]),bytes_per_row);
		#else
			bcopy(&(pixels[bytes_per_row *(imageDest->height-i-1)]),&(imageDest->pixels[i*bytes_per_row]),bytes_per_row);
		#endif
// #else
// 	for (i=0;i<imageDest->height ;i++)
// 		#ifdef WIN32
// 			memcpy(&(imageDest->pixels[i*bytes_per_row]),&(pixels[i*bytes_per_row]),bytes_per_row);
// 		#else
// 			bcopy(&(pixels[i*bytes_per_row]),&(imageDest->pixels[i*bytes_per_row]),bytes_per_row);
// 		#endif
// 
// #endif	
	free(pixels);	

	fclose(fp);
	return 1;
}

int write_png(const char *file_name, unsigned char *image, int width, int height, int depth)
{
   FILE *fp;
   png_structp png_ptr;
   png_infop info_ptr;
   int k;
   png_bytep *row_pointers;
   /* open the file */
   fp = fopen(file_name, "wb");
   if (fp == NULL)
      return 0;
   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (!png_ptr)
   {
      fclose(fp);
      return 0;
   }
   /* Allocate/initialize the image information data.  REQUIRED */
   info_ptr = png_create_info_struct(png_ptr);
   if (!info_ptr)
   {
      fclose(fp);
      png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
      return 0;
   }
   /* Set error handling.  REQUIRED if you aren't supplying your own
    * error hadnling functions in the png_create_write_struct() call.
    */
   if (setjmp(png_ptr->jmpbuf))
   {
      /* If we get here, we had a problem writing the file */
      fclose(fp);
      png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
      return 0;
   }
   /* set up the output control if you are using standard C streams */
   png_init_io(png_ptr, fp);
   /* Set the image information here.  Width and height are up to 2^31,
    * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
    * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
    * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
    * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
    * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
    * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
    */
   	switch (depth) {
		case 1:
		png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_GRAY,\
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		break;
		case 2:
		png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_GRAY_ALPHA,\
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		break;
		case 3:
		png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,\
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		break;
		case 4:
		png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB_ALPHA,\
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
		break;
		default:
			printf("Profondeur de couleur non support? pour la sauvegarde en png.\n");
	}	
		
   /* Write the file header information.  REQUIRED */
   png_write_info(png_ptr, info_ptr);
   /* The easiest way to write the image (you may have a different memory
    * layout, however, so choose what fits your needs best).  You need to
    * use the first method if you aren't handling interlacing yourself.
    */
   row_pointers= (png_bytep *)malloc(sizeof(png_bytep) *height);
   for (k = 0; k < height; k++)
     row_pointers[k] = image + k*width*depth;
   /* write out the entire image data in one call */
   png_write_image(png_ptr, row_pointers);
   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(png_ptr, info_ptr);
   /* clean up after the write, and free any memory allocated */
   png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
   /* close the file */
   fclose(fp);
   /* that's it */
   free (row_pointers);
   return (1);
}


/*------------------------------------------------------------------*/
/*					fonction exportee par le module					*/
#define MAX_STRING 2048
Image chargeImage(const char *path)
{
	#ifdef WIN32
	char drive[MAX_STRING];
	char dir[MAX_STRING];
	char fname[MAX_STRING];
	char ext[MAX_STRING];
	#else 
	char ext[MAX_STRING];
	#endif
	
	Image newImage=(Image)malloc(sizeof(internalImageRepresentation));

	#ifdef WIN32
	_splitpath( path, drive, dir, fname, ext );
	#else 
	strcpy(ext,strrchr(path,'.'));
	#endif
	
	if ( (strcmp(ext,".png")==0) || (strcmp(ext,".PNG")==0) ) {
		if (loadPng( newImage, path ) ) {
			return (newImage);
		} else {
			return (NULL);
		}
	}
	free (newImage);	
	return (NULL);		
}
#undef MAX_STRING 

Image creeImage(unsigned char *pixels, int width, int height, int depth)
{
	Image newImage=(Image)malloc(sizeof(internalImageRepresentation));
	newImage->pixels=pixels;
	newImage->width=width;
	newImage->height=height;
	newImage->depth=depth;
	return (newImage);
}

void detruireImage(Image *imageADetruire)
{
	free ((*imageADetruire)->pixels);
	free (*imageADetruire);	
	*imageADetruire=NULL;	
}

void imageSauvePng(Image image, const char* path)
{

		unsigned int i;
		unsigned char *zap_image = (unsigned char*)malloc(image->width*image->height*image->depth*sizeof(unsigned char));

		int bytes_per_row = image->width*image->depth;
		for (i=0;i<image->height ;i++)
			#ifdef WIN32
				memcpy(&(zap_image[i*bytes_per_row]),&(image->pixels[bytes_per_row*(image->height-i-1)]),bytes_per_row);
			#else
				bcopy(&(image->pixels[bytes_per_row*(image->height-i-1)]),&(zap_image[i*bytes_per_row]),bytes_per_row);
			#endif

		if ( !write_png(path, zap_image, image->width,image->height,image->depth) )
			printf ("Probleme d'ecriture ... \n");

		free (zap_image);			
}

unsigned char *imageGetPixels(Image image)
{
	return (image->pixels);	
}

int imageGetWidth(Image image)
{
	return (image->width);
}

int imageGetHeight(Image image)
{
	return (image->height);
}

int imageGetDepth(Image image)
{
	return (image->depth);
}





     