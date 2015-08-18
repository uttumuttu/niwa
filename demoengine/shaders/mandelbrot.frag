/**
 * @file
 * @author Mikko Kauppila
 *
 * Mandelbrot fractal.
 *
 * Copyright (C) Mikko Kauppila 2009.
 */
 
uniform float time_seconds;
uniform float scale;

varying vec4 position;

void main() {
    vec2 c = vec2(
               position.x*(4.0/3.0) * (scale) + 0.41,
               position.y * (scale) + 0.309);
    
    vec2 z = c;

    int i = 100;
    
    do {
        if(dot(z,z) > 4.0) {
            break;
        }
        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
    } while(--i > 0);
    
    float n = 100.0 - float(i);
    
    float co = n + (log(2.0*log(2.0)) - log(log(length(z)))) / log(2.0);
    
    co = 1.0 - co / 100.0;
    
    float red = co;
    float green = pow(co, 2.0 + sin(time_seconds/3.0));
    float blue = pow(co, 3.0 + sin(time_seconds/5.0) * 2.0);
    
    gl_FragColor = vec4(red, green, blue, 1.0);
}