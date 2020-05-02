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
CGame* g_Game{};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(1374600);
#endif

	/*CBFNTBaker Baker{};
	Baker.AddCharRange(SCharRange(0, 0x33DD));
	Baker.AddCharRange(SCharRange(0x4E00, 0x9FFF)); // CJK ideographs
	Baker.AddCharRange(SCharRange(0xAC00, 0xD7A3)); // Korean [°¡, ÆR]
	Baker.AddCharRange(SCharRange(0xFF00, 0xFFEE)); // Full-width forms
	Baker.BakeFont("Asset\\D2Coding.ttf", 16, "Asset");*/

	static constexpr XMFLOAT2 KGameWindowSize{ 1280.0f, 720.0f };
	CGame Game{ hInstance, KGameWindowSize };
	g_Game = &Game;

	Game.CreateWin32(WndProc, u8"JEngine", true, false);

	Game.SetRenderingFlags(CGame::EFlagsRendering::UseLighting | CGame::EFlagsRendering::DrawMiniAxes | CGame::EFlagsRendering::DrawGrid |
		CGame::EFlagsRendering::DrawTerrainHeightMapTexture | CGame::EFlagsRendering::DrawTerrainMaskingTexture | CGame::EFlagsRendering::DrawIdentifiers |
		CGame::EFlagsRendering::DrawTerrainFoliagePlacingTexture | CGame::EFlagsRendering::TessellateTerrain | 
		CGame::EFlagsRendering::Use3DGizmos | CGame::EFlagsRendering::UsePhysicallyBasedRendering);

	//Game.CreateDynamicSky("Asset\\Sky.xml", 30.0f);
	Game.CreateStaticSky(30.0f);

	//Game.LoadScene("Scene\\mayan_dungeon.scene");
	Game.LoadScene("Scene\\ai_test.scene");
	Game.SetMode(CGame::EMode::Play);


	CGUI Gui{ Game.GetDevicePtr(), Game.GetDeviceContextPtr() };
	Gui.Create(Game.GethWnd());
	
	Gui.CreateButton("btn", SInt2(96, 32));
	Gui.GetWidget("btn")->SetOffset(SInt2(20, 20));
	Gui.GetWidget("btn")->SetCaption(u8"½Ã½ºÅÛ");
	Gui.GetWidget("btn")->SetCaptionColor(KDefaultFontColor);

	{
		Gui.CreateWindowWidget(CGUI::EWindowType::Default, "wnd");
		CWindow* const Window{ (CWindow*)Gui.GetWidget("wnd") };
		Window->SetOffset(SInt2(200, 40));
		Window->SetCaption(u8"½ÃÇè¿ë Ã¢!");

		Gui.CreateText("tx", SInt2(100, 30), u8"°¡³ª´Ùabc", Window);
		CText* tx{ (CText*)Window->GetChild("tx") };
		tx->SetBackgroundColor(SFloat4(1, 1, 1, 0.25f));

		Gui.CreateImageButton(CGUI::EImageButtonType::Button, "imgbtn", SInt2(288, 72), Window);
		Window->GetChild("imgbtn")->SetOffset(SInt2(20, 30));
		Window->GetChild("imgbtn")->SetSelectionSize(SInt2(230, 60));
		Window->GetChild("imgbtn")->SetCaption(u8"ÀÌ¹ÌÁö ¹öÆ°");
		Window->GetChild("imgbtn")->SetCaptionColor(SFloat4(1, 1, 0.75f, 1));

		Gui.CreateTextEdit("edit", SInt2(100, 30), Window);
		CTextEdit* edit{ (CTextEdit*)Window->GetChild("edit") };
		edit->SetOffset(SInt2(0, 100));

		Gui.CreateButton("quit", SInt2(100, 30), Window);
		Window->GetChild("quit")->SetOffset(SInt2(0, 140));
		Window->GetChild("quit")->SetCaption(u8"Á¾·á");
		Window->GetChild("quit")->SetCaptionColor(KDefaultFontColor);
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
			
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else
		{
			//if (KeyDown == VK_SPACE) Game.JumpPlayer(5.0f);
			if (KeyDown == VK_DELETE) Game.DeleteSelectedObjects();
			if (GetKeyState(VK_CONTROL) && (KeyDown == 'c' || KeyDown == 'C')) Game.CopySelectedObject();
			if (GetKeyState(VK_CONTROL) && (KeyDown == 'v' || KeyDown == 'V')) Game.PasteCopiedObject();

			while (Gui.HasEvent())
			{
				auto Evenet{ Gui.GetEvent() };
				auto& eEventType{ Evenet.eEventType };
				
				if (eEventType == EEventType::Clicked)
				{
					CWindow* const Window{ (CWindow*)Gui.GetWidget("wnd") };
					if (Evenet.Widget == Gui.GetWidget("btn"))
					{
						Window->Open();
					}
					else if (Evenet.Widget == Window->GetChild("quit"))
					{
						Game.Destroy();
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
	if (g_GUI)
	{
		if (g_GUI->GenerateEvent(hWnd, Msg, wParam, lParam))
		{
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		}
	}

	if (g_Game->GetMode() != CGame::EMode::Play)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
			return 0;
	}

	switch (Msg)
	{
	case WM_LBUTTONDOWN:
		g_Game->NotifyMouseLeftDown();
		Mouse::ProcessMessage(Msg, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		g_Game->NotifyMouseLeftUp();
		Mouse::ProcessMessage(Msg, wParam, lParam);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
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
	case WM_ACTIVATEAPP:
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(Msg, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_IME_CHAR: // @important
		return 0;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}
