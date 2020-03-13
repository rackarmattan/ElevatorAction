#pragma once

//////////////////////////////////////TAKEN FROM THE LABS///////////////////////////////////////////////////

class Vector2D {
public:
	double x;
	double y;

	Vector2D operator+(const Vector2D& rhs)
	{
		return Vector2D(x + rhs.x, y + rhs.y);
	}

	Vector2D operator-(const Vector2D& rhs)
	{
		return Vector2D(x - rhs.x, y - rhs.y);
	}

	Vector2D operator*(const Vector2D& rhs)
	{
		return Vector2D(x * rhs.x, y * rhs.y);
	}

	Vector2D operator/(const double& rhs)
	{
		return Vector2D(x / rhs, y / rhs);
	}

	Vector2D operator*(const double& rhs)
	{
		return Vector2D(x * rhs, y * rhs);
	}

	double dotProduct(const Vector2D& rhs)
	{
		return x * rhs.x + y * rhs.y;
	}

	Vector2D(double x, double y) {
		this->x = x;
		this->y = y;
	}
	Vector2D() {
		this->x = 0;
		this->y = 0;
	}
};