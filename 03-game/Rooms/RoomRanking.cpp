#include <DXUT.h>
#include "../Sys.h"
#include "../Components.h"

#include <fstream>

struct RRanking : Room {
	list<std::pair<std::string, int>> score_list;
	std::string user_initial = "";
	void OnEnter() override {
		std::ifstream ifs("score.dat");
		if (ifs.bad()) throw;
		std::string init;
		int score;
		while (!ifs.eof()) {
			ifs >> init >> score;
			score_list.push_back(std::make_pair(init, score));
		}
		score_list.sort([](const std::pair<std::string, int>& lhs, const std::pair<std::string, int>& rhs) {
			return lhs.second > rhs.second;
			});

		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto & wg = obj.newProp<Widget>();
			obj.tran.pos = { 0, 0};
			obj.tran.siz = { 30, 30 };
			wg.text = "←";
			obj.OnClick = func_{
				L"Snd/general_ok_button_click.wav"_snd.Play();
			if (next_room == nullptr)  newRoom < RStart>();
			};
			});
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto& wg = obj.newProp<Widget>();
			obj.tran.pos = { 30, 30};
			obj.tran.siz = { 580, 500 };
			wg.text = "←";
			obj.OnStep = func_{
				wg.text = "";
				for (auto tup : score_list) {
					wg.text += tup.first + " : " + std::to_string(tup.second) + "\n";
				}
			};
			});

		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto& wg = obj.newProp<Widget>();
			obj.tran.pos = { 30, 530};
			obj.tran.siz = { 580, 80 };
			wg.text = "←";
			obj.OnStep = func_{
				if (S.global_variables.write_score) {
					wg.text = "당신의 스코어: " + std::to_string(S.global_variables.score) + "\n이니셜 입력: " + user_initial;
					}
				else
					wg.text = "";
			};
			});
	}
	void OnLeave()override {
		std::ofstream ofs("score.dat");
		if (ofs.bad()) throw;
		int k = 0;
		for (auto tup : score_list) {
			ofs << tup.first << "\t" << tup.second;
			++k;
			if (k < score_list.size())
				ofs << std::endl;
		}
		ofs.close();
	}
	void OnStep() override {
		S.Render("Img/sprites/BG_cave2_1024.png"_tex, f2(), f2(640, 640));
		Room::OnStep();
	}
	void OnRender() override {
		Room::OnRender();
	}
	void OnMsg(UINT Msg, WPARAM wParam, LPARAM lParam) override {
		if (Msg == WM_LBUTTONDOWN) {
			POINT p;
			GetCursorPos(&p);
			Box cursor(f2((p.x / S.SURF_H - S.SURF_W / S.SURF_H * 0.25f) * 640, p.y / S.SURF_H * 640), f2(1, 1));

			Entity* re = nullptr;

			std::for_each(entities.begin(), entities.end(), [&](Entity* e) {
				if (e->hasProp<Widget>() && e->hasProp<Hitbox>() && cursor & e->hitb.getBox())
					re = e;
				});
			if (re != nullptr) re->EventClick();
		}
		else if (Msg == WM_KEYDOWN) {
			if (wParam >= 'A' && wParam <= 'Z' && user_initial.size() < 3) {
				user_initial.push_back(wParam);
			}
			else if (wParam == VK_BACK && user_initial.size() > 0) {
				user_initial.pop_back();
			}
			else if (wParam == VK_RETURN && user_initial.size() == 3 && S.global_variables.write_score) {
				score_list.push_back(std::make_pair(user_initial, S.global_variables.score));
				score_list.sort([](const std::pair<std::string, int>& lhs, const std::pair<std::string, int>& rhs) {
					return lhs.second > rhs.second;
					});
				S.global_variables.write_score = false;
			}
		}
		Room::OnMsg(Msg, wParam, lParam);
	}
};
asr(RRanking);