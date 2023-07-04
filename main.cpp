#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <vector>
const TGAColor white = TGAColor{255, 255, 255, 255};
const TGAColor red   = TGAColor{0, 0,   255,   255};
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
void triangle (Vec2i*tr,TGAImage &image,TGAColor color) {
    // std::vector<Vec2i>tr;
    // tr.push_back(p0);
    // tr.push_back(p1);
    // tr.push_back(p2);
    // Vec2i tr[3] = {p0,p1,p2};
    Vec2i border_min(image.width() - 1,image.height() - 1);
    Vec2i border_max(0,0);
    for (int i = 0; i < 3; ++i) {
        border_max.x = std::max(border_max.x,tr[i].x);
        border_max.y = std::max(border_max.y,tr[i].y);
        border_min.x = std::min(border_min.x,tr[i].x);
        border_min.y = std::min(border_min.y,tr[i].y);
    }
    border_max.x = std::min(border_max.x,image.width() - 1);
    border_max.y = std::min(border_max.y,image.height() - 1);
    for (int x = border_min.x; x <= border_max.x; ++x) {
        for (int y = border_min.y; y <= border_max.y; ++y) {
            Vec3f bc_screen = barycentric(Vec2i(x,y),tr);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)  continue;
            // std::cerr << x << ' ' << y << '\n';
            image.set(x,y,color);
        }
    }
}
void Render(TGAImage &image) {
    int width = image.width();
    int height = image.height();
    Model model("D:\\note\\my_openGL\\my_opengl\\obj\\african_head\\african_head.obj");
    // for (int i=0; i< model.nfaces(); i++) { 
    //     std::vector<int> face = model.face(i); 
    //     for (int j=0; j<3; j++) { 
    //         Vec3f v0 = model.vert(face[j]); 
    //         Vec3f v1 = model.vert(face[(j+1)%3]); 
    //         int x0 = (v0.x+1.)*width/2.; 
    //         int y0 = (v0.y+1.)*height/2.; 
    //         int x1 = (v1.x+1.)*width/2.; 
    //         int y1 = (v1.y+1.)*height/2.; 
    //         line(x0, y0, x1, y1, image, white); 
    //     } 
    // }
    // TGAImage frame(200, 200, TGAImage::RGB); 
    Vec3f light_dir = Vec3f(0,0,-1);
    for (int i=0; i<model.nfaces(); i++) { 
        std::vector<int> face = model.face(i); 
        Vec2i screen_coords[3]; 
        Vec3f _temp[3];
        for (int j=0; j<3; j++) { 
            Vec3f world_coords = model.vert(face[j]); 
            screen_coords[j] = Vec2i((world_coords.x+1.)*width/2., (world_coords.y+1.)*height/2.); 
            _temp[j] = world_coords;
        } 
        Vec3f n = (_temp[2] - _temp[0]) ^ (_temp[1] - _temp[0]);
        n.normalize();
        auto light_intensity = n * light_dir;
        if (light_intensity > 0) {
            Vec2i tr[3] = {screen_coords[0], screen_coords[1], screen_coords[2]};
            triangle(tr, image, TGAColor{(uint8_t)(255*light_intensity), (uint8_t)(255*light_intensity), (uint8_t)(255*light_intensity), 255}); 
        }
        // delete []_temp;
    }  
}
const int WIDTH = 512;
const int HEIGHT = 512;
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