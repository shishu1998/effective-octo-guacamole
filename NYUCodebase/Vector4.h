#pragma once
class Vector4 {
public:
	Vector4();
	Vector4(float x, float y, float z);
	float x;
	float y;
	float z;
	float homogeneous = 1.0;
};