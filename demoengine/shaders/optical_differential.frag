/**
 * @file
 * @author Mikko Kauppila
 *
 * Given two RGB frames (whose grayscaled versions are denoted NEW and OLD),
 * computes the optical differential (called DIFF), defined as
 *
 *   DIFF[x,y] = (vec4(DX[x,y], DY[x,y], DT[x,y], 0) + 1) / 2,
 *
 * where
 *
 *   DX[x,y] = MEAN[x+1,y] - MEAN[x-1,y],
 *   DY[x,y] = MEAN[x,y+1] - MEAN[x,y-1],
 *   DT[x,y] = NEW[x,y] - OLD[x,y],
 *
 * and
 *
 *   MEAN = (NEW + OLD) / 2.
 *
 * Notice that if the original frames contain values only in [0,1],
 * DIFF will also contain values only in [0,1].
 *
 * Copyright (C) Mikko Kauppila 2009.
 */
 
uniform sampler2D new_frame;
uniform sampler2D old_frame;

uniform float pixel_width;
uniform float pixel_height;

varying vec4 window_position; // \in [-1,1]

vec4 mean_frame(vec2 texpos) {
    return (texture2D(new_frame, texpos) + texture2D(old_frame, texpos)) * 0.5;
}

float grayscale(vec4 color) {
    return (color.x + color.y + color.z) / 3.0;
}

void main() {
    vec2 texpos = (window_position.xy + 1.0) * 0.5; // \in [0,1]
    
    vec2 hx = vec2(pixel_width, 0.0);
    vec2 hy = vec2(0.0, pixel_height);
    
    float dx = grayscale(mean_frame(texpos+hx) - mean_frame(texpos-hx)); // \in [-1,1]
    float dy = grayscale(mean_frame(texpos+hy) - mean_frame(texpos-hy)); // \in [-1,1]

    float dt = grayscale(texture2D(new_frame, texpos) - texture2D(old_frame, texpos)); // \in [-1,1]

    gl_FragColor = (vec4(dx, dy, dt, 0.0) + 1.0) * 0.5; // \in [0,1]
}