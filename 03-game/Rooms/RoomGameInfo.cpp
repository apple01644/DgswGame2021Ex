#include <DXUT.h>
#include "../Sys.h"
#include "../Components.h"

struct RGameInfo : Room {
	void OnEnter() override {
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto & wg = obj.newProp<Widget>();
			obj.tran.pos = { 30, 30};
			obj.tran.siz = { 580,580 };
			wg.text = "2000년 부터 지능형 바이러슨는 역사적인 바이러스\n변이를 거쳐 인류의 바이러세 대한 저항력을\n측정해왔다. 어쩌구 저쩌구 VIM을 통해 퇴치한다";
			});
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto& wg = obj.newProp<Widget>();
			obj.tran.pos = { 0, 0};
			obj.tran.siz = { 30, 30 };
			wg.text = "←";
			obj.OnClick = func_{
				L"Snd/general_ok_button_click.wav"_snd.Play();
			if (next_room == nullptr)  newRoom < RStart>();
			};
			});
	}
	void OnLeave()override {
	}
	void OnStep() override {
		Room::OnStep();
	}
	void OnRender() override {
		S.Render("Img/sprites/BG_cave2_1024.png"_tex, f2(), f2(640, 640));
		S.Render("Img/game_info.png"_tex, f2(30, 30), f2(580, 580));
		Room::OnRender();
	}
	void OnMsg(UINT Msg, WPARAM wParam, LPARAM lParam) override {
		if (Msg == WM_LBUTTONDOWN) {
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(DXUTGetHWND(), &p);

			Box cursor(f2((p.x / S.SURF_H - S.SURF_W / S.SURF_H * 0.25f) * 640, p.y / S.SURF_H * 640), f2(1, 1));
			Entity* re = nullptr;

			std::for_each(entities.begin(), entities.end(), [&](Entity* e) {
				if (e->hasProp<Widget>() && e->hasProp<Hitbox>() && cursor & e->hitb.getBox())
					re = e;
				});
			if (re != nullptr) re->EventClick();
		}
		Room::OnMsg(Msg, wParam, lParam);
	}
};
asr(RGameInfo);