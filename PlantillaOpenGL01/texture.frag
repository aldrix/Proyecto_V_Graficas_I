uniform sampler2D texBaked_flat;
uniform sampler2D texBaked_fill01;
uniform sampler2D texBaked_fill02;
uniform sampler2D texBaked_keyrabbit;
uniform sampler2D texBaked_checker;

uniform float _mix01;
uniform float _mix02;
uniform float _mix03;
uniform float _mix04;

uniform float _rfill01;
uniform float _gfill01;
uniform float _bfill01;

uniform float _rfill02;
uniform float _gfill02;
uniform float _bfill02;

uniform float _rkeyrabbit;
uniform float _gkeyrabbit;
uniform float _bkeyrabbit;

uniform int _pattern01;
uniform int _pattern02;
uniform int _pattern03;
uniform int _pattern04;

uniform bool _filtering;

uniform bool _light1;
uniform bool _light2;
uniform bool _light3;
uniform bool _light4;

#define textureWidth 600.0
#define textureHeight 800.0	
#define texel_size_x 1.0 
#define texel_size_y 1.0 


vec4 texture2D_bilinear( sampler2D tex, vec2 uv ) {
	vec2 f;

	f.x	= fract( uv.x * textureWidth );
	f.y	= fract( uv.y * textureHeight );

	vec4 t00 = texture2D( tex, uv + vec2( 0.0, 0.0 ));
	vec4 t10 = texture2D( tex, uv + vec2( texel_size_x, 0.0 ));
	vec4 tA = mix( t00, t10, f.x);

	vec4 t01 = texture2D( tex, uv + vec2( 0.0, texel_size_y ) );
	vec4 t11 = texture2D( tex, uv + vec2( texel_size_x, texel_size_y ) );
	vec4 tB = mix( t01, t11, f.x );

	return mix( tA, tB, f.y );
}


void main(void) {

	vec4 cFinal;
	vec4 colorPattern;
	vec4 light = vec4(0.0,0.0,0.0,0.0);

	if (_filtering && _light1){
		light = vec4(1.0,0.0,0.0,1.0);
	} else if (_filtering && _light2){
		light = vec4(0.0,1.0,0.0,1.0);
	} else if (_filtering && _light3){
		light = vec4(0.0,0.0,1.0,1.0);
	} else{
		light = vec4(0.0,1.0,1.0,1.0);
	} 

	//Tomamos las texturas.
	vec4 cTex01 = texture2D(texBaked_flat, gl_TexCoord[0].st);;
	vec4 cTex02 = texture2D(texBaked_fill01, gl_TexCoord[0].st);
	vec4 cTex03 = texture2D(texBaked_fill02, gl_TexCoord[0].st);
	vec4 cTex04 = texture2D(texBaked_keyrabbit, gl_TexCoord[0].st);
	vec4 cTex05 = texture2D(texBaked_checker, gl_TexCoord[0].st);

	cTex04 = texture2D_bilinear(texBaked_keyrabbit, gl_TexCoord[0].st) * light;

	//Colores para el patron del piso.
	vec4 color00 = vec4(0,0,0,1);     //Color neutro.
	vec4 color01 = vec4(0,0.6,0.6,1); //Color cian.
	vec4 color02 = vec4(0.7,0.6,0,1); //Amarillo tostado.
	vec4 color03 = vec4(0.4,0,0.7,1); //Morado.
	vec4 color04 = vec4(0,0.7,0,1);   //Verde limon.

	if (_pattern01==1) 
		colorPattern = color01;
	else if (_pattern02==1) 
			colorPattern = color02;
	else if (_pattern03==1) 
			colorPattern = color03;
	else if (_pattern04==1) 
			colorPattern = color04;
	else 
		colorPattern = color00;

	//Modificamos las componentes RGB.
	cTex02 = vec4(max(_rfill01,1.0)*cTex02.x   , max(_gfill01,1.0)*cTex02.y   , max(_bfill01,1.0)*cTex02.z   , cTex02.w);
	cTex03 = vec4(max(_rfill02,1.0)*cTex03.x   , max(_gfill02,1.0)*cTex03.y   , max(_bfill02,1.0)*cTex03.z   , cTex03.w);
	cTex04 = vec4(max(_rkeyrabbit,1.0)*cTex04.x, max(_gkeyrabbit,1.0)*cTex04.y, max(_bkeyrabbit,1.0)*cTex04.z, cTex04.w);

	//Obtenemos la imagen final.
	cFinal = max(_mix01,0.0)*cTex01*max(_mix04,0.0)*cTex04 +  
	         max(_mix01,0.0)*cTex01*max(_mix02,0.0)*cTex02 + 
			 max(_mix01,0.0)*cTex01*max(_mix03,0.0)*cTex03;

	cFinal = mix(cFinal,colorPattern*cTex05,0.5);
	
	gl_FragColor = cFinal;
}