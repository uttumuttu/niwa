/**
 * @file
 * @author Mikko Kauppila
 *
 * Quaternion Julia fractal.
 *
 * Copyright (C) Mikko Kauppila 2009.
 */
 
varying vec4 position;

uniform float time_seconds;

uniform vec4 fractal_constant;
uniform vec4 diffuse_color;

uniform mat4 camera_rotation;

#define QUATERNION_W 0.0

#define CAMERA_DISTANCE 2.5

#define MARCHING_EPSILON 0.0005

#define MARCHING_ITERATIONS 64

#define BOUNDING_RADIUS_2 3.0

#define ESCAPE_RADIUS_2 4.0

#define ACCELERATION_CONSTANT 1.3
#define ACCELERATION_BASE 1.01

#define JULIA_LOOP UNROLL_9

/**
 * General quaternion multiplication. (This method isn't used anywhere
 * since inlining is much faster, but it serves as a nice reference.)
 */
vec4 quat_mul(const vec4 q, const vec4 r) {
    vec4 s;
    s.x   = q.x*r.x - dot( q.yzw, r.yzw );
    s.yzw = q.x*r.yzw + r.x*q.yzw + cross( q.yzw, r.yzw );
    return s;
}

/**
 * Quaternion squaring; faster than general multiplication.
 * (This method isn't used anywhere since inlining is much faster, but
 * it serves as a nice reference.)
 */
vec4 quat_square(const vec4 q) {
    vec4 r;
    r.x   = q.x*q.x - dot( q.yzw, q.yzw );
    r.yzw = 2.0*q.x*q.yzw;
    return r;
}

#define UNROLL_0(BODY)
#define UNROLL_1(BODY) UNROLL_0(BODY); BODY
#define UNROLL_2(BODY) UNROLL_1(BODY); BODY
#define UNROLL_3(BODY) UNROLL_2(BODY); BODY
#define UNROLL_4(BODY) UNROLL_3(BODY); BODY
#define UNROLL_5(BODY) UNROLL_4(BODY); BODY
#define UNROLL_6(BODY) UNROLL_5(BODY); BODY
#define UNROLL_7(BODY) UNROLL_6(BODY); BODY
#define UNROLL_8(BODY) UNROLL_7(BODY); BODY
#define UNROLL_9(BODY) UNROLL_8(BODY); BODY
#define UNROLL_10(BODY) UNROLL_9(BODY); BODY
#define UNROLL_11(BODY) UNROLL_10(BODY); BODY
#define UNROLL_12(BODY) UNROLL_11(BODY); BODY
#define UNROLL_13(BODY) UNROLL_12(BODY); BODY
#define UNROLL_14(BODY) UNROLL_13(BODY); BODY
#define UNROLL_15(BODY) UNROLL_14(BODY); BODY
#define UNROLL_16(BODY) UNROLL_15(BODY); BODY

/**
 * Distance estimate to the Julia fractal surface.
 */
float julia_distance(const vec4 q0) {
    vec4 c = fractal_constant;
    
    vec4 q  = q0;
    vec4 qp = vec4(1,0,0,0);

    vec4 qn;
    vec4 qpn;

    float normq2;
    
    float qx2 = q.x*q.x;
    float qyzw2 = dot(q.yzw, q.yzw);

    JULIA_LOOP({    
        qpn.x   = q.x*qp.x - dot( q.yzw, qp.yzw );
        qpn.yzw = q.x*qp.yzw + qp.x*q.yzw + cross( q.yzw, qp.yzw );
        qp = qpn * 2.0;

        qn.x   = qx2 - qyzw2;
        qn.yzw = 2.0*q.x*q.yzw;
        q = qn + c;
        
        qx2 = q.x*q.x;
        qyzw2 = dot(q.yzw, q.yzw);
            
        normq2 = qx2 + qyzw2;
        
        if(normq2 > ESCAPE_RADIUS_2) {
            float normqp2 = dot(qp,qp);

            return sqrt(normq2 / normqp2) * log(normq2) * .25;
        }
    })

    float normqp2 = dot(qp,qp);

    return sqrt(normq2 / normqp2) * log(normq2) * .25;
}

/**
 * @param Whether or not the ray hit the sphere.
 */
bool raytrace_bounding_sphere(
        const vec4 origin, const vec4 direction, out vec4 hit) {
    float c = dot(origin, origin) - BOUNDING_RADIUS_2;
    
    if(c < 0.0) {
        // We're already inside the sphere.
        hit = origin;
        return true;
    }
    
    float b = dot(origin, direction);
    
    float D = b*b - c;
    
    if(D < 0.0) {
        return false;
    } else {
        float t = -b - sqrt(D);
        
        hit = origin + direction * t;
        
        return true;
    }
}

void main() {
    vec4 ray_origin = vec4(0, 0, CAMERA_DISTANCE, QUATERNION_W);
    vec4 ray_direction = normalize(vec4(position.x, position.y * (3.0/4.0), -1, 0));

    vec4 origin = camera_rotation * ray_origin;
    vec4 direction = camera_rotation * ray_direction;

    if(!raytrace_bounding_sphere(origin, direction, origin)) {
        discard;
        return;
    }

    int i = MARCHING_ITERATIONS;
    
    float acceleration = ACCELERATION_CONSTANT;
    
    float depth = 0.0;
    
    do {
        float d = julia_distance(origin);

        depth += d;
        
        if(d < MARCHING_EPSILON) {
            float occlusion = float(i-1) / float(MARCHING_ITERATIONS-1);
            
            gl_FragColor = diffuse_color * occlusion;
            return;
        } else if(d*d > BOUNDING_RADIUS_2) {
            discard;
            return;
        }
        
        origin += direction * d * acceleration;
        
        acceleration *= ACCELERATION_BASE;
    } while(--i > 0);

    // Failure to converge; we probably would have
    // hit the object with enough iterations.
    // Black color signifies full occlusion.
    gl_FragColor = vec4(0,0,0,1);
}