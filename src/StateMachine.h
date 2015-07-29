#pragma once

#include "State.h"
#include "cinder/app/App.h"

#include <cassert>
#include <memory>
#include <map>
#include <string>

namespace atarabi {

template<class SharedData = EmptyData>
class StateMachine {
public:
	using StatePtr = std::shared_ptr<State<SharedData>>;
	using data_type = SharedData;

	StateMachine() : mEnabled{ true }
	{
		mData = std::make_shared<SharedData>();
		init();
	}

	bool isEnabled() const { return mEnabled; }

	void enable(bool enabled = true) { mEnabled = enabled; }

	void toggle() { mEnabled = !mEnabled; }

	template<class State, class... Args>
	void addState(const std::string& name, Args&&... args)
	{
		auto result = mStates.insert(std::make_pair(name, std::make_shared<State>(std::forward<Args>(args)...)));
		if (result.second)
		{
			auto state = result.first->second;
			state->setParent(this);
			state->setup();
		}
	}

	void connect(const std::string &from, const std::string &to)
	{
		mConnections.insert(std::make_pair(from, to));
	}

	void transition()
	{
		assert(!mCurrentStateName.empty());

		auto it = mConnections.find(mCurrentStateName);
		assert(it != mConnections.end());

		transition(it->second);
	}

	void transition(const std::string &name)
	{
		if (name == mCurrentStateName)
		{
			return;
		}

		auto it = mStates.find(name);
		assert(it != mStates.end());

		if (mCurrentState)
		{
			mCurrentState->onExit();
		}

		mCurrentState = it->second;
		mCurrentStateName = name;

		mCurrentState->onEnter();
	}

	void execute(const std::string &action) { if (mEnabled && mCurrentState) mCurrentState->execute(action); }

	std::shared_ptr<SharedData> getSharedData() const { return mData; }

	StatePtr getCurrentState() const 
	{
		assert(mCurrentState); return mCurrentState; 
	}

	std::string getCurrentStateName() const
	{
		return mCurrentStateName;
	}

	StatePtr getState(const std::string &name)
	{
		auto it = mStates.find(name);
		
		if (it != mStates.end())
		{
			return it->second;
		}

		return nullptr;
	}

private:
	void init()
	{
		auto app = ci::app::App::get();
		auto window = app->getWindow();

		app->getSignalUpdate().connect([this]() { if (mEnabled && mCurrentState) mCurrentState->update(); });
		window->getSignalDraw().connect([this]() { if (mEnabled && mCurrentState) mCurrentState->draw(); });

		window->getSignalMouseDown().connect([this](ci::app::MouseEvent &event) { if (mEnabled && mCurrentState) mCurrentState->mouseDown(event); });
		window->getSignalMouseUp().connect([this](ci::app::MouseEvent &event) { if (mEnabled && mCurrentState) mCurrentState->mouseUp(event); });
		window->getSignalMouseWheel().connect([this](ci::app::MouseEvent &event) { if (mEnabled && mCurrentState) mCurrentState->mouseWheel(event); });
		window->getSignalMouseMove().connect([this](ci::app::MouseEvent &event) { if (mEnabled && mCurrentState) mCurrentState->mouseMove(event); });
		window->getSignalMouseDrag().connect([this](ci::app::MouseEvent &event) { if (mEnabled && mCurrentState) mCurrentState->mouseDrag(event); });

		window->getSignalTouchesBegan().connect([this](ci::app::TouchEvent &event) { if (mEnabled && mCurrentState) mCurrentState->touchesBegan(event); });
		window->getSignalTouchesMoved().connect([this](ci::app::TouchEvent &event) { if (mEnabled && mCurrentState) mCurrentState->touchesMoved(event); });
		window->getSignalTouchesEnded().connect([this](ci::app::TouchEvent &event) { if (mEnabled && mCurrentState) mCurrentState->touchesEnded(event); });

		window->getSignalKeyDown().connect([this](ci::app::KeyEvent &event) { if (mEnabled && mCurrentState) mCurrentState->keyDown(event); });
		window->getSignalKeyUp().connect([this](ci::app::KeyEvent &event) { if (mEnabled && mCurrentState) mCurrentState->keyUp(event); });

		window->getSignalResize().connect([this]() { if (mEnabled && mCurrentState) mCurrentState->resize(); });
		window->getSignalFileDrop().connect([this](ci::app::FileDropEvent &event) { if (mEnabled && mCurrentState) mCurrentState->fileDrop(event); });
	}

private:
	bool mEnabled;
	std::shared_ptr<SharedData> mData;

	std::map<std::string, StatePtr> mStates;
	StatePtr mCurrentState;
	std::string mCurrentStateName;
	std::map<std::string, std::string> mConnections;
};

} //namespace end