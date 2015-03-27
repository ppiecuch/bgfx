/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * Copyright 2014-2015 Pawel Piecuch. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef BGFX_GLCONTEXT_QT_H_HEADER_GUARD
#define BGFX_GLCONTEXT_QT_H_HEADER_GUARD

#if BGFX_USE_QT || defined QT_OPENGL_LIB

namespace bgfx { namespace gl
{
	struct SwapChainGL;

	struct GlContext
	{
		GlContext() : m_current(NULL)
		{
		}

		void create(uint32_t _width, uint32_t _height);
		void destroy();
		void resize(uint32_t _width, uint32_t _height, bool _vsync);

		static bool isSwapChainSupported();
		SwapChainGL* createSwapChain(void* _nwh);
		void destroySwapChain(SwapChainGL*  _swapChain);
		void swap(SwapChainGL* _swapChain = NULL);
		void makeCurrent(SwapChainGL* _swapChain = NULL);

		void import();

	  bool isValid() const;

		SwapChainGL* m_current;
	};
} /* namespace gl */ } // namespace bgfx

#endif // BGFX_USE_QT || defined QT_OPENGL_LIB

#endif // BGFX_GLCONTEXT_QT_H_HEADER_GUARD
