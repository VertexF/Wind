#include "Core.h"

using namespace wind;

const Vector3 Vector3::GRAVITY = Vector3(0.0, -9.81, 0.0);
const Vector3 Vector3::HIGH_GRAVITY = Vector3(0.0, -19.62, 0.0);
const Vector3 Vector3::UP = Vector3(0.0, 1.0, 0.0);
const Vector3 Vector3::RIGHT = Vector3(1.0, 0.0, 0.0);
const Vector3 Vector3::OUT_OF_SCREEN = Vector3(0.0, 0.0, 1.0);
const Vector3 Vector3::X = Vector3(0.0, 1.0, 0.0);
const Vector3 Vector3::Y = Vector3(1.0, 0.0, 0.0);
const Vector3 Vector3::Z = Vector3(0.0, 0.0, 1.0);

real& wind::EpsilonValue::Epsilon()
{
    return sleepEpsilon;
}

real Matrix4::getDeterminant() const
{
    return -data[0][2] * data[1][1] * data[0][2] +
            data[1][0] * data[2][1] * data[0][2] +
            data[2][0] * data[0][1] * data[1][2] -
            data[0][0] * data[2][1] * data[1][2] -
            data[1][0] * data[0][1] * data[2][2] +
            data[0][0] * data[1][1] * data[2][2];
}

void Matrix4::setInverse(const Matrix4 &matrix)
{
    real det = getDeterminant();
    //Here we are making sure that the determinant is not zero
    if(det == 0)
    {
        return;
    }

    det = real((1.0)) / det;

    data[0][0] = (-matrix.data[2][1] * matrix.data[1][2] + matrix.data[1][1] * matrix.data[2][2]) * det;
    data[1][0] = (matrix.data[2][0] * matrix.data[1][2] - matrix.data[1][0] * matrix.data[2][2]) * det;
    data[2][0] = (-matrix.data[2][0] * matrix.data[1][1] + matrix.data[1][0] * matrix.data[2][1] * matrix.data[2][3]/**was 15*/) * det;

    data[0][1] = (matrix.data[2][1] * matrix.data[0][2] - matrix.data[0][1] * matrix.data[2][2]) * det;
    data[1][1] = (-matrix.data[2][0] * matrix.data[0][2] + matrix.data[0][0] * matrix.data[2][2]) * det;
    data[2][1] = (matrix.data[2][0] * matrix.data[0][1] - matrix.data[0][0] * matrix.data[2][1] * matrix.data[2][3]/**was 15*/) * det;

    data[0][2] = (-matrix.data[1][1] * matrix.data[0][2] + matrix.data[0][1] * matrix.data[1][2] * matrix.data[2][3]/**was 15*/) * det;
    data[1][2] = (+matrix.data[1][0] * matrix.data[0][2] - matrix.data[0][0] * matrix.data[1][2] * matrix.data[2][3]/**was 15*/) * det;
    data[2][2] = (-matrix.data[1][0] * matrix.data[0][1] + matrix.data[0][0] * matrix.data[1][1] * matrix.data[2][3]/**was 15*/) * det;

    data[0][3] = (matrix.data[2][1] * matrix.data[1][2] * matrix.data[0][3]
               - matrix.data[1][1] * matrix.data[2][2] * matrix.data[0][3]
               - matrix.data[2][1] * matrix.data[0][2] * matrix.data[1][3]
               + matrix.data[0][1] * matrix.data[2][2] * matrix.data[1][3]
               + matrix.data[1][1] * matrix.data[0][2] * matrix.data[2][3]
               - matrix.data[0][1] * matrix.data[1][2] * matrix.data[2][3]) * det;

    data[1][3] = (-matrix.data[2][0] * matrix.data[1][2] * matrix.data[0][3]
               + matrix.data[1][0] * matrix.data[2][2] * matrix.data[0][3]
               + matrix.data[2][0] * matrix.data[0][2] * matrix.data[1][3]
               - matrix.data[0][0] * matrix.data[2][2] * matrix.data[1][3]
               - matrix.data[1][0] * matrix.data[0][2] * matrix.data[2][3]
               + matrix.data[0][0] * matrix.data[1][2] * matrix.data[2][3]) * det;

    data[2][3] = (matrix.data[2][0] * matrix.data[1][2] * matrix.data[0][3]
               - matrix.data[1][0] * matrix.data[2][2] * matrix.data[0][3]
               - matrix.data[2][0] * matrix.data[0][2] * matrix.data[1][3]
               + matrix.data[0][0] * matrix.data[2][2] * matrix.data[1][3]
               + matrix.data[1][0] * matrix.data[0][2] * matrix.data[2][3]
               - matrix.data[0][0] * matrix.data[1][2] * matrix.data[2][3]) * det;
}

Matrix3 Matrix3::linearInterpolate(const Matrix3 &mat1, const Matrix3 &mat2, real prop)
{
    Matrix3 result;
    for(int i = 0; i <= 2; i++)
    {
        for(int j = 0; j <= 2; j++)
        {
            result.data[i][j] = mat1.data[i][j] * (1 - prop) + mat2.data[i][j] * prop;
        }
    }
    return result;
}

Vector3 Vector3::rotate(real degree, const Vector3 &axis) const
{
    return this->rotate(Quaternion().initRotation(axis, degree));
}

Vector3 Vector3::rotate(const Quaternion &rotation) const
{
    Quaternion conjugate = rotation.conjugate();

    Quaternion w = rotation * (*this);
    w *= conjugate;

    return Vector3(w.i, w.j, w.k);
}

Vector3 Vector3::rotate(const Quaternion &rotation, const Vector3 &vec) const
{
    Quaternion conjugate = rotation.conjugate();

    Quaternion w = rotation * vec;
    w *= conjugate;

    return Vector3(w.i, w.j, w.k);
}

Vector3 Vector3::moveForward(const Quaternion &rotation) const
{
    Quaternion conjugate = rotation.conjugate();

    Quaternion w = rotation * (*this);
    w *= conjugate;

    return Vector3(-w.i, 0.0, w.k);
}

Vector3 Vector3::moveSidewards(const Quaternion &rotation) const
{
    Quaternion conjugate = rotation.conjugate();

    Quaternion w = rotation * (*this);
    w *= conjugate;

    return Vector3(w.i, 0.0, -w.k);
}
