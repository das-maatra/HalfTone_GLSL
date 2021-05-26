#include "HalfTone.h"

void HalfTone::setup( int cameraResX, int cameraResY ) {
	mResX = cameraResX;
	mResY = cameraResY;
	loadShaders();
	initParams();
}

void HalfTone::initParams() {
	mScale = 1.0f;
	mAngle = 45.0f;
	mGridDensity = 60.0f;
	mSST = 0.9f;
	mSSQ = 0.2f;
	mBgColor = Color(1.0, 1.0, 1.0);
	mPortraitColor = Color(0.0, 0.0, 0.0);
}

void HalfTone::loadShaders() {
	try {
		mHalfToneRenderProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("shaders/halftone.vert"))
			.fragment(loadAsset("shaders/halftone.frag")));
	}
	catch (const std::exception & e) {
		console() << e.what() << std::endl;
		CI_LOG_I("error loading shaders in model");
	}
}

//gl::Texture2dRef HalfTone::getHalfToneTexRef() {
//
//}

void HalfTone::update( gl::TextureRef cameraTex ) {
	mCameraTex = cameraTex;
}

void HalfTone::draw() {
	gl::ScopedTextureBind			texScope0(mCameraTex, uint8_t(0));
	gl::ScopedGlslProg prog(mHalfToneRenderProg);
	mHalfToneRenderProg->uniform("uCamTex", 0);
	//mHalfToneRenderProg->uniform("uMaskTex", 1);
	//mHalfToneRenderProg->uniform("uSourceVignetteTex", 2);
	//mHalfToneRenderProg->uniform("uNoiseTex", 3);
	mHalfToneRenderProg->uniform("uRes", vec2(mResX, mResY));
	mHalfToneRenderProg->uniform("uScale", mScale);
	mHalfToneRenderProg->uniform("uUseBlackDots", true);
	mHalfToneRenderProg->uniform("uAngle", mAngle);
	mHalfToneRenderProg->uniform("uGridDensity", mGridDensity);
	mHalfToneRenderProg->uniform("uPortraitColor", mPortraitColor);
	mHalfToneRenderProg->uniform("uBgColor", mBgColor);
	mHalfToneRenderProg->uniform("uNoiseAmp", 01.0f);
	mHalfToneRenderProg->uniform("uUseCamColor", false);
	mHalfToneRenderProg->uniform("uSST", mSST);
	mHalfToneRenderProg->uniform("uSSQ", mSSQ);


	gl::drawSolidRect(Rectf(vec2(0, 0), vec2(mResX, mResY)));
}