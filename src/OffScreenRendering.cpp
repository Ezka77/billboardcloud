
#define GLX_GLXEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/glxext.h>

#include "OffScreenRendering.h"
#ifndef IRIX
#include <iostream>
#include <cstring>
using namespace std;
#else
#include <iostream.h>
#include <string.h>
#endif
// private Offscreen rendering data and methods

const char* OffScreenRendering::_errorMessages[] = {
	"OffScreenRendering : no error",
	"OffScreenRendering : unsupported needed GL extensions",
	"OffScreenRendering : couldn't get a valid X display",
	"OffScreenRendering : missing GLX Pbuffer extension",
	"OffScreenRendering : invalid parameters for pbuffer creation",
	"OffScreenRendering : invalid off screen context",
	"OffScreenRendering : Internal openGL Error, use glGetError() for more info",
	"OffScreenRendering : glXMakeCurrent failed for offscreen rendering",
	"OffScreenRendering : glXMakeCurrent failed for old context",
	"OffScreenRendering : can't get XVisualInfo from FBconfig",
	"OffScreenRendering : couldn't create GLXContext",
	"OffScreenRendering : Unable to find an active context for sharing.",
	"OffScreenRendering : realy, not yet implemented ~8-) !!! ",
	None
};

int OffScreenRendering::OffScreenRenderingQueryForPbuffers(Display *dpy, int screen)
{
#if defined(GLX_SGIX_fbconfig) && defined(GLX_SGIX_pbuffer)
	char *extensions;

	extensions = (char *) glXQueryServerString(dpy, screen, GLX_EXTENSIONS);
	if (!strstr(extensions,"GLX_SGIX_fbconfig")) {
		return 0;
	}
	if (!strstr(extensions,"GLX_SGIX_pbuffer")) {
		return 0;
	}
    return 1;
#else
#error GLX_SGIX_fbconfig and GLX_SGIX_pbuffer not defined at compile time
   return 0;
#endif
}


static int OffScreenRenderingXErrorFlag = 0;
static int OffScreenRenderingHandleXError( Display *dpy, XErrorEvent *event )
{
    dpy=NULL;
    event=NULL;
    OffScreenRenderingXErrorFlag = 1;
    return 0;
}

#ifndef IRIX
GLXPbuffer OffScreenRendering::OffScreenRenderingCreatePbuffer( Display *dpy, GLXFBConfigSGIX fbConfig, int width, int height, int *pbAttribs )
#else
GLXPbufferSGIX OffScreenRendering::OffScreenRenderingCreatePbuffer( Display *dpy, GLXFBConfigSGIX fbConfig, int width, int height, int *pbAttribs )
#endif
{
	int (*oldHandler)( Display *, XErrorEvent * );
#ifndef IRIX
	GLXPbuffer pBuffer = None;
#else
	GLXPbufferSGIX pBuffer = None;
#endif

	/* Catch X protocol errors with our own error handler */
	oldHandler = XSetErrorHandler( OffScreenRenderingHandleXError );

	OffScreenRenderingXErrorFlag = 0;

    pBuffer = glXCreateGLXPbufferSGIX(dpy, fbConfig, width, height, pbAttribs);


	/* Restore original X error handler */
	(void) XSetErrorHandler( oldHandler );

	/* Return pbuffer (may be None) */
	if (!OffScreenRenderingXErrorFlag && pBuffer!=None) {
		unsigned int w,h;
#ifndef IRIX
    	glXQueryGLXPbufferSGIX(dpy, pBuffer, GLX_WIDTH, &w);
    	glXQueryGLXPbufferSGIX(dpy, pBuffer, GLX_HEIGHT, &h);
#else
    	glXQueryGLXPbufferSGIX(dpy, pBuffer, GLX_WIDTH_SGIX, &w);
    	glXQueryGLXPbufferSGIX(dpy, pBuffer, GLX_HEIGHT_SGIX, &h);
#endif
		return pBuffer;
	} else {
		return None;
	}
}


#ifndef IRIX
GLXPbuffer OffScreenRendering::MakePbuffer( Display *dpy, int screen, int width, int height )
#else
GLXPbufferSGIX OffScreenRendering::MakePbuffer( Display *dpy, int screen, int width, int height )
#endif
{

	int fbAttribs[100] = {
			/* Single buffered, with depth buffer */
			GLX_RENDER_TYPE_SGIX, GLX_RGBA_BIT_SGIX,
			GLX_DRAWABLE_TYPE_SGIX, GLX_PIXMAP_BIT_SGIX,
			GLX_RED_SIZE, 1,
			GLX_GREEN_SIZE, 1,
			GLX_BLUE_SIZE, 1,
			GLX_DEPTH_SIZE, 1,
			GLX_ALPHA_SIZE, 1,
			GLX_DOUBLEBUFFER, 0,
			GLX_STENCIL_SIZE, 1,
			None
	};
	int pbAttribs[9] = {
		GLX_LARGEST_PBUFFER_SGIX, True,
		GLX_PRESERVED_CONTENTS_SGIX, False,
/*		GLX_PBUFFER_WIDTH,0,
		GLX_PBUFFER_HEIGHT,0,*/
		None
	};
	/* Create the pbuffer using first fbConfig in the list that works. */
//	pbAttribs[5]=width+1;
//	pbAttribs[7]=height;


	GLXFBConfigSGIX *fbConfigs;
#ifndef IRIX
	GLXPbuffer pBuffer = None;
#else
	GLXPbufferSGIX pBuffer = None;
#endif
	int nConfigs;
	int i;
	int redSize, greenSize, blueSize, alphaSize;
	int depthSize, stencilSize;

	/* Get list of possible frame buffer configurations */
#ifndef IRIX
	fbConfigs = glXChooseFBConfig(dpy, screen, fbAttribs, &nConfigs);
#else
	fbConfigs = glXChooseFBConfigSGIX(dpy, screen, fbAttribs, &nConfigs);
#endif
	if (nConfigs==0 || !fbConfigs) {
		return None;
	}

#ifdef IRIX
cerr << nConfigs << " compatible trouvees\n";
#endif
	for (i=0;i<nConfigs;i++) {
		// search for first config such that rgba=8888,stencil=8,depth=24
#ifndef IRIX
		glXGetFBConfigAttrib(dpy, fbConfigs[i], GLX_RED_SIZE, &redSize);
		glXGetFBConfigAttrib(dpy, fbConfigs[i], GLX_GREEN_SIZE, &greenSize);
		glXGetFBConfigAttrib(dpy, fbConfigs[i], GLX_BLUE_SIZE, &blueSize);
		glXGetFBConfigAttrib(dpy, fbConfigs[i], GLX_ALPHA_SIZE, &alphaSize);
		glXGetFBConfigAttrib(dpy, fbConfigs[i], GLX_DEPTH_SIZE, &depthSize);
		glXGetFBConfigAttrib(dpy, fbConfigs[i], GLX_STENCIL_SIZE, &stencilSize);
#else
		glXGetFBConfigAttribSGIX(dpy, fbConfigs[i], GLX_RED_SIZE, &redSize);
		glXGetFBConfigAttribSGIX(dpy, fbConfigs[i], GLX_GREEN_SIZE, &greenSize);
		glXGetFBConfigAttribSGIX(dpy, fbConfigs[i], GLX_BLUE_SIZE, &blueSize);
		glXGetFBConfigAttribSGIX(dpy, fbConfigs[i], GLX_ALPHA_SIZE, &alphaSize);
		glXGetFBConfigAttribSGIX(dpy, fbConfigs[i], GLX_DEPTH_SIZE, &depthSize);
		glXGetFBConfigAttribSGIX(dpy, fbConfigs[i], GLX_STENCIL_SIZE, &stencilSize);
#endif
#ifdef IRIX
		cerr << "R"<< redSize << ", G"<< greenSize << ", B"<< blueSize << ", A"<< alphaSize;
		cerr << ", D"<< depthSize << ", S"<< stencilSize << endl;
		if ((redSize>=8)&&(greenSize>=8)&&(blueSize>=8)&&(alphaSize>=8)&&(depthSize>=24)&&(stencilSize>=8)) {
			pBuffer = OffScreenRendering::OffScreenRenderingCreatePbuffer(dpy, fbConfigs[i], width, height, pbAttribs);
			if (pBuffer) {
				_currentFBconfig = fbConfigs[i];
				break;
			}
		}

#else
		if ((redSize>=8)&&(greenSize>=8)&&(blueSize>=8)&&(alphaSize>=8)&&(depthSize>=24)&&(stencilSize>=8)) {
			pBuffer = OffScreenRendering::OffScreenRenderingCreatePbuffer(dpy, fbConfigs[i], width, height, pbAttribs);
			if (pBuffer) {
				_currentFBconfig = fbConfigs[i];
				break;
			}
		}
#endif
	}

	XFree(fbConfigs);

	return pBuffer;
}



// Begin of OffScreenRendering callable method
OffScreenRendering::OffScreenRendering(int width,int height, bool share) : \
		_currentDisplay(0), _previousContext(0), _previousDrawable(0), _currentContext(0), \
		_currentPbuffer(0), _currentFBconfig(0), _validOffScreenRenderingObject(false), \
		_lastError(NO_ERROR), _inBlock(false),_width(width), _height(height)

{
	if ((width < 1) || (height < 1)) {
		_lastError=INVALID_PARAMETERS;
	} else {
		_currentDisplay=glXGetCurrentDisplay();
		if (_currentDisplay==NULL) {
			_currentDisplay=XOpenDisplay(NULL);
		}
		if (!_currentDisplay) {
			_lastError=MISSING_X_DISPLAY;
			_validOffScreenRenderingObject=false;
		} else {
			if ( OffScreenRendering::OffScreenRenderingQueryForPbuffers(_currentDisplay,DefaultScreen(_currentDisplay)) ) {
				_validOffScreenRenderingObject=true;
			} else {
				_lastError=PBUFFERS_UNAVAILABLE;
				_validOffScreenRenderingObject=false;
			}
		}
		if (_validOffScreenRenderingObject)
		{
			_currentPbuffer = MakePbuffer( _currentDisplay, DefaultScreen(_currentDisplay), _width, _height );
			if (_currentPbuffer!=None)
			{
#ifndef IRIX
   				XVisualInfo *visInfo = glXGetVisualFromFBConfig(_currentDisplay, _currentFBconfig);
#else
   				XVisualInfo *visInfo = glXGetVisualFromFBConfigSGIX(_currentDisplay, _currentFBconfig);
#endif
				if (!visInfo) {
					_lastError=VISUAL_INFO_ERROR;
					_validOffScreenRenderingObject=false;
				} else {
					if (share) {
						GLXContext activeContext=glXGetCurrentContext ();
						if (activeContext) {
							_currentContext = glXCreateContext(_currentDisplay, visInfo, activeContext, True);
						} else {
							_lastError=NO_CUR_CTX_AVAILABLE;
							_currentContext = glXCreateContext(_currentDisplay, visInfo, NULL, True);
						}
					} else {
						_currentContext = glXCreateContext(_currentDisplay, visInfo, NULL, True);
					}
					if (!_currentContext) {
						_lastError=CONTEXT_CREATION_ERROR;
						_validOffScreenRenderingObject=false;
					}else {
						_validOffScreenRenderingObject=true;
					}
					XFree(visInfo);
				}
			} else {
				_lastError=PBUFFERS_UNAVAILABLE;
				_validOffScreenRenderingObject=false;
			}
		}
	}
}

OffScreenRendering::~OffScreenRendering()
{
	if (_validOffScreenRenderingObject)
#ifndef IRIX
		glXDestroyPbuffer(_currentDisplay,_currentPbuffer);
#else
		glXDestroyGLXPbufferSGIX(_currentDisplay,_currentPbuffer);
#endif
}

OffScreenRendering::OffScreenRenderingError OffScreenRendering::begin()
{
	if (!_inBlock) {
		if (_validOffScreenRenderingObject) {
			_previousDrawable = glXGetCurrentDrawable();
			_previousContext = glXGetCurrentContext ();
			if (!glXMakeCurrent (_currentDisplay, _currentPbuffer, _currentContext)) {
				_lastError=ACTIVATE_FAIL;
			} else {
				_lastError=NO_ERROR;
				_inBlock=true;
			}
		} else {
			_lastError=INVALID_CONTEXT;
		}
	} else {
		_lastError=BLOCK_ERROR;
	}
	return _lastError;
}


OffScreenRendering::OffScreenRenderingError OffScreenRendering::end()
{
	if (_inBlock) {
		if (_validOffScreenRenderingObject) {
			if (!glXMakeCurrent (_currentDisplay, _previousDrawable, _previousContext)) {
					_lastError=DESACTIVATE_FAIL;
			} else {
				_lastError=NO_ERROR;
				_inBlock=false;
			}
		} else {
			_lastError=INVALID_CONTEXT;
		}
	} else {
		_lastError=BLOCK_ERROR;
	}
	return _lastError;
}

OffScreenRendering::OffScreenRenderingError OffScreenRendering::getError() const
{
	return _lastError;
}


const char *OffScreenRendering::getErrorMsg() const
{
	return _errorMessages[_lastError];
}

OffScreenRendering::OffScreenRenderingError OffScreenRendering::copyToTexture(unsigned int *gltextureObject, GLenum texture, GLenum target)
{
    if(!_validOffScreenRenderingObject)
    {
        _lastError=INVALID_CONTEXT;
        return _lastError;
    }

	if (!_inBlock)
    {
        begin();
        if (_lastError!=NO_ERROR)
            return _lastError;
        if (glIsTexture(*gltextureObject)) {
            glBindTexture(texture, *gltextureObject);
        } else {
            unsigned char *theTexture = new unsigned char [_width*_height*3];
            glGenTextures( 1, gltextureObject );
            glBindTexture(texture, *gltextureObject);
            glTexImage2D(target, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, theTexture);
            delete []theTexture;
        }

        glCopyTexSubImage2D( target, 0, 0, 0, 0, 0, _width, _height );
        _lastError=NO_ERROR;

        end();
	}
    else
    {
        if (glIsTexture(*gltextureObject)) {
            glBindTexture(texture, *gltextureObject);
        } else {
            unsigned char *theTexture = new unsigned char [_width*_height*3];
            glGenTextures( 1, gltextureObject );
            glBindTexture(texture, *gltextureObject);
            glTexImage2D(target, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, theTexture);
            delete []theTexture;
        }

        glCopyTexSubImage2D( target, 0, 0, 0, 0, 0, _width, _height );
        _lastError=NO_ERROR;
	}
	return _lastError;
}

OffScreenRendering::OffScreenRenderingError OffScreenRendering::copyToMemory(int x, int y, int width, int height, GLenum format, GLenum type, void *bufferOut)
{
	if (!_inBlock) {
		if (_validOffScreenRenderingObject) {
			begin();
			if (_lastError!=NO_ERROR)
				return _lastError;
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
   			glReadPixels(x, y, width, height, format, type, bufferOut);
			_lastError = (glGetError() == GL_NO_ERROR) ? NO_ERROR : INTERNAL_OPENGL_ERROR;
			if (_lastError!=NO_ERROR)
				return _lastError;
			end();
		} else {
			_lastError=INVALID_CONTEXT;
		}
	} else {
		_lastError=INVALID_OPERATION;
	}
	return _lastError;
}

int OffScreenRendering::getWidth(void)
{
    return _width;
}

int OffScreenRendering::getHeight(void)
{
    return _height;
}
