/* FRAGMENT SHADER
 * ----------- 
 * Author: E. Pronk
 * Descr.: Water mirror effect
 */

uniform sampler2D _uTexture;
uniform float _uOpacity;
uniform float _uYOpacity;
uniform float _uTime;

varying vec2 _vTexCoord;
varying vec3 _vNormal;

float rand(vec2 co)
{
    return (sin(cos(_uTime) * co.x * 30) * sin(sin(_uTime) * co.y * 50) + cos(co.x * 23) * cos(sin(_uTime) * co.y * 13)) * 0.5 + 0.5;
}

void main()
{
	float intensity = min(max(dot(_vNormal, vec3(0, 0, -1)), 0), 1);
	float mirror = (_uOpacity + _vTexCoord.y * _uYOpacity);
	intensity *= intensity;
	mirror *= mirror;
	float r1 = (1 - _vTexCoord.y) * (1 - _uOpacity) * _uYOpacity * rand(_vTexCoord.xy);
	vec2 tex = vec2(_vTexCoord.x + r1/4, _vTexCoord.y + r1/4);
	gl_FragColor = intensity * mirror * texture2D(_uTexture, tex);
}