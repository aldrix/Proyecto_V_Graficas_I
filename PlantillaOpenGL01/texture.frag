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

uniform float _rfill01;
uniform float _gfill01;
uniform float _bfill01;

uniform float _rfill02;
uniform float _gfill02;
uniform float _bfill02;

uniform float _rkeyrabbit;
uniform float _gkeyrabbit;
uniform float _bkeyrabbit;

void main(void) {

	vec4 cFinal;

	vec4 cTex01 = texture2D(texBaked_flat, gl_TexCoord[0].st);;
	vec4 cTex02 = texture2D(texBaked_fill01, gl_TexCoord[0].st);
	vec4 cTex03 = texture2D(texBaked_fill02, gl_TexCoord[0].st);
	vec4 cTex04 = texture2D(texBaked_keyrabbit, gl_TexCoord[0].st);
	vec4 cTex05 = texture2D(texBaked_checker, gl_TexCoord[0].st);

	cTex02 = vec4(max(_rfill01,1.0)*cTex02.x, max(_gfill01,1.0)*cTex02.y, max(_bfill01,1.0)*cTex02.z, cTex02.w);
	cTex03 = vec4(max(_rfill02,1.0)*cTex03.x, max(_gfill02,1.0)*cTex03.y, max(_bfill02,1.0)*cTex03.z, cTex03.w);
	cTex04 = vec4(max(_rkeyrabbit,1.0)*cTex04.x, max(_gkeyrabbit,1.0)*cTex04.y, max(_bkeyrabbit,1.0)*cTex04.z, cTex04.w);

	cFinal = max(_mix01*cTex01,0.0) + max(_mix02*cTex02,0.0) + max(_mix03*cTex03,0.0) + max(_mix04*cTex04,0.0);

	gl_FragColor = cFinal;
}