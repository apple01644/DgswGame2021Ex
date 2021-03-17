#include <DXUT.h>
#include "../Sys.h"

struct RIntro : Room {
	float stopwatch_start = 0;
	void OnEnter() override {
		stopwatch_start = T;
	}
	void OnLeave()override {
	}
	void OnStep() override {
		Room::OnStep();
		if (T - stopwatch_start > 6 && next_room == nullptr)  newRoom < RStart>();
	}
	void OnRender() override {
		S.Render(f2(0, 0), f2(640, 640), f4(1, 1, 1, 1));
		S.Render("Img/intro.png"_tex, f2(0, -400 * (T - stopwatch_start)), f2(640, 800 * 3));
		Room::OnRender();
	}
	void OnMsg(UINT Msg, WPARAM wParam, LPARAM lParam) override {
		Room::OnMsg(Msg, wParam, lParam);
	}
};
asr(RIntro);