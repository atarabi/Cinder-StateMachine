#include "StateMachine.h"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace atarabi;

struct SharedData {
	vec2 position;
	Color color;
};

class CircleState : public State<SharedData> {
public:
	CircleState(float radius) : radius_(radius) {}

	void mouseDown(MouseEvent event) override
	{
		transition();
	}

	void draw() override
	{
		gl::clear(ColorA(0, 0, 0, 0));

		auto shared_data = getSharedData();
		gl::color(shared_data->color);
		gl::drawSolidCircle(shared_data->position, radius_);
	}

private:
	float radius_;
};

class RectState : public State<SharedData> {
public:
	RectState(float width, float height) : width_(width), height_(height) {}

	void mouseDown(MouseEvent event) override
	{
		transition();
	}

	void draw() override
	{
		gl::clear(ColorA(0, 0, 0, 0));

		auto shared_data = getSharedData();
		gl::color(shared_data->color);
		vec2 center = shared_data->position;
		Rectf rect(center.x - 0.5f * width_, center.y - 0.5f * height_, center.x + 0.5f * width_, center.y + 0.5f * height_);
		gl::drawSolidRect(rect);
	}

private:
	float width_;
	float height_;
};

class PostdrawApp : public App {
public:
	void setup() override;
	void mouseMove(MouseEvent event) override;
	void update() override;
	void draw() override;

private:
	params::InterfaceGlRef gui_;
	StateMachine<SharedData> state_machine_;
};

void PostdrawApp::setup()
{
	auto shared_data = state_machine_.getSharedData();
	shared_data->color = Color(1, 0, 0);

	gui_ = params::InterfaceGl::create("Postdraw", { 180, 150 });
	gui_->addParam("Position X", &shared_data->position.x, true);
	gui_->addParam("Position Y", &shared_data->position.y, true);
	gui_->addParam("Color", &shared_data->color);

	state_machine_.addState<CircleState>("Circle", 200.f);
	state_machine_.addState<RectState>("Rect", 300.f, 100.f);
	state_machine_.connect("Circle", "Rect");
	state_machine_.connect("Rect", "Circle");
	state_machine_.setPostdraw([this]() { gui_->draw(); });
}

void PostdrawApp::mouseMove(MouseEvent event)
{
	state_machine_.getSharedData()->position = event.getPos();
}

void PostdrawApp::update()
{
}

void PostdrawApp::draw()
{
}

CINDER_APP(PostdrawApp, RendererGl)
