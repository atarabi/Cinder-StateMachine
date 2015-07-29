#pragma once

#include "cinder/app/App.h"

namespace atarabi {

struct EmptyData {};

template<class SharedData>
class StateMachine;

template<class SharedData = EmptyData>
class State {
public:
	State() : mParent{ nullptr } {}
	virtual ~State() {}

	void setParent(StateMachine<SharedData> *parent) { mParent = parent; }

	virtual void execute(const std::string &action) {}

	virtual void onEnter() {}
	virtual void onExit() {}

	virtual void setup() {}
	virtual void update() {}
	virtual void draw() {}

	virtual void mouseDown(ci::app::MouseEvent event) {}
	virtual void mouseUp(ci::app::MouseEvent event) {}
	virtual void mouseWheel(ci::app::MouseEvent event) {}
	virtual void mouseMove(ci::app::MouseEvent event) {}
	virtual void mouseDrag(ci::app::MouseEvent event) {}

	virtual void touchesBegan(ci::app::TouchEvent event) {}
	virtual void touchesMoved(ci::app::TouchEvent event) {}
	virtual void touchesEnded(ci::app::TouchEvent event) {}

	virtual void keyDown(ci::app::KeyEvent event) {}
	virtual void keyUp(ci::app::KeyEvent event) {}

	virtual void resize() {}

	virtual void fileDrop(ci::app::FileDropEvent event) {}

protected:
	void transition() const
	{
		assert(mParent);

		mParent->transition();
	}

	void transition(const std::string &name) const
	{
		assert(mParent);

		mParent->transition(name);
	}

	std::shared_ptr<SharedData> getSharedData() const
	{
		assert(mParent);

		return mParent->getSharedData();
	}

	std::string getCurrentStateName() const
	{
		assert(mParent);

		return mParent->getCurrentStateName();
	}

	void getState(const std::string &name) const
	{
		assert(mParent);

		return mParent->getState(name);
	}

private:
	StateMachine<SharedData> *mParent;
};

} //namespace end