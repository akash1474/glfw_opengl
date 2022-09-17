#version 330 core

attribute vec4 position;
attribute vec4 aColor;
attribute vec2 aTex;


uniform float scale;

out vec4 color;
out vec2 texCoord;


void main(){
	gl_Position=vec4(position.x*scale,position.y*scale,position.z*scale,1.0f);
	color=aColor;
	texCoord=aTex;
}
