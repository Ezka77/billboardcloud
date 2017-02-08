
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/
 
#include "Plane.h"
#include "Material.h"

//#include "OpenGL.h"

Plane::Plane(int gridSizeX, int gridSizeY,const QString& name)
    : Entity(name)
{
    initialiaze(gridSizeX,gridSizeY);
}
Plane::Plane(const QString& name)
    : Entity(name)
{
    initialiaze(1,1);
}
void Plane::initialiaze(int gridSizeX, int gridSizeY)
{
    mSizeX = gridSizeX;
    mSizeY = gridSizeY;
    gridSizeX++;
    gridSizeY++;
    mVertices = new Vector3[gridSizeX*gridSizeY];
    mIndexes = new GLuint[(gridSizeX-1)*(gridSizeY-1)*4]; // 1 face = 1 quad = 4 sommets par face !

    // compute vertex
    for(int j=0 ; j<gridSizeY ; ++j)
    {
        float y = (float)j/(float)(gridSizeY-1);
        //y = y*2. - 1.;
        for(int i=0 ; i<gridSizeX ; ++i)
        {
            float x = (float)i/(float)(gridSizeX-1);
            //x = x*2. -1.;
            mVertices[j*gridSizeX+i] = Vector3(x, y, 0.);
        }
    }
    // compute indexes
    for(int j=0 ; j<(gridSizeY-1) ; ++j)
    {
        for(int i=0 ; i<(gridSizeX-1) ; ++i)
        {
            mIndexes[(j*(gridSizeX-1) + i)*4 + 0] = j*gridSizeX + i;
            mIndexes[(j*(gridSizeX-1) + i)*4 + 1] = j*gridSizeX + i + 1;
            mIndexes[(j*(gridSizeX-1) + i)*4 + 2] = (j+1)*gridSizeX + i + 1;
            mIndexes[(j*(gridSizeX-1) + i)*4 + 3] = (j+1)*gridSizeX + i ;
        }
    }
}
//--------------------------------------------------------------------------------
void Plane::drawGeometry(void)
{
    /*******************************************************************/
    /* Début code à faire/completer par l'étudiant                     */
    /* Texture                                                         */
    /*******************************************************************/

    // à faire : spécifier les coordonnées de texture en multi-texturing
    
    glVertexPointer(3, GL_FLOAT, 0, mVertices);
    glEnableClientState(GL_VERTEX_ARRAY);

    for(int i=0 ; i<mpMaterial->getNumberOfLayers() ; ++i)
    {
        glClientActiveTexture(GL_TEXTURE0+i);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vector3), mVertices);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    // set the normales for all vertex
    glNormal3f(0., 0., 1.);
    glDrawElements(GL_QUADS, mSizeX*mSizeY*4, GL_UNSIGNED_INT, mIndexes);
    glDisableClientState(GL_VERTEX_ARRAY);

    for(int i=0 ; i<mpMaterial->getNumberOfLayers() ; ++i)
    {
        glClientActiveTexture(GL_TEXTURE0+i);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    glClientActiveTexture(GL_TEXTURE0);
    /*******************************************************************/
    /* Fin code à faire/completer par l'étudiant                       */
    /* Texture                                                         */
    /*******************************************************************/
}
//--------------------------------------------------------------------------------
void Plane::getEquation(double eq[4]) const
{
    Vector3 n(0., 0. , 1.);
    n = mTransform.getOrientation() * n;
    n.normalise();
    eq[0]=n.x; eq[1]=n.y; eq[2]=n.z;
    eq[3] = -n.dotProduct(mTransform.getPosition());
}

