#include <iostream>
#include <fstream>
#include "float.h"
#include "ObjectSet.h"
#include "Sphere.h"
#include "Camera.h"
#include "Material.h"
#include "Rectangle.h"

using namespace std;

Vector3D color(const Ray& r, Object *s, int depth) {
  IntersectInfo info;
  if (s->Intersect(r, 0.001 , MAXFLOAT, info)) {
    Ray scattered ;
    Vector3D attenuation;
    Vector3D emitted = info.materialPointer->Emitted(info.pointOfIntersection);
    if (depth < 50 && info.materialPointer->Scatter(r, info, attenuation, scattered)) {
      return emitted + attenuation * color(scattered, s, depth + 1);
    } else {
      return emitted;
    }
  } else {
    // Vector3D unitDirection = GetUnitizedCopy(r.direction);
    // float t = 0.5 * (unitDirection.y + 1);
    // return (1-t) * Vector3D(1,1,1) + t * Vector3D(0.5, 0.7, 1);
    return Vector3D(0, 0, 0);
  }
}

ObjectSet *randomSet() {
    int n = 500;
    Object **list = new Object*[n+1];
    Texture *checker = new CheckerTexture(new PureColorTexture(Vector3D(0.2, 0.3, 0.1)), new PureColorTexture(Vector3D(0.9, 0.9, 0.9)));
    list[0] =  new Sphere(Vector3D(0,-1000,0), 1000, new Diffuse(checker));
    int i = 1;
    // for (int a = -11; a < 11; a++) {
    //   for (int b = -11; b < 11; b++) {
    //     float choose_mat = drand48();
    //     Vector3D center(a+0.9*drand48(),0.2,b+0.9*drand48()); 
    //     if ((center-Vector3D(4,0.2,0)).Length() > 0.9) { 
    //       if (choose_mat < 0.8) {  // diffuse
    //         list[i++] = new Sphere(center, 0.2, new Diffuse(Vector3D(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
    //       }
    //       else if (choose_mat < 0.95) { // metal
    //         list[i++] = new Sphere(center, 0.2, new Metal(Vector3D(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48()))));
    //       }
    //       else {  // glass
    //         list[i++] = new Sphere(center, 0.2, new Glass(1.5));
    //       }
    //     }
    //   }
    // }
    Texture *marble = new MarbleTexture(100);
    list[i++] = new Sphere(Vector3D(0, 1, 0), 1.0, new Diffuse(marble));
    // list[i++] = new Sphere(Vector3D(0, 1, 0), 1.0, new Glass(1.5));
    Texture *pure = new PureColorTexture(Vector3D(0.4, 0.2, 0.1));
    list[i++] = new Sphere(Vector3D(-4, 1, 0), 1.0, new Diffuse(pure));
    list[i++] = new Sphere(Vector3D(4, 1, 0), 1.0, new Metal(Vector3D(0.7, 0.6, 0.5)));

    return new ObjectSet(list,i);
}

Object *simpleLight() {
  Object **list = new Object*[6];
  Texture *pertext = new MarbleTexture(100);
  Texture *checker = new CheckerTexture(new PureColorTexture(Vector3D(0.2, 0.3, 0.1)), new PureColorTexture(Vector3D(0.9, 0.9, 0.9)));
  list[0] =  new Sphere(Vector3D(0,-1000,0), 1000, new Diffuse(checker));
  list[1] = new Sphere(Vector3D(0, 1, 0), 1, new Diffuse(pertext));
  list[2] = new Sphere(Vector3D(0, 7, 0), 2, new Light(new PureColorTexture(Vector3D(1, 1, 1))));
  list[3] = new Rectangle(Vector3D(1.3, 0, 0.5), Vector3D(2.5, 1.6, 2.3), new Diffuse(new PureColorTexture(Vector3D(0.8, 0.6, 0.7))));
  list[4] = new Sphere(Vector3D(-4, 1, 0), 1.0, new Glass(1.5));
  list[5] = new Sphere(Vector3D(4, 1, 0), 1.0, new Metal(Vector3D(0.7, 0.6, 0.5)));
  return new ObjectSet(list, 6);
}

int main() {
  int nx = 300;
  int ny = 300;
  int ns = 1000;
  ofstream outfile("test.ppm");
  outfile << "P3\n" << nx << " " << ny << "\n255\n";

  // Object* arr[5];
  // arr[0] = new Sphere(Vector3D(0,0,-1), 0.5, new Diffuse(Vector3D(0.1, 0.2, 0.5)));
  // arr[1] = new Sphere(Vector3D(0,-100.5,-1), 100, new Diffuse(Vector3D(0.8, 0.8, 0)));
  // arr[2] = new Sphere(Vector3D(1,0,-1), 0.5, new Metal(Vector3D(0.8, 0.6, 0.2)));
  // arr[3] = new Sphere(Vector3D(-1,0,-1), 0.5, new Glass(1.5));
  // arr[4] = new Sphere(Vector3D(-1,0,-1), -0.45, new Glass(1.5));
  // Object *set = new ObjectSet(arr, 5);
  // Object *set = randomSet();
  Object *set = simpleLight();


  Camera cam(Vector3D(13, 2, 3), Vector3D(0, 0, 0), Vector3D(0, 1, 0), 60, float(nx)/float(ny));
  // Camera cam;

  for (int j = ny - 1; j >= 0; j--) {
    for (int i = 0; i < nx; i++) {
      Vector3D col = Vector3D(0, 0, 0);
      for (int s = 0; s < ns; s++) {
        float u = float(i + drand48()) / float(nx);
        float v = float(j + drand48()) / float(ny);
        Ray r = cam.getRay(u, v);
        col += color(r, set, 0);
      }
      col /= float(ns);
      col = Vector3D(sqrt(col.x), sqrt(col.y), sqrt(col.z));
      int ir = int(255.99*col.x);
      int ig = int(255.99*col.y);
      int ib = int(255.99*col.z);
      outfile << ir << " " << ig << " " << ib << "\n";
    }
  }
  outfile.close();
}