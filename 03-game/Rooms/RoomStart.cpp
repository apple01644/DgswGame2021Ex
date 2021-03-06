#include <DXUT.h>
#include "../Sys.h"
#include "../Components.h"

struct RStart : Room {
	void OnEnter() override {
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto & wg = obj.newProp<Widget>();
			obj.tran.pos = {320, 320};
			obj.tran.siz = { 100, 30 };
			obj.tran.pos.x -= obj.tran.siz.x / 2;
			obj.tran.pos.y -= obj.tran.siz.y / 2;
			wg.text = "시작하기";
			obj.OnClick = func_{
				L"Snd/general_ok_button_click.wav"_snd.Play();
			if (next_room == nullptr)  newRoom<RStage2>();
			};
			});
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto& wg = obj.newProp<Widget>();
			obj.tran.pos = {320, 360};
			obj.tran.siz = { 100, 30 };
			obj.tran.pos.x -= obj.tran.siz.x / 2;
			obj.tran.pos.y -= obj.tran.siz.y / 2;
			wg.text = "게임소개";
			obj.OnClick = func_{
				L"Snd/general_ok_button_click.wav"_snd.Play();
			if (next_room == nullptr)  newRoom<RGameInfo>();
			};
			});
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto& wg = obj.newProp<Widget>();
			obj.tran.pos = {320, 400};
			obj.tran.siz = { 100, 30 };
			obj.tran.pos.x -= obj.tran.siz.x / 2;
			obj.tran.pos.y -= obj.tran.siz.y / 2;
			wg.text = "게임방법";
			obj.OnClick = func_{
				L"Snd/general_ok_button_click.wav"_snd.Play();
			if (next_room == nullptr)  newRoom<RGameRule>();
			};
			});
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto& wg = obj.newProp<Widget>();
			obj.tran.pos = {320, 440};
			obj.tran.siz = { 100, 30 };
			obj.tran.pos.x -= obj.tran.siz.x / 2;
			obj.tran.pos.y -= obj.tran.siz.y / 2;
			wg.text = "게임랭킹";
			obj.OnClick = func_{
				L"Snd/general_ok_button_click.wav"_snd.Play();
			if (next_room == nullptr)  newRoom < RRanking>();
			};
			});
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto& wg = obj.newProp<Widget>();
			obj.tran.pos = {320, 480};
			obj.tran.siz = { 100, 30 };
			obj.tran.pos.x -= obj.tran.siz.x / 2;
			obj.tran.pos.y -= obj.tran.siz.y / 2;
			wg.text = "크레딧";
			obj.OnClick = func_{
				L"Snd/general_ok_button_click.wav"_snd.Play();
			if (next_room == nullptr)  newRoom < RCredit>();
			};
			});
	}
	void OnLeave()override {
	}
	void OnStep() override {
		Room::OnStep();
	}
	void OnRender() override {
		Room::OnRender();
	}
	void OnMsg(UINT Msg, WPARAM wParam, LPARAM lParam) override {
		if (Msg == WM_LBUTTONDOWN) {
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(DXUTGetHWND(), &p);

			Box cursor(f2(p.x, p.y), f2(1, 1));
			Entity* re = nullptr;

			std::for_each(entities.begin(), entities.end(), [&](Entity* e) {
				if (e->hasProp<Widget>() && e->hasProp<Hitbox>() && cursor & e->hitb.getBox())
					re = e;
				});
			if (re != nullptr) re->EventClick();
		}
	}
};

asr(RStart);