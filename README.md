/* 
 * Justin Lee
 *
 */


Implemented in C++ 17.  Requires C++ 17 and cmake






DESCRIPTION:   
This is an implementation of a winding number algorithm.   A winding number is a method in which we determine if a point is within a closed curve in a 2D coordinate plane. The winding number is calculated by drawing a line from the point to each position as 
the closed curve is traversed, and determining the number of counter clockwise turns the object makes relative to the origin.  
 
The polygon reader class (poly_io.cpp)  reads the input polygon text files contain an x-vector and y-vector that describe the closed curve as well as the position of the point.  This information is then passed onto the algorithm I wrote in the winding.cpp class
to calculate the winding number.




IMPLEMENTAION DETAILS:

Can be found in src/winding.cpp
