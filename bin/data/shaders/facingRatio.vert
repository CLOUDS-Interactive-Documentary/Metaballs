varying vec3 norm;
varying vec3 ePos;

void main()
{
	norm = normalize( gl_NormalMatrix * gl_Normal );
	norm = norm * .5 + .5;
	
	vec3 lPos = vec3( 0., 1., 0.);
	
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ePos = vec3(0., -1., 0.);normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
}

