#include "Game.h"
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include "Functions.h"

Game::Game():
	clientStatus(ClientStatus::NOTCONNECTED), 
	quit(false), 
	timeToCheckDisconnect(0), 
	timeToReconnect(0), 
	NumberOfScripts(0),
	quitButton(font, "Quit", sf::Vector2f(100,50), sf::Vector2f(WinWidth - 75, 50), sf::Color(50, 50, 50)),
	win(sf::VideoMode(WinWidth, WinHeight), "ComputerPhone")
{
	if (!font.loadFromFile("C:/windows/fonts/arial.ttf"))
	{
		std::cout << "cannot load font" << std::endl;
		// error...
	}

	quitButton.setUp();

	textVersion = 0;
	currentTextVersion = 0;
	win.setFramerateLimit(20);

	
	port = startPort;
	while (listener.listen(port) != sf::Socket::Done && port < (startPort + 10))
	{
		port++;
	}

	clientStatus = ClientStatus::TRYINGTOCONNECT;
	clientThread = new std::thread(&Game::getClient, this);

	
	static const std::string NormalIP = "192.168.1.";//What we normaly have for kinda IP
	std::string ipAddress = sf::IpAddress::getLocalAddress().toString();

	//Go through the most normal local ip to see if we can get 
	int i = 0;
	for (; i < NormalIP.size(); i++)
	{
		if (NormalIP[i] != ipAddress[i])
		{
			break;
		}
	}
	for (; i < ipAddress.size(); i++) {
		IpCode += ipAddress[i];
	}
	

	text.setFont(font);
	text.setFillColor(sf::Color::White);
	text.setScale(1, 1);
	text.setCharacterSize(52);
	textVersion = 1;
}

Game::~Game()
{
	delete clientThread;
}

State Game::Update(float dt)
{
	if (!win.isOpen())
	{
		Sleep(100);
	}
	if (clientStatus == ClientStatus::CONNECTED && timeToCheckDisconnect > 10)
	{
		timeToCheckDisconnect = 0;
		checkDisconnect();
	}
	else if(clientStatus == ClientStatus::CONNECTED){
		timeToCheckDisconnect += dt;
	}
	else {
		timeToCheckDisconnect = 0;
	}
	if (clientStatus == ClientStatus::NOTCONNECTED && timeToReconnect > 2)
	{
		std::cout << "client was not connected trying again" << std::endl;
		timeToReconnect = 0;
		clientThread->join();
		delete clientThread;
		clientStatus = ClientStatus::TRYINGTOCONNECT;
		clientThread = new std::thread(&Game::getClient, this);

		textVersion = 1;
	}
	else {
		timeToReconnect += dt;
	}
	if (quit)
	{
		listener.close();
		client.setBlocking(false);
		client.disconnect();
		clientThread->join();
		return State::EXIT;
	}

	if(!win.isOpen() &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && 
		sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) &&
		sf::Keyboard::isKeyPressed(sf::Keyboard::F6))
	{
		win.create(sf::VideoMode(WinWidth, WinHeight), "ComputerPhone");
		win.setFramerateLimit(20);
	}

	if (textVersion != currentTextVersion)
	{
		currentTextVersion = textVersion;
		switch (currentTextVersion)
		{
		case 0:
			text.setFillColor(sf::Color::Green);
			text.setString("A Client connected");
			text.setOrigin(
				text.getGlobalBounds().width / 2,
				text.getGlobalBounds().height / 2
			);
			text.setPosition(WinWidth / 2, WinHeight / 2);
			break;
		case 1:
			text.setFillColor(sf::Color::White);
			text.setString("Code: " + IpCode + "." + std::to_string(port - startPort));
			text.setOrigin(
				text.getGlobalBounds().width / 2,
				text.getGlobalBounds().height / 2
			);
			text.setPosition(WinWidth / 2, WinHeight / 2);
			break;
		default:
			break;
		}
	}
	

	return State();
}

void Game::Render()
{
	if (!win.isOpen()) { return; }

	win.clear(sf::Color(20, 20, 20, 1));

	win.draw(text);
	win.draw(quitButton);

	win.display();
}

void Game::HandleEvents()
{
	if (!win.isOpen()) { return; }

	sf::Event event;
	while (win.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			win.close();
			return;
		}
	}
	if (quitButton.update(win))
	{
		quit = true;
	}
}

int Game::CreateSecKey()
{
	return rand();
}

void Game::ScrambleMessage(char* msg)
{
}

void Game::UnScrambleMessage(char* msg)
{
}

void Game::getClient()
{
	std::cout << "trying to get client" << std::endl;
	if (listener.accept(client) != sf::Socket::Done)
	{
		clientStatus = ClientStatus::NOTCONNECTED;
		return;//WE DIDN'T GET ANY USERS
	}
	else {
		std::cout << "got a client" << std::endl;
		clientStatus = ClientStatus::CONNECTED;
		setUpClient();
		textVersion = 0;
		recvFromClient();
	}
}

void Game::setUpClient()
{
	//Give client how many buttons it should have
	auto dirIter = std::filesystem::directory_iterator("../Scripts");
	int fileCount = std::count_if(
		begin(dirIter),
		end(dirIter),
		[](auto& entry) { return entry.is_regular_file(); }
	);
	NumberOfScripts = fileCount;

	//MAX NR OF FILES ARE 100 else we migh break the buffer size, (should go up to about 165)
	fileCount = std::clamp(fileCount, 0, 100);

	//Send all the fileNames (max 8 char)
	static const int MAXFILESTRINGSIZE = 8;
	std::string* filenames = new std::string[fileCount + SizeOfFunctionArray];
	{
		int i = 0;
		for (const auto& entry : std::filesystem::directory_iterator("../Scripts"))
		{
			if (entry.is_regular_file())
			{
				std::string filename = entry.path().filename().string();
				filename = filename.substr(
					0, 
					std::clamp((int)filename.find('.'), 0, MAXFILESTRINGSIZE)
				);
				filenames[i++] = filename;
			}
		}
		for (int i2 = 0; i2 < SizeOfFunctionArray; i2++)
		{
			filenames[i++] = functionsArray[i2].name.substr(
				0,
				std::clamp((int)functionsArray[i2].name.size(), 0, MAXFILESTRINGSIZE)
			);
		}
	}

	std::cout << "Nr of files " << fileCount << std::endl;
	std::cout << "Nr of functions " << SizeOfFunctionArray << std::endl;
	int pointer = sizeof(int);
	//Send that we are gonna do a Setup
	int tempSrc = NetworkCall::SETUP;
	memcpy(&sendBuffer[pointer], &tempSrc, sizeof(int));
	pointer += sizeof(int);

	//Send Number of scripts + functions
	tempSrc = fileCount + SizeOfFunctionArray;
	memcpy(&sendBuffer[pointer], &tempSrc, sizeof(int));
	pointer += sizeof(int);

	
	//String is sent as sizeof string + the string
	for (int i = 0; i < fileCount; i++)
	{
		int strSize = (int)filenames[i].size();//THIS SHOULD BE MAX MAXFILESTRINGSIZE
		memcpy(&sendBuffer[pointer], &strSize, sizeof(int));
		pointer += sizeof(int);

		memcpy(&sendBuffer[pointer], &filenames[i][0], strSize);
		pointer += strSize;  
	}
	//Same thing but for functions
	for (int i = 0; i < SizeOfFunctionArray; i++)
	{
		int strSize = (int)functionsArray[i].name.size();//THIS SHOULD BE MAX MAXFILESTRINGSIZE
		memcpy(&sendBuffer[pointer], &strSize, sizeof(int));
		pointer += sizeof(int);
	
		memcpy(&sendBuffer[pointer], &functionsArray[i].name[0], strSize);
		pointer += strSize;
	}

	//Send Size of the package
	tempSrc = pointer;
	memcpy(&sendBuffer[0], &tempSrc, sizeof(int));

	if (client.send(sendBuffer, pointer) != sf::Socket::Done)
	{
		std::cout << "could not send first setup" << std::endl;
	}


	ZeroMemory(sendBuffer, pointer);
	pointer = sizeof(int);
	tempSrc = NetworkCall::TEST;
	memcpy(&sendBuffer[pointer], &tempSrc, sizeof(int));
	pointer += sizeof(int);

	tempSrc = pointer;
	memcpy(&sendBuffer[0], &tempSrc, sizeof(int));

	if (client.send(sendBuffer, pointer) != sf::Socket::Done)
	{
		std::cout << "could not send Test" << std::endl;
	}
	else {
		std::cout << "sent package" << std::endl;
	}

	delete[] filenames;
}

void Game::recvFromClient()
{

	while (!quit && clientStatus == ClientStatus::CONNECTED)
	{
		size_t recvSize = 0;
		if (client.receive(recvBuffer, BUFFERSIZE, recvSize) != sf::Socket::Done)
		{
			clientStatus = ClientStatus::NOTCONNECTED;
			break;
		}
		std::cout << "Got buffer " << recvBuffer << std::endl;
		if (recvSize == sizeof(int32_t))
		{
			int32_t recvInt;
			memcpy(&recvInt, recvBuffer, sizeof(int32_t));
			handleIntegear(recvInt);
		}
		else {
			//WTF?
		}
	}
}

std::wstring ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

void Game::handleIntegear(const int32_t& iRef)
{
	if (iRef >= NetworkCall::ACTIONS)//Do a script
	{
		if (iRef - (int)NetworkCall::ACTIONS > NumberOfScripts)
		{
			//Do a function instead
			int whatFunction = iRef - (NetworkCall::ACTIONS + NumberOfScripts) - 1;
			functionsArray[whatFunction].function();//FUCK THIS WAS EASY
			return;
		}
		int actionNumber = iRef - NetworkCall::ACTIONS;
		std::cout << "current path " << std::filesystem::current_path() << std::endl;
		std::cout << "Do action " << actionNumber << std::endl;
		std::string ScriptPath = std::filesystem::current_path().string() + "/../Scripts/Script" + std::to_string(actionNumber) + ".bat";
		std::string command = "cmd.exe /c \"" + ScriptPath + "\"";
		std::wstring wcommand(command.begin(), command.end());

		STARTUPINFOW si;
		PROCESS_INFORMATION pi;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// Create the process
		if (CreateProcessW(
			NULL,                       // Application name (NULL = use command line)
			&wcommand[0],               // Command line (mutable buffer)
			NULL,                       // Process security attributes
			NULL,                       // Thread security attributes
			FALSE,                      // Inherit handles
			CREATE_NO_WINDOW,           // Creation flags
			NULL,                       // Environment (inherit parent)
			NULL,                       // Current directory (inherit parent)
			&si,                        // Startup info
			&pi                         // Process info (gets filled)
		)) {
			// Cleanup
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else {
			std::cerr << "Failed to start script. Error: " << GetLastError() << std::endl;
		}
	}
	switch (iRef)
	{
	case NetworkCall::QUIT:
		client.disconnect();
		clientStatus = ClientStatus::NOTCONNECTED;
		this->quit = true;
		break;
	case NetworkCall::SECKEY:
		//?
		break;
	case NetworkCall::MESSAGE:
		tempString = (recvBuffer + 4);
		std::cout << tempString << std::endl;
		break;
	}
	
}

void Game::checkDisconnect()
{
	std::cout << "check Disconnect" << std::endl;
	ZeroMemory(sendBuffer, BUFFERSIZE);

	int pointer = sizeof(int32_t);
	int tempSrc = 0;
	NetworkCall temp = NetworkCall::CHECKDISCONNECT;
	memcpy(&sendBuffer[pointer], &temp, sizeof(int32_t));
	pointer += sizeof(int32_t);
	
	tempSrc = pointer;
	memcpy(&sendBuffer[0], &tempSrc, sizeof(int32_t));


	if (client.send(sendBuffer, pointer) == sf::Socket::Disconnected)
	{
		clientStatus = ClientStatus::NOTCONNECTED;
		client.disconnect();
	}
}
