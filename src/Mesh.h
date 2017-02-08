#ifndef _Mesh_h_
#define _Mesh_h_

#include <GL/glew.h>

#include <GL/glut.h>
#include "OpenGL.h"

#include <QFile>
#include <QTextStream>
#include <QString>
#include <sys/stat.h>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <assert.h>
#include <time.h>


#include "Color.h"
#include "Entity.h"
#include "Density.h"


/** Classe representant un objet sous la forme d'un maillage.
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

class Mesh : public Entity
{
public:

/** @name Mesh
	*	@brief Constructeur : Charge un objet 3ds,off ou obj a partir du fichier filename
	*	@param filename nom du fichier a charger
	*/
	Mesh(const QString& filename, const QString& name = "", bool checkVBO=true, int mode=0);

	virtual ~Mesh();

/** @name  load3DS
	* @brief Charge un objet de type .3ds 
	* @param filename : chemin d'accès de l'objet
*/
	void load3DS(const QString& filename);

/** @name  loadOFF
	*	@brief Charge un objet de type .off
	* @param filename : chemin d'accès de l'objet
*/
	void loadOFF(const QString& filename);

/** @name  loadGTS
	*	@brief Charge un objet de type .gts
	* @param filename : chemin d'accès de l'objet
*/
	void loadGTS(const QString& filename);

/** @name  loadOBJ
	*	@brief Charge un objet de type .obj
	* @param filename : chemin d'accès de l'objet
*/
	void loadOBJ(const QString& filename);

/** @name  loadPLY
	*	@brief Charge un objet de type .ply
	* @param filename : chemin d'accès de l'objet
*/
	void loadPLY(const QString& filename);

	virtual void deleteVBO(void);

/** @name afficheInfo
  * @brief Active/Désactive l'affichage des informations à l'écran, ou place l'affichage
 	* @param mode : Selection 
 	* @p 1 : place l'information en x=-0.51
 	* @p 2 : place l'information en x=-1.05	
 	* @p 3 : place l'information en x=0.0
 	* @p 4 : Désactive l'affichage
 	* @p autre : Active/Désactive l'affichage
 	*/
	virtual void afficheInfoM(int mode);

/** @name fillDensity
  * @brief Permet de remplir la Densité en fonction des normales de l'objet.
	* @param coverage : indice de couverture
 	* @param pDensity : matrice de densité
 	*/
	virtual void fillDensity(int coverage, Density *pDensity);
protected:

	/** Methode chargee du trace de la geometrie de l'objet.
			Le trace doit utiliser les vertex array
	*/
	virtual void drawGeometry(void);
	virtual std::vector<Vector3> getVertexArray();

/** @name getCenterM
	* @brief retourne dans un vector, le centre de gravité de l'objet 
	* @return un Vector3 contenant la position X,Y,Z du centre de gravité
	*/

	virtual Vector3 getCenterM(void);
	long filelength(int f);


protected:
/** @name: computeNormals
	* @brief Calcul les normales
	* @param filename: chemin d'accès de l'objet
*/
	void computeNormals(void);
	void normalize(void);
	void prepareForShadowVolume(void);

	/** Represente un sommet du maillage
	*/
	struct Vertex
	{
			Vertex(void) : position(Vector3::ZERO), normal(Vector3::ZERO) {};
			Vertex(const Vector3& pos)
					: position(pos), normal(Vector3::ZERO)
			{
					positionData[0]=pos.x;
					positionData[1]=pos.y;
					positionData[2]=pos.z;
					texcoord[0] = 0.;
					texcoord[1] = 0.;
			}
			Vertex(const Vector3& pos, const Vector3& nor)
			{
				position=pos;
				normal=nor;
			}
			Vector3 position;
			Vector3 normal;
			float positionData[3];
			float texcoord[2];
	};


	/** Represente un triangle via une liste de 3 indices.
	*/
	struct FaceIndex
	{
			FaceIndex() {};
			FaceIndex(int i0, int i1, int i2)
			{
					indexes[0] = i0;
					indexes[1] = i1;
					indexes[2] = i2;
			};
			FaceIndex(const int* pIds)
			{
					indexes[0] = pIds[0];
					indexes[1] = pIds[1];
					indexes[2] = pIds[2];
			};
			unsigned int indexes[3];
			inline unsigned int& operator[] (int k)
			{
				assert(k<3);
				return indexes[k];
			}
			inline unsigned int operator[] (int k) const
			{
				assert(k<3);
				return indexes[k];
			}
	};
	/** Represente une liste de sommets
	*/
	typedef std::vector<Vertex> VertexArray;

	/** Represente une liste de triangles
	*/
	typedef std::vector<FaceIndex> FaceIndexArray;

	/** pour l'instant tous les sous-objets sont stockes sous la forme d'un seul gros objet
	*/
	VertexArray mVertices;
	FaceIndexArray mFaces;
	
	bool hasNormals;
	bool hasTextCoord;
	bool useVBO;

	//VBO handlers
	GLuint m_indexBuffer;
	GLuint m_vertexBuffer;
	GLuint m_normalBuffer;
	GLuint m_texCoordBuffer;

private:
	QString fName;
	double time;
	void drawText();
	void createVBO(void);
	bool Binfo;
	Vector3 position;
};
#endif
