#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include "imgui/kiero/kiero.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
typedef uintptr_t PTR;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool bIsReady = false;
bool bShowWindow = true;
bool init = false;
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	if (bShowWindow) {
		ImGui::SetNextWindowSize(ImVec2(360, 558));
		ImGui::SetNextWindowBgAlpha(0.7f);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		const auto Flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

		{
			ImGui::Begin("Uranium Menu", reinterpret_cast<bool*>(true), Flags);

			if (ImGui::BeginTabBar("")) {
				if (!bIsReady) { // we only show this when your in the lobby
					if (ImGui::BeginTabItem("Lobby")) {

						static char InputMap[512] = "";

						ImGui::InputText("##Map", InputMap, IM_ARRAYSIZE(InputMap));

						ImGui::SameLine();

						if (ImGui::Button("LoadMap")) {
							std::string MapAsString = (const char*)InputMap;
							std::wstring MapAsWString = std::wstring(MapAsString.begin(), MapAsString.end());

							MapAsWString += L"?Game=/Game/Athena/Athena_GameMode.Athena_GameMode_C";

							playerControllerFunctions->SwitchLevel(MapAsWString.c_str());
							bIsReady = true;
						}

						//ImGui::NewLine();

						ImGui::Text("To load into a match, enter the name of the map\nyou want, then press \"F1\" after your\nloading bar is full!");

						ImGui::NewLine();

						ImGui::Text("List of maps:");
						ImGui::Text("Artemis_Terrain - Main battle royale island");
						ImGui::Text("Apollo_Papaya - Party Royale island");

						ImGui::EndTabItem();
					}
				}
				else { //Only show ingame
					if (ImGui::BeginTabItem("Game")) {
						if (ImGui::Button("Exit Game")) {
							exit(0);
						}

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Player")) {
						int Height = 60000;

						ImGui::InputInt("##Height", &Height);

						ImGui::SameLine();

						if (ImGui::Button("TeleportToSkydive")) {
							pawnFunctions->TeleportToSkydive(Height);
						}

						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Level Actors")) {
						auto levelActors = kismetLibraryFunctions->GetAllActorsOfClass(FindObject("/Script/Engine.Actor"));

						//const char* Items[] = {""};

						for (int i = 0; i < levelActors.Num(); i++)
						{
							auto Actor = levelActors[i];

							if (Actor != nullptr) {
								//Items[i] = Actor->GetName().c_str();
								ImGui::Text(Actor->GetName().c_str());
							}
						}
					}
				}

				if (ImGui::BeginTabItem("About")) {
					ImGui::Text("This project was created by Jacobb626 and M1.\nWith help from others such as Kemo, Dani, Ender and Fevers.\nIt is currently maintained by Jacobb626 and Fevers!");
				}

				ImGui::EndTabBar();
			}

			ImGui::End();

			ImGui::Render();

			pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
	}

	return oPresent(pSwapChain, SyncInterval, Flags);
}

void SetupGUI()
{
	bool init_hook = false;
	do
	{
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	return;
}