#include "StateMachine.h"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace atarabi;

class ColorState : public State<> {
public:
	ColorState(const Color &color) : color_(color) {}

	void mouseDown(MouseEvent event) override
	{
		transition();
	}

	void draw() override
	{
		gl::clear(color_);
	}

private:
	Color color_;
};

class StateMachineApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

private:
	StateMachine<> state_machine_;
};

void StateMachineApp::setup()
{
	state_machine_.addState<ColorState>("Green", Color(0.f, 1.f, 0.f));
	state_machine_.addState<ColorState>("Yellow", Color(1.f, 1.f, 0.f));
	state_machine_.addState<ColorState>("Red", Color(1.f, 0.f, 0.f));

	state_machine_.connect("Green", "Yellow");
	state_machine_.connect("Yellow", "Red");
	state_machine_.connect("Red", "Green");
}

void StateMachineApp::mouseDown(MouseEvent event)
{
}

void StateMachineApp::update()
{
}

void StateMachineApp::draw()
{
}

CINDER_APP(StateMachineApp, RendererGl)
