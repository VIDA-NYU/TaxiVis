// COTD Entry submitted by John W. Ratcliff [jratcliff@verant.com]

// ** THIS IS A CODE SNIPPET WHICH WILL EFFICIEINTLY TRIANGULATE ANY
// ** POLYGON/CONTOUR (without holes) AS A STATIC CLASS.  THIS SNIPPET
// ** IS COMPRISED OF 3 FILES, TRIANGULATE.H, THE HEADER FILE FOR THE
// ** TRIANGULATE BASE CLASS, TRIANGULATE.CPP, THE IMPLEMENTATION OF
// ** THE TRIANGULATE BASE CLASS, AND TEST.CPP, A SMALL TEST PROGRAM
// ** DEMONSTRATING THE USAGE OF THE TRIANGULATOR.  THE TRIANGULATE
// ** BASE CLASS ALSO PROVIDES TWO USEFUL HELPER METHODS, ONE WHICH
// ** COMPUTES THE AREA OF A POLYGON, AND ANOTHER WHICH DOES AN EFFICENT
// ** POINT IN A TRIANGLE TEST.
// ** SUBMITTED BY JOHN W. RATCLIFF (jratcliff@verant.com) July 22, 2000

/**********************************************************************/
/************ HEADER FILE FOR TRIANGULATE.H ***************************/
/**********************************************************************/


#ifndef TRIANGULATOR_H

#define TRIANGULATOR_H

/*****************************************************************/
/** Static class to triangulate any contour/polygon efficiently **/
/** You should replace Vector2d with whatever your own Vector   **/
/** class might be.  Does not support polygons with holes.      **/
/** Uses STL vectors to represent a dynamic array of vertices.  **/
/** This code snippet was submitted to FlipCode.com by          **/
/** John W. Ratcliff (jratcliff@verant.com) on July 22, 2000    **/
/** I did not write the original code/algorithm for this        **/
/** this triangulator, in fact, I can't even remember where I   **/
/** found it in the first place.  However, I did rework it into **/
/** the following black-box static class so you can make easy   **/
/** use of it in your own code.  Simply replace Vector2d with   **/
/** whatever your own Vector implementation might be.           **/
/*****************************************************************/


#include <vector>  // Include STL vector class.

namespace Triangulator
{

class Vector2d
{
public:
  Vector2d(double x,double y)
  {
    Set(x,y);
  };

  double GetX(void) const { return mX; };

  double GetY(void) const { return mY; };

  void  Set(double x,double y)
  {
    mX = x;
    mY = y;
  };
private:
  double mX;
  double mY;
};

// Typedef an STL vector of vertices which are used to represent
// a polygon/contour and a series of triangles.
typedef std::vector< Vector2d > Vector2dVector;


class Triangulate
{
public:

  // triangulate a contour/polygon, places results in STL vector
  // as series of triangles.
  static bool Process(const Vector2dVector &contour,
                      Vector2dVector &result);

  // compute area of a contour/polygon
  static double Area(const Vector2dVector &contour);

  // decide if point Px/Py is inside triangle defined by
  // (Ax,Ay) (Bx,By) (Cx,Cy)
  static bool InsideTriangle(double Ax, double Ay,
                      double Bx, double By,
                      double Cx, double Cy,
                      double Px, double Py);


private:
  static bool Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V);

};
}

#endif

// /************************************************************************/
// /*** END OF CODE SECTION TRIANGULATE.CPP BEGINNING OF TEST.CPP A SMALL **/
// /*** TEST APPLICATION TO DEMONSTRATE THE USAGE OF THE TRIANGULATOR     **/
// /************************************************************************/

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <assert.h>


// #include "triangulate.h"

// void main(int argc,char **argv)
// {

//   // Small test application demonstrating the usage of the triangulate
//   // class.


//   // Create a pretty complicated little contour by pushing them onto
//   // an stl vector.

//   Vector2dVector a;

//   a.push_back( Vector2d(0,6));
//   a.push_back( Vector2d(0,0));
//   a.push_back( Vector2d(3,0));
//   a.push_back( Vector2d(4,1));
//   a.push_back( Vector2d(6,1));
//   a.push_back( Vector2d(8,0));
//   a.push_back( Vector2d(12,0));
//   a.push_back( Vector2d(13,2));
//   a.push_back( Vector2d(8,2));
//   a.push_back( Vector2d(8,4));
//   a.push_back( Vector2d(11,4));
//   a.push_back( Vector2d(11,6));
//   a.push_back( Vector2d(6,6));
//   a.push_back( Vector2d(4,3));
//   a.push_back( Vector2d(2,6));


//   // allocate an STL vector to hold the answer.

//   Vector2dVector result;

//   //  Invoke the triangulator to triangulate this polygon.
//   Triangulate::Process(a,result);

//   // print out the results.
//   int tcount = result.size()/3;

//   for (int i=0; i<tcount; i++)
//   {
//     const Vector2d &p1 = result[i*3+0];
//     const Vector2d &p2 = result[i*3+1];
//     const Vector2d &p3 = result[i*3+2];
//     printf("Triangle %d => (%0.0f,%0.0f) (%0.0f,%0.0f) (%0.0f,%0.0f)\n",i+1,p1.GetX(),p1.GetY(),p2.GetX(),p2.GetY(),p3.GetX(),p3.GetY());
//   }

// }
