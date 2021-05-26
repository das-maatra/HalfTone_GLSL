
#version 430

#include "portraitCommon.glsl"

#define SST 0.9
#define SSQ 0.001
#define D2R(d) radians(d)
#define ORIGIN (0.5 * uRes.xy)

layout(binding = 0) uniform sampler2D uCamTex;
//layout(binding = 3) uniform sampler2D uNoiseTex;

uniform vec2 uRes;
uniform float uScale;
uniform bool uUseBlackDots;
uniform float uAngle;
uniform float uGridDensity;
uniform vec3 uPortraitColor;
uniform vec3 uBgColor;
uniform float uNoiseAmp;
uniform bool uUseCamColor;
uniform float uSST;
uniform float uSSQ;

in vec2 vTexCoord0;
out vec4 oFragColor;

float rotate;
float samples;
vec4 sourceVignetteTex;
vec4 sourceTex;
vec4 sourceColorTex;
vec4 maskTex;


vec3 cmyki2rgb(in vec4 c)
{
	return c.rgb * c.a;
}

vec2 grid(in vec2 px){
	return px - mod(px,samples);
}

vec4 smoothStepHalfTone(in vec4 v){
    return smoothstep(uSST-uSSQ, uSST+SSQ, v);
}


//get2DOff returns 2D offset value from the optical flow texture
vec2 get2DOff(sampler2D tex ,vec2 coord) {
    vec4 col = texture(tex, coord);
    if (col.w >0.5)  col.z=col.z*-1;
    return vec2(-1*(col.y-col.x),col.z);
}


vec4 halftone(in vec2 fc,in mat2 m){
	//vec2 smp = (grid(m*fc) + 0.5f*samples) * m;
	vec2 smp = (grid(m*fc) + 0.5f*samples) * m;
    float s = min(length(fc-smp) / (uScale*samples), 1.0);
	//return vec4(s,s,s, 1.0f);
	return vec4(sourceTex.rgb, 1.0)+s;
}
vec4 rgb2cmyki(in vec3 c)
{
	float k = max(max(c.r, c.g), c.b);
	return min(vec4(c.rgb / k, k), 1.0);
}

void main(){
	vec2 fc = gl_FragCoord.xy;// - ORIGIN;
	sourceTex = texture(uCamTex, vTexCoord0);
	sourceTex.rgb = vec3(sourceTex.r+sourceTex.g+sourceTex.b)/3.0f;
	
	/*
	vec2 noise = texture( uNoiseTex, px2uv(gl_FragCoord.xy) ).xy;
	noise.x = map(noise.x, 0.0,1.0, -1.0, 1.0);
	noise.y = map(noise.y, 0.0,1.0, -1.0, 1.0);
	noise *=uNoiseAmp;
	*/
	
    rotate = (uAngle)*3.1416/180.f;
	
	samples = uRes.x/uGridDensity;
    mat2 m = rotate2d(rotate);
	//vec3 c = smoothStepHalfTone( halftone( fc+noise, m ) ).rgb;
	vec3 c = smoothStepHalfTone( halftone( fc, m ) ).rgb;
	//c = halftone( fc, m ).rgb;
	
	oFragColor = vec4( uBgColor*c, 1.0);
	
}
