#include <DXUT.h>
#include "../Sys.h"

struct RRanking : Room {
	void OnEnter() override {
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
	}
};
asr(RRanking);