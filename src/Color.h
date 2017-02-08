
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _Color_h_
#define _Color_h_

/** 
  @version 0.1
  @author Gangster
  @date Juin 2007
  \nosubgrouping
 */

class Color
{
public:
    float r, g, b, a;
    Color(float _r=1., float _g=1., float _b=1., float _a=1.)
        : r(_r), g(_g), b(_b), a(_a)
    {
    }
    Color(const Color& c)
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
    }
    inline Color& operator = ( const Color& c )
    {
        r = c.r;
        g = c.g;
        b = c.b;
        a = c.a;
        return *this;
    }
    inline Color operator * (float f) const
    {
        Color c;
        c.r = r * f;
        c.g = g * f;
        c.b = b * f;
        c.a = a;
        return c;
    }
    inline Color& operator *= (float f)
    {
        r *= f;
        g *= f;
        b *= f;
        a *= f;
        return *this;
    }
    inline Color& operator += (const Color& c)
    {
        r += c.r;
        g += c.g;
        b += c.b;
        a += c.a;
        return *this;
    }
    inline operator float* ()
    {
        return &r;
    }
    inline operator const float* () const
    {
        return &r;
    }
    inline void fromLerp(float t, const Color& c0, const Color& c1)
    {
        float t1 = (1.-t);
        r = t1*c0.r + t*c1.r;
        g = t1*c0.g + t*c1.g;
        b = t1*c0.b + t*c1.b;
        a = t1*c0.a + t*c1.a;
    }
    inline void fromBGRA(unsigned long rgba)
    {
        r = ((float)(rgba>>16 & 0xff))/255.;
        g = ((float)(rgba>>8 & 0xff))/255.;
        b = ((float)(rgba>>0 & 0xff))/255.;
        a = ((float)(rgba>>24 & 0xff))/255.;
    }
    inline void fromRGBA(unsigned long rgba)
    {
        r = ((float)(rgba & 0xff))/255.;
        g = ((float)(rgba>>8 & 0xff))/255.;
        b = ((float)(rgba>>16 & 0xff))/255.;
        a = ((float)(rgba>>24 & 0xff))/255.;
    }
    inline void fromRGB(unsigned long rgba)
    {
        r = ((float)(rgba & 0xff))/255.;
        g = ((float)(rgba>>8 & 0xff))/255.;
        b = ((float)(rgba>>16 & 0xff))/255.;
        a = 1.;
    }
    inline void fromBGR(unsigned long rgba)
    {
        b = ((float)(rgba & 0xff))/255.;
        g = ((float)(rgba>>8 & 0xff))/255.;
        r = ((float)(rgba>>16 & 0xff))/255.;
        a = 1.;
    }
    inline unsigned long toRGBA(void) const
    {
        unsigned char val8;
        unsigned long val32 = 0;

        // Convert to 32bit pattern
        // (ABRG = 8888)

        // Alpha
        val8 = (unsigned char)(a * 255);
        val32 = val8 << 24;

        // Blue
        val8 = (unsigned char)(b * 255);
        val32 += val8 << 16;

        // Green
        val8 = (unsigned char)(g * 255);
        val32 += val8 << 8;

        // Red
        val8 = (unsigned char)(r * 255);
        val32 += val8;


        return val32;
    }
    inline unsigned long toABGR(void) const
    {
        unsigned char val8;
        unsigned long val32 = 0;

        // Convert to 32bit pattern
        // (RGBA = 8888)

        // Red
        val8 = (unsigned char)(r * 255);
        val32 = val8 << 24;

        // Green
        val8 = (unsigned char)(g * 255);
        val32 += val8 << 16;

        // Blue
        val8 = (unsigned char)(b * 255);
        val32 += val8 << 8;

        // Alpha
        val8 = (unsigned char)(a * 255);
        val32 += val8;

        return val32;
    }

    static const Color WHITE;
    static const Color BLACK;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color GREY30;
    static const Color GREY50;
    static const Color GREY80;
    static const Color YELLOW;
};


#endif
