#include "Engine.h"
#include "Level/Level.h"
#include "Utils/Utils.h"

#include <Windows.h>
#include <iostream>

// ���� ���� ȣ��ȭ
Engine* Engine::instance = nullptr;

BOOL WINAPI ConsoleMessageProcedure(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		// Engine�� �޸� ����
		Engine::Get().CleanUp();
		return false;
	}
	return false;
}

Engine::Engine()
{
	instance = this;

	CONSOLE_CURSOR_INFO info;
	info.bVisible = false;
	info.dwSize = 1;

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

	// �ܼ� â �̺�Ʈ ���
	SetConsoleCtrlHandler(ConsoleMessageProcedure, TRUE);

	// ���� ���� �ε�
	LoadEngineSettings();
}

Engine::~Engine()
{
	CleanUp();
}

void Engine::Run()
{
	// �и� ������ ������ �ð��� �˷���
	//float currentTime = timeGetTime();
	LARGE_INTEGER currentTime;
	LARGE_INTEGER previousTime;
	QueryPerformanceCounter(&currentTime);
	previousTime = currentTime;

	// �ϵ���� �ð��� ���е�(���ļ�) �������� : ���߿� �ʷ� ��ȯ�ϱ� ����
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	// Ÿ�� ������
	float targetFrameRate = (settings.framerate == 0.0f) ? 60.0f : settings.framerate;
	float oneFrameTime = 1.0f / targetFrameRate;	// Ÿ�� �� ������ �ð�

	while (true)
	{
		if (isQuit)
		{
			break;
		}

		// ������ �ð� ��� : (���� �ð� - ���� �ð�) / ���ļ� = �ʴ���
		QueryPerformanceCounter(&currentTime);
		float deltaTime = (currentTime.QuadPart - previousTime.QuadPart) / (float)frequency.QuadPart;

		// �Է��� �ִ��� ����
		input.ProcessInput();

		// ����������
		if (deltaTime >= oneFrameTime)
		{
			BeginPlay();
			Tick(deltaTime);
			Render();

			// ���� FPS ���
			char title[50] = {};
			sprintf_s(title, 50, "FPS: %f", (1.0f / deltaTime));
			SetConsoleTitleA(title);

			// �ð� ������Ʈ
			previousTime = currentTime;

			// ���� �������� �Է��� ���
			input.SavePreviousKeyStates();
		}
	}

	// ���� (���� ����� ����)


	// ��� �ؽ�Ʈ ���� ������� ����
	Utils::SetConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

void Engine::Quit()
{
	isQuit = true;
}

void Engine::CleanUp()
{
	// ���� ����
	SafeDelete(mainLevel);
}

void Engine::AddLevel(Level* newLevel)
{
	if (mainLevel != nullptr)
	{
		delete mainLevel;
	}

	mainLevel = newLevel;
}

Engine& Engine::Get()
{
	return *instance;
}

void Engine::BeginPlay()
{
	if (mainLevel != nullptr)
	{
		mainLevel->BeginPlay();
	}
}

void Engine::Tick(float deltaTime)
{
	// ���� ������Ʈ
	if (mainLevel != nullptr)
	{
		mainLevel->Tick(deltaTime);
	}
}

void Engine::Render()
{
	Utils::SetConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	if (mainLevel != nullptr)
	{
		mainLevel->Render();
	}
}

void Engine::LoadEngineSettings()
{
	FILE* file = nullptr;
	fopen_s(&file, "../Settings/EngineSettings.txt", "rt");
	if (file == nullptr)
	{
		std::cout << "Failed to load engine settings.\n";
		__debugbreak();
		return;
	}

	// �ε�
	fseek(file, 0, SEEK_END);		// File Position �����͸� ���� �ڷ�
	size_t fileSize = ftell(file);	// �� ��ġ ���ϱ�
	rewind(file);					// �ٽ� ù ��ġ�� �ǵ�����

	// ���� ������ ������ ���� �Ҵ� �� ���� �б�
	char* buffer = new char[fileSize + 1];
	memset(buffer, 0, fileSize + 1);
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	// ������ �Ľ�
	char* context = nullptr;
	char* token = nullptr;
	token = strtok_s(buffer, "\n", &context);
	while (token != nullptr)
	{
		// Ű/�� �и�
		char header[10] = {};

		// �Ʒ� ������ ����� �����Ϸ��� ��ĭ�� �־����
		sscanf_s(token, "%s", header, 10);

		// ��� ���ڿ� ��
		if (strcmp(header, "framerate") == 0)
		{
			sscanf_s(token, "framerate = %f", &settings.framerate);
		}
		/*else if (strcmp(header, "width") == 0)
		{
			sscanf_s(token, "width = %d", &settings.width);
		}
		else if (strcmp(header, "height") == 0)
		{
			sscanf_s(token, "height = %d", &settings.height);
		}*/

		// �� ������ �и�
		token = strtok_s(nullptr, "\n", &context);
	}

	SafeDeleteArray(buffer);		// ���� ����
	fclose(file);					// ���� �ݱ�
}