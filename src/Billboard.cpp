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

#include "Billboard.h"



float Billboard::distVertices(Vector3 vect1,Vector3 vect2){
	return (sqrt((vect1.x - vect2.x)*(vect1.x - vect2.x)+(vect1.y - vect2.y)*(vect1.y - vect2.y)+(vect1.z - vect2.z)*(vect1.z - vect2.z)));
}


void Billboard::projection(std::vector<Vector3> vect){

  float a = plan[0];
  float b = plan[1];
  float c = plan[2];
  float d = plan[3];

  float xp,yp,zp;
  if(a!=0.){
    xp=-d/a;
    yp=0.;
    zp=0.;
  }
	else if(b!=0.){
    xp=0.;
    yp=-d/b;
    zp=0.;
	}
	else {
    xp=0.;
    yp=0.;
    zp=-d/c;
	}

	//la projection
	for(std::vector<Vector3>::iterator it=vect.begin();it<vect.end();it++){
    Vector3 sommet = *it;
    Vector3 proj;  

    proj.x = sommet.x - (((a*(sommet.x - xp) + b*(sommet.y - yp) + c*(sommet.z - zp)) * a)/(a*a+b*b+c*c));  
    proj.y = sommet.y - (((a*(sommet.x - xp) + b*(sommet.y - yp) + c*(sommet.z - zp)) * b)/(a*a+b*b+c*c));  
    proj.z = sommet.z - (((a*(sommet.x - xp) + b*(sommet.y - yp) + c*(sommet.z - zp)) * c)/(a*a+b*b+c*c));  
  	projections.push_back(proj);
  }
}


Vector3 Billboard::acp(const std::vector<Vector3> &vect, std::vector<Vector3> &frame){

   // calcul du centre du nuage de points
   Vector3 center(0.,0.,0.);
   for (unsigned int i=0; i<vect.size(); i++)
          center += vect[i];
   center /= vect.size();

   // calcule la matrice de covariance
   float sumXX = 0., sumXY = 0., sumXZ = 0., sumYY = 0., sumYZ = 0., sumZZ = 0.;

   for (unsigned int i=0; i<vect.size(); i++){
      Vector3 diff = center - vect[i];
      sumXX += diff[0] * diff[0];
      sumXY += diff[0] * diff[1];
      sumXZ += diff[0] * diff[2];
      sumYY += diff[1] * diff[1];
      sumYZ += diff[1] * diff[2];
      sumZZ += diff[2] * diff[2];
    }

    Matrix3 cov(
            sumXX, sumXY, sumXZ,
            sumXY, sumYY, sumYZ,
            sumXZ, sumYZ, sumZZ);

    // decomposition en valeurs propres et vecteurs propres
    float eigenValue[3];
    Vector3 eigenVector[3];
    cov.EigenSolveSymmetric(eigenValue, eigenVector);

//trie des valeurs/vecteurs propres
	if (eigenValue[0] >= eigenValue[1]) {
    if (eigenValue[0] >= eigenValue[2]) {
        frame.push_back(eigenVector[0]);
        if (eigenValue[1] >= eigenValue[2]) {
            frame.push_back(eigenVector[1]);
            frame.push_back(eigenVector[2]);
        } else {
            frame.push_back(eigenVector[2]);
            frame.push_back(eigenVector[1]);
        }
    } else {
        frame.push_back(eigenVector[2]);
        frame.push_back(eigenVector[0]);
        frame.push_back(eigenVector[1]);
    }
	} else {
    if (eigenValue[1] >= eigenValue[2]) {
        frame.push_back(eigenVector[1]);
        if (eigenValue[0] >= eigenValue[2]) {
            frame.push_back(eigenVector[0]);
            frame.push_back(eigenVector[2]);
        } else {
            frame.push_back(eigenVector[2]);
            frame.push_back(eigenVector[0]);
        }
    } else {
        frame.push_back(eigenVector[2]);
        frame.push_back(eigenVector[1]);
        frame.push_back(eigenVector[0]);
    }
	}

	//les vecteurs propres ne forment pas un triède direct
	
  	//creation du repere direct O XYZ

	//OZ
	Vector3 v1 = frame[2] - center;
	//O(-Z)
	Vector3 v2 = (-frame[2]) - center;

	Vector3 centre = entity->getCenter();
	//OP
	Vector3 Z = centre - center;

	//les cosinus des angles
	float cosa = v1.dotProduct(Z) / sqrt(v1.squaredLength() * Z.squaredLength());
	float cosb = v2.dotProduct(Z) / sqrt(v2.squaredLength() * Z.squaredLength());
	//maj de Z
	if(acos(cosa)>acos(cosb)) frame[2]=-frame[2];

	Vector3 Y(frame[1].x + 0.1,frame[1].y + 0.1,frame[1].z + 0.1);
	//calcul de Y par rapport a l'angle
	float cosc = Y.dotProduct( frame[2]) / sqrt(Y.squaredLength() * frame[2].squaredLength());
	if(cosc<0.) frame[1] = -frame[1];

	//produit vectoriel pour calculer X
	frame[0].x = frame[2].y * frame[1].z - frame[2].z * frame[1].y;
	frame[0].y = frame[2].z * frame[1].x - frame[2].x *frame[1].z;
	frame[0].z = frame[2].x * frame[1].y - frame[2].y *frame[1].x;

	return center;
}


Vector3* Billboard::findCoord(Matrix4* repere ,std::vector<Vector3> vect){
  Vector3 res;
  Vector3 minX,minY,maxX,maxY;

//recherche du rectangle englobant
	for(int j=0;j<3;j++){
		minX.x=vect[0].x;
		minX.y=vect[0].y;
		minX.z=vect[0].z;

		minY.x=vect[0].x;
		minY.y=vect[0].y;
		minY.z=vect[0].z;

		maxX.x=vect[0].x;
		maxX.y=vect[0].y;
		maxX.z=vect[0].z;

		maxY.x=vect[0].x;
		maxY.y=vect[0].y;
		maxY.z=vect[0].z;
	}

	for(unsigned int i=1;i<vect.size();i++){
		if(minX.x>vect[i].x) {
			minX.x=vect[i].x;
			minX.y=vect[i].y;
			minX.z=vect[i].z;
		} 
		if(minY.y>vect[i].y) {
			minY.x=vect[i].x;
			minY.y=vect[i].y;
			minY.z=vect[i].z;
		} 

		if(maxX.x<vect[i].x) {
			maxX.x=vect[i].x;
			maxX.y=vect[i].y;
			maxX.z=vect[i].z;
		} 
		if(maxY.y<vect[i].y) {
			maxY.x=vect[i].x;
			maxY.y=vect[i].y;
			maxY.z=vect[i].z;
		} 
	}

//on stocke les coordonnees du rectangle englobant
  std::vector<Vector3> vecto;
  Vector3 v3;

  v3.x=minX.x;
  v3.y=maxY.y;
  v3.z=0.;
  vecto.push_back(v3);

  v3.x=minX.x;
  v3.y=minY.y;
  v3.z=0.;
  vecto.push_back(v3);

  v3.x=maxX.x;
  v3.y=minY.y;
  v3.z=0.;
  vecto.push_back(v3);

  v3.x=maxX.x;
  v3.y=maxY.y;
  v3.z=0.;
  vecto.push_back(v3);

  //on stocke aussi le centre + epsilon qui sera la position de la camera
  res.x=(minX.x+maxX.x)/2.;
  res.y=(minY.y+maxY.y)/2.; 
  res.z=-epsilon;//car Z pointe vers l'objet

  vecto.push_back(res);

  std::vector<Vector3> vectoInv;
	for (std::vector<Vector3>::iterator it=vecto.begin(); it != vecto.end(); it++) {
	// projection sur le plan
	Vector3 v = *repere * (*it);
	vectoInv.push_back(v);
	}
	//sauvegarde des coordonnees du billboard
	std::vector<Vector3>::iterator r;
	r=vectoInv.begin();

	verticesBB[0]=(*r).x;
	verticesBB[1]=(*r).y;
	verticesBB[2]=(*r).z;

	r++;
	verticesBB[3]=(*r).x;
	verticesBB[4]=(*r).y;
	verticesBB[5]=(*r).z;
	r++;
	verticesBB[6]=(*r).x;
	verticesBB[7]=(*r).y;
	verticesBB[8]=(*r).z;
	r++;
	verticesBB[9]=(*r).x;
	verticesBB[10]=(*r).y;
	verticesBB[11]=(*r).z;

	r=(vectoInv.end() -1);  
	//retourne la position de la camera
	return &(*r);
}

void Billboard::createBox(){

	std::vector<Vector3> vect = entity->getVertexArrayE();
//recherche de la boite englobante
	Vector3 minX(vect[0]);
	Vector3 maxX(vect[0]);
	Vector3 minY(vect[0]);
	Vector3 maxY(vect[0]);
	Vector3 minZ(vect[0]);
	Vector3 maxZ(vect[0]);


	for(unsigned int i=1;i<vect.size();i++){
		if(minX.x>vect[i].x) minX=vect[i];
		if(minY.y>vect[i].y) minY=vect[i];
		if(minZ.z>vect[i].z) minZ=vect[i];
		if(maxX.x<vect[i].x) maxX=vect[i];
		if(maxY.y<vect[i].y) maxY=vect[i];
		if(maxZ.z<vect[i].z) maxZ=vect[i];
	}

//sauvegarde de la boite englobante
  Vector3 sommets;
        sommets.x = minX.x;
        sommets.y = minY.y;
        sommets.z = minZ.z;
	box.push_back(sommets);
        sommets.x = minX.x;
        sommets.y = minY.y;
        sommets.z = maxZ.z;
	box.push_back(sommets);
        sommets.x = minX.x;
        sommets.y = maxY.y;
        sommets.z = minZ.z;
	box.push_back(sommets);
        sommets.x = minX.x;
        sommets.y = maxY.y;
        sommets.z = maxZ.z;
	box.push_back(sommets);
        sommets.x = maxX.x;
        sommets.y = maxY.y;
        sommets.z = minZ.z;
	box.push_back(sommets);
        sommets.x = maxX.x;
        sommets.y = maxY.y;
        sommets.z = maxZ.z;
	box.push_back(sommets);
        sommets.x = maxX.x;
        sommets.y = minY.y;
        sommets.z = minZ.z;
	box.push_back(sommets);
        sommets.x = maxX.x;
        sommets.y = minY.y;
        sommets.z = maxZ.z;
	box.push_back(sommets);


	//recherche de la longueur maximale des aretes de la boite pour reference a la resolution de texture
	float d1,d2,d3;
	d1 = distVertices(box[0],box[1]);
	d2 = distVertices(box[0],box[2]);
	d3 = distVertices(box[0],box[6]);

	distMaxBox = d1;
	if(d2>d1) distMaxBox = d2;
	if(d3>distMaxBox) distMaxBox = d3;
}

int Billboard::resolution(float a){
	if(a>0.5) return resMax;
	else if(a>0.25) return resMax/2;
	else return resMax/4;

}

void Billboard::createBillboard(){

	//projette les sommets de la boite englobante sur le plan
	projection(box);
	std::vector<Vector3> frame;

	// on parametre le plan : ACP + centre
	Vector3 center = acp(projections, frame);

	// on construit la matrice de changement de repere 
	Matrix4 mat(
	frame[0][0], frame[1][0],frame[2][0], center[0],
	frame[0][1], frame[1][1],frame[2][1], center[1],
	frame[0][2], frame[1][2],frame[2][2], center[2],
	0., 0., 0., 1. 
	);

	mat = mat.inverse();
	std::vector<Vector3> projections2D;

	for (std::vector<Vector3>::iterator it=projections.begin(); it != projections.end(); it++) {
		// projection sur le plan
		Vector3 v = mat * (*it);
		projections2D.push_back(v);
	}
	mat = mat.inverse();

	//sauvegarde des normales du Billboard
	normals.x = -frame[2].x;
	normals.y = -frame[2].y;
	normals.z = -frame[2].z;


	//parametrisation de la camera

	Vector3* position = findCoord(&mat,projections2D); 

	cam->setPosition(*position);

	cam->setX(frame[0]);
	cam->setY(frame[1]);
	cam->setZ(frame[2]);

	//parametrisation de la camera pour la texture
	float d1 = sqrt((verticesBB[0] - verticesBB[3])*(verticesBB[0] - verticesBB[3])+(verticesBB[1] - verticesBB[4])*(verticesBB[1] - verticesBB[4])+(verticesBB[2] - verticesBB[5])*(verticesBB[2] - verticesBB[5]));		

	float d2 = sqrt((verticesBB[0] - verticesBB[9])*(verticesBB[0] - verticesBB[9])+(verticesBB[1] - verticesBB[10])*(verticesBB[1] - verticesBB[10])+(verticesBB[2] - verticesBB[11])*(verticesBB[2] - verticesBB[11]));
		

	float rapportX,rapportY;
	bool t=false;
	if(d1>d2){
		rapportX = d1/distMaxBox;
		rapportY = d2/distMaxBox;
		t=true;
	}			
	else{
		rapportY = d1/distMaxBox;
		rapportX = d2/distMaxBox;
	}

	vX = resolution(rapportX);
	vY = resolution(rapportY);

	if(t) cam->setViewport2( vX,vY,d1,d2,epsilon);
	else cam->setViewport2( vX,vY,d2,d1,epsilon);
	//activation du glOrtho
	cam->setPerspective(false);
}



void Billboard::generateTexture(){
	pixels = new unsigned char[vX*vY*4];
	glReadBuffer(GL_BACK);
	glReadPixels(0, 0,vX,vY, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glGenTextures(1, _textureID);
  glBindTexture(GL_TEXTURE_2D, _textureID[0]); 
  gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,vX,vY,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
	glBindTexture(GL_TEXTURE_2D,0);
	//desactivation du glOrtho,mode perspective
	cam->setPerspective(true);
}

void Billboard::drawGeometry(){
	
	//affichage du billboard
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  glEnable(GL_LIGHTING);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_TEXTURE_2D);
 	glBindTexture(GL_TEXTURE_2D, _textureID[0]);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR
	glBegin(GL_QUADS);
  glNormal3f(normals.x,normals.y,normals.z);glTexCoord2d(tCoord[0],tCoord[1]); glVertex3f(verticesBB[0],verticesBB[1],verticesBB[2]); 
 glNormal3f(normals.x,normals.y,normals.z); glTexCoord2d(tCoord[2],tCoord[3]); glVertex3f(verticesBB[3],verticesBB[4],verticesBB[5]);
 glNormal3f(normals.x,normals.y,normals.z); glTexCoord2d(tCoord[4],tCoord[5]); glVertex3f(verticesBB[6],verticesBB[7],verticesBB[8]);       
 glNormal3f(normals.x,normals.y,normals.z); glTexCoord2d(tCoord[6],tCoord[7]); glVertex3f(verticesBB[9],verticesBB[10],verticesBB[11]);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// le mode filiaire
	if(viewer->isWireframeEnabled()){
		
		glEnable(GL_POLYGON_SMOOTH );
		glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
 		glLineWidth(1.5);
		glBegin(GL_QUADS);
   	glVertex3f(verticesBB[0],verticesBB[1],verticesBB[2]); 
    glVertex3f(verticesBB[3],verticesBB[4],verticesBB[5]);
 		glVertex3f(verticesBB[6],verticesBB[7],verticesBB[8]);       
   	glVertex3f(verticesBB[9],verticesBB[10],verticesBB[11]);
		glEnd();
	}
	if (Binfo) drawText();
}

Vector3 Billboard::getCenterM(void)
{
	Vector3 center;
	center.x=(verticesBB[0]+verticesBB[3]+verticesBB[6]+verticesBB[9])/4.;
	center.y=(verticesBB[1]+verticesBB[4]+verticesBB[7]+verticesBB[10])/4.;
	center.z=(verticesBB[2]+verticesBB[5]+verticesBB[8]+verticesBB[11])/4.;
	return center;
}

std::vector<Vector3> Billboard::getVertexArray(){
	std::vector<Vector3> vect;
	return vect;
}


Billboard::Billboard(float *vect,Entity* entite,float ep, Viewer*view,int res):entity(entite),cam(view->getCamera()),epsilon(ep),viewer(view),resMax(res),isLoad(false),error(false), Binfo(false){
  for(int i=0;i<4;i++) plan[i] = vect[i];
	tCoord[0] = 0.;
	tCoord[1] = 1.;
	tCoord[2] = 0.;
	tCoord[3] = 0.;
	tCoord[4] = 1.;
	tCoord[5] = 0.;
	tCoord[6] = 1.;
	tCoord[7] = 1.;
	viewer->setWireframeEnabled(false);
	createBox();
	createBillboard();
	viewer->updateGL();
	generateTexture();
}	


Billboard::Billboard(Entity* entite, Viewer*view): 	 entity(entite),cam(view->getCamera()),viewer(view),error(false), Binfo(false){
	createBox();
}

Billboard::Billboard(const QString& filename,Viewer *v):viewer(v),isLoad(true),error(false), Binfo(false){
	viewer->setWireframeEnabled(false);
	QFile BBFile(filename);
	  if(!BBFile.open(QIODevice::ReadOnly))
  {
    std::cerr << "erreur :chargement Billboard : " << filename.toStdString() << std::endl;
    return;
  }
	QTextStream in(&BBFile);
	QString header;
 	in>>header;
	QString path=(filename.section(".bb",0,0))+".png";
	Image im; 
  im = chargeImage(path.toAscii());
	if (im == NULL)
	{
		error=true;
		return;
	}

	vX = imageGetWidth(im);
	vY = imageGetHeight(im);

	glGenTextures(1, _textureID);
  glBindTexture(GL_TEXTURE_2D, _textureID[0]); 
  gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,vX,vY,GL_RGBA,GL_UNSIGNED_BYTE,imageGetPixels(im));
	glBindTexture(GL_TEXTURE_2D,0);
	int i=0;
	while(i<12){
		verticesBB[i++]=header.toFloat();
		in>>header;
	}
	 i=0;
	while(!(in.atEnd())){
		normals[i++]=header.toFloat();
		in>>header;
	}
	tCoord[0] = 0.;
	tCoord[1] = 1.;
	tCoord[2] = 0.;
	tCoord[3] = 0.;
	tCoord[4] = 1.;
	tCoord[5] = 0.;
	tCoord[6] = 1.;
	tCoord[7] = 1.;
}


Billboard::Billboard(Image im,Viewer *v,float *sommets,float *texCoord,Vector3 norm):viewer(v),normals(norm),isLoad(true),error(false), Binfo(false){
	viewer->setWireframeEnabled(false);
	vX = imageGetWidth(im);
	vY = imageGetHeight(im);
  glBindTexture(GL_TEXTURE_2D, _textureID[0]); 
  gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,vX,vY,GL_RGBA,GL_UNSIGNED_BYTE,imageGetPixels(im));
	glBindTexture(GL_TEXTURE_2D,0);
	for(int i=0;i<12;i++) verticesBB[i] = sommets[i];
	for(int i=0;i<8;i++) tCoord[i]=texCoord[i];
}


void Billboard::drawBillboard(){
	drawGeometry();
}

void Billboard::fillDensity(int coverage, Density *pDensity){}

void Billboard::deleteVBO(){}	

unsigned char * Billboard::getPixels(){
	return pixels;
}

int Billboard::getnX(){
	return vX;
}

int Billboard::getnY(){
	return vY;
}

QString Billboard::saveBillboard(const QString &filename){
	QString body;
  QString str;
	body=body+getCoord();
	Image ima;
	ima= creeImage(pixels,vX,vY,4);
	imageSauvePng(ima,filename.toAscii()+".png");

	return body;
}
QString Billboard::getCoord(){
		QString str,body;
		for(int i=0;i<12;i++) {
		body=body+str.setNum(verticesBB[i])+" ";
		if(i%3==2) body=body+"\n";
	}
	body=body+str.setNum(normals.x)+" ";
	body=body+str.setNum(normals.y)+" ";
	body=body+str.setNum(normals.z)+"\n";
	return body;
}

std::vector<Vector3> Billboard::getBox(){
	return box;
}


/// Tout ce qui suit concerne l'affichage des informations

void Billboard::drawText()
{
	char info[10]; 
	bool Ecolormat = glIsEnabled(GL_COLOR_MATERIAL);
	bool Elight = glIsEnabled(GL_LIGHTING);
	bool Etext = glIsEnabled(GL_TEXTURE_2D); 
	char strMsg[100] = {0};
	int taille;

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
	//actionBoite_Englobante
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
	sprintf ( strMsg, "%s %f sec.", info,time);

	for (unsigned int i=0;i<strlen(strMsg);i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*(strMsg+i));

	glRasterPos3f(position.x,position.y-0.05,position.z);
	sprintf ( strMsg, "1 Billboard");

	for (unsigned int i=0;i<strlen(strMsg);i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*(strMsg+i));


	glPopMatrix();

	if(Elight) glEnable(GL_LIGHTING);
	if (Ecolormat) glEnable(GL_COLOR_MATERIAL);
	if (Etext) glEnable(GL_TEXTURE_2D);
}

void Billboard::afficheInfoM(int split){
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

void Billboard::setMode(int mode, bool info)
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

void Billboard::setTime(double t)
{
	time = t;
}
