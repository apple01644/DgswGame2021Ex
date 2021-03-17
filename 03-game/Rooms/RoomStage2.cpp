#include "DXUT.h"
#include "../Sys.h"
#include "../Components.h"

struct RStage2 : Room {
	float lefttime = 180;

	obj_ player = func_obj{
		obj.newProp<Tran>();
		obj.tran.pos = f2(Window.w / 2, Window.h / 2);
		obj.tran.siz = f2(64, 64);
		obj.newProp<Sprite>("Img/sprites/helmet_5.png"_tex);
		obj.newProp<Hitbox>(f2(20, 20), f2(34, 36));
		obj.name = "player";
		auto& pPlayer = obj.newProp<Player>();

		if (int idx = rand() % 4; idx == 0)
			pPlayer.curr = { 0, rand() % map_size.h };
		else if (idx == 1)
			pPlayer.curr = { map_size.w - 1, rand() % map_size.h };
		else if (idx == 2)
			pPlayer.curr = { rand() % map_size.w, 0 };
		else
			pPlayer.curr = { rand() % map_size.w, map_size.h - 1 };
		pPlayer.prev = pPlayer.curr;
	};

	obj_ speed_virus = func_obj{
		obj.newProp<Tran>();
		obj.name = "virus";
		obj.newProp<Sprite>("Img/sprites/redback_new.png"_tex);
		auto& virus = obj.newProp<Virus>();
		virus.siz = { 3,3 };
		virus.move_speed = 15.f;
	};

	obj_ size_virus = func_obj{
		obj.newProp<Tran>();
		obj.name = "virus";
		obj.newProp<Sprite>("Img/sprites/redback_new.png"_tex);
		auto& virus = obj.newProp<Virus>();
		virus.siz = { 6,6 };
		virus.move_speed = 5.f;
	};

	obj_ flash_virus = func_obj{
		obj.newProp<Tran>();
		obj.name = "virus";
		obj.newProp<Sprite>("Img/sprites/explosion6.png"_tex);
		auto& virus = obj.newProp<Virus>();
		virus.delta = { 0 ,0 };
		virus.siz = { 6,6 };
		virus.move_speed = 0.2f;
	};

	obj_ toxino_virus = func_obj{
		obj.newProp<Tran>();
		obj.name = "boss";
		obj.newProp<Sprite>("Img/sprites/draconic_base-green_new.png"_tex);
		auto& virus = obj.newProp<Virus>();
		virus.siz = { 12, 12 };
		virus.move_speed = 15.f;
		obj.OnStep = func_{
			if ("virus"_ent_count < 10 && virus.flag == 0) {
				virus.flag = 1;
				for (int x = 0; x < 5; ++x)
				{
					auto& e = CreateEntity(speed_virus);
					e.prop<Virus>().random_pos = 0;
					e.prop<Virus>().curr = virus.curr;
				}
				for (int x = 0; x < 4; ++x)
				{
					auto& e = CreateEntity(size_virus);
					e.prop<Virus>().random_pos = 0;
					e.prop<Virus>().curr = virus.curr;
				}
				for (int x = 0; x < 3; ++x)
				{
					auto& e = CreateEntity(flash_virus);
					e.prop<Virus>().random_pos = 0;
					e.prop<Virus>().curr = virus.curr;
				}
			}
		};
	};

	void OnEnter() override {
		for (int x = 0; x < map_size.w; ++x)
			for (int y = 0; y < map_size.h; ++y)
			{
				map[x][y] = Tile(x, y);
				if (x == 0 || y == 0 || x == map_size.w - 1 || y == map_size.h - 1)
					map[x][y].healed = true;
			}
		for (int z = 0; z < 7; ++z) {
			int r = 2 + rand() % 6;
			int cx = rand() % (map_size.w - r * 2) + r;
			int cy = rand() % (map_size.h - r * 2) + r;
			for (int x = 0; x < map_size.w; ++x)
				for (int y = 0; y < map_size.h; ++y)
					if (powf(x - cx, 2) + powf(y - cy, 2) < powf(r, 2))
						map[x][y].blocked = true;
		}
		CreateEntity(player);
		for (int x = 0; x < 5; ++x)
			CreateEntity(speed_virus);
		for (int x = 0; x < 4; ++x)
			CreateEntity(size_virus);
		for (int x = 0; x < 3; ++x)
			CreateEntity(flash_virus);
		CreateEntity(toxino_virus);
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto & wg = obj.newProp<Widget>();
			obj.tran.pos = { 30, 170};
			obj.tran.siz = { 580, 300 };
			obj.name = "gameover";
			wg.text = "VIM이 파괴되었습니다. Enter를 누르면 다시 시작\nBackspace를 누르면 메뉴로 돌아갑니다.";
			wg.visible = false;
			});
		CreateEntity(func_obj{
			obj.newProp<Tran>();
			auto& wg = obj.newProp<Widget>();
			obj.tran.pos = { 30, 170};
			obj.tran.siz = { 580, 300 };
			obj.name = "game_win";
			obj.OnStep = func_{
				wg.text = "구역이 정화되었습니다. Enter를 누르면 랭킹등록합니다\n점수:" + std::to_string(S.global_variables.score);
			};
			wg.visible = false;
			});
		S.global_variables.write_score = false;
		S.global_variables.hp_score = 0;
	}
	void OnLeave()override {
	}
	void OnStep() override {
		Room::OnStep();
		if (S.global_variables.superman)
			"player"_ent->prop<Player>().life = 5;
		if (("player"_ent->prop<Player>().life <= 0 || lefttime <= 0) && !"game_win"_ent->prop<Widget>().visible)
			"gameover"_ent->prop<Widget>().visible = true;
	}
	void OnRender() override {
		S.depth = 1;
		int healed_count = 0;
		int all_count = (map_size.w - 2) * (map_size.h - 2);
		for (int x = 0; x < map_size.w; ++x)
			for (int y = 0; y < map_size.h; ++y)
			{
				if (x >= 1 && x <= map_size.w - 2 && y >= 1 && y <= map_size.h - 2 && map[x][y].healed)
					++healed_count;

				auto& tile = map[x][y];
				float colorx = (2 + sinf(T + x / 3.f) + sinf(T + y / 3.f)) / 4 * 0.1 + 0.1;
				float colory = (2 + sinf(T + x / 3.f) + sinf(T + y / 3.f)) / 4 * 0.5;
				float colorz = (2 + sinf(T * 15 + x) + sinf(T * 15 + y)) / 4 * 0.5 + 0.5;
				float colorw = 0.5 * (1 + sinf((x + y) / 1.f + 0.75 * sinf((x - y) / 3.f))) * 0.05;
				if (tile.blocked) {
					S.Render(tile.box.pos, tile_size, f4(colorw, colorw, colorw, 1));
					all_count -= 1;
				}
				else if (tile.healed) S.Render(tile.box.pos, tile_size, f4(0.1 + colory, 0.5 + colory, 0.5 + colory, 1));
				else if (tile.painted) S.Render(tile.box.pos, tile_size, f4(colorz, colorz, colorz - 0.5, 1));
				else S.Render(tile.box.pos, tile_size, f4{ colorx, colorx, colorx, 1 });
				//S.RenderLine(tile.box.pos, tile_size, f4{ 0.3, 0.3, 0.3, 1 });
			}
		S.depth = 0;
		Room::OnRender();

		int percent = 100.f * healed_count / all_count;
		string text = "치료율 " + std::to_string(percent) + "%";
		if (percent >= 80 && "boss"_ent == nullptr && !"gameover"_ent->prop<Widget>().visible)
		{
			"game_win"_ent->prop<Widget>().visible = true;
			S.global_variables.write_score = true;
		}
		lefttime -= DT;
		S.RenderText(Font("Consolas", 16), text, f2(2, 6), f4(0, 0, 0, 1));
		S.RenderText(Font("Consolas", 16), text, f2(0, 4), f4(1, 1, 1, 1));
		text = "남은시간: " + std::to_string((int)lefttime) + "   점수: " + std::to_string(healed_count);
		S.global_variables.score = healed_count;
		S.RenderText(Font("Consolas", 16), text, f2(2, 20), f4(0, 0, 0, 1));
		S.RenderText(Font("Consolas", 16), text, f2(0, 18), f4(1, 1, 1, 1));
	}
	void OnMsg(UINT Msg, WPARAM wParam, LPARAM lParam) override {
		if (Msg == WM_KEYDOWN) {
			if (wParam == VK_F1) {
				S.global_variables.superman = !S.global_variables.superman;
			}
			else if (wParam == VK_F3) {
				"player"_ent->prop<Player>().life = 5;
			}
			else if ("gameover"_ent->prop<Widget>().visible) {
				if (wParam == VK_RETURN)
				{
					ContinueGame();
				}
				else if (wParam == VK_BACK)
				{
					if (next_room == nullptr) newRoom<RStart>();
				}
			}
			else if ("game_win"_ent->prop<Widget>().visible) {
				if (wParam == VK_RETURN)
				{
					if (next_room == nullptr) newRoom<RRanking>();
				}
			}
		}
		Room::OnMsg(Msg, wParam, lParam);
	}

	void ContinueGame();
};
asr(RStage2);
void RStage2::ContinueGame() {
	if (next_room == nullptr)  newRoom < RStage2>();
}