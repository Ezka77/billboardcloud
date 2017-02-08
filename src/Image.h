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
#ifndef __ImageTexture__
#define __ImageTexture__

/** \addtogroup Image

	\bug	A voir
*/
/*@{*/

/** Type opaque Image.
*/
typedef struct _image *Image;

/** Cr?ation d'une image.
	L'image cr??e sera charg?e a partir du fichier dont 
	le nom est pass? en param?tre. Les formats support?s sont : 
	- JPEG (attention, compr?ssion avec perte).
	- PNG (conseill?, le plus g?n?ral).
	\param path chemin d'acc?s ? l'image.
	\return l'image charg?e, NULL si erreur.
*/
Image chargeImage(const char *path);

/** Cr?ation d'une image.
	Cr?ation de la structure Image a partir d'un tableau de width*height pixels,
	chaque pixel etant cod? par depth unsigned char.
	Le tableau pixels contient l'image rang?e par ligne . Le pixel (i,j) 
	comence donc ? l'adresse pixels + (i*width*depth) + j*depth .
	
	\attention Si la taille de pixels n'est pas width*height*depth, il y aura 
	segmentation fault, voire pire!!!
	
	\param pixels tableau des pixels.
	\param width largeur de l'image.
	\param height hauteur de l'image.
	\param depth profondeur de l'image.
	
*/
Image creeImage(unsigned char *pixels, int width, int height, int depth);

/** Sauvegarde de l'image au format png.
	\param image image ? sauver.
	\param path nom du fichier destination.
*/
void imageSauvePng(Image image, const char* path);

/** Lib?ration de la structure image.
*/
void detruireImage(Image *imageADetruire);

/** Acces au tableau de pixels.
*/
unsigned char *imageGetPixels(Image image);

/** Acces ? la largeur de l'image.
*/
int imageGetWidth(Image image);

/** Acces ? la hauteur de l'image.
*/
int imageGetHeight(Image image);

/** Acces ? la profondeur de l'image.
*/
int imageGetDepth(Image image);

/** @} end */


#endif
