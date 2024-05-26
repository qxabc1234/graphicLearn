#include "Matrix.h"
#include <string>

Matrix Matrix::Translate(float x, float y, float z)
{
	Matrix mat;
	memset(mat.m, 0, sizeof(mat.m));
	mat.m[0][0] = 1.0f;
	mat.m[1][1] = 1.0f;
	mat.m[2][2] = 1.0f;
	mat.m[3][3] = 1.0f;
	mat.m[0][3] = x;
	mat.m[1][3] = y;
	mat.m[2][3] = z;

	return mat;
}

Matrix Matrix::Scale(float x, float y, float z)
{
	Matrix mat;
	memset(mat.m, 0, sizeof(mat.m));
	mat.m[0][0] = x;
	mat.m[1][1] = y;
	mat.m[2][2] = z;
	mat.m[3][3] = 1.0f;
	return mat;
}

Matrix Matrix::Rotate(float degree, float x, float y, float z)
{

	Matrix unitMat = Matrix::Translate(0.0f, 0.0f, 0.0f);
	Matrix k({ 0.0f, -z, y, 0.0f }, { z, 0.0f, x, 0.0f }, { -y, x, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f });
	Matrix squreK = k * k;
	Matrix ksin = k * sin(degree);
	Matrix kcos = squreK * (1.0 - cos(degree));

	Matrix  result = ksin + kcos + unitMat;

	return result;
}

Matrix Matrix::View(Vector4 pos, Vector4 target, Vector4 up)
{
	Matrix translateMat = Matrix::Translate(-pos.x, -pos.y, -pos.z);
	Vector4 forward(target.x - pos.x, target.y - pos.y, target.z - pos.z, 0.0f);
	Vector4 right(forward.Cross(up));

	Matrix  viewMat(right.Normalize(), up.Normalize(), forward.Normalize() * (-1.0f), {0.0f, 0.0f, 0.0f, 1.0f});

	return viewMat * translateMat;
}

Matrix Matrix::Ortho(float l, float r, float t, float b, float n, float f)
{

	Matrix translateMat = Matrix::Translate(-(r + l) / 2, -(t + b) / 2, (n + f) / 2);
	Matrix scaleMat = Matrix::Scale(2/(r - l), 2/(t - b), -2/(n - f));

	return scaleMat * translateMat;

}

Matrix Matrix::Persp(float fov, float n, float f, float ratio)
{
	double top = tan(fov / 2) * n;
	float right = top * ratio;
	Matrix ortho = Matrix::Ortho(-right, right, top, -top, n, f);
	Matrix persptoOrtho = { {-n, 0.0f, 0.0f, 0.0f}, {0.0f, -n, 0.0f, 0.0f}, {0.0f, 0.0f, -(n + f), -n * f}, {0.0f, 0.0f, 1.0f, 0.0f} };
	return ortho * persptoOrtho;
}


Matrix Matrix::operator*(const Matrix& other) const
{
	Matrix result;
	memset(result.m, 0, sizeof(result.m));

	for (int k = 0; k < 4; k++)
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				result.m[k][j] += m[k][i] * other.m[i][j];

	return result;
}

Matrix Matrix::operator+(const Matrix& other) const
{
	Matrix result;
	memset(result.m, 0, sizeof(result.m));

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			result.m[i][j] = m[i][j] + other.m[i][j];

	return result;
}

Matrix Matrix::operator *(float x) const
{
	Matrix result;
	memset(result.m, 0, sizeof(result.m));

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			result.m[i][j] = m[i][j] * x;

	return result;
}

Vector4 Matrix::operator*(const Vector4& v) const
{

	return Vector4(m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
		m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
		m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
		m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
		);
}