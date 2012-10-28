#version 120

uniform sampler2D texture0;

uniform float specularPower;
uniform float diffusePower;
varying vec3 Normal;
varying vec3 LightVector;
varying vec3 EyeVector;

void main()
{
	vec4 texColor = texture2D(texture0, gl_TexCoord[0].xy);
	//gl_FragColor = texColor;
	
	vec3 light = normalize(LightVector);
	vec3 norm = normalize(Normal);
	vec3 halfvec = normalize(LightVector + EyeVector);

	float diffElem = max(0.0, dot(norm, light));
	diffElem = pow(diffElem, diffusePower);
	vec3 diffuse = gl_LightSource[0].diffuse.rgb * diffElem;

	vec3 specular = vec3(0.0);
	if (diffElem > 0.0) {
		specular.rgb = gl_LightSource[0].specular.rgb * pow(max(0.0, dot(norm, halfvec)), specularPower);
	}

	gl_FragColor.rgb = (gl_LightSource[0].ambient.rgb*gl_Color.rgb*texColor.rgb + diffuse*gl_Color.rgb*texColor.rgb +specular);
	gl_FragColor.a = 1.0;
}
