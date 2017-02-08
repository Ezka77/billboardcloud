
/***************************************************************************
 *   Copyright (C) 2005 by Gael GUENNEBAUD                                 *
 *   guenneba@irit.fr                                                      *
 *                                                                         *
 ***************************************************************************/
 

#ifndef _OpenGL_h_ 
#define _OpenGL_h_

#include <GL/glew.h>
// disable auto-include of the glext.h file
#define GL_GLEXT_LEGACY        1

//#include <GL/gl.h>

// get the definition of all extension functions
#define GL_GLEXT_PROTOTYPES    1

#ifndef GLAPI
// the glext.h nvidia file contains some bugs, so :
#define GLAPI APIENTRY
#endif

#ifndef GL_EXT_blend_equation_separate
#define GL_EXT_blend_equation_separate 1
#ifdef GL_GLEXT_PROTOTYPES
extern void glBlendEquationSeparateEXT (GLenum, GLenum);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (* PFNGLBLENDEQUATIONSEPARATEEXTPROC) (GLenum modeRGB, GLenum modeAlpha);
#endif


#include <GL/glext.h>

#include <GL/glu.h>


#define _SUPPORT_GLSLANG_
#define _SUPPORT_STENCIL_TWO_SIDE_


#ifdef DEBUG
    #define GL_TEST_ERR\
        {\
            GLenum eCode;\
            if((eCode=glGetError())!=GL_NO_ERROR)\
                std::cerr << "OpenGL error : " <<  gluErrorString(eCode) << " in " <<  __FILE__ << " : " << __LINE__ << std::endl;\
        }
#else
    #define GL_TEST_ERR  
#endif


#endif

 
