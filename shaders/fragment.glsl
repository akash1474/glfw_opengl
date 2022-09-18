#version 330 core 

//importing from vertext shader
in vec4 color;
in vec2 texCoord;

uniform sampler2D tex0;

void main(){
	gl_FragColor = texture(tex0,texCoord);
}
