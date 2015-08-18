/**
 * @file
 * @authro Mikko Kauppila
 *
 * Motion blur.
 *
 * Copyright (C) Mikko Kauppila 2009.
 */
 
/**
 * Screen position in [-1,1].
 */
varying vec4 window_position;

void main() {
    gl_Position = ftransform();
    
    window_position = gl_Position;
}