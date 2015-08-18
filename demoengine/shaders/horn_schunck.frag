/**
 * @file
 * @author Mikko Kauppila
 *
 * Horn--Schunck optical flow.
 *
 * Performs one Jacobi iteration of the Horn--Schunck algorithm.
 *
 * Velocities are represented in frame-space coordinates, in range [-1,1].
 * For example, a velocity of 1/4 means
 * that the pixel moves a quarter of the screen in one frame.
 *
 * Given a frame-space velocity VEL in [-1,1], the velocity is stored as
 *
 *  STORED_VEL = (VEL + 1) / 2
 *
 * to normalize the scale to [0,1].
 * 
 * Copyright (C) Mikko Kauppila 2009.
 */

/**
 * Optical differential in [0,1].
 */
uniform sampler2D optical_differential;

/**
 * Current stored frame-space velocity normalized to [0,1].
 */
uniform sampler2D current_velocity;

uniform float pixel_width;
uniform float pixel_height;

/**
 * Regularization parameter (alpha in the original Horn--Schunck paper).
 */
uniform float regularization;

varying vec4 window_position; // \in [-1,1]

vec2 mean_velocity(vec2 texpos) {
    vec2 hx = vec2(pixel_width, 0.0);
    vec2 hy = vec2(0.0, pixel_height);
    
    vec4 adjacent = 
        texture2D(current_velocity, texpos + hx) +
        texture2D(current_velocity, texpos - hx) +
        texture2D(current_velocity, texpos + hy) +
        texture2D(current_velocity, texpos - hy);

    vec4 diagonal = 
        texture2D(current_velocity, texpos - hx - hy) +
        texture2D(current_velocity, texpos + hx - hy) +
        texture2D(current_velocity, texpos + hx + hy) +
        texture2D(current_velocity, texpos - hx + hy);
        
    return adjacent.xy / 6.0 + diagonal.xy / 12.0;
}

void main() {
    vec2 texpos = (window_position.xy + 1.0) * 0.5; // \in [0,1]

    vec4 diff = texture2D(optical_differential, texpos) * 2.0 - 1.0; // in [-1,1]
    
    float hx = pixel_width;
    float hy = pixel_height;

    float Ix = diff.x / (2.0*hx); // frame-space derivatives, \in frame_width*[-1,1]/2
    float Iy = diff.y / (2.0*hy); // frame-space derivative,  \in frame_height*[-1,1]/2
    float It = diff.z;            // frame-time derivative, \in [-1,1]

    vec2 w = mean_velocity(texpos) * 2.0 - 1.0; // frame-space velocity, \in [-1,1]

    float numerator = Ix * w.x + Iy * w.y + It;
    float denominator = regularization*regularization + Ix*Ix + Iy*Iy;
    
    vec2 new_velocity = w - vec2(Ix,Iy) * (numerator / denominator); // in [-1,1]
    
    gl_FragColor = vec4((new_velocity + 1.0) * 0.5, 0.0, 0.0); // in [0,1]
}