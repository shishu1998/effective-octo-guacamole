
uniform sampler2D diffuse;
uniform float alphaValue;
varying vec2 texCoordVar;

void main() {
	vec4 texture = texture2D(diffuse, texCoordVar);
	texture.w = alphaValue;
	if(texture.rgb == vec3(0.0,0.0,0.0))
		discard;
    gl_FragColor = texture;
}