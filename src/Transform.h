
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _Transform_h_
#define _Transform_h_

#include <GL/glew.h>
#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix3.h"
#include "Matrix4.h"

class Transform
{
public:

    Transform(void);

    Transform(const Matrix4& mat);
    
    Transform(const Transform& t);
    
    const Transform& operator = (const Transform t);
    
    void setIdentity(void);

    void setScale(const Vector3& vScale);
    void setScale(float sx, float sy, float sz);
    void setScale(float s);
    inline const Vector3& getScale(void) const
    {
        return mScale;
    }

    void setOrientation(const Quaternion& q);
    void setOrientation(float angle, float x, float y, float z);
    inline const Quaternion& getOrientation(void) const
    {
        return mOrientation;
    }

    void setPosition(const Vector3& pos);
    void setPosition(float x, float y, float z);
    inline const Vector3& getPosition(void) const
    {
        return mPosition;
    }

    Matrix4 getMatrix(void) const
    {
        Matrix4 mat(Matrix4::IDENTITY);
        Matrix3 rotMat;
        mOrientation.ToRotationMatrix(rotMat);
        mat = rotMat;
        mat[0][0] *= mScale.x;
        mat[0][1] *= mScale.x;
        mat[0][2] *= mScale.x;
        mat[1][0] *= mScale.y;
        mat[1][1] *= mScale.y;
        mat[1][2] *= mScale.y;
        mat[2][0] *= mScale.z;
        mat[2][1] *= mScale.z;
        mat[2][2] *= mScale.z;
        mat.setTrans(mPosition);
        return mat;
    }
    
    Matrix4 getInverseMatrix(void) const
    {
        Matrix4 mat(Matrix4::IDENTITY);
        Matrix3 rotMat;
        mOrientation.ToRotationMatrix(rotMat);
        mat = rotMat;
        mat[0][0] *= mScale.x;
        mat[0][1] *= mScale.x;
        mat[0][2] *= mScale.x;
        mat[1][0] *= mScale.y;
        mat[1][1] *= mScale.y;
        mat[1][2] *= mScale.y;
        mat[2][0] *= mScale.z;
        mat[2][1] *= mScale.z;
        mat[2][2] *= mScale.z;
        mat.setTrans(mPosition);
        return mat.inverse();
    }
    inline Vector3 toLocalSpace(const Vector3& v) const
    {
        return getInverseMatrix() * v;
    }

    void multCurrentMatrix(void) const;

protected:
    Vector3 mPosition;
    Quaternion mOrientation;
    Vector3 mScale;
};

#endif
