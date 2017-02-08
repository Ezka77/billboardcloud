
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#include "Transform.h"

Transform::Transform(void)
    : mPosition(0., 0. ,0.), mScale(1., 1., 1.)
{
}

Transform::Transform(const Matrix4& mat)
    : mScale(1., 1., 1.)
{
    mPosition = Vector3(mat[0][3], mat[1][3], mat[2][3]);
    Matrix3 rot;
    mat.extract3x3Matrix(rot);
    mOrientation.FromRotationMatrix(rot);
}

Transform::Transform(const Transform& t)
{
    mPosition = t.mPosition;
    mScale = t.mScale;
    mOrientation = t.mOrientation;
}

const Transform& Transform::operator = (const Transform t)
{
    mPosition = t.mPosition;
    mScale = t.mScale;
    mOrientation = t.mOrientation;
    return *this;
}
void Transform::setIdentity(void)
{
    mPosition = Vector3::ZERO;
    mScale = Vector3::UNIT_SCALE;
    mOrientation = Quaternion();
}

void Transform::setScale(const Vector3& vScale)
{
    mScale = vScale;
}
void Transform::setScale(float sx, float sy, float sz)
{
    mScale = Vector3(sx, sy, sz);
}
void Transform::setScale(float s)
{
    mScale = Vector3(s, s, s);
}
void Transform::setOrientation(const Quaternion& q)
{
    mOrientation = q;
}
void Transform::setOrientation(float angle, float x, float y, float z)
{
    mOrientation.FromAngleAxis(angle, Vector3(x, y, z));
}
void Transform::setPosition(const Vector3& pos)
{
    mPosition = pos;
}
void Transform::setPosition(float x, float y, float z)
{
    mPosition = Vector3(x, y, z);
}

void Transform::multCurrentMatrix(void) const
{
    glTranslatef(mPosition.x, mPosition.y, mPosition.z);
    float a;
    Vector3 axis;
    mOrientation.ToAngleAxis(a, axis);
    glRotatef(a*180./M_PI, axis.x, axis.y, axis.z);
    glScalef(mScale.x, mScale.y, mScale.z);
}


