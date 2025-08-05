#include "Engine.h"
#include "Level/Level.h"
#include "Utils/Utils.h"

#include <Windows.h>
#include <iostream>

// 정적 변수 호기화
Engine* Engine::instance = nullptr;

BOOL WINAPI ConsoleMessageProcedure(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		// Engine의 메모리 해제
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

	// 콘솔 창 이벤트 등록
	SetConsoleCtrlHandler(ConsoleMessageProcedure, TRUE);

	// 엔진 설정 로드
	LoadEngineSettings();
}

Engine::~Engine()
{
	CleanUp();
}

void Engine::Run()
{
	// 밀리 세컨드 단위로 시간을 알려줌
	//float currentTime = timeGetTime();
	LARGE_INTEGER currentTime;
	LARGE_INTEGER previousTime;
	QueryPerformanceCounter(&currentTime);
	previousTime = currentTime;

	// 하드웨어 시계의 정밀도(주파수) 가져오기 : 나중에 초로 변환하기 위해
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	// 타겟 프레임
	float targetFrameRate = (settings.framerate == 0.0f) ? 60.0f : settings.framerate;
	float oneFrameTime = 1.0f / targetFrameRate;	// 타겟 한 프레임 시간

	while (true)
	{
		if (isQuit)
		{
			break;
		}

		// 프레임 시간 계산 : (현재 시간 - 이전 시간) / 주파수 = 초단위
		QueryPerformanceCounter(&currentTime);
		float deltaTime = (currentTime.QuadPart - previousTime.QuadPart) / (float)frequency.QuadPart;

		// 입력은 최대한 빨리
		input.ProcessInput();

		// 고정프레임
		if (deltaTime >= oneFrameTime)
		{
			BeginPlay();
			Tick(deltaTime);
			Render();

			// 제목에 FPS 출력
			char title[50] = {};
			sprintf_s(title, 50, "FPS: %f", (1.0f / deltaTime));
			SetConsoleTitleA(title);

			// 시간 업데이트
			previousTime = currentTime;

			// 현재 프레임의 입력을 기록
			input.SavePreviousKeyStates();
		}
	}

	// 정리 (게임 종료시 실행)


	// 모든 텍스트 색상 원래대로 변경
	Utils::SetConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

void Engine::Quit()
{
	isQuit = true;
}

void Engine::CleanUp()
{
	// 레벨 삭제
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
	// 레벨 업데이트
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

	// 로드
	fseek(file, 0, SEEK_END);		// File Position 포인터를 가장 뒤로
	size_t fileSize = ftell(file);	// 이 위치 구하기
	rewind(file);					// 다시 첫 위치로 되돌리기

	// 파일 내용을 저장할 버퍼 할당 후 내용 읽기
	char* buffer = new char[fileSize + 1];
	memset(buffer, 0, fileSize + 1);
	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	// 데이터 파싱
	char* context = nullptr;
	char* token = nullptr;
	token = strtok_s(buffer, "\n", &context);
	while (token != nullptr)
	{
		// 키/값 분리
		char header[10] = {};

		// 아래 구문이 제대로 동작하려면 빈칸이 있어야함
		sscanf_s(token, "%s", header, 10);

		// 헤더 문자열 비교
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

		// 그 다음줄 분리
		token = strtok_s(nullptr, "\n", &context);
	}

	SafeDeleteArray(buffer);		// 버퍼 해제
	fclose(file);					// 파일 닫기
}