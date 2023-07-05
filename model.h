#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
	std::vector<Vec2f> texts_;
	std::vector<std::vector<int>> face_texts_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int ntext();
	int nfaces();
	Vec3f vert(int i);
	Vec2f text(int i);//texture coordinate
	std::vector<int> face(int idx);
	std::vector<int> face_text(int idx);
};

#endif //__MODEL_H__