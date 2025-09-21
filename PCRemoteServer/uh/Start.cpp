#include "Game.h"
#include "DeltaTime.h"
#include "GameState.h"

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	srand((unsigned int)time(0));
	DeltaTime dt;
	GameState* game = new Game();
	State gameState = NO_CHANGE;
	while (gameState != EXIT) {
		dt.restartClock();
		switch (gameState)
		{
		case START:
			break;
		default:
			break;
		}
		gameState = game->Update((float)dt.dt());
		game->Render();
		game->HandleEvents();
	}
	delete game;
	return 0;
}

#ifdef _WIN32
#include <windows.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return main(); // Forward to normal main()
}
#endif