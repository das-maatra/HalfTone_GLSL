#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/Capture.h"
#include "cinder/Log.h"
#include "cinder/CinderImGui.h"

#include "HalfTone.h"


using namespace ci;
using namespace ci::app;
using namespace std;

int										appScreenWidth = 1280;
int										appScreenHeight = 720;

class HalfTone_GLSLApp : public App {
  public:
	void								setup() override;
	void								mouseDown( MouseEvent event ) override;
	void								update() override;
	void								draw() override;

	//capture camera feed
	CaptureRef							mCapture;
	gl::TextureRef						mDefaultCamTexture;
	gl::FboRef							mCamTexFlippedFbo;
	gl::TextureRef						flipCamTexture();
	int									mCamResX = 640;
	int									mCamResY = 480;

	// HalfTone object
	HalfTone							mHalfTone;
};

void HalfTone_GLSLApp::setup(){
	gl::enableAlphaBlending();

	// amera setup
	try {
		mCapture = Capture::create(mCamResX, mCamResY);
		mCapture->start();
		CI_LOG_I("Default camera started");
	}
	catch (ci::Exception & exc) {
		CI_LOG_EXCEPTION("Failed to init capture ", exc);
	}

	gl::Fbo::Format format;
	format.setColorTextureFormat(gl::Texture::Format().internalFormat(GL_RGBA));
	mCamTexFlippedFbo = gl::Fbo::create(mCamResX, mCamResY, format);

	mHalfTone.setup( mCamResX, mCamResY );

	#if ! defined( CINDER_GL_ES )
		ImGui::Initialize();
	#endif

	#if defined( CINDER_COCOA_TOUCH )
		getSignalSupportedOrientations().connect([] { return InterfaceOrientation::LandscapeAll; });
	#endif
}

void HalfTone_GLSLApp::mouseDown( MouseEvent event ){
}

void HalfTone_GLSLApp::update(){
	//IMGUI
	#if ! defined( CINDER_GL_ES )
		ImGui::Begin("HalfTone Options");
		ImGui::DragFloat("Scale", &mHalfTone.mScale, 0.1f, 0.1f, 5.0f);
		ImGui::DragFloat("SST", &mHalfTone.mSST, 0.01f, 0.1f, 2.0f);
		ImGui::DragFloat("SSQ", &mHalfTone.mSSQ, 0.001f, 0.005f, 1.0f);
		ImGui::DragFloat("Angle", &mHalfTone.mAngle, 1.0, 0.0f, 360.f);
		ImGui::DragFloat("Grid Density", &mHalfTone.mGridDensity, 1.0f, 10.0f, 150.0f);
		ImGui::Separator();
		ImGui::ColorPicker3("BG Color", &mHalfTone.mBgColor);
		ImGui::ColorPicker3("FG Color", &mHalfTone.mPortraitColor);
		ImGui::End();
	#endif
	//optical flow update
	if (mCapture && mCapture->checkNewFrame()) {
		if (!mDefaultCamTexture) {
			// Capture images come back as top-down, and it's more efficient to keep them that way
			mDefaultCamTexture = gl::Texture::create(*mCapture->getSurface(), gl::Texture::Format());
		}
		else {
			mDefaultCamTexture->update(*mCapture->getSurface());
		}
	}

	mHalfTone.update( flipCamTexture() );
}

gl::TextureRef HalfTone_GLSLApp::flipCamTexture() {
	gl::clear(Color::black());
	gl::ScopedFramebuffer fbScpLastTex(mCamTexFlippedFbo);
	gl::ScopedViewport scpVpLastTex(ivec2(0), mCamTexFlippedFbo->getSize());
	gl::ScopedMatrices matFlipTex;
	gl::setMatricesWindow(mCamTexFlippedFbo->getSize());
	gl::scale(-1, 1);
	gl::translate(-mCamTexFlippedFbo->getWidth(), 0);
	gl::draw(mDefaultCamTexture, mCamTexFlippedFbo->getBounds());
	return mCamTexFlippedFbo->getColorTexture();
}

void HalfTone_GLSLApp::draw(){
	gl::clear(Color(0, 0, 0));

	// draw camera feed
	{
		gl::ScopedMatrices matCamFeed;
		Rectf bounds(0, 0, mCamResX, mCamResY);
		gl::draw(flipCamTexture(), bounds);
	}

	//draw halftone image
	{
		gl::ScopedMatrices matHalfTone;
		gl::translate(appScreenWidth - mCamResX, 0);
		mHalfTone.draw();
	}
}

CINDER_APP(HalfTone_GLSLApp, RendererGl, [](App::Settings* settings) {
	settings->setWindowSize(appScreenWidth, appScreenHeight);
	settings->setMultiTouchEnabled(false);
})

