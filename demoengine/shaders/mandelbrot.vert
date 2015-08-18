/**
 * @file
 * @authro Mikko Kauppila
 *
 * Mandelbrot fractal.
 *
 * Copyright (C) Mikko Kauppila 2009.
 */
 
varying vec4 position;

void main() {
    gl_Position = ftransform();
    
    position = gl_Position;
}