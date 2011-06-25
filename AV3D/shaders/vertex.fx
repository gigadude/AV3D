attribute vec4 _inVertex;
attribute vec4 _inNormal;
attribute vec2 _inTexCoord;

uniform mat4 _inModelViewProjection;
uniform mat4 _inNormalMatrix;

varying vec2 _vTexCoord;
varying vec4 _vNormal;

void main()
{
	_vTexCoord = _inTexCoord;
	_vNormal = _inNormalMatrix * _inNormal;
	gl_Position = _inModelViewMatrix * _inVertex;
}