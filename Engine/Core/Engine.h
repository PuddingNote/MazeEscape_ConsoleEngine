#pragma once

#include "Core.h"
#include "Input.h"

#include <Windows.h>

// 엔진 설정 구조체
struct EngineSettings
{
	float framerate = 0.0f;
};

class Level;
class Engine_API Engine
{
public:
	Engine();
	virtual ~Engine();

	void Run();
	void Quit();
	virtual void CleanUp();	// 메모리 해제 함수

	void AddLevel(Level* newLevel);

	static Engine& Get();

private:
	void BeginPlay();
	void Tick(float deltaTime = 0.0f);	// Update()
	void Render();

	void LoadEngineSettings();	// 엔진 설정 로드 함수

protected:
	bool isQuit = false;		// 엔진 종료 플래그

	Level* mainLevel = nullptr;	// 메인 레벨

	Input input;				// 입력 관리자

	EngineSettings settings;	// 엔진 설정

	static Engine* instance;	// 싱글톤 변수
};