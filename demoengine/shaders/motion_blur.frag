/**
 * @file
 * @author Mikko Kauppila
 *
 * Motion blur.
 *
 * Velocities are represented in frame-space (range [-1,1]).
 * Velocity VEL in [-1,1] is stored as 
 *
 *   STORED_VEL = (VEL+1) / 2
 *
 * to normalize the range to [0,1].
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

uniform sampler2D new_frame;
uniform sampler2D old_frame;

/**
 * Shutter time in number of frames.
 */
uniform float shutter_time;

/**
 * Optical flow in [0,1].
 */
uniform sampler2D optical_flow;

varying vec4 window_position; // in [-1,1]

vec4 trace_path() {
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    
    int n_iterations = 64;
    
    vec2 texpos = (window_position.xy + 1.0) * 0.5; // in [0,1]
    
    float velocity_constant = shutter_time / float(n_iterations) * (-1.5);
    
    for(int i=0; i<n_iterations; ++i) {
        vec4 velocity = texture2D(optical_flow, texpos) * 2.0 - 1.0; // in [-1,1]
        
        vec2 newpos = texpos + velocity.xy * velocity_constant;
        vec2 midpos = (texpos + newpos) * 0.5;
        
        color += texture2D(old_frame, midpos);
        
        velocity = texture2D(optical_flow, midpos) * 2.0 - 1.0; // in [-1,1]
        
        texpos += velocity.xy * velocity_constant;
    }
    
    return color / float(n_iterations);
}

void main() {
    gl_FragColor = trace_path();
}