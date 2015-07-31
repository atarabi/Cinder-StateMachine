#pragma once

#include "State.h"

#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

namespace atarabi {

template<class SharedData = EmptyData>
class BlendedState : public State<SharedData> {
public:
	BlendedState(double duration) : mDuration{ duration } {}

	void setDuration(double duration) { mDuration = duration; }

	void setup() override
	{
		gl::GlslProgRef program;
		try {
			program = gl::GlslProg::create(
				R"(#version 150

uniform mat4 ciModelViewProjection;

in vec4 	ciPosition;
in vec2 	ciTexCoord0;

out vec2 TexCoord;

void main( void ) 
{
	TexCoord 	= ciTexCoord0;
	gl_Position = ciModelViewProjection * ciPosition;
})", 
				R"(#version 150

uniform sampler2D uCurrentTex;
uniform sampler2D uNextTex;

uniform float uRate;

in vec2 TexCoord;

out vec4 oColor;

void main( void )
{
	vec4 current_color = texture( uCurrentTex, TexCoord );
	vec4 next_color = texture( uNextTex, TexCoord );
	oColor = mix( current_color, next_color, uRate );
})"
			);
		} catch (const gl::GlslProgCompileExc& ex) {
			console() << ex.what() << std::endl;
			return;
		}

		const ci::gl::VboMeshRef rect = ci::gl::VboMesh::create(ci::geom::Rect());
		mBatch = ci::gl::Batch::create(rect, program);
		mBatch->getGlslProg()->uniform("uCurrentTex", 0);
		mBatch->getGlslProg()->uniform("uNextTex", 1);

		createFbo();
	}

	void onEnter() override
	{
		auto names = split(getCurrentStateName(), "/");
		mCurrentStateName = names[0];
		mNextStateName = names[1];
		if (mDuration <= 0.0)
		{
			transition(mNextStateName);
			return;
		}

		mStartTime = getElapsedSeconds();
		mCurrentState = getState(mCurrentStateName);
		mNextState = getState(mNextStateName);
	}

	void resize() override
	{
		createFbo();
	}

	void draw() override
	{
		{
			gl::ScopedFramebuffer scoped_fbo(mCurrentFbo);
			mCurrentState->draw();
		}

		{
			gl::ScopedFramebuffer scoped_fbo(mNextFbo);
			mNextState->draw();
		}

		double rate = (getElapsedSeconds() - mStartTime) / mDuration;
		{
			gl::ScopedViewport scopedViewport(toPixels(getWindowSize()));
			gl::ScopedMatrices scopedMatrices;
			gl::ScopedTextureBind scopedTexture0(mCurrentFbo->getColorTexture(), 0);
			gl::ScopedTextureBind scopedTexture1(mNextFbo->getColorTexture(), 1);
			gl::translate(getWindowCenter());
			gl::scale(getWindowSize());
			gl::clear(ColorA(0, 0, 0, 0));
			mBatch->getGlslProg()->uniform("uRate", (float)rate);
			mBatch->draw();
		}

		if (rate >= 1.0)
		{
			transition(mNextStateName);
		}
	}

private:
	void createFbo()
	{
		int width = getWindowWidth();
		int height = getWindowHeight();
		mCurrentFbo = ci::gl::Fbo::create(width, height, true);
		mNextFbo = ci::gl::Fbo::create(width, height, true);
	}

	double mDuration, mStartTime;
	ci::gl::BatchRef mBatch;
	std::string mCurrentStateName, mNextStateName;
	StateRef<SharedData> mCurrentState, mNextState;
	ci::gl::FboRef mCurrentFbo, mNextFbo;

};

} //namespace end