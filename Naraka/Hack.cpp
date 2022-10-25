#include "Sdk.h"
#include "includes.h"
#include "Hack.h"
#include <vector>

MemoryToolsWrapper* memorytools;
Driver* pdriver;

std::vector<uint64_t> entitylist;
DWORD dwLoopThreadId = 0;
extern HANDLE hFinishEvent;

namespace config
{
	bool ���﷽�� = true;
	bool �����ǳ� = true;
	bool �Զ��� = true;
}

void InitializeAddress()
{
	HWND NarakaWindow = NULL;
	DWORD NarakaProcess = NULL;
	while (NarakaWindow == NULL || NarakaProcess == NULL)
	{
		NarakaWindow = FindWindowA("UnityWndClass", "Naraka");
		if (NarakaWindow)
			GetWindowThreadProcessId(NarakaWindow, &NarakaProcess);
		Sleep(2000);
	}
	pdriver = new Driver();
	pdriver->InitializeInterface();
	memorytools = new MemoryToolsWrapper(NarakaProcess, pdriver);
	ULONG64 UnityPlayer = 0;
	ULONG64 GameAssembly = 0;

	while (true)
	{
		UnityPlayer = pdriver->GetModule(NarakaProcess, L"UnityPlayer.dll");
		GameAssembly = pdriver->GetModule(NarakaProcess, L"GameAssembly.dll");
		if (UnityPlayer && GameAssembly)
			break;
	}
	sdk::initialize(UnityPlayer, GameAssembly);

	MessageBoxA(NULL, "Ok", "Done", MB_ICONINFORMATION);
}

void draw_menu(Nvidia* overlay)
{
	static bool open = true;
	if (GetAsyncKeyState(VK_INSERT) & 1) {
		open = !open;
	}
	if (open)
	{
		overlay->draw_text_yellow(10, 50, L"[INS ��ʾ->����]");

		if (config::���﷽��)
		{
			overlay->draw_text_yellow(10, 70, L"��ҷ��� -> F6");
		}
		if (!config::���﷽��)
		{
			overlay->draw_text_white(10, 70, L"��ҷ��� -> F6");
		}
		if (config::�����ǳ�)
		{
			overlay->draw_text_yellow(10, 90, L"������� -> F7");
		}
		if (!config::�����ǳ�)
		{
			overlay->draw_text_white(10, 90, L"������� -> F7");
		}

		if (config::�Զ���)
		{
			overlay->draw_text_yellow(10, 110, L"�Զ��� -> F8");
		}
		if (!config::�Զ���)
		{
			overlay->draw_text_white(10, 110, L"�Զ��� -> F8");
		}
		if (GetAsyncKeyState(VK_F6) & 1)
		{
			config::���﷽�� = !config::���﷽��;
		}
		if (GetAsyncKeyState(VK_F7) & 1)
		{
			config::�����ǳ� = !config::�����ǳ�;
		}
		if (GetAsyncKeyState(VK_F8) & 1)
		{
			config::�Զ��� = !config::�Զ���;
		}
	}
}

void draw_esp(Nvidia* overlay)
{
	auto getboxrect = [](D2D1_RECT_F& BoxRect, Vector3& Bottom, Vector3& Top) {
		BoxRect.bottom = Bottom.y - Top.y;
		BoxRect.right = BoxRect.bottom / 2;
		BoxRect.left = Top.x - BoxRect.bottom / 4;
		BoxRect.top = Top.y;
	};
	for (auto entity : entitylist)
	{
		Vector3 entitypos = sdk::get_enitypos(entity);
		Vector3 headpos = entitypos;
		headpos.y += 1.8;
		if (sdk::get_view_matrix().ToScreenPos(entitypos, 1920, 1080)
			&&
			sdk::get_view_matrix().ToScreenPos(headpos, 1920, 1080)
			)
		{
			D2D1_RECT_F box;
			getboxrect(box, entitypos, headpos);

			if (config::���﷽��)
			{
				overlay->draw_rect(box.left, box.top, box.right, box.bottom, 1.5);
			}
			if (config::�����ǳ�)
				overlay->draw_text_white(headpos.x, headpos.y + 3, sdk::get_name(entity).c_str());
		}

	}
}

void update()
{
	entitylist.clear();
	uint32_t playercount = sdk::get_playerCount();
	uintptr_t playeritems = sdk::get_playeritems();
	uintptr_t localaddr = sdk::get_local();
	uintptr_t localteamid = sdk::get_team(localaddr);
	for (size_t i = 0; i < playercount; i++)
	{
		uintptr_t entityaddr = sdk::get_entityaddr(playeritems, i);
		uintptr_t entityteamid = sdk::get_team(entityaddr);
		if (localaddr != entityaddr && entityteamid != localteamid)//�ų����� �� �Լ�
		{
			entitylist.push_back(entityaddr);
		}
	}
}

void entity_loop()
{
	if (!config::�Զ���) return;
	auto local = sdk::get_local();
	for (auto entity : entitylist)
	{
		if (sdk::is_weapon(sdk::get_weapon(entity)) && sdk::is_weapon(sdk::get_weapon(local)))//�ж��Ƿ��ֳ����� 
		{
			if (sdk::get_endurelevel(local) < 21)//�жϵ�ǰ״̬ �Ƿ���Ҫȥ��
			{
				Vector3 entitypos = sdk::get_enitypos(local);
				Vector3 localpos = sdk::get_enitypos(entity);
				auto distance = localpos.distTo(entitypos);
				float angle = fabsf(math::AngleDifference(sdk::get_enityangle(entity), math::VectorToRotationYaw(math::FindLookAtVector(entitypos, localpos))));//�������� �Լ� �� �Ƕ�

				if (distance > 5.9f)
					continue;

				//�����幥��?
				if (!sdk::is_attacking(entity))
					continue;

				if (angle > 68)//�жϵ����Ƿ�����Լ� �ų�����
					continue;

				//����G��Ϊ���ȼ�
				keybd_event('G', MapVirtualKey('G', MAPVK_VK_TO_VSC), 0, 0);
				Sleep(10);
				keybd_event('G', MapVirtualKey('G', MAPVK_VK_TO_VSC), 2, 0);
				Sleep(400);
			}
		}

	}

}