attribute vec3 Pos; 
attribute vec3 Color;
attribute vec2 TexCoord;

varying vec3 m_color;
varying vec2 m_texcoord;

uniform mat4x4 ModelMat;
uniform mat4x4 ViewMat;
uniform mat4x4 ProjMat;

void main(){
	m_color = Color;
	m_texcoord = TexCoord;
	m_texcoord.y *= -1;
	gl_Position = ProjMat * ViewMat * ModelMat * vec4(Pos, 1.0f);
}