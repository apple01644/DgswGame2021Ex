#include <DXUT.h>
#include "../Sys.h"

struct RStage1 : Room {
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
asr(RStage1);