#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            std::vector<int> t;
            int itrash, face_idx;
            int text_idx;
            iss >> trash;
            while (iss >> face_idx >> trash >> text_idx >> trash >> itrash) {
                face_idx--; // in wavefront obj all indices start at 1, not zero
                text_idx--;
                t.push_back(text_idx);
                f.push_back(face_idx);
            }
            faces_.push_back(f);
        } else if (!line.compare(0,3,"vt ")) {
            Vec2f v;
            float ftrash;
            for (int i = 0; i < 2; ++i)     iss >> v.raw[i];
            iss >> ftrash;
            texts_.push_back(v);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() 
    << "# vt#" << texts_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

int Model::ntext() {
    return (int)texts_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

std::vector<int> Model::face_text(int idx) {
    return face_texts_[idx];
}


Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec2f Model::text(int i) { //text[i]:texture coordinate
    return texts_[i];
}