#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "GameState.h"
#include "Button.h"
#include <thread>
#include <atomic>


//NETWORK PACKAGE SHOULD BE FIRST SIZE OF PACKAGE
//THEN WHAT EVER WE WANT TO SEND

enum NetworkCall
{
	QUIT	= -1,
	SECKEY	= 0,
	MESSAGE = 1,
	SETUP	= 2,
	CHECKDISCONNECT = 3,
	TEST = 4,
	ACTIONS = 1000,
};

enum ClientStatus
{
	NOTCONNECTED,
	TRYINGTOCONNECT,
	CONNECTED
};

class Game: public GameState {
public:
	Game();
	virtual ~Game();
	State Update(float dt);
	void Render();
	void HandleEvents();
private:
	int CreateSecKey();
	void ScrambleMessage(char* msg);
	void UnScrambleMessage(char* msg);

	void getClient();
	void setUpClient();
	void recvFromClient();
	void handleIntegear(const int32_t& iRef);
	void checkDisconnect();

	ClientStatus clientStatus;
	float timeToReconnect;
	float timeToCheckDisconnect;
	bool quit;

	std::string tempString;

	std::thread* clientThread;

	static const int BUFFERSIZE = 2048;
	char recvBuffer[BUFFERSIZE] = {};
	char sendBuffer[BUFFERSIZE] = {};
	sf::TcpListener listener;
	sf::TcpSocket client;

	sf::RenderWindow win;
	

	static const int WinWidth = 620;
	static const int WinHeight = 620;

	uint32_t NumberOfScripts;

	//GRAPHICS
	sf::Font font;
	sf::Text text;

	Button quitButton;

	std::string IpCode = "";
	static const uint32_t startPort = 49152;
	uint32_t port;

	std::atomic<uint8_t> textVersion;//0 = Client Connected, 1 = Code
	uint8_t currentTextVersion;
};