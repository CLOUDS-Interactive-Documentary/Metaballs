
#version 120
#extension GL_ARB_texture_rectangle : enable

varying vec3 norm;
varying vec3 ePos;

void main()
{
	norm = normalize( gl_NormalMatrix * gl_Normal );
	norm = norm * .5 + .5;
	
	vec4 pos = gl_Vertex;
//	pos.y = -.3;
	vec4 ecPosition = gl_ModelViewMatrix * pos;
	ePos = vec3(0., -1., 0.);//normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
}

