#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "StateMachine.h"
#include "BlendedState.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace atarabi;

struct SharedData {
	vec2 position;
};

class CircleState : public State<SharedData> {
public:
	CircleState(const Color &color, float radius) : color_(color), radius_(radius) {}

	void mouseDown(MouseEvent event) override
	{
		transition();
	}

	void draw() override
	{
		gl::clear(ColorA(0, 0, 0, 0));
		vec2 position = getSharedData()->position;
		gl::color(color_);
		gl::drawSolidCircle(position, radius_);
	}

	void setColor(const Color &color)
	{
		color_ = color;
	}

private:
	Color color_;
	float radius_;
};

class BlendedStateApp : public App {
public:
	void setup() override;
	void mouseMove(MouseEvent event) override;
	void update() override;
	void draw() override;

private:
	StateMachine<SharedData> state_machine_;
};

void BlendedStateApp::setup()
{
	state_machine_.addState<CircleState>("Green", Color(0, 1, 0), 100.f);
	state_machine_.addState<CircleState>("Yellow", Color(1, 1, 0), 100.f);
	state_machine_.addState<CircleState>("Red", Color(1, 0, 0), 100.f);
	state_machine_.addState<BlendedState<SharedData>>("*", 1.0);
	state_machine_.connect("Green", "Green/Yellow");
	state_machine_.connect("Yellow", "Yellow/Red");
	state_machine_.connect("Red", "Red/Green");
}

void BlendedStateApp::mouseMove(MouseEvent event)
{
	state_machine_.getSharedData()->position = event.getPos();
}

void BlendedStateApp::update()
{
}

void BlendedStateApp::draw()
{
}

CINDER_APP(BlendedStateApp, RendererGl(RendererGl::Options().msaa(16)))