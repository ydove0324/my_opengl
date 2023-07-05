#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
const TGAColor white = TGAColor{255, 255, 255, 255};
const TGAColor red   = TGAColor{0, 0,   255,   255};
const int WIDTH = 1024;
const int HEIGHT = 1024;
void line(int x0,int y0,int x1,int y1,TGAImage &image, TGAColor color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    bool steep = false;
    if (dx < dy) {
        steep = true;
        std::swap(x0,y0);
        std::swap(x1,y1);
    }
    if (x0 > x1) {
        std::swap(x0,x1);
        std::swap(y0,y1);
    }
    int y = y0;
    int delta = 0;
    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    int error = abs(y1 - y0);
    bool Y_increase = (y1 > y0);
    if (!steep)
        for (int x = x0; x <= x1; ++x) {
            image.set(x,y,color);
            delta += error;
            if (delta >= dx) {
                y += (Y_increase)?1:-1;
                delta -= dx;
            }
        }
    else {
        for (int x = x0; x <= x1; ++x) {
            image.set(y,x,color);
            delta += error;
            if (delta >= dx) {
                y += (Y_increase)?1:-1;
                delta -= dx;
            }        
        }
    }
}
void line(Vec2i p0,Vec2i p1,TGAImage &image,TGAColor color) {
    line(p0.x,p0.y,p1.x,p1.y,image,color);
}
Vec3f barycentric(Vec2i p0,Vec2i* tr) {
    Vec2i PA = tr[0] - p0;
    Vec2i AB = tr[1] - tr[0];
    Vec2i AC = tr[2] - tr[0];
    Vec3f _temp = Vec3f(PA.x,AB.x,AC.x) ^ Vec3f(PA.y,AB.y,AC.y);
    if (_temp.x == 0)   return Vec3f(-1,1,1);
    return Vec3f(1. - (_temp.y + _temp.z) / _temp.x,_temp.y / _temp.x,_temp.z / _temp.x);
}
void triangle (Vec3i*tr,Vec3f*text,float *z_buffer,TGAImage &image,float intensity,
const TGAImage&diffiuse_image) {
    int width = image.width() - 1;
    int height = image.height() - 1;
    Vec2i border_min(width,height);
    Vec2i border_max(0,0);
    for (int i = 0; i < 3; ++i) {
        border_max.x = std::max(border_max.x,tr[i].x);
        border_max.y = std::max(border_max.y,tr[i].y);
        border_min.x = std::min(border_min.x,tr[i].x);
        border_min.y = std::min(border_min.y,tr[i].y);
    }
    border_max.x = std::min(border_max.x,width);
    border_max.y = std::min(border_max.y,height);
    Vec2i D2_tr[3];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            D2_tr[i].raw[j] = tr[i].raw[j];
        }
    }
    auto ID = [=](int x,int y) -> int {
        return x * width + y;
    };
    for (int x = border_min.x; x <= border_max.x; ++x) {
        for (int y = border_min.y; y <= border_max.y; ++y) {
            Vec3f bc_screen = barycentric(Vec2i(x,y),D2_tr);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)  continue;
            float H = 0;
            Vec3f text_coord(0,0,0);
            for (int j = 0; j < 3; ++j) {
                text_coord = text_coord + text[j] * bc_screen.raw[j];
            }
            TGAColor _color = diffiuse_image.get((int)(text_coord.raw[0] * WIDTH),
            (int)(text_coord.raw[1] * HEIGHT));
            _color *= intensity;
            for (int j = 0; j < 3; ++j)     H += bc_screen.raw[j] * tr[j].z;
            if (H > z_buffer[ID(x,y)]) {
                z_buffer[ID(x,y)] = H;
                image.set(x,y,_color);
            }
            
        }
    }
}
void Render(TGAImage &image) {
    int width = image.width() - 1;
    int height = image.height() - 1;
    Model model("D:\\note\\my_openGL\\my_opengl\\obj\\african_head\\african_head.obj");
    Vec3f light_dir = Vec3f(0,0,-1);
    float *z_buffer = new float [width * height];
    std::fill(z_buffer,z_buffer+(width*height),-std::numeric_limits<float>::max());
    // std::cerr << z_buffer[0] << '\n';

    TGAImage diffuse_image(WIDTH,HEIGHT,TGAImage::RGB);
    // flip_vertically
    diffuse_image.read_tga_file("D:\\note\\my_openGL\\my_opengl\\obj\\african_head\\african_head_diffuse.tga");
    diffuse_image.flip_vertically();
    for (int i=0; i<model.nfaces(); i++) { 
        std::vector<int> face = model.face(i); 
        std::vector<int> face_text = model.face_text(i);
        // std::cerr << face_text[0] << ' ' <<face_text[1] << ' ' << face_text[2] << '\n';
        // std::cerr << face[0] << ' ' << face[1] << ' ' << face[2] << "face" << '\n';
        Vec3i screen_coords[3]; 
        Vec3f _temp[3];
        Vec3f _text[3];
        for (int j=0; j<3; j++) { 
            Vec3f world_coords = model.vert(face[j]);
            Vec3f text_coords = model.text(face_text[j]);
            // std::cerr << world_coords << " face " <<  text_coords << '\n';
            screen_coords[j] = Vec3i((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.,(world_coords.z)); //bug:text_coords全是0
            _temp[j] = world_coords;
            _text[j] = text_coords;
        } 
        Vec3f n = (_temp[2] - _temp[0]) ^ (_temp[1] - _temp[0]);
        n.normalize();
        auto light_intensity = n * light_dir;
        if (light_intensity > 0) {
            Vec3i tr[3] = {screen_coords[0], screen_coords[1], screen_coords[2]};
            triangle(tr,_text, z_buffer,image, light_intensity,diffuse_image); 
        }
        // delete []_temp;
    }  
    delete []z_buffer;
}
int main(int argc, char** argv) {
    TGAImage image(WIDTH,HEIGHT, TGAImage::RGB);
    // image.set(52, 41, red);

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    Render(image);
    // std::cerr << "fuck" << '\n';
    image.write_tga_file("output.tga");
    system("pause");
    return 0;
}