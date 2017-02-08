//varying vec3 normal, lightDir;
uniform sampler2D tex;

void main()
{
	vec4 color;
	vec4 texel;
	float alpha,r,g,b ;
	
	texel = texture2D(tex,gl_TexCoord[0].st);
	alpha = texel.a;
	
	r = ((-(alpha-1.0)*(alpha-1.0)/0.1)+1.0);
	g = (5.0*(-((alpha-0.5)*(alpha-0.5))/0.4)+1.0);
	b = ((-(alpha * alpha)/0.15)+1.0);
	
	if (r<0) r=0 ;
	if (g<0) g=0 ;
	if (b<0) b=0 ;
	
	color = vec4(r,g,b,alpha);
	gl_FragColor = color;
}
