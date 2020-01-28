/*
 * Author: Justin Lee
 * Email: jsL118@ucsd.edu
 *
 */



#include <winding.hpp>
#include <math.h> //for sqrt
#include <utility>

namespace winding_number {
namespace {

//Base Code
class BadWindingNumberAlgorithm : public IWindingNumberAlgorithm {
    std::optional<int> CalculateWindingNumber2D(float x, float y, poly::Polygon polygon) override {
        // Clearly we can do better...
        error_message("Unimplemented algorithm.");
        return std::nullopt;
    }
};


//Improved Code
class ImprovedWindingNumberAlgorithm : public IWindingNumberAlgorithm {  
   
   //    /***** Patch v0.9 BETA  *****/   -Justin Lee
   //Right here is where we have to implement the algorithm.  In terms of how the code works, 
   //essentially the goal here is to accomplish the following:

   //We are given a polygon object.   Within the polygon object, there is a x vector and a y vector. 
   //There is a bijection between the two vectors where the index of each scalar corresponds with the 
   //scalar at the same index of the other vector.   The x and y vectors define the polygon object,
   //and are components of the parametric equation that represents the closed curve that the polygon
   //inhabits.

   //We are given a defined center(x,y) point.   We must continuously draw a line from the center point
   //to the parametric equation of the closed curve f(t) at each t.   To calculate the winding number,
   //we must account for the number of times and the direction in which the line crosses the origin
   //relative to the center point.   We increment for counter clockwise and decrement for clockwise

   //Therefore we want to make a loop and for each index of x and y, the index represents
   //the parameters t in the parametric function of the closed curve.   At each t, we pull the vector
   //(x,y) that represents the point at t.   We then set the vector with relative to the center point
   //as the origin (0,0), then we normalize this vector, and therefore the point at t is now represented
   //by a point in the unit circle with center(x,y) as the center of the unit circle.  

   //And now, we can picture a theoretical line from the center(x,y) to the point on the unit circle
   //that represents the direction of (x(t), y(t)) at t.   By comparing the represented locations on the unit circle
   //of each point at t with the previous point at t, we can tell if it crosses the origin (0 degrees), as well
   //as what direction it crossed the origin based on the partition of each point. 

   //    /***** Patch 2.0 *****/   -Justin Lee
   //  In patch 2.0, we add a second method of calculating the winding number.   We use this second method
   //  when we deduce that the center point lies on the closed curve of the polygon.  This happens in 2 cases
   //  Case 1: The center is the corner of an edge of the polygon
   //  Case 2: The center lies directly along the edge of the polygon
   //  
   //  In either case, this second method will increment the winding number and from now on increment the winding
   //  number only in these cases using this method.   We increment only in the positive direction in this method
   //  because if a line goes through a point, we cannot say which direction (clockwise or counter clockwise) the line
   //  goes without having scanning for further information about the closed curve.

    std::optional<int> CalculateWindingNumber2D(float x, float y, poly::Polygon polygon) override {
        //Base case when the expected closed curve line is not a closed curve or a point
        if(!polygon.IsClosed(tolerance())){ 
           return std::nullopt;
        } 
        

        int windingNumber = 0; //we initialize a counter first
        int onEdge = 0; //bit to test if we've encountered an edge that center lies on, i.e change method
        

        //initialize variables to hold data
        float x0, y0;  //previous point
        float magnitude; //to calculate magnitude of the vector
        float x1, y1; //current point
 

        //loop through each t in (x(t), y(t))
        for(int t=0; t<polygon.x_vec_.size(); t++){ //we assume that x_vec_ and y_vec_ are the same size
                                                    //via bijection
            //we pull x(t) and y(t), where t is represented by the index 
            //the point vector at f(t) is represent by (x(t), y(t))
            x1 = polygon.x_vec_[t] - x;    //we subtract here so that the point is set relative to center(x,y)
            y1 = polygon.y_vec_[t] - y;    //as the origin at (0,0)
 
            //from this point forward, origin = translated center(x,y) value
            

            /* Patch 2.0 */
            //Here we check to see if the origin is the corner of two edges 
            if(x1 == 0 && y1 == 0){
                x1 = 1;    //We set this to (1,0) because (0,0) and (1,0) both equal 0 degrees
                y1 = 0;    // and we can't normalize (0,0)
                
                //sets onEdge if its not set so we know to find windingNumber from this method
                if(onEdge == 0){  
                    windingNumber = 1;
                    onEdge = 1;
                }
                //we do not increment at the last iteration because that means we already incremented
                //at the start and start == end for a closed polygon
                else if(t != polygon.x_vec_.size() -1){
                    ++windingNumber;
                }
            }  
           
            //we normalize the vector at t
            magnitude = x1 * x1;  
            magnitude += (y1 * y1);
            magnitude = sqrtf(magnitude);
            x1 /= magnitude;
            y1 /= magnitude;
       
            

            //We do not execute the below patch 2.0 diameter check and patch 0.9 4 outer if statements on the first iteration
            if(t == 0){  
               x0 = x1;
               y0 = y1;
               continue;
            }
           
            /* Patch 2.0 */
            //Here we check to see if the origin lies along the edge.  We check this by seeing if the unit circle
            //representations of the current point and the previous point is 180 degrees from each other
            if( x0 == (x1 * -1.0f) && y0 == (y1 * -1.0f) ){
                if(onEdge == 0){    //we set onEdge so that we now only use patch 2.0 method
                    onEdge = 1;
                    windingNumber = 1;
                }
                else{
                    ++windingNumber;
                }
            }
            if(onEdge){  //If using patch 2.0 method, do not execute patch 0.9 method
                x0 = x1;
                y0 = y1;
                continue;
            }
            
            /* Patch 0.9 */ //Used when origin is not along the boundary of the polygon
             
            //The four outer if loops are to check to see which quadrant the current point is in.  Based on which 
            //quadrant the current point is, we know if it is or is not possible that the origin has been crossed
            //based on the quadrant the previous point was in.  If there is ambiguity, based on the previous point, that
            //the origin has been crossed, then we check to see if the previous point is to the left or right of the current
            //point.
            if(x1 >= 0 && y1 > 0) {  //if this normalized point at t is in the first quadrant
                if(x0 >=0 && y0 <= 0){ //if the previous point at t was in the fourth quadrant
                    ++windingNumber;
                }
                if(x0 <0 && y0 < 0){ //if the previous point at t was in the third quadrant
                    if( x1 >= (x0 * -1.0f)) { //if the current x value has greater or equal abs value than previous x
                        ++windingNumber;
                    }
                }

            }
            if(x1 < 0 && y1 >= 0){ //if this normalized point at t lies in the second quadrant
                if(x0 >= 0 && y0 <= 0){ //if the previous point at t was in the fourth quadrant
                    if((x1 * -1.0f) <= x0){ //if the current x(t) value has a lower or equal abs value than the previous x(t)
                        ++windingNumber;
                    } 
                }
            }
            if(x1 <= 0 && y1 <0){ //if the normalized point at t lies in the third quadrant
                if(x0 > 0 && y0 >0){ //if the previous point at t was in the first quadrant
                    if((x1 * -1.0f) < x0){ //if the current x(t) value has a lower abs value than previous x(t)
                        --windingNumber;
                    }
                }

            }
            if(x1 >0 && y1 <=0){ //if the normalized point at t lies in the fourth quadrant
                if(x0 >0 &&  y0 >0){ //if the previous point at t was in the first quadrant
                    --windingNumber;
                }
                if(x0 <= 0 && y0 >0){ //if the previous piont at t was in the second quadrant
                    if(x1 > (x0 * -1.0f)) { //if the current x value has a greater abs value than previous x
                        --windingNumber;
                    }
                }
            }
            x0 = x1; //set prev to curr for next iteration
            y0 = y1;
         
        }   //end for loop
        return windingNumber; 
    }
};

}  // namespace poly

std::unique_ptr<IWindingNumberAlgorithm> IWindingNumberAlgorithm::Create() {
    return std::make_unique<ImprovedWindingNumberAlgorithm>(); //improved winding number algorithm
}

void IWindingNumberAlgorithm::tolerance(float tolerance) noexcept {
    tolerance_ = tolerance;
}

float IWindingNumberAlgorithm::tolerance() const noexcept {
    return tolerance_;
}

std::string IWindingNumberAlgorithm::error_message() const noexcept {
    return error_message_;
}

void IWindingNumberAlgorithm::error_message(std::string error_message) noexcept {
    error_message_ = std::move(error_message);
}

}  // namespace winding_number
