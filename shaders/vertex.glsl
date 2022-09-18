#version 330 core

attribute vec4 position;
attribute vec4 aColor;
attribute vec2 aTex;


uniform float scale;


//ouputing to fragment shader
out vec4 color;
out vec2 texCoord;

// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main(){
	gl_Position=proj * view * model * position;
	color=aColor;
	texCoord=aTex;
}
