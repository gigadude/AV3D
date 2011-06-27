/* VERTEX SHADER
 * ----------- 
 * Author: E. Pronk
 * Descr.: Basic vertex shader
 */

attribute vec4 _inVertex;
attribute vec4 _inNormal;
attribute vec2 _inTexCoord;

varying vec2 _vTexCoord;
varying vec3 _vNormal;

void main()
{
	_vTexCoord = _inTexCoord;
	_vNormal = gl_NormalMatrix * _inNormal.xyz;
	gl_Position = ftransform();
}