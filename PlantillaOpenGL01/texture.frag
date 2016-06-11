uniform sampler2D texBaked_flat;
uniform sampler2D texBaked_fill01;
uniform sampler2D texBaked_fill02;
uniform sampler2D texBaked_keyrabbit;
uniform sampler2D texBaked_checker;

uniform float _mix01;
uniform float _mix02;
uniform float _mix03;
uniform float _mix04;
uniform float _mix05;

void main(void) {

	vec4 cFinal;

	vec4 cTex01;
	vec4 cTex02;
	vec4 cTex03;
	vec4 cTex04;
	vec4 cTex05;

	cTex01 = texture2D(texBaked_flat,gl_TexCoord[0].st);
	cTex02 = texture2D(texBaked_fill01,gl_TexCoord[0].st);
	cTex03 = texture2D(texBaked_fill02,gl_TexCoord[0].st);
	cTex04 = texture2D(texBaked_keyrabbit,gl_TexCoord[0].st);
	cTex05 = texture2D(texBaked_checker,gl_TexCoord[0].st);

	cFinal = max(_mix01*cTex01,0.0) + max(_mix02*cTex02,0.0) + max(_mix03*cTex03,0.0) + max(_mix04*cTex04,0.0);

	gl_FragColor = cFinal;
}