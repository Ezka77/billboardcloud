
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#include "Mesh.h"

#define SWAP_NORMALS_NO

Mesh::Mesh(const QString& filename,const QString& name, bool checkVBO, int mode)
    : Entity(name), hasNormals(false), hasTextCoord(false), Binfo(true)
{	
	///Initialisation des variables
	useVBO=(glewIsSupported("GL_ARB_vertex_buffer_object") & checkVBO);

  fName=filename;

	if (mode==0)
		position.x=-1.0;
	else if (mode==1)
		position.x=-0.51;
	else position.x=0.0;
	position.y=0.65;
	position.z=-1.;

	///Initialisation du compteur de chargement
  double debut, fin = clock();
  debut = clock();
  if(filename.contains("off"))
    loadOFF(filename);
  else if(filename.contains("gts"))
    loadGTS(filename);
  else if(filename.contains("obj"))
    loadOBJ(filename);
  else if(filename.contains("3ds"))
    load3DS(filename);
	else if(filename.contains("ply"))
		loadPLY(filename);

  if (!hasNormals)
    computeNormals();
	
  normalize();

	if (useVBO)
		createVBO();

  fin = clock();
  time = (fin - debut) / CLOCKS_PER_SEC;
}

void Mesh::loadPLY(const QString& filename)
{
	QFile plyFile(filename);
  if(!plyFile.open(QIODevice::ReadOnly))
  {
    std::cerr << "erreur :Chargement PLY : " << filename.toStdString() << std::endl;
    return;
  }

	QString mot, header;
  QTextStream in(&plyFile);
	int nofVertices=0, nofFaces=0;
	in >> header;
	if(header != "ply")
  {
    std::cerr << "Wrong header = " << header.toStdString() << std::endl;
    return;
  }
	bool end_header =false;
	bool isAscii;

// Lecture de l'header
	while(!end_header)
	{
		in >> header;
		if(header == "format")
		{
			in >> header;
			std::cout<<header.toStdString()<<std::endl;
			if (header == "ascii")
				isAscii=true;
			else
				isAscii=false;
 		}
		if(header == "vertex")
		{
			 in >> nofVertices;
		}
		if(header == "face")
		{
			 in >> nofFaces;
		}
		if(header == "end_header")
			end_header=true;
	}

	if (nofVertices ==0 | nofFaces==0)
	{
    std::cerr << "erreur : l'en-tête du fichier est incorrecte : Sommets : " <<nofVertices<<", Facettes :"<<nofFaces << std::endl;
    return;
  }
	
	if (isAscii)
	{
		Vector3 vect;
		int nb, id0, id1, id2;
		for(int i=0 ; i<nofVertices ; ++i)
		{
			// on place les coordonnées du sommet dans x, y et z du vecteur v
			in >> vect.x >> vect.y >> vect.z;
			// on place le vecteur v dans mVertices qui contient tout les sommets
			mVertices.push_back(vect);
		}
	
		// Pour toutes les faces du fichier
		for(int i=0 ; i<nofFaces ; ++i)
		{
			// on place les elements de la ligne dans nb, id0,id1,id2
			in >> nb >> id0 >> id1 >> id2;
			// on verrifie que nb = 3
			assert(nb==3);
			// on place les sommets dans mFaces qui contient toute les faces
	#ifdef SWAP_NORMALS
			mFaces.push_back(FaceIndex(id2, id1, id0));
	#else
			mFaces.push_back(FaceIndex(id0, id1, id2));
	#endif
		}
		
		//Pour les coordonnées de textures
		float u,v;
		for(int i=0 ; i<nofVertices ; ++i)
		{
			// on place les coordonnées du sommet dans x, y et z du vecteur v
			in >>u >> v;
			mVertices[i].texcoord[0] = u;
			mVertices[i].texcoord[1] = v;
		}
		plyFile.close();
	}
	else
	{
		std::cout<<"Le format binaire des ply n'est pas pris en compte."<<std::endl;
	}
}




long Mesh::filelength(int f)
{
    struct stat buf;

    fstat(f, &buf);

    return(buf.st_size);
}


void Mesh::load3DS(const QString& filename)
{
	FILE *pFile; // Pointeur de fichier
	
	int i;
	unsigned short l_chunk_id;
	unsigned short l_qty;

	unsigned int l_chunk_lenght;
	
	unsigned char l_char;

	float x,y,z,u,v;
  unsigned short a,b,c;
	int nb_sommets=0,nb_sommets_prov=0;
	unsigned short l_face_flags;
	//Ouverture du fichier & test
	if ((pFile=fopen (filename.toAscii(), "rb"))== NULL)
	{
		std::cerr << "fichier non trouvé !" << std::endl;
		return ;
	}

	while (ftell (pFile) < filelength (fileno (pFile))) //Boucle qui parcours l'ensemble du fichier
	{
			fread (&l_chunk_id, sizeof(unsigned short), 1, pFile); //Lecture de la tête du morceau
			fread (&l_chunk_lenght, sizeof(unsigned int), 1, pFile); //Lecture de la longueur du morceau
			
			switch (l_chunk_id)
      {
					case 0x4d4d: 
					break;
					
					case 0x3d3d: 
					break;
					//Lecture du nom
					case 0x4000: 
						i=0;
						//std::cout<<"Nom:";
						do
						{
							fread (&l_char, sizeof(unsigned char), 1, pFile);
							//if (l_char != '\0') std::cout<<l_char;
							i++;
						}while(l_char != '\0');
						//std::cout<<"!"<<std::endl;
					break;

					case 0x4100: 
					break;
					
					// Lecture des sommets
					case 0x4110:
						fread (&l_qty, sizeof (unsigned short), 1, pFile);
						//mVertices.clear();
						//std::cout<<"Lecture des sommets"<<std::endl;
						//std::cout<<":SOMMETS:"<<std::endl;
						nb_sommets_prov+=l_qty;
						for (i=0; i<l_qty; i++)
            {
							fread (&x, sizeof(float), 1, pFile);
							fread (&y, sizeof(float), 1, pFile);
							fread (&z, sizeof(float), 1, pFile);
							Vector3 position(x,y,z);
      				mVertices.push_back(position);
						}
					break;
					
					// Lecture des faces
					case 0x4120:
						fread (&l_qty, sizeof (unsigned short), 1, pFile);
						//std::cout<<"Lecture des faces"<<std::endl;
						//mFaces.clear();
						//std::cout<<":FACES:"<<std::endl;
						for (i=0; i<l_qty; i++)
						{
							fread (&a, sizeof (unsigned short), 1, pFile);
							fread (&b, sizeof (unsigned short), 1, pFile);
							fread (&c, sizeof (unsigned short), 1, pFile);
							fread (&l_face_flags, sizeof (unsigned short), 1, pFile);
							//std::cout<<"["<<a<<","<<b<<","<<c<<"]";
							mFaces.push_back(FaceIndex(a+nb_sommets,b+nb_sommets,c+nb_sommets));
						}
						//std::cout<<"::"<<std::endl;
						nb_sommets=nb_sommets_prov;
					break;
					
					// Lecture des coordonnées de texture
					case 0x4140:
						fread (&l_qty, sizeof (unsigned short), 1, pFile);
						//std::cout<<"Lecture des coordonnees de texture"<<std::endl;
						for (i=0; i<l_qty; i++)
						{
							fread (&u, sizeof (float), 1, pFile);
							fread (&v, sizeof (float), 1, pFile);
							mVertices[i+nb_sommets].texcoord[0] = u;
        			mVertices[i+nb_sommets].texcoord[1] = v;
						}
					break;
					
					default: 
					fseek(pFile, l_chunk_lenght-6, SEEK_CUR);
			}
	}
	fclose (pFile);
}

void Mesh::loadOBJ(const QString& filename)
{
  // Ouverture du fichier .obj
	QFile objFile(filename);
  if(!objFile.open(QIODevice::ReadOnly))
  {
    std::cerr << "erreur :load OBJ : " << filename.toStdString() << std::endl;
    return;
  }

	//////////////////////////////////////////////
	// Initialisation des variables
  int v=0, vt=0, vn=0, f=0, g=0, id0,id1,id2,id;
	int taille;
  int i,id_face,id_norm=0,id_coordt=0;
  int nb_min=0, nb_max=0;
  Vector3 vertice;
  std::vector<unsigned int> vect_id,vect_norm;
  std::vector<int>vect_coordt; 
	std::vector<Vector3> vertices_prov;
	std::vector<Vector3> normales_prov;
	std::vector<Vector3> indiceNorprov;
	std::vector<Vector3> coordtex_prov;
	std::vector<Vector3> indiceCTprov;
	Vector3 indNorprov, indCTprov;
	QString m,fc, header;
	//////////////////////////////////////////////
	// Les éléments du fichier sont chargé dans la variable 'in'
  QTextStream in(&objFile);
  
 	// Boucle de lecture du fichier : tant qu'on a pas atteind la fin !
  while(!in.atEnd())
  {
    //Lecture du premier caractère stocké dans la variable 'header'
		in >> header;
    /* vertex */
    if(header == "v")
    {
      //lecture des coordonnées du sommet
			in >> vertice.x >> vertice.y >> vertice.z;
			//On stock les coordonnées dans mVertices => au cas ou l'objet n'a pas de normales pré calculé
      mVertices.push_back(vertice);
			//On stock les coordonnées dans vertices_prov => au cas ou l'objet a des normales pré calculé
			vertices_prov.push_back(vertice);
			//On compte le nombre de sommets
      v++;
			//On passe a la ligne suivante
      header = in.readLine();
    }
    /* texture coords. */
    else if(header == "vt")
    {
      in >> vertice.x >> vertice.y >> vertice.z;
			// On place les coordonnées de texture(généralement seulement x et y)
			coordtex_prov.push_back(vertice);
			//On compte le nombre de coordonnées
      vt++;
      hasTextCoord = true;
    }
    /* normal vector */
    else if(header == "vn")
    {
      //idem avec les normales
			in >> vertice.x >> vertice.y >> vertice.z;
      normales_prov.push_back(vertice);
      vn++;
      hasNormals = true;
      header = in.readLine();
    }
    /* face */
    else if(header == "f")
    {
      header = in.readLine();
			//on initialise i à 1 car on ne sait pas combien il y aura de sommet par face
      i =1;
			// on 'découpe' chaque portion contenant vertices/coortex/normale
      m = header.section (' ',i,i);
			// le premier élément du vecteur d'index est à zéro, il contiandra le nombre de sommets sur cette face
      vect_id.push_back(0);
			// lecture de chaque groupe vertices/coortex/normale de la face
      while(!m.isEmpty())
      {
        i++;
				// On recherche di le groupe contien '//' : vertice et normale,(1,1) étant la deuxieme partie (les normales)
        fc = m.section ("//",1,1);
        if (!fc.isEmpty())
        {
          id_norm = fc.toInt();
					//(0,0) est la premiere partie (les vertices)
          fc = m.section ("//",0,0);
          id_face = fc.toInt();
        }
        else
        {
					// Sinon on cherche s'il y a au moins un '/' 
          fc = m.section ("/",1,1);
          if (!fc.isEmpty())
          {
						// s'il y en a un, la premiere partie (1,1) sera les coordonnées de texture
            id_coordt = fc.toInt();
						// la premiere partie (0,0) sera les vertices
            fc = m.section ("/",0,0);
            id_face = fc.toInt();
						// on cherche s'il y a une troisieme partie (2,2) : les normales
            fc = m.section ("/",2,2);
            if (!fc.isEmpty()) id_norm = fc.toInt();
          }
          else
          {
						// s'il n'y a pas de '/' c'est qu'il n'y a que les indices des sommets
            id_face = m.toInt();
          }
        }
				// on enleve 1 aux indices pour que ça coincide avec un tableau en C++
        vect_id.push_back(id_face-1);
        vect_norm.push_back(id_norm-1);
        vect_coordt.push_back(id_coordt-1);
				// puis cherche le prochain groupe vertices/coortex/normale de cette face
        m = header.section (' ',i,i);
      }
      i--;
			// on stock le nombre de sommet dans cette face
      vect_id[0]=i;
			//On parcours tout les sommets de la face en partant du 2ème
			for (int j=2;j<i;j++)
			{
				//le premier sommet est le sommet de référence, on ajoute les 2 suivants pour faire un triangle
				id0=vect_id[1];id1=vect_id[j];id2=vect_id[j+1];	
				mFaces.push_back(FaceIndex(id2, id1, id0));
				//S'il y a des normales, on triangularise ses coordonnées
				if(hasNormals)
				{
					indNorprov.x=vect_norm[j];
					indNorprov.y=vect_norm[j-1];
					indNorprov.z=vect_norm[0];
					indiceNorprov.push_back(indNorprov);
				}
				//S'il y a des coordonnées de texture, on triangularise ses coordonnées
				if(hasTextCoord)
				{
					indCTprov.x=vect_coordt[j];
					indCTprov.y=vect_coordt[j-1];
					indCTprov.z=vect_coordt[0];
					indiceCTprov.push_back(indCTprov);
				}
			}
			// on réinitialise les vector provisoire pour la nouvelle face
      vect_id.clear();
      vect_norm.clear();
      vect_coordt.clear();
			// calcul du nombre min et max de sommets par face
      if( i<nb_min || nb_min==0)
        nb_min=i;
      if (i>nb_max)
        nb_max=i;
			//calcul du nombre de facettes
      f++;
    }
		// groups
    else if(header == "g")
    {
      g++;
      header = in.readLine();
    }
  }
	// Si l'objet a ses propres coordonnées de texture
	taille=mFaces.size();
	if (hasNormals)
	{
		// On efface les vertices déjà entré
		mVertices.clear();
		for (i=0;i<taille;i++)
		{
			for (int j=0;j<3;j++)
			{
				// comme on a triangularisé, on a besoin de réorganisé les faces et les sommets
				// ça servira pour les coordonnées de textures
				mVertices.push_back(vertices_prov[mFaces[i].indexes[j]]);
				mFaces[i].indexes[j]=i*3+j;
				// On rajoute les normales correspondant aux nouveau sommets
				id=(int)indiceNorprov[i][j];
				mVertices[i*3+j].normal= normales_prov[id];
			}
		}
	}
	else
	{
		// sinon on va calculer les normales avec la topologie initiale
		computeNormals();
		hasNormals = true;
	}
	// On réinitialise ensuite les vertices et les normales provisoire
	vertices_prov.clear();
	normales_prov.clear();
	taille=mVertices.size();
	// Pour tout les sommets, on stock provisoirement leur valeur et leur normale
	for (i=0;i<taille;i++)
	{
		vertices_prov.push_back(mVertices[i].position);	
		normales_prov.push_back(mVertices[i].normal);
	}
	// On réinitialise ensuite mVertice => Ça permet de mettre au même niveau les
	// vertices avec les normales calculé, et celles avec les normales recopiés du fichier
	mVertices.clear();
	taille=mFaces.size();
	//Pour toute les faces
	for (i=0;i<taille;i++)
	{
		// Pour chaque sommets de la face(il n'y maitnenant plus que 3 sommets par face)
		for (int j=0;j<3;j++)
		{
			// on réinjecte les bons sommets avec leur normales
			mVertices.push_back(Vertex(vertices_prov[mFaces[i].indexes[j]], normales_prov[mFaces[i].indexes[j]]));
			// s'il y a des coordonnées de texture, on les rajoute ici
			if (hasTextCoord)
			{
				// on récupere l'indice de la coordonnée de texture correspond au sommet j de la face i
				id=(int)indiceCTprov[i][j];
				// on ajoute u et v à la structure
				mVertices[i*3+j].texcoord[0] = coordtex_prov[id][0];
				mVertices[i*3+j].texcoord[1] = coordtex_prov[id][1];
			}
			// le tableau d'index des sommets est modifié
			mFaces[i].indexes[j]=i*3+j;
		}
	}
	// fin du fichier
  objFile.close();
}

void Mesh::loadOFF(const QString& filename)
{
  //Charge le fichier $filename
  QFile offFile(filename);

  //Ouvre le fichier => erreur si non trouve
  if(!offFile.open(QIODevice::ReadOnly))
  {
    std::cerr << "erreur : load OFF : " << filename.toStdString() << std::endl;
    return;
  }
  //Lecture du fichier dans la variable "in"
  QTextStream in(&offFile);

  //Declare une variable de type String : "header"
  QString header;

  //met le premier mot de "in" dans "header"
  in >> header;

  // le premier mot doit etre "OFF"
  if(header != "OFF")
  {
    std::cerr << "Wrong header = " << header.toStdString() << std::endl;
    return;
  }

  //Nombre de Vertices, Nombre de Face et null
  int nofVertices, nofFaces, inull;
  // nombre de sommet par face, sommet1, sommet2 , sommet3
  int nb, id0, id1, id2;
  //Vecteur size 3
  Vector3 v;
  //Place le nombre de vertices totale et de faces totale contenu dans la deuxieme lignes, le dernier est 0
  in >> nofVertices >> nofFaces >> inull;

  // Pour tous les sommets du fichier
  for(int i=0 ; i<nofVertices ; ++i)
  {
    // on place les coordonnées du sommet dans x, y et z du vecteur v
    in >> v.x >> v.y >> v.z;
    // on place le vecteur v dans mVertices qui contient tout les sommets
    mVertices.push_back(v);
  }

  // Pour toutes les faces du fichier
  for(int i=0 ; i<nofFaces ; ++i)
  {
    // on place les elements de la ligne dans nb, id0,id1,id2
    in >> nb >> id0 >> id1 >> id2;
    // on verrifie que nb = 3
    assert(nb==3);
    // on place les sommets dans mFaces qui contient toute les faces
#ifdef SWAP_NORMALS
    mFaces.push_back(FaceIndex(id2, id1, id0));
#else
    mFaces.push_back(FaceIndex(id0, id1, id2));
#endif

  }
  // on ferme le fichier
  offFile.close();
}

void Mesh::loadGTS(const QString& filename)
{
  QFile gtsFile(filename);
  std::vector< std::pair<int,int> > lEdges;

  if(!gtsFile.open(QIODevice::ReadOnly))
  {
    std::cerr << "erreur : load GTS : " << filename.toStdString() << std::endl;
    return;
  }

  QTextStream in(&gtsFile);
  QString header;

  int nofVertices, nofEdges, nofFaces;
  int id0, id1, id2;
  Vector3 v;

  in >> nofVertices >> nofEdges >> nofFaces;

  for(int i=0 ; i<nofVertices ; ++i)
  {
    in >> v.x >> v.y >> v.z;
    mVertices.push_back(v);
  }

  for(int i=0 ; i<nofEdges ; ++i)
  {
    in >> id0 >> id1;
    lEdges.push_back( std::pair<int,int>(id0-1,id1-1) );

  }

  for(int i=0 ; i<nofFaces ; ++i)
  {
    in  >> id0 >> id1 >> id2;
#ifdef SWAP_NORMALS
    mFaces.push_back(FaceIndex(lEdges[id2-1].first, lEdges[id1-1].first, lEdges[id0-1].first));
#else
    mFaces.push_back(FaceIndex(lEdges[id0-1].first, lEdges[id1-1].first, lEdges[id2-1].first));
#endif
  }
  gtsFile.close();
}

void Mesh::computeNormals(void)
{
  // compute normals
  Vector3 f_normal;
  Vector3 p0, p1, p2,p3;

	for(FaceIndexArray::iterator f_iter = mFaces.begin() ; f_iter!=mFaces.end() ; ++f_iter)
	{
		p0 = mVertices[f_iter->indexes[2]].position;
		p1 = mVertices[f_iter->indexes[1]].position;
		p2 = mVertices[f_iter->indexes[0]].position;
		f_normal = (p1-p0).crossProduct(p2-p0);
		f_normal.normalise();
		mVertices[f_iter->indexes[2]].normal += f_normal;
		mVertices[f_iter->indexes[1]].normal += f_normal;
		mVertices[f_iter->indexes[0]].normal += f_normal;
	}

  for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
  {
    v_iter->normal.normalise();
  }
}

void Mesh::normalize(void)
{
  Vector3 min(1e99, 1e99, 1e99);
  Vector3 max(-1e99, -1e99, -1e99);

  for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
  {
    min.makeFloor(v_iter->position);
    max.makeCeil(v_iter->position);
  }

  Vector3 diff = max - min;
  float scale = diff.x > diff.y ? (diff.x > diff.z ? diff.x : diff.y) : (diff.y>diff.z ? diff.y : diff.z);
  scale = 1./scale;
  for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
  {
    v_iter->position -= min;
    v_iter->position *= scale;
  }
}

Mesh::~Mesh()
{
	if (useVBO)
		deleteVBO();
}

void Mesh::deleteVBO(void)
{
	glDeleteBuffers(1, &m_indexBuffer);
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_normalBuffer);
	glDeleteBuffers(1, &m_texCoordBuffer);
}

void Mesh::createVBO(void)
{	
	// Calcul de la taille qu'occupera le tableau d'index 
	GLsizeiptr IndexSize = mFaces.size() *3 * sizeof(GLuint);
	// Calcul de la taille qu'occupera le tableau de sommets (et de normales) 
	GLsizeiptr PositionSize = mVertices.size() * 3 * sizeof(GLfloat);
	// Calcul de la taille qu'occupera le tableau de coordonnées de texture
	GLsizeiptr CTSize = mVertices.size() * 2 * sizeof(GLfloat);
	// Nombre d'élément dans le tableau de sommets
	int taille = mVertices.size();
	// Création et allocation du tableau de sommets qui sera envoyé à la carte graphique
	float *PositionData;
	PositionData = new float [taille*3];
	// Création et allocation du tableau de normales qui sera envoyé à la carte graphique
	float *NormalData;
	NormalData = new float [taille*3];
	// Création et allocation du tableau de coordonnées de textures qui sera envoyé à la carte graphique
	float *CTData;
	CTData = new float [taille*2];
	// Chargement des valeurs dans les tableaux qu'on vien de créer
	for (int i=0;i<taille;i++)
	{
		PositionData[i*3]=mVertices[i].position.data()[0];
		PositionData[i*3+1]=mVertices[i].position.data()[1];
		PositionData[i*3+2]=mVertices[i].position.data()[2];

 		NormalData[i*3]=mVertices[i].normal.data()[0];
 		NormalData[i*3+1]=mVertices[i].normal.data()[1];
 		NormalData[i*3+2]=mVertices[i].normal.data()[2];

		CTData[i*2]=mVertices[i].texcoord[0];
		CTData[i*2+1]=mVertices[i].texcoord[1];	
	}
		
	// Création de l'objet qui contiendra l'adresse du tableau d'index 
	glGenBuffers(1, &m_indexBuffer);
	// On lie cet objet avec le paramètre GL_ELEMENT_ARRAY_BUFFER pour dire que c'est un tableau d'index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	//Maintenant on désigne le tableau d'index, sa taille en octet et son mode de chargement
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexSize, mFaces[0].indexes, GL_STATIC_DRAW);
	// on délie le buffer pour pouvoir l'utiliser ensuite
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	//Meme chose avec les tableaux de sommets, de normales et de coordonnées de texture
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, PositionSize, PositionData, GL_STATIC_DRAW);

 	glGenBuffers(1, &m_normalBuffer);
 	glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
 	glBufferData(GL_ARRAY_BUFFER,	PositionSize,	NormalData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, CTSize, CTData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Suppression des tableaux provisoire, ils sont maintenant dans la mémoire du GPU, inutile de les conserver
	delete [] PositionData;
	delete [] NormalData;
	delete [] CTData;
}

void Mesh::drawGeometry(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	if(useVBO)
	{
		// Nombre d'index : 3 index par face
		GLsizei VertexCount = mFaces.size()*3;
		// On lie le VBO des sommets, la valeur NULL signifie qu'on appelera l'affichage des VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
	
		glBindBuffer(GL_ARRAY_BUFFER, m_normalBuffer);
		glNormalPointer(GL_FLOAT, 0, NULL);
		

		glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
		glTexCoordPointer( 2, GL_FLOAT, 0, NULL );

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

		glDrawElements(GL_TRIANGLES, VertexCount, GL_UNSIGNED_INT, 0);
	}
	else
	{
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), mVertices[0].position.data());
		glNormalPointer(GL_FLOAT, sizeof(Vertex), mVertices[0].normal.data());
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), mVertices[0].texcoord);
		glDrawElements(GL_TRIANGLES, 3*mFaces.size(), GL_UNSIGNED_INT, mFaces[0].indexes);
	}
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (Binfo) drawText();
}

Vector3 Mesh::getCenterM(void)
{
  Vector3 center;
  float	tmp_X=0,tmp_Y=0,tmp_Z=0;
  int taille = mVertices.size();

  for (int i=0; i< taille;i++)
  {
    tmp_X += mVertices[i].position.data()[0];
    tmp_Y += mVertices[i].position.data()[1];
    tmp_Z += mVertices[i].position.data()[2];
  }

  tmp_X = tmp_X/taille;
  tmp_Y = tmp_Y/taille;
  tmp_Z = tmp_Z/taille;

  center.x = tmp_X;
  center.y = tmp_Y;
  center.z = tmp_Z;
  return center;
}

std::vector<Vector3> Mesh::getVertexArray(){
	std::vector<Vector3> vect;
	int taille= mVertices.size();
	for(int i=0;i<taille;i++) vect.push_back(mVertices[i].position.data()); 
	return vect;
}

void Mesh::drawText()
{
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
	glColor3f(1.,1.,0.2);
	glRasterPos3f(position.x,position.y,position.z);
	sprintf ( strMsg, "Chargement : %f sec.", time);

	for (unsigned int i=0;i<strlen(strMsg);i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*(strMsg+i));

	taille=mVertices.size();
	glRasterPos3f(position.x,position.y-0.05,position.z);
	sprintf ( strMsg, "%d Sommets.", taille);
	for (unsigned int i=0;i<strlen(strMsg);i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*(strMsg+i));

	glRasterPos3f(position.x,position.y-0.1,position.z);
	taille = mFaces.size();

	sprintf ( strMsg, "%d Facettes.", taille);
	for (unsigned int i=0;i<strlen(strMsg);i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*(strMsg+i));

	
	if(useVBO)
		sprintf ( strMsg, "Utilisation VBOs");
	else
		sprintf ( strMsg, "pas d'utilisation des VBOs");

	glRasterPos3f(position.x,position.y-0.15,position.z);
	for (unsigned int i=0;i<strlen(strMsg);i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,*(strMsg+i));

	glPopMatrix();
	if(Elight) glEnable(GL_LIGHTING);
	if (Ecolormat) glEnable(GL_COLOR_MATERIAL);
	if (Etext) glEnable(GL_TEXTURE_2D);
}


void Mesh::afficheInfoM(int mode){
 	if (mode==1)
			position.x=-0.51;
	else if (mode==2)
			position.x=-1.0;
	else if (mode==3)
			position.x=0.0;
 	else if (mode==4)
		Binfo=false;
	else
		Binfo=!Binfo;
}
void Mesh::fillDensity(int coverage, Density *pDensity)
{
	
  // initialisation
	Vector3 center=getCenter();
	Vector3 densityCoord;
	Vector3 startPosition;
	Vector3 vectorStartTo1;
	Vector3 vectorStartTo2;
	Vector3 normalVect;
	float maxd = 1.;
	float areaMax=0;
	unsigned int *index;
  
// On récupère le nombre de face de notre objet.
	int NbFace = mFaces.size();

  //On effectuera deux passes pour calculer notre matrice
	for (int nbPass=1;nbPass<=2;nbPass++)
	{
		for (int currentFace=0; currentFace<NbFace;currentFace++)
		{
			index            = mFaces.at(currentFace).indexes;
			startPosition    = mVertices.at(index[0]).position;
			vectorStartTo1   = startPosition - mVertices.at(index[1]).position;
			vectorStartTo2   = startPosition - mVertices.at(index[2]).position;

			float cosFace = fabsf( vectorStartTo1.dotProduct(vectorStartTo2) / sqrt(vectorStartTo1.squaredLength()*vectorStartTo2.squaredLength()));
			float hauteurFace=sqrt(vectorStartTo1.squaredLength()-cosFace*cosFace*vectorStartTo2.squaredLength());
			float area=vectorStartTo1.length()*hauteurFace/2;


			normalVect       = vectorStartTo2.crossProduct( vectorStartTo1);
			normalVect.normalise();
			Vector3 vectorStartToO = startPosition - center;
			float cosZ1=normalVect.dotProduct(vectorStartToO) / sqrt(normalVect.squaredLength() * vectorStartToO.squaredLength());
      
			float cosZ2=(-normalVect).dotProduct(vectorStartToO) / sqrt((-normalVect).squaredLength() * vectorStartToO.squaredLength());

			if(cosZ1>cosZ2) normalVect=-normalVect;




			float distance   = fabsf(normalVect.x*startPosition.x+normalVect.y*startPosition.y+normalVect.z*startPosition.z);
      
			if(nbPass==1)
			{
				if(maxd<distance)
					maxd         = distance;
				if(areaMax<area)
					areaMax      = area;
			}
			else{
	
				float ro       = acos(normalVect.z);
				float phi      = atan2f(normalVect.y,normalVect.x);
				int dim        = pDensity->getRow();
        //coordonnés de notre plan dans la matrice densité 
				densityCoord.x = (int)floor((ro)*(dim-1)/(3.1416));
				densityCoord.y = (int)floor((phi+3.1416)*(dim-1)/(3.1416*2));
				densityCoord.z = (int)floor((distance)*(dim-1)/(maxd));
				for(int covOfX = -coverage; covOfX <= coverage; covOfX++)
				{
					for(int covOfY = -coverage; covOfY <= coverage; covOfY++)
					{
						for(int covOfZ = -coverage; covOfZ <= coverage; covOfZ++)
						{
							if( densityCoord.x+covOfX<dim && densityCoord.y+covOfY<dim && densityCoord.z+covOfZ<dim
												 && densityCoord.x+covOfX>=0 && densityCoord.y+covOfY>=0 && densityCoord.z+covOfZ>=0)
							{

								int neighbourDist=abs(covOfX);
								if(neighbourDist<abs(covOfY))
									neighbourDist=abs(covOfY);
								if(neighbourDist<abs(covOfZ))
									neighbourDist=abs(covOfZ);
								pDensity->fill( (int) densityCoord.x+covOfX, (int) densityCoord.y+covOfY, 
																 (int) densityCoord.z+covOfZ, area*pow(0.8,neighbourDist));
							}
						}
					}
				}
			}
		}
	}
	pDensity->setDistanceMax( maxd);
}

