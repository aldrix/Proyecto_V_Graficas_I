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

uniform bool _pattern01;
uniform bool _pattern02;
uniform bool _pattern03;
uniform bool _pattern04;

uniform bool _filtering;

uniform int _iheight;
uniform int _iwidth;

#define texel_size_x 1.0 / _iwidth
#define texel_size_y 1.0 / _iheight	


/*Funcion que permite aplicar un filtro bilineal a una textura*/
vec4 texture2D_bilinear( sampler2D tex, vec2 uv ) {
	vec2 f;

	f.x	= fract( uv.x * _iwidth );
	f.y	= fract( uv.y * _iheight );

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

	//Tomamos las texturas.
	vec4 cTex01 = texture2D(texBaked_flat, gl_TexCoord[0].st);
	vec4 cTex02 = texture2D(texBaked_fill01, gl_TexCoord[0].st);
	vec4 cTex03 = texture2D(texBaked_fill02, gl_TexCoord[0].st);
	vec4 cTex04 = texture2D(texBaked_keyrabbit, gl_TexCoord[0].st);
	vec4 cTex05 = texture2D(texBaked_checker, gl_TexCoord[0].st);

	//Activamos el filtro bilineal si se presiona la tecla correspondiente.
	if (_filtering) { 
		cTex01 = texture2D_bilinear(texBaked_flat, gl_TexCoord[0].st);
		cTex02 = texture2D_bilinear(texBaked_fill01, gl_TexCoord[0].st);
		cTex03 = texture2D_bilinear(texBaked_fill02, gl_TexCoord[0].st);
		cTex04 = texture2D_bilinear(texBaked_keyrabbit, gl_TexCoord[0].st);
		cTex05 = texture2D_bilinear(texBaked_checker, gl_TexCoord[0].st);
	}

	//Colores para el patron del piso.
	vec4 color00 = vec4(0,0,0,cTex05.w);     //Color neutro.
	vec4 color01 = vec4(0,0.6,0.6,cTex05.w); //Color cian.
	vec4 color02 = vec4(0.7,0.6,0,cTex05.w); //Amarillo tostado.
	vec4 color03 = vec4(0.4,0,0.7,cTex05.w); //Morado.
	vec4 color04 = vec4(0,0.7,0,cTex05.w);   //Verde limon.

	if (_pattern01) 
		colorPattern = color01;
	else if (_pattern02) 
			colorPattern = color02;
	else if (_pattern03) 
			colorPattern = color03;
	else if (_pattern04) 
			colorPattern = color04;
	else 
		colorPattern = color00;

	//Modificamos las componentes RGB.
	cTex02 = vec4(max(_rfill01,1.0)*cTex02.x   , max(_gfill01,1.0)*cTex02.y   , max(_bfill01,1.0)*cTex02.z   , cTex02.w);
	cTex03 = vec4(max(_rfill02,1.0)*cTex03.x   , max(_gfill02,1.0)*cTex03.y   , max(_bfill02,1.0)*cTex03.z   , cTex03.w);
	cTex04 = vec4(max(_rkeyrabbit,1.0)*cTex04.x, max(_gkeyrabbit,1.0)*cTex04.y, max(_bkeyrabbit,1.0)*cTex04.z, cTex04.w);

	//Obtenemos la imagen final.
	cFinal = (max(_mix01,0.0)*cTex01*max(_mix04,0.0)*cTex04 +  
	         max(_mix01,0.0)*cTex01*max(_mix02,0.0)*cTex02 + 
			 max(_mix01,0.0)*cTex01*max(_mix03,0.0)*cTex03);
	

	gl_FragColor = cFinal * mix(cFinal, colorPattern*cTex05, 0.5);
}