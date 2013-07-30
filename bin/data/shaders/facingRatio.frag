
#version 120
#extension GL_ARB_texture_rectangle : enable

float spec = 1.;
float expo = 4.;

uniform vec3 c1;
uniform vec3 c2;
uniform float alpha1;
uniform float alpha2;

uniform float mixScale;

varying vec3 ePos;
varying vec3 norm;

float nearClip = 1.;
float farClip = 500.;

float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}

void main(void)
{
	float depth = linearizeDepth( gl_FragCoord.z );
	
	float fr = dot( -normalize(ePos), norm ) * .5 + .5;
	float amnt = pow(fr * spec, 2.0) * mixScale;
	float alpha = pow( amnt, 2.);
	
	gl_FragColor = vec4( mix( c1, c2, amnt), mix( alpha1, alpha2, alpha )) * (1.-depth);
}
