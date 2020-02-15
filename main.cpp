#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include "Core/Game.h"
#include "GUI/GUI.h"

// @TODO
// implement anti-aliasing
// implement SSAO

IMGUI_IMPL_API LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);

CGUIBase* g_GUI{};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1374600);
#endif

	static constexpr XMFLOAT2 KGameWindowSize{ 1280.0f, 720.0f };
	CGame Game{ hInstance, KGameWindowSize };

	Game.CreateWin32(WndProc, u8"JEngine", true);

	Game.SetRenderingFlags(CGame::EFlagsRendering::UseLighting | CGame::EFlagsRendering::DrawMiniAxes | CGame::EFlagsRendering::DrawGrid |
		CGame::EFlagsRendering::DrawTerrainHeightMapTexture | CGame::EFlagsRendering::DrawTerrainMaskingTexture | CGame::EFlagsRendering::DrawIdentifiers |
		CGame::EFlagsRendering::DrawTerrainFoliagePlacingTexture | CGame::EFlagsRendering::TessellateTerrain | 
		CGame::EFlagsRendering::Use3DGizmos | CGame::EFlagsRendering::UsePhysicallyBasedRendering);

	//Game.CreateDynamicSky("Asset\\Sky.xml", 30.0f);
	Game.CreateStaticSky(30.0f);

	//Game.LoadScene("Scene\\mayan_dungeon.scene");
	//Game.LoadScene("Scene\\ai_test.scene");
	//Game.SetMode(CGame::EMode::Play);

	CGUI Gui{ Game.GetDevicePtr(), Game.GetDeviceContextPtr() };
	Gui.Create(Game.GethWnd());
	
	Gui.CreateButton("btn", SInt2(100, 48));
	Gui.GetWidget("btn")->SetOffset(SInt2(100, 40));
	Gui.GetWidget("btn")->SetCaption(u8"윈도우 열기");

	{
		Gui.CreateWindowWidget(CGUI::EWindowType::Default, "wnd");
		CWindow* const Window{ (CWindow*)Gui.GetWidget("wnd") };
		Window->SetOffset(SInt2(200, 40));

		Gui.CreateImageButton(CGUI::EImageButtonType::Button, "imgbtn", SInt2(288, 72), Window);
		Window->GetChild("imgbtn")->SetOffset(SInt2(20, 50));
		Window->GetChild("imgbtn")->SetSelectionSize(SInt2(230, 60));
		Window->GetChild("imgbtn")->SetCaption(u8"이미지 버튼");
		Window->GetChild("imgbtn")->SetCaptionColor(SFloat4(1, 1, 0.75f, 1));
	}
	
	g_GUI = &Gui;

	// Main loop
	while (true)
	{
		static MSG Msg{};
		static char KeyDown{};
		if (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (Msg.message == WM_QUIT) break;

			if (Msg.message == WM_KEYDOWN) KeyDown = (char)Msg.wParam;
			
			if (Msg.message == WM_LBUTTONDOWN) Game.NotifyMouseLeftDown();
			if (Msg.message == WM_LBUTTONUP) Game.NotifyMouseLeftUp();

			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else
		{
			//if (KeyDown == VK_SPACE) Game.JumpPlayer(5.0f);
			if (KeyDown == VK_DELETE) Game.DeleteSelectedObjects();
			if (GetKeyState(VK_CONTROL) && (KeyDown == 'c' || KeyDown == 'C')) Game.CopySelectedObject();
			if (GetKeyState(VK_CONTROL) && (KeyDown == 'v' || KeyDown == 'V')) Game.PasteCopiedObject();

			if (Gui.HasEvent())
			{
				auto Evenet{ Gui.GetEvent() };
				auto& eEventType{ Evenet.eEventType };
				
				if (eEventType == EEventType::Clicked)
				{
					if (Evenet.Widget == Gui.GetWidget("btn"))
					{
						CWindow* const Window{ (CWindow*)Gui.GetWidget("wnd") };
						Window->Open();
					}
					if (Evenet.Widget == Gui.GetWidget("wnd")->GetChild(CGUI::KSysCloseID))
					{
						CWindow* const Window{ (CWindow*)Gui.GetWidget("wnd") };
						Window->Close();
					}
				}
				
			}

			Game.WalkPlayerToPickedPoint(2.75f);
			
			Game.BeginRendering(Colors::CornflowerBlue);

			Game.Update();
			Game.Draw();

			Gui.Render();

			Game.EndRendering();

			KeyDown = 0;
		}
	}

	return 0;
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	if (g_GUI) g_GUI->GenerateEvent(hWnd, Msg, wParam, lParam);

	if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
		return 0;

	switch (Msg)
	{
	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(Msg, wParam, lParam);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(Msg, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(Msg, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}