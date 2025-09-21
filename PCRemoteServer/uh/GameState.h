#pragma once

enum State {NO_CHANGE, EXIT, START};
class GameState {
protected:
	bool done;
public:
	GameState();
	virtual ~GameState();
	virtual State Update(float dt) = 0;
	virtual void Render() = 0;
	virtual void HandleEvents() = 0;
};