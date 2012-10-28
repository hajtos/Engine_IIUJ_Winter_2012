#version 120

uniform float animParam;

varying vec3 Normal;
varying vec3 LightVector;
varying vec3 EyeVector;

void main()
{
	vec4 v = gl_Vertex;
	
    //v.x += 0.1*sin(animParam*3.0+v.z*2.0+v.x+v.y); 
    //v.y += 0.025*sin(animParam*1.0+v.z*1.0+v.y*1.0); 
	
	//gl_Position = gl_ModelViewProjectionMatrix * v;
	
	//gl_FrontColor = gl_Color;
	
	
	gl_Position = ftransform();
	v = gl_ModelViewMatrix * v;
	LightVector = normalize(gl_LightSource[0].position.xyz - v.xyz);
	EyeVector = normalize(-v.xyz);
	Normal = gl_NormalMatrix * gl_Normal;
	gl_FrontColor = gl_Color;

	gl_TexCoord[0] = gl_MultiTexCoord0;
}
