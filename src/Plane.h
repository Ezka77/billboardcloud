
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/
 
#ifndef _Plane_h_
#define _Plane_h_

#include "Entity.h"

/** Repr�sente un plan unitaire dans le plan xy et centr� en (0.5, 0.5, 0).
    Le plan est en faite trac� sous la forme d'une grille.
*/
class Plane : public Entity
{
public:
    /** Consructeur.
        \param nbQuadX nombre de subdivision selon l'axe des X
        \param nbQuadY nombre de subdivision selon l'axe des Y
    */
    Plane(int nbQuadX, int nbQuadY, const QString& name = "");

    Plane(const QString& name = "");

    void getEquation(double eq[4]) const;

protected:

    void initialiaze(int nbQuadX, int nbQuadY);

    /** Trac� de la grille (du plan).
        Le trac� utilise les vertex array.
        \note � compl�ter par l'�tudiant (texture)
    */
    virtual void drawGeometry(void);

    Vector3 *mVertices;
    unsigned int *mIndexes;
    int mSizeX, mSizeY;

};

#endif
