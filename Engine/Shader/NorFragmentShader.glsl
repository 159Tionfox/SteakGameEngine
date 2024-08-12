varying vec3 m_color;
varying vec2 m_texcoord;

uniform sampler2D smp;

void main(){
	vec3 rgb = texture2D(smp,m_texcoord).rgb;
	gl_FragColor = vec4(rgb + m_color * 0.01f, 1.0f);
}