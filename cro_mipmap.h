/*
CRO Libaries -- Mipmap Generation -- public domain
        no warranty is offered or implied; use this code at your own risk

C code for created various types of mipmaps for textures.

Pixel types supported: 8 bit 4 channels (i.e. 32-bit such as RGBA, ABGR, etc)
                       32-bit floating point pixels (ie depth textures)

Down sampling functions: Average (traditional mipmap generation)
                         Minimum (for depth values)
                         Maximum (for depth values)
			 Minimum and maximum (done at the same time to 
			                      avoid redundant work of 
					      deparate calls)

=========================================================================
LICENSE

This software is in the public domain. Where that dedication is not
recognized, you are granted a perpetual, irrevocable license to copy,
distribute, and modify this file as you see fit.
=========================================================================

Credits:
    Written by Cody Olivier


 */

#ifndef cro_mipmap_h
#define cro_mipmap_h

#ifdef CRO_Min
#undef CRO_Min;
#endif

#ifdef CRO_Max
#undef CRO_Max;
#endif

#define CRO_Max( x, y ) ( (x) > (y) ? (x) : (y) )
#define CRO_Min( x, y ) ( (x) < (y) ? (x) : (y) )

static inline void cro_GetMipMapSize( unsigned int width, unsigned int height,
				 unsigned int *newWidth, unsigned int *newHeight)
{
  *newWidth = width >> 1;
  *newHeight = height >> 1;
}

//
//----------------------------------------------------
//    
// Integer MipMaps
// -These functions assume that each pixel is made up of 4 channels, each 1 byte.
// -Odd image dimensions will be rounded down when down sampled.
//     -Odd width will ignore last column, odd height will ignore last row
// -These functions will not allocate memory. If you are unsure how big the 
//  generated mip map will be, use cro_GetMipMapSize.
//
// Main Use: 32-bit color images
//
//----------------------------------------------------
//
static inline int cro_AvgBits( int n1, int n2, int n3, int n4, int bitShifts )
{
	int byte1 = (n1 >> bitShifts) & 0xff;
	int byte2 = (n2 >> bitShifts) & 0xff;
	int byte3 = (n3 >> bitShifts) & 0xff;
	int byte4 = (n4 >> bitShifts) & 0xff;
	return ( byte1 + byte2 + byte3 + byte4 ) >> 2;
}

static inline int cro_MaxBits( int n1, int n2, int n3, int n4, int bitShifts )
{

	int byte1 = (n1 >> bitShifts) & 0xff;
	int byte2 = (n2 >> bitShifts) & 0xff;
	int byte3 = (n3 >> bitShifts) & 0xff;
	int byte4 = (n4 >> bitShifts) & 0xff;
	return CRO_Max( CRO_Max( byte1, byte2 ), CRO_Max( byte3, byte4 ) );
}

static inline int cro_MinBits( int n1, int n2, int n3, int n4, int bitShifts )
{
	int byte1 = (n1 >> bitShifts) & 0xff;
	int byte2 = (n2 >> bitShifts) & 0xff;
	int byte3 = (n3 >> bitShifts) & 0xff;
	int byte4 = (n4 >> bitShifts) & 0xff;
	return CRO_Min( CRO_Min( byte1, byte2 ), CRO_Min( byte3, byte4 ) );
}


static inline void cro_MinMaxBits( int n1, int n2, int n3, int n4, int bitShifts, int *min, int *max )
{
  int byte1 = (n1 >> bitShifts) & 0xff;
  int byte2 = (n2 >> bitShifts) & 0xff;
  int byte3 = (n3 >> bitShifts) & 0xff;
  int byte4 = (n4 >> bitShifts) & 0xff;

  int max1, max2, min1, min2;

  if ( byte1 > byte2 )
  {
    max1 = byte1;
    min1 = byte2;
  }
  else
  {
    max1 = byte2;
    min1 = byte1;    
  }

  if ( byte3 > byte4 )
  {
    max2 = byte3;
    min2 = byte4;
  }
  else
  {
    max2 = byte4;
    min2 = byte3;    
  }

  *min = CRO_Min( min1, min2 );
  *max = CRO_Max( max1, max2 );
}

static inline int cro_GetMipMapLevels( unsigned int width, unsigned int height )
{
	int levels = 0;
	while ( width > 1 && height > 1 )
	{
		width >>= 1;
		height >>= 1;
		levels++;
	}
	return levels;
}

#define CRO_GenMipMapI_Template( name, fn )			       \
  static inline void cro_GenMipMap##name##I( const int *image, unsigned int width, unsigned int height, int *newImage ) \
  {								       \
    if ( width == 0 || height == 0 )				       \
      return;							       \
    								       \
    unsigned int newWidth, newHeight;					\
    int *newPixel = newImage;						\
    cro_GetMipMapSize( width, height, &newWidth, &newHeight );	\
									\
    for ( int h = 0; h < newHeight; ++h )				\
    {									\
      const int *row1 = &image[ width * ( 2 * h + 0 ) ];		\
      const int *row2 = &image[ width * ( 2 * h + 1 ) ];		\
      for ( int w = 0; w < newWidth; ++w, ++newPixel, row1 += 2, row2 += 2 ) \
	{								\
	  int p1 = row1[0];						\
	  int p2 = row2[0];						\
	  int p3 = row1[1];						\
	  int p4 = row2[1];						\
	  								\
	  int byte1 = fn( p1, p2, p3, p4, 24 );				\
	  int byte2 = fn( p1, p2, p3, p4, 16 );				\
	  int byte3 = fn( p1, p2, p3, p4, 8 );				\
	  int byte4 = fn( p1, p2, p3, p4, 0 );				\
	  								\
	  *newPixel = ( byte1 << 24 ) |					\
	    ( byte2 << 16 ) |						\
	    ( byte3 << 8  ) |						\
	    ( byte4 << 0  );						\
	}								\
    }									\
  }

// template macros for: cro_GenMipMapAvgI
//                      cro_GenMipMapMinI
//                      cro_GenMipMapMaxI
CRO_GenMipMapI_Template( Avg, cro_AvgBits );
CRO_GenMipMapI_Template( Min, cro_MinBits );
CRO_GenMipMapI_Template( Max, cro_MaxBits );

static inline void cro_GenMipMapMinMaxI( const int *image, unsigned int width, unsigned int height, int *minMip, int *maxMip )
{
  if ( width == 0 || height == 0 )
    return;

  unsigned int newWidth, newHeight;
  int *minPixel = minMip;
  int *maxPixel = maxMip;
  cro_GetMipMapSize( width, height, &newWidth, &newHeight );
  
  for ( int h = 0; h < newHeight; ++h )
  {
    const int *row1 = &image[ width * ( 2 * h + 0 ) ];
    const int *row2 = &image[ width * ( 2 * h + 1 ) ];
    for ( int w = 0; w < newWidth; ++w, ++minPixel, ++maxPixel, row1 += 2, row2 += 2 )
    {
      int p1 = row1[0];
      int p2 = row2[0];
      int p3 = row1[1];
      int p4 = row2[1];

      int max1, max2, max3, max4;
      int min1, min2, min3, min4;

      cro_MinMaxBits( p1, p2, p3, p4, 24, &min1, &max1 );
      cro_MinMaxBits( p1, p2, p3, p4, 16, &min2, &max2 );
      cro_MinMaxBits( p1, p2, p3, p4, 8, &min3, &max3 );
      cro_MinMaxBits( p1, p2, p3, p4, 0, &min4, &max4 );
      
      *minMip = ( min1 << 24 ) | ( min2 << 16 ) | ( min3 << 8 ) | ( min4 << 0  );
      *maxMip = ( max1 << 24 ) | ( max2 << 16 ) | ( max3 << 8 ) | ( max4 << 0  );
    }
  }
}

//
//----------------------------------------------------
//    
// Float MipMaps
// -Odd image dimensions will be rounded down when down sampled.
//     -Odd width will ignore last column, odd height will ignore last row
// -These functions will not allocate memory. If you are unsure how big the 
//  generated mip map will be, use cro_GetMipMapSize.
//
// Main Use: determining min/max values of a depth texture.
//           Avg varient is added for completeness
//
//----------------------------------------------------
//
static inline float cro_AvgF( float n1, float n2, float n3, float n4 )
{
  //NOTE: doesn't take into account if sum overflows before being multiplied
  return ( n1 + n2 + n3 + n4 ) * 0.25f;
}

static inline float cro_MinF( float n1, float n2, float n3, float n4 )
{
  return CRO_Min( CRO_Min( n1, n2 ), CRO_Min( n3, n4 ));
}

static inline float cro_MaxF( float n1, float n2, float n3, float n4 )
{
  return CRO_Max( CRO_Max( n1, n2 ), CRO_Max( n3, n4 ));
}

static inline void cro_MinMaxF( float n1, float n2, float n3, float n4, float *min, float *max )
{
  float max1, max2, min1, min2;

  if ( n1 > n2 )
  {
    max1 = n1;
    min1 = n2;
  }
  else
  {
    max1 = n2;
    min1 = n1;    
  }

  if ( n3 > n4 )
  {
    max2 = n3;
    min2 = n4;
  }
  else
  {
    max2 = n4;
    min2 = n3;    
  }

  *min = CRO_Min( min1, min2 );
  *max = CRO_Max( max1, max2 );
}


#define CRO_GenMipMapF_Template( name, fn )			        \
  static inline void cro_GenMipMap##name##F( const float *image, unsigned int width, unsigned int height, float *newImage ) \
  {                                                                     \
    if ( width == 0 || height == 0 )				        \
      return;							        \
                                                                        \
    unsigned int newWidth, newHeight;					        \
    float *newPixel = newImage;					        \
    cro_GetMipMapSize( width, height, &newWidth, &newHeight );        \
                                                                        \
    for ( int h = 0; h < newHeight; ++h )                               \
    {                                                                   \
      const float *row1 = &image[ width * ( 2 * h + 0 ) ];              \
      const float *row2 = &image[ width * ( 2 * h + 1 ) ];	        \
      for ( int w = 0; w < newWidth; ++w, ++newPixel, row1 += 2, row2 += 2 ) \
      {								        \
        float p1 = row1[0];                                             \
        float p2 = row2[0];                                             \
        float p3 = row1[1];                                             \
        float p4 = row2[1];                                             \
        *newPixel = fn( p1, p2, p3, p4 );                               \
      }                                                                 \
    }									\
  }

// template macros for: cro_GenMipMapAvgF
//                      cro_GenMipMapMinF
//                      cro_GenMipMapMaxF
CRO_GenMipMapF_Template( Avg, cro_AvgF );
CRO_GenMipMapF_Template( Min, cro_MinF );
CRO_GenMipMapF_Template( Max, cro_MaxF );

static inline void cro_GenMipMapMinMaxF( const float *image, unsigned int width, unsigned int height, float *minMip, float *maxMip )
{
  if ( width == 0 || height == 0 )
    return;

  unsigned int newWidth, newHeight;
  float *minPixel = minMip;
  float *maxPixel = maxMip;
  cro_GetMipMapSize( width, height, &newWidth, &newHeight );
  
  for ( int h = 0; h < newHeight; ++h )
  {
    const float *row1 = &image[ width * ( 2 * h + 0 ) ];
    const float *row2 = &image[ width * ( 2 * h + 1 ) ];
    for ( int w = 0; w < newWidth; ++w, ++minPixel, ++maxPixel, row1 += 2, row2 += 2 )
    {
      int p1 = row1[0];
      int p2 = row2[0];
      int p3 = row1[1];
      int p4 = row2[1];
      cro_MinMaxF( p1, p2, p3, p4, minPixel, maxPixel );
    }
  }
}

#endif
