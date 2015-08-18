/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/objects/Triangle.h"

#include "niwa/raytrace/ray3f.h"
#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/Constants.h"

namespace niwa {
    namespace raytrace {
        namespace objects {
            using math::vec3f;
            using math::packed_vec3f;

            Triangle::Triangle() 
                : position_(0,0,0), normal_(0,0,1) {
                projectionDimensions_[0] = 0;
                projectionDimensions_[1] = 1;

                matrix_[0] = matrix_[3] = 1;
                matrix_[1] = matrix_[2] = 0;
            }

            Triangle::Triangle(vec3f vertices[3]) 
                : position_(vertices[0]) {
                vec3f edges[2] = {
                    vertices[1] - position_,
                    vertices[2] - position_
                };

                normal_ = vec3f::cross(edges[0], edges[1]);
                normal_.normalize();

                int mainDimension = 0;
                if(fabs(normal_.y) > fabs(normal_.x)) {
                    mainDimension = 1;
                }
                if(fabs(normal_.z) > fabs(normal_[mainDimension])) {
                    mainDimension = 2;
                }

                projectionDimensions_[0] = mainDimension == 0 ? 1 : 0;
                projectionDimensions_[1] = mainDimension == 2 ? 1 : 2;

                // this is the non-inverted barycentric matrix
                float a = edges[0][projectionDimensions_[0]];
                float b = edges[1][projectionDimensions_[0]];
                float c = edges[0][projectionDimensions_[1]];
                float d = edges[1][projectionDimensions_[1]];

                float invDet = 1 / (a*d - b*c);

                matrix_[0] = d*invDet;
                matrix_[1] = -b*invDet;
                matrix_[2] = -c*invDet;
                matrix_[3] = a*invDet;
            }
        }
    }
}