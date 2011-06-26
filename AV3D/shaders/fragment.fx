uniform sampler2D _uTexture;

varying vec2 _vTexCoord;
varying vec4 _vNormal;

void main()
{
	float intensity = max(dot(_vNormal, vec4(0, 0, -1, 0)), 0);
	gl_FragColor = texture2D(_uTexture, _vTexCoord);
}