#ifndef __OFF_SCREEN_RENDERING__
#define __OFF_SCREEN_RENDERING__
#include <GL/glew.h>
#include <GL/glx.h>

/** Off screen rendering class. OffScreenRendering

	This class is designed to make off screen rendering method transparent to the user.

	See the provided offscreendemo.cpp source file to have informations about using it.
	@version 0.1
	@author Mathias Paulin - IRIT-UPS-CNRS
	@date January 2002
	@todo Things that remains to verify or develop.
	@todo Verify robustness of the class.
	@todo Port to windows.
	@todo Test under IRIX.

	\nosubgrouping

*/

/*
    17/10/2003 - Gael - new methods : getWidth() and getHeight()
    09/11/2005 - Gael - It is now allowed to use the copyToTexture method inside a block begin/end
*/

class OffScreenRendering {
	// static private method and attributes
	private :
		static const char* _errorMessages[];
		static int OffScreenRenderingQueryForPbuffers(Display *dpy, int screen);
#ifndef IRIX
		static GLXPbuffer OffScreenRenderingCreatePbuffer( Display *dpy, GLXFBConfigSGIX fbConfig, int width, int height, int *pbAttribs );
#else
		static GLXPbufferSGIX OffScreenRenderingCreatePbuffer( Display *dpy, GLXFBConfigSGIX fbConfig, int width, int height, int *pbAttribs );
#endif
	// public inner enumeration for error handling
	public :
		/** @name Error management.
			\anchor ErrorGroup
			Error management types and methods.
			Error management works as follow :
			- methods never stops the calling programm, even in case of error
			- methods set the last error code on the state machine
			- the caller must ensure that no error occurs after a call to a method
		*/
		/*@{*/
		/** This enum indicates the error number of the last off screen rendering command.
			The values could be returned bye the function \ref getError () .
			To obtain a human readable description of this error, call \ref getErrorMsg () .
		*/
		enum OffScreenRenderingError {
			NO_ERROR = 0,  			/*!< OffScreenRendering : no error. */
			MISSING_GL_EXTENSIONS,  /*!< OffScreenRendering : unsupported needed GL extensions. */
			MISSING_X_DISPLAY,  	/*!< OffScreenRendering : couldn't get a valid X display. */
			PBUFFERS_UNAVAILABLE, 	/*!< OffScreenRendering : missing GLX Pbuffer extension. */
			INVALID_PARAMETERS, 	/*!< OffScreenRendering : invalid parameters for pbuffer creation. */
			INVALID_CONTEXT, 		/*!< OffScreenRendering : invalid off screen context. */
			INTERNAL_OPENGL_ERROR, 	/*!< OffScreenRendering : Internal openGL Error, use glGetError() for more info. */
			ACTIVATE_FAIL, 			/*!< OffScreenRendering : glXMakeCurrent failed for offscreen rendering. */
			DESACTIVATE_FAIL, 		/*!< OffScreenRendering : glXMakeCurrent failed for old context. */
			VISUAL_INFO_ERROR,		/*!< OffScreenRendering : can't get XVisualInfo from FBconfig. */
			CONTEXT_CREATION_ERROR,	/*!< OffScreenRendering : couldn't create GLXContext */
			NO_CUR_CTX_AVAILABLE,	/*!< OffScreenRendering : Unable to find an active context for sharing. */
			INVALID_OPERATION,		/*!< OffScreenRendering : Calling of a non authorized method beetween begin() and end(). */
			BLOCK_ERROR,			/*!< OffScreenRendering : Wrong begin() - end() structure. */
			NOT_YET_IMPLEMENTED,	/*!< OffScreenRendering : really, not yet implemented ~8-) !!! */
		};

		/** Gets the last error code.
			\anchor getError
			@return error code. Use \ref getErrorMsg () to have more informations
			\sa \ref ErrorGroup "Error management"
		*/
		OffScreenRenderingError getError() const;

		/** Gets the last error message.
			\anchor getErrorMsg
			@return error message build from the last \ref ErrorGroup "Error management".
		*/
		const char *getErrorMsg() const;

		/*@}*/

	// private data and member methods
	private :
		// For X connection
		Display *_currentDisplay;

		/** @name Context switching management attributes.
			These attributes are used for management of rendering context switching.
			Each attribute is set when calling \ref begin () and used whe calling \ref end () to restore the previous
			rendering context.
		*/
		/*@{*/

		/**	the active rendering context before activation of the off screen rendering context.
		*/
		GLXContext _previousContext;

		/**	the active drawable context before activation of the off screen rendering context.
		*/
		GLXDrawable _previousDrawable;

		/*@}*/

		/** @name Off screen Context related attributes.
			These attributes are used for management of rendering context switching.
			Each attribute is initialized in the constructor and used when calling \ref begin () set the off screen
			rendering context.
		*/
		/*@{*/
		/** The "real" off screen rendering context.
		*/
		GLXContext _currentContext;
		/** The drawable for offscreen rendering (a p-buffer).
		*/
#ifndef IRIX
		GLXPbuffer _currentPbuffer;
#else
		GLXPbufferSGIX _currentPbuffer;
#endif
		/** The frame buffer config for the offscreen rendering context.
		*/
		GLXFBConfigSGIX _currentFBconfig;

		/*@}*/

		/** @name Internal state variable attributes.
			These attributes are used for error management and internal state checking.
		*/
		/*@{*/

		/** Indicates if the off screen context is usable.
		*/
		bool _validOffScreenRenderingObject;

		/** The last occured internal error.
			\sa \ref ErrorGroup "Error management"
		*/
		OffScreenRenderingError _lastError;

		/** begin() - end() strucutre state.
		*/
		bool _inBlock;

		/** Width of the PBuffer.
		*/
		int _width;

		/** Height of the PBuffer.
		*/
		int _height;

		/*@}*/

		// Initialisation of the PBuffer
#ifndef IRIX
		GLXPbuffer MakePbuffer( Display *dpy, int screen, int width, int height );
#else
		GLXPbufferSGIX MakePbuffer( Display *dpy, int screen, int width, int height );
#endif

	// public member methods
	public :
		/** @name Constructor and destructor.
		*/
		/*@{*/
		/** OffScreenRendering : create an offscreen rendering context through the allocation of a pbuffer.
			Use \ref getErrorMsg () to have informations about the created (or not) rendering context.

			The created OffScreenRendering context has the following properties :
			Single buffered.
			Size given by width and height parameters.
			RGBA has 8888 bits.
			Stencil has 8 bits.
			Depth has 24 bits.

			@param width width of the offscreen rendering context
			@param height height of the offscreen rendering context
			@param share indicate if we must share display_lists and textures with the current rendering context

			@todo 	Make off screen context parameters modifiable (Color depth, Number of buffer ...).
			@bug 	There is no verification of the upper limit in the size required by the user.
					May kill the X-Server if the size is too high.
		*/
		OffScreenRendering(int width=256,int height=256, bool share=false);

		/**	Destroy the OffScreenRendering context.
		*/
		~OffScreenRendering();
		/*@}*/

		/** @name Context switching methods.
		*/
		/*@{*/

		/** Activate the OffScreenRendering context.
			\anchor begin
			When an OffScreenRendering context is activated, all the subsequents calls to OpenGL functions
			implie that the target rendering context is this OffScreenRendering context.

			@return error code. Use \ref getErrorMsg () to have more informations.
		*/
		OffScreenRenderingError begin();

		/** Desactivate the OffScreenRendering context.
			\anchor end
			When an OffScreenRendering context is desactivated, the old rendering context, obtained when begin was
			called is restore.

			@return error code. Use \ref getErrorMsg () to have more informations.
		*/
		OffScreenRenderingError end();
		/*@}*/

		/** @name Data extraction methods.
		*/
		/*@{*/
		/** Copy the content of the colorBuffer from the OffScreenRendering context to the gltextureObject.

			When this function is called, it first verify if the gltextureObject is a valid one.
			If true, the assumption that texture size matches pbuffer size is made. If assumption fails,
			may kill the X Server.
			If gltextureObject is  valid, moves the color buffer content to this texture object as
			level-of-detail number 0.
			If gltextureObject is  valid, eventually deletes this texture object and allocate a new
			one with the content of the pbuffer as pixel.

			Note that this method does not alter the current rendering context neither the content of the
			OffScreenRendering buffers.

			@param gltextureObject : the texture object to use or create;
			@return error code. Use \ref getErrorMsg () to have more informations.
		*/
		OffScreenRenderingError copyToTexture(unsigned int *gltextureObject, GLenum texture=GL_TEXTURE_2D,GLenum target=GL_TEXTURE_2D);

		/** Copy the content of the specified buffer to provided buffer.

			@param x, y Specify the window coordinates of the first pixel that
            	   is read from the frame buffer.  This location is the
            	   lower left corner of a rectangular block of pixels.
			@param width, height
            	   Specify the dimensions of the pixel rectangle.  width
            	   and height of one correspond to a single pixel.
			@param format
            	   Specifies the format of the pixel data.  The following
            	   symbolic values are accepted:  GL_COLOR_INDEX,
            	   GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_RED, GL_GREEN,
            	   GL_BLUE, GL_ALPHA, GL_RGB, GL_RGBA, GL_LUMINANCE, and
            	   GL_LUMINANCE_ALPHA.
			@param type Specifies the data type of the pixel data.  Must be one
            	   of GL_UNSIGNED_BYTE, GL_BYTE, GL_BITMAP,
            	   GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT,
            	   or GL_FLOAT.
			@param bufferOut
            	   Returns the pixel data.
			@return error code. Use \ref getErrorMsg () to have more informations.
		*/
		OffScreenRenderingError copyToMemory(int x, int y, int width, int height, GLenum format, GLenum type, void *bufferOut);

        int getWidth(void);
        int getHeight(void);
		/*@}*/


};
#endif
