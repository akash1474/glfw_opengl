#version 330 core

attribute vec4 position;
attribute vec4 aColor;

uniform float scale;

out vec4 color;
void main(){
	gl_Position=vec4(position.x*scale,position.y*scale,position.z*scale,1.0f);
	color=aColor;
}
