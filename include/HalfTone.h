#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class HalfTone {
	public:
		float					mScale;
		float					mAngle;
		float					mGridDensity;
		float					mSST;
		float					mSSQ;
		Color					mBgColor;
		Color					mPortraitColor;
	
		void					setup(int cameraResX, int cameraResY);
		void					update(gl::TextureRef cameraTex);
		void					draw();

	private:
		int						mResX;
		int						mResY;
		gl::GlslProgRef			mHalfToneRenderProg = nullptr;
		gl::TextureRef			mHalfToneTexRef;       // halftone texture.
		gl::Fbo::Format			mFboFormat;
		gl::FboRef				mFboHalfTone;
		gl::TextureRef			mCameraTex;

		void					loadShaders();
		void					initParams();

}; 