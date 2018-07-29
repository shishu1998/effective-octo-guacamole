
uniform sampler2D diffuse;
uniform float alphaValue;
varying vec2 texCoordVar;

void main() {
	vec4 texture = texture2D(diffuse, texCoordVar);
	if(texture.w == 0)
		discard;
	texture.w = alphaValue;
    gl_FragColor = texture;
}