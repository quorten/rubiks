#ifndef __PLANE_H
#define __PLANE_H

#include "Vector.h"

class Plane
{
public:
     Vector N;      // plane normal
     float  D;      // plane shift constant

     // constructors

     // Ax + By + Cz - D = 0
     Plane(scalar_t a = 1, scalar_t b = 0, scalar_t c = 0, scalar_t d = 0)
     {
          N = Vector(a, b, c);
          D = d;
     }

     // instantiate a plane with normal Normal and D = d
     Plane(const Vector& normal, scalar_t d = 0)
     {
          N = normal;
          D = d;
     }

     // instantiate a copy of Plane
     Plane(const Plane& plane)
     {
          N = plane.N;
          D = plane.D;
     }

     // instantiate a plane with three points
     Plane(const Vector& vertexA, const Vector& vertexB, const Vector& vertexC)
     {
          Vector normalA((vertexC - vertexA) | 1.0);  // unit normal of C - A
          Vector normalB((vertexC - vertexB) | 1.0);  // unit normal of C - B

          N = (normalA ^ normalB) | 1.0;    // normalize cross product
          D = -vertexA % N;                 // calculate distance
     }

     // assignment operator
     const Plane& operator=(const Plane& plane)
     {    
          N = plane.N;
          D = plane.D;

          return *this;
     }

     // equality operator
     const bool operator==(const Plane& plane) const
     {    
          return N == plane.N && D == plane.D;
     }

     // inequality operator
     const bool operator!=(const Plane& plane) const
     {
          return !(*this == plane);
     }

     // is point on this plane?
     const bool inline PointOnPlane(const Vector& point) const
     {
          return DistanceToPlane(point) == 0;
     }

     // return the distance of point to the plane
     const float inline DistanceToPlane(const Vector& point) const
     {
          return N % point + D;
     }

     // return the intersection point of the ray to this plane
     const Vector inline RayIntersection(const Vector& rayPos, const Vector& rayDir) const
     {
          const scalar_t a = N % rayDir;
          if (a == 0) 
                return rayPos;     // ray is parallel to plane
          
          return rayPos - rayDir * (DistanceToPlane(rayPos) / a);
     }
};

#endif    //__PLANE_H
