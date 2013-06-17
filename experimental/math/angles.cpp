
float pixelToRad( int pos, int maxPos, float maxRad )
{
	int delta; // pixel diff to 0 (0 is in the middle)
	float ang, x;
	
	/* maybe this will work?
	*     sin(ang) = delta/x
	*          ang = arcsin(delta/x)
	*  sin(maxRad/2) = maxPos/2/x <- I assume x is a constant value for all angles ...
	*            x = maxPos/2/sin(maxRad/2)
	*  =>      ang = arcsin(delta*sin(maxRad/2)/(maxPos/2))
	*/ 

	if ( pos == maxPos/2 )
	{
		return 0.0f; // in this case we don't have to calculate anything
	}
	else 
	{
		delta = pos - maxPos/2;
	}
	
	x = maxPos/2/sin( maxRad/2 );
	
	ang = asin( delta/x );
	
	return ang;
}

float pixelToDeg( int pos, int maxPos, float maxDeg )
{

	return radToDegree( pixelToRad( pos, maxPos, degreeToRad( maxDeg ) ) );

}

float degreeToRad( float degree ) 
{
	return degree*( 3.14159/180 );
}

float radToDegree( float rad ) 
{
	return rad*( 180/3.14159 );
}
