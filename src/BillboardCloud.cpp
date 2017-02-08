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


#include "BillboardCloud.h"

BillboardCloud::BillboardCloud(Entity* entite,std::vector<float*> vect,Viewer*v,float ep,int reso): entity(entite),cam(v->getCamera()),viewer(v),isLoad(false), error(false), Binfo(false)
{
	//pour chaque plan on cree un Billboard
	for(std::vector<float*>::iterator it = vect.begin();it!=vect.end();it++){
			Billboard *BB = new Billboard(*it, entity,ep,viewer,reso);
			listBillboard.push_back(BB);
	}
}

BillboardCloud::BillboardCloud(const QString& filename,Viewer *v):viewer(v),isLoad(true), error(false), Binfo(false){
		QFile BBCFile(filename);
	  if(!BBCFile.open(QIODevice::ReadOnly))
  {
    std::cerr << "erreur :chargement BillboardCloud : " << filename.toStdString() << std::endl;
    return;
  }
	QTextStream in(&BBCFile);
	QString header;
	in>>header;
	QString path=(filename.section(".bbc",0,0))+".png";
	Image im; 
  im = chargeImage(path.toAscii());
	if (im == NULL)
	{
		error=true;
		return;
	}
	int nbBB = header.toInt();
	float texCoord[8];
	float coord[12];
	for(int i=0;i<nbBB;i++){
		for(int j=0;j<8;j++){	
			in>>header;
			texCoord[j]=header.toFloat();
		}
		for(int j=0;j<12;j++){	
			in>>header;
			coord[j]=header.toFloat();
		}
		float f[3];
		for(int k=0;k<3;k++){	
			in>>header;
			f[k]=header.toFloat();
		}
		Vector3 vect(f[0],f[1],f[2]);
			Billboard *BB = new Billboard(im,viewer,coord,texCoord,vect);	
			listBillboard.push_back(BB);	
	}
}
	

BillboardCloud::~BillboardCloud(){

}
 

QString BillboardCloud::saveBillboardCloud(const QString &filename){
	QString body;
  QString str;

  int count=0;
	body=str.setNum(listBillboard.size())+"\n";
  while(!(filename.section('/',count,count)).isEmpty()) count++;
  count--;
  
		int largeur=0,hauteur=0;
		for(std::vector<Billboard*>::iterator it=listBillboard.begin();it!=listBillboard.end();it++){
			if((*it)->getnX()>largeur) largeur = (*it)->getnX();
			hauteur += (*it)->getnY();
		}
	unsigned char* image = new unsigned char[largeur * hauteur * 4];
	int h,l;
	int index = 0;
	float u,v,hauteurCourante=0.;
	for(std::vector<Billboard*>::iterator it=listBillboard.begin();it!=listBillboard.end();it++){
			l = (*it)->getnX() * 4;
			h = (*it)->getnY();
			u=(l/4.)/largeur;
			v=((float)h/(float)hauteur);

			hauteurCourante+=v;

			body=body+str.setNum(0.)+" ";
			body=body+str.setNum(hauteurCourante)+"\n";

			body=body+str.setNum(0.)+" ";
			body=body+str.setNum(hauteurCourante - v)+"\n";
			
		
			body=body+str.setNum(u)+" ";
			body=body+str.setNum(hauteurCourante-v)+"\n";


			body=body+str.setNum(u)+" ";
			body=body+str.setNum(hauteurCourante)+"\n";

			body=body+(*it)->getCoord();
			for(int i=0;i<h;i++){	
				for(int j=0;j<largeur*4;j++){
					if(j<l)image[index++]=((*it)->getPixels())[i*l+j];
					else if(j==l) image[index++]=((*it)->getPixels())[i*l+j-1];
					else image[index++] = 255;							
				}
				
			}
		
	}
	Image ima;
	ima= creeImage(image,largeur,hauteur,4);
	imageSauvePng(ima,filename.toAscii()+".png");

	return body;
}

void BillboardCloud::drawGeometry(){
	for(std::vector<Billboard*>::iterator it=listBillboard.begin();it!=listBillboard.end();it++){
		(*it)->drawBillboard();
	}
	if (Binfo) drawText();
}


Vector3 BillboardCloud::getCenterM(void)
{
		Vector3 center;
		Vector3 centerBB;
		std::vector<float>::iterator ite;
		float nb = (float)listBillboard.size();
		float x=0.,y=0., z=0.;

 		for(std::vector<Billboard*>::iterator it=listBillboard.begin();it!=listBillboard.end();it++) {
 			centerBB = (*it)->getCenter();
 			x+= centerBB.x;
 			y+= centerBB.y;
 			z+= centerBB.z;
 		}
		center.x=x/nb;
		center.y=y/nb;
		center.z=z/nb;

		return center;
}

std::vector<Vector3> BillboardCloud::getVertexArray(){
	std::vector<Vector3> vect;
	return vect;
}


void BillboardCloud::deleteVBO(){}

void BillboardCloud::fillDensity(int coverage, Density *pDensity){}


/// Tout ce qui suit concerne l'affichage des informations

void BillboardCloud::drawText()
{
	bool Ecolormat = glIsEnabled(GL_COLOR_MATERIAL);
	bool Elight = glIsEnabled(GL_LIGHTING);
	bool Etext = glIsEnabled(GL_TEXTURE_2D); 
	char strMsg[100] = {0};
	int taille;
	char info[10];
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glLoadIdentity();
	if (error)
	{
		glColor3f(1.,0.2,0.2);
		glRasterPos3f(position.x,position.y,position.z);
		sprintf ( strMsg, "Erreur de chargement : Texture introuvable");
	
		for (unsigned int i=0;i<strlen(strMsg);i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*(strMsg+i));
		
		glPopMatrix();

		if(Elight) glEnable(GL_LIGHTING);
		if (Ecolormat) glEnable(GL_COLOR_MATERIAL);
		if (Etext) glEnable(GL_TEXTURE_2D);
		return ;
	}

	glColor3f(1.,1.,0.2);
	glRasterPos3f(position.x,position.y,position.z);
	if (isLoad)
		sprintf (info,"Chargement :");
	else
		sprintf (info,"Cree en ");

	sprintf ( strMsg, "%s %f sec.", info, time);

	for (unsigned int i=0;i<strlen(strMsg);i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*(strMsg+i));

	int nb = listBillboard.size();
	glRasterPos3f(position.x,position.y-0.05,position.z);
	sprintf ( strMsg, "%d Billboards",nb);

	for (unsigned int i=0;i<strlen(strMsg);i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*(strMsg+i));


	glPopMatrix();

	if(Elight) glEnable(GL_LIGHTING);
	if (Ecolormat) glEnable(GL_COLOR_MATERIAL);
	if (Etext) glEnable(GL_TEXTURE_2D);
}

void BillboardCloud::afficheInfoM(int split){
 	if (split==1)
			position.x=-0.51;
	else if (split==2)
			position.x=-1.0;
	else if (split==3)
			position.x=0.0;
 	else if (split==4)
		Binfo=false;
	else
		Binfo=!Binfo;
}

void BillboardCloud::setMode(int mode, bool info)
{
	Binfo=info;
	if (mode==0)
		position.x=-1.0;
	else if (mode==1)
		position.x=-0.51;
	else position.x=0.0;
	position.y=0.65;
	position.z=-1.;
}

void BillboardCloud::setTime(double t)
{
	time = t;
}

