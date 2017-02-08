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
#include "Density.h"
#include "math.h"
#include <vector>
#include "Vector3.h"
#include "Matrix4.h"

#include "textfile.h"

#define Pi 3.1416

Density::Density(const int nbRow, bool UseTex3D)
  :DRow(nbRow),DensityTex3D(UseTex3D),minAlpha(0.0)
{
  _textureID[0]=0;
  if ( nbRow%2!=0 && (nbRow/2)%2 != 0 || nbRow<=0) {
     std::cout << "Taille de matrice de densitée incorrecte ! La taille doit être une puissance de 2." << std::endl;
     Data=NULL;
     DRow=0;
  }
  else {
  int nbCell = nbRow*nbRow*nbRow ;
  Data = new float [nbCell]; // Allocation du tableau
  for (int i=0; i<nbCell; i++)
    Data[i]=0.0f; // Mise a 0 de toutes les valeurs : Noir Transparent
  }
}

Density::~Density()
{
	this->removeShaders();
  delete [] Data;
  Data = NULL;
  _textureID[0] = 0;
}

int Density::getRow()
{
  return DRow;
}


float Density::getElt(int coord)
{
	return Data[coord];
}
float Density::getElt(int x, int y, int z)
{
  int coord = (x+(DRow*y)+(z*DRow*DRow));
  return Data[coord] ;
}



void Density::setElt(int x, int y, int z, float c)
{
  int coord = (x+(DRow*y)+(z*DRow*DRow));
  Data[coord]=c;
}


void Density::setElt(int coord, float c)
{
  Data[coord]=c;
}


void Density::fill(int x, int y, int z, float alphaAdd) // x y z
{
	float c = this->getElt(x,y,z);
  c += alphaAdd ;
  this->setElt(x,y,z,c);
}


void Density::reset()
{
  Density::reset(0.0f);
}


void Density::reset(const float defVal)
{
  int nbCell = DRow*DRow*DRow ;
  for (int i=0; i<nbCell; i++)
    Data[i]=defVal;
}


void Density::setMinAlpha(float minf)
{
  minAlpha=minf;
}

/**
 *  InitTest : remplis la matrice avec des couleurs pour faire les tests.
 * --> actuellement trace une sphère creuse dans la matrice
 * ajoute une division du volume en suivant une courbe d'équation cubique.
 */
void Density::InitTest()
{
  // dessine un cercle
  int rayon = (int)(DRow*(DRow/sqrt(DRow))) ;
  int centre = DRow/2 ;
  int tmp ;
  for(int i=0; i<DRow ; i++)
    for(int j=0; j<DRow ; j++)
      for(int k=0; k<DRow ; k++)
      {
				setElt(i,j,k,0.1);
			  tmp = (int)(i-centre)*(i-centre)*(i-centre)/(int)(DRow*DRow*.2);
				if ((j-centre) <= tmp)
					setElt(i,j,k,.4);
				
				//pour la sphère
				tmp = ((i-centre)*(i-centre) + (j-centre)*(j-centre) + (k-centre)*(k-centre)) ;
				if (tmp <= rayon && tmp >= rayon-centre) 
					setElt(i,j,k,0.95);
      }
}


void Density::initDraw()
{
	glGenTextures(1, _textureID); // On génére la texture
	
		// On initialiser les shaders pour la coloration
	setShaders();
	//InitTest();
	
  // Si en texture 2D
  if (!DensityTex3D)
	{
	 glBindTexture(GL_TEXTURE_2D, _textureID[0]);
    /*glTexImage2D ( GL_TEXTURE_2D, //GLenum  target,
    0, //GLint level,
    GL_RGBA, // GLint internalFormat,
    DRow, // GLsizei width,
    DRow*DRow, //  GLsizei height,
    0, // GLint border,
    GL_RGBA, // GLenum format,
    GL_FLOAT, // GLenum type,
    Data);  // const GLvoid * data
    */
    // On préfèrera les mipmaps pour pouvoir
    // utiliser des textures de grande taille.
    gluBuild2DMipmaps(GL_TEXTURE_2D,
											GL_ALPHA,
		      DRow,
		      DRow*DRow,
					GL_ALPHA,
		      GL_FLOAT,
		      Data);

    glBindTexture(GL_TEXTURE_2D,0);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);// Linear Filtering GL_NEAREST_MIPMAP_LINEAR.
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_NEAREST);// Linear Filtering GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  } else { // Si en texture 3D
    glBindTexture(GL_TEXTURE_3D, _textureID[0]);
    glTexImage3D(GL_TEXTURE_3D, // enum target
		       0, // int level
					 GL_ALPHA, // enum internalformat
		       DRow,
		       DRow,
		       DRow,
		       0, // int border,
		       GL_ALPHA, // enum format
		       GL_FLOAT, // enum type,
		       Data); // const void* pixels

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // GL_NEAREST GL_LINEAR
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST GL_LINEAR
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  }
}


void Density::drawDensity (Camera *camera)
{
  if (_textureID == 0)
    std::cout << "drawDensity() : Draw's not initialized. Please run initDraw() before." << std::endl;
  else
  {
    glDisable (GL_LIGHTING);
    //glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,GL_SEPARATE_SPECULAR_COLOR);
    /* Alpha test : affiche les pixels qui ont un alpha suffisant */
    glEnable (GL_ALPHA_TEST);
    glAlphaFunc (GL_GREATER, minAlpha);
    /* BLENDING */
    glBlendFunc (GL_SRC_ALPHA,GL_ONE);
    glEnable (GL_BLEND);
    glDisable (GL_DEPTH_TEST);
    /* La couleur de la texture remplace la couleur par defaut*/
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // GL_MODULATE GL_REPLACE

    /* crée les plans texturés */
    if (!DensityTex3D)
      drawSlicesTex2D (camera);
    else
      drawSlicesTex3D (camera);
    drawBorder();
		
    glDisable (GL_ALPHA_TEST);
    glDisable (GL_BLEND);
    glEnable (GL_DEPTH_TEST);
  }
}


void Density::drawSlicesTex2D(Camera *camera)
{
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, _textureID[0]);

  double haut, bas, gauche, droite, pos;
  double div=1.0/DRow;
  double div1=2.0/(DRow-1);

  if ((camera->getPosition()).z > 0 )
  {
      // Traçage : a partir du dernier plan vers le premier
    haut=1. ; bas=1. ; gauche=0. ; droite=1.;pos = -1.0;
    for (int i=DRow; i>0 ; i--)
    {
      bas=haut;
      haut-=div;

      glBegin(GL_QUADS);
      glNormal3i(0,0,1);
      glTexCoord2d(gauche,bas); glVertex3f( -1.f, -1.f, pos); // 0,1
      glTexCoord2d(droite,bas); glVertex3f( 1.f, -1.f, pos); // 1,1
      glTexCoord2d(droite,haut); glVertex3f( 1.f, 1.f, pos);// 1,0
      glTexCoord2d(gauche,haut); glVertex3f( -1.f, 1.f, pos);// 0,0
      glEnd();
      pos+=div1;
    }
  }
  else
  {
      // Traçage : a partir du premier plan vers le dernier
    haut=0. ,bas=0. ,gauche=0.,droite=1., pos=1.;
    for (int i=DRow; i>0 ; i--)
    {
      haut=bas;
      bas+=div;

      glBegin(GL_QUADS);
      glNormal3i(0,0,-1);
      glTexCoord2d(gauche,bas); glVertex3f( -1.f, -1.f, pos); // 0,1
      glTexCoord2d(droite,bas); glVertex3f( 1.f, -1.f, pos); // 1,1
      glTexCoord2d(droite,haut); glVertex3f( 1.f, 1.f, pos);// 1,0
      glTexCoord2d(gauche,haut); glVertex3f( -1.f, 1.f, pos);// 0,0
      glEnd();
      pos-=div1;
    }
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
}


void Density::drawSlicesTex3D(Camera *cm)
{

  glEnable(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_3D, _textureID[0]);

   float pos = 1.5;
  //float pos = -0.8;
  float div1=3./(DRow-1);
  // le tableau des points a tracer :
  Vector3 Ptab [6];
  Vector3 Ttab [6];
  int nbpoint = 0 ;

for (int i=DRow; i>0 ; i--)
  {
  // on génére le polygone
    generatePolygon(nbpoint, Ptab, Ttab, cm, pos);
  // on le rend convexe
    makeConvexPolygon(nbpoint,Ptab,Ttab);
 // et on trace les points
    glBegin(GL_POLYGON);
    //glBegin(GL_LINE_LOOP);
    glColor3ub(255,255,255);
  //glNormal3i(0,0,1);
    for (int i=0; i<nbpoint ; i++)
    {
      glTexCoord3f(Ttab[i].x,Ttab[i].y,Ttab[i].z);
      glVertex3f(Ptab[i].x,Ptab[i].y,Ptab[i].z);
    }
    glEnd();
		
    pos-=div1;
  }

  glBindTexture(GL_TEXTURE_3D, 0);
  glDisable(GL_TEXTURE_3D);
}


void Density::extractAllPlane(float alpha,int billboard,std::vector<float*> &temp)
{
	float maxtemp=32000;
	
	int j=1;
	float* CoordMax=this->convertMaxPlane();

	float max=CoordMax[4];

	float *coord = new float[4];
	for(int i=0;i<4;i++) coord[i]=CoordMax[i];
	temp.push_back(coord);


	while(alpha*max<=maxtemp && j<billboard)
	{
		float* CoordMax=this->convertMaxPlane();

		maxtemp=CoordMax[4];

		float *coord = new float[4];
		for(int i=0;i<4;i++) coord[i]=CoordMax[i];

		if(maxtemp>alpha)
			temp.push_back(coord);
	
		j++;
	}
}

float* Density::convertMaxPlane()
{


	//initialisation
	int dim=this->getRow();
	int suppress = 3;
	float* carthMaxPlane=new float[5];

	//recherche de la coordonnées du maximum
	float* densityCoordMax=this->extractMaxPlane(/*0,0,0,dim*/);

	
	for(int covOfX = -suppress; covOfX <= suppress; covOfX++)
	{
		for(int covOfY = -suppress; covOfY <= suppress; covOfY++)
		{
			for(int covOfZ = -suppress; covOfZ <= suppress*10/(this->getDistanceMax()); covOfZ++)
			{
				if( densityCoordMax[0]+covOfX<dim && densityCoordMax[1]+covOfY<dim && densityCoordMax[2]+covOfZ<dim
								&& densityCoordMax[0]+covOfX>=0 && densityCoordMax[1]+covOfY>=0 && densityCoordMax[2]+covOfZ>=0)
				{

					int neighbourDist=abs(covOfX);
					if(neighbourDist<abs(covOfY))
						neighbourDist=abs(covOfY);
					if(neighbourDist<abs(covOfZ))
						neighbourDist=abs(covOfZ);
					this->setElt( (int) densityCoordMax[0]+covOfX, (int) densityCoordMax[1]+covOfY, 
													 (int) densityCoordMax[2]+covOfZ,0.);
				}
			}
		}
	}

	float phi =( ((densityCoordMax[0]/(this->getRow()-1))*3.1416 ) + ( (densityCoordMax[0]+1)/(this->getRow()-1)*3.1416) )/2;
	float ro  =( ((densityCoordMax[1]/(this->getRow()-1))*3.1416*2+3.1416 ) + ( (densityCoordMax[1]+1)/(this->getRow()-1)*3.1416*2+3.1416) )/2;

	float a=sin(phi)*cos(ro);

	float b=sin(phi)*sin(ro);
	float c=cos(phi);
	float d= ( densityCoordMax[2]/(this->getRow()) )*( this->getDistanceMax() );

	carthMaxPlane[0]=a;
	carthMaxPlane[1]=b;
	carthMaxPlane[2]=c;
	carthMaxPlane[3]=d;
	carthMaxPlane[4]=densityCoordMax[3];

	return carthMaxPlane;
}



float* Density::extractMaxPlane(){

	float* planeMax=new float[4];
	planeMax[3]=0.;
	for(int i=0;i<this->DRow;i++)
	{
		for(int j=0;j<this->DRow;j++)
		{
			for(int k=0;k<this->DRow;k++)
			{
				float c = this->getElt(i,j,k);
				if(planeMax[3]<c)
				{
					planeMax[0]=i;
					planeMax[1]=j;
					planeMax[2]=k;
					planeMax[3]=c;
				}
			}
		}
	}

	return planeMax;
}

void Density::generatePolygon(int & nbPoint, Vector3 Ptab[6], Vector3 Ttab[6], Camera *cm, float pos)
{
   // les tableaux des 12 points
  float Ztab [4] = {10.,10.,10.,10.};
  float Ytab [4] = {10.,10.,10.,10.};
  float Xtab [4] = {10.,10.,10.,10.};
  float helpPtab1 [4] = {1,-1,-1,1};
  float helpPtab2 [4] = {1,1,-1,-1};
  float helpTtab1 [4] = {1,0,0,1};
  float helpTtab2 [4] = {1,1,0,0};
  int n ;

  std::vector<Vector3> cam = cm->getValCam();
  Vector3 mCamZ = cam.at(3) ;

  mCamZ.normalise();

  if (mCamZ.z != 0)
  {
    Ztab[0]= -(pos + mCamZ.x + mCamZ.y)/mCamZ.z ; //  1  1
    Ztab[1]= -(pos - mCamZ.x + mCamZ.y)/mCamZ.z ; // -1  1
    Ztab[2]= -(pos - mCamZ.x - mCamZ.y)/mCamZ.z ; // -1 -1
    Ztab[3]= -(pos + mCamZ.x - mCamZ.y)/mCamZ.z ; //  1 -1
  }
  if (mCamZ.x != 0)
  {
    Xtab[0]= -(pos + mCamZ.y + mCamZ.z)/mCamZ.x ;//  1  1
    Xtab[1]= -(pos - mCamZ.y + mCamZ.z)/mCamZ.x ;// -1  1
    Xtab[2]= -(pos - mCamZ.y - mCamZ.z)/mCamZ.x ;// -1 -1
    Xtab[3]= -(pos + mCamZ.y - mCamZ.z)/mCamZ.x ;//  1 -1
  }
  if (mCamZ.y != 0)
  {
    Ytab[0]= -(pos + mCamZ.x + mCamZ.z)/mCamZ.y ;//  1  1
    Ytab[1]= -(pos - mCamZ.x + mCamZ.z)/mCamZ.y ;// -1  1
    Ytab[2]= -(pos - mCamZ.x - mCamZ.z)/mCamZ.y ;// -1 -1
    Ytab[3]= -(pos + mCamZ.x - mCamZ.z)/mCamZ.y ;//  1 -1
  }
  // on a calculé tout nos points, on filtres ceux qui correspondent a des points a tracer :
    n=0;
    for (int i=0 ; i < 4 ; i++)
    {
      if (Ztab[i]>-1 && Ztab[i]<=1)
      {
        Ptab[n]=Vector3(helpPtab1[i], helpPtab2[i], Ztab[i]);
				Ttab[n]=Vector3(helpTtab1[i], helpTtab2[i], (Ztab[i]+1.)/2.);
        n++;
      }
      if (Ytab[i]>-1 && Ytab[i]<=1)
      {
        Ptab[n]=Vector3(helpPtab1[i], Ytab[i], helpPtab2[i]);
				Ttab[n]=Vector3(helpTtab1[i], (Ytab[i]+1.)/2., helpTtab2[i]);
        n++;
      }
      if (Xtab[i]>-1 && Xtab[i]<=1)
      {
        Ptab[n]=Vector3(Xtab[i], helpPtab1[i], helpPtab2[i]);
        Ttab[n]=Vector3((Xtab[i]+1.)/2., helpTtab1[i], helpTtab2[i]);
        n++;
      }
    }
  //on connait aussi le nombre de points a tracer
    nbPoint = n ;
}

void Density::makeConvexPolygon ( int n, Vector3 *Ptab, Vector3 *Ttab )
{
	int helptab[6] = {0,-1,-1,-1,-1,-1}; //comporte le n° de sommets contenu dans Ptab mais ici ordonnée
	int helptab2[6] = {0,-1,-1,-1,-1,-1}; //pour se souvenir quel vecteur est mis dans le contour
	Vector3 centre = Vector3 ( 0,0,0 ) ; // centre de gravité des points
	if ( n !=0 )
	{
		for ( int i=0; i<n; i++ )
		{
			centre += Ptab[i];
		}
		centre /= n;

		Vector3 vect1 = Vector3 ( 0,0,0 );
		Vector3 vect2 = Vector3 ( 0,0,0 );
		Vector3 normale = Vector3 ( 0,0,0 );
		Vector3 tmpnorm = Vector3 ( 0,0,0 );
		float min = Pi ;
		float tmp ;
		int place=-1 ;
		// on prendra ø = acos (v1.v2 / |v1|.|v2|) minimum
		vect1 = Ptab[0]-centre ;
		
		for ( int j=1 ; j<n ; j++ )
		{
			for ( int i=0 ; i<n ; i++ )
			{
				if (helptab2[i]==-1)
				{ // si le sommet ne fait pas déjà partie du polygone
					vect1.normalise();
					vect2 = Ptab[i]-centre;
					vect2.normalise();
					tmp = acosf ( vect1.dotProduct ( vect2 ) );
					tmpnorm = vect1.crossProduct(vect2);
					tmpnorm.normalise();
					if ( tmp < min && tmp!=0 )
					{
						// on vérifie que la future normale est dans le même sens que l'ancienne
						if (((normale.x < 0 && tmpnorm.x< 0)||(normale.x > 0 && tmpnorm.x > 0)
											 &&(normale.y < 0 && tmpnorm.y< 0)||(normale.y > 0 && tmpnorm.y > 0)											 
											 &&(normale.z < 0 && tmpnorm.z< 0)||(normale.z > 0 && tmpnorm.z > 0))
												|| normale == Vector3(0,0,0))
						{
							min = tmp ;
							place = i;
							normale = tmpnorm;
						}
					}
				}
			}
			// pour le prochain tour on partira du meilleur sommet
			if ( place != -1 )
			{
				vect1=Ptab[place]-centre; // le meilleur dernierement trouvé
				helptab[j]=place; // on le met dans le contour a sa place
				helptab2[place]=place; // et on se souvient qu'il est dans le contour
			}
			place = -1;
			min = Pi;
		}
		// on place les points dans l'ordre de helptab
		Vector3 tmptab[2][6] ;
		for(int i=0 ; i<n;i++) // on les sauve ici
		{
			tmptab[0][i]=Ptab[i];
			tmptab[1][i]=Ttab[i];
		}
		for(int i=0 ; i<n;i++) // et on les replace comme il faut ici
		{
			Ptab[i]=tmptab[0][helptab[i]];
			Ttab[i]=tmptab[1][helptab[i]];
		}
	}
}

void Density::drawBorder()
{
/* Pour repérer le cube de rendu traçage du contour */
    glPushAttrib (GL_ALL_ATTRIB_BITS);
    glDisable (GL_LIGHTING);
    glColor3ub (255,0,0);
    glBegin (GL_LINE_LOOP);
    glVertex3i (-1,-1,-1);
    glVertex3i (-1,1,-1);
    glVertex3i (1,1,-1);
    glVertex3i (1,-1,-1);
    glEnd();
    glBegin (GL_LINE_LOOP);
    glVertex3i (-1,-1,1);
    glVertex3i (-1,1,1);
    glVertex3i (1,1,1);
    glVertex3i (1,-1,1);
    glEnd();
    glColor3ub(255,255,0); //en Jaune le coté tranche
    glBegin (GL_LINES);
    glVertex3i (-1,-1,-1); glVertex3i (-1,-1,1);
    glVertex3i (-1,1,-1); glVertex3i (-1,1,1);
    glVertex3i (1,-1,-1); glVertex3i (1,-1,1);
    glVertex3i (1,1,-1); glVertex3i (1,1,1);
    glEnd();
    glPopAttrib();
}

void Density::coloring()
{/*
	int nbCell = DRow*DRow*DRow*4 ;
	Color c ;
	float r, g, b, alpha;
	for (int i=0; i<nbCell; i+=4)
	{
		c = getColor(i);
		alpha = c.a ;
		r = ((-(alpha-1.0)*(alpha-1.0)/0.1)+1.0);
		g = (5.0*(-((alpha-0.5)*(alpha-0.5))/0.4)+1.0);
		b = ((-(alpha * alpha)/0.15)+1.0);
	
		if (r<0) r=0 ;
		if (g<0) g=0 ;
		if (b<0) b=0 ;
		setColor(i,Color(r,g,b,c.a));
	}*/
}

void Density::setShaders() 
{
	char *vs = NULL,*fs = NULL ;
	GLuint f, p, v;
	
	//std::cout << "Initialising shader ... " << std::endl;
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	
	mVertShader= v;
	mFragShader = f ;
	
	vs = textFileRead("./shaders/shader.vert");
	if (DensityTex3D) 
		fs = textFileRead("./shaders/shader3D.frag");
	else
		fs = textFileRead("./shaders/shader.frag");
	
	const char * ff = fs;
	const char * vv = vs;
	
	if (ff==NULL || fs==NULL)	
	{
		std::cerr << "Density::setShaders : Shaders files not found !" << std::endl;
		std::cout << "Using : pre-coloration ... wait !" << std::endl;
		coloring();
		return ;
	}
	
	glShaderSource(v, 1, &vv,NULL);
	glShaderSource(f, 1, &ff,NULL);
	
	free(vs);
	free(fs);

	glCompileShader(v);
	glCompileShader(f);

	p = glCreateProgram();
	glAttachShader(p,f);
	glAttachShader(p,v);

	glLinkProgram(p);
	glUseProgram(p);
	
	mProgShader=p;
}

void Density::removeShaders()
{
	glDetachShader(mProgShader, mFragShader);
	glDetachShader(mProgShader, mVertShader);
	
	glDeleteShader(mFragShader);
	glDeleteProgram(mVertShader);
	
	glDeleteShader(mFragShader);
	glDeleteProgram(mVertShader);
}

void Density::setDistanceMax(float dmax)
{
  this->distMax=dmax;
}


float Density::getDistanceMax()
{
  return this->distMax;
}
