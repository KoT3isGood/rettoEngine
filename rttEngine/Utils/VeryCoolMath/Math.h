#pragma once
#include "glm\glm.hpp"
using namespace glm;

inline mat3 getMatrixFromRotation(float pitch, float yaw, float roll) {
	float sp = glm::sin(pitch);
	float cp = glm::cos(pitch);
	float sy = glm::sin(roll);
	float cy = glm::cos(roll);
	float sr = glm::sin(yaw);
	float cr = glm::cos(yaw);

	mat3 matrix =
		mat3{
		1,0,0,
		0,cy,-sy,
		0,sy,cy
	}*mat3{
			cp,0,sp,
			0,1,0,
			-sp,0,cp
	}*mat3{
		cr,-sr,0,
		sr,cr,0,
		0,0,1
	};
	return matrix;
}

inline mat2 getMatrixFromRotation(float pitch) {
	float sp = glm::sin(pitch);
	float cp = glm::cos(pitch);

	mat2 matrix =
		mat2{
		cp,-sp,sp,cp
	};
	return matrix;
}