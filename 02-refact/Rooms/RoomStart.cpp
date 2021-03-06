#include "DXUT.h"

constexpr struct {
	int w = 8;
	int h = 8;
} map_size;

constexpr f2 tile_size(Window.w / (float)map_size.w, Window.h / (float)map_size.h);
struct Tile {
	Tile() {}
	Tile(int x, int y) : box(Box(f2(x* tile_size.x, y* tile_size.y), tile_size)) {}
	bool painted = false;
	Box box;
};

struct Tran : Prop {
	def_c(Tran);
	f2 pos = {};
	f2 siz = {};
};

struct Sprite : Prop {
	Sprite(ent, Texture tex) : Prop(e), tex(tex) {}
	Texture tex = nullptr;
	float depth = 0.5f;
	void OnRender() override {
		if (tex != nullptr) S.Render(tex, e.tran.pos, e.tran.siz);
	}
};

struct Hitbox : Prop {
	Hitbox(ent, f2 pos_d, f2 siz) :Prop(e), pos_d(pos_d), siz(siz) {}
	f2 pos_d = {};
	f2 siz = {};
	bool draw_hitbox = true;
	Box getBox() {
		return {
			f2(e.tran.pos.x + pos_d.x, e.tran.pos.y + pos_d.y),
			siz
		};
	}
	void OnRender() override {
		auto hitbox = getBox();
		if (draw_hitbox)S.RenderLine(hitbox.pos, hitbox.siz, f4(1, 0, 0, 1));
	}
};

struct RStart : Room {
	const char* GetName() override { return "[Room Start]"; }

	f2 player_pos{ Window.w / 2, Window.h / 2 };
	Tile map[map_size.w][map_size.h];
	obj_ player = func_obj{
		obj.newProp<Tran>();
		obj.tran.pos = f2(Window.w / 2, Window.h / 2);
		obj.tran.siz = f2(64, 64);
		obj.newProp<Sprite>("Img/sprites/helmet_5.png"_tex);
		obj.newProp<Hitbox>(f2(20, 20), f2(34, 36));
		obj.name = "player";
		obj.OnStep = func_{
			if (GetAsyncKeyState('A')) e.tran.pos.x -= 64 * DT;
			if (GetAsyncKeyState('D')) e.tran.pos.x += 64 * DT;
			if (GetAsyncKeyState('W')) e.tran.pos.y -= 64 * DT;
			if (GetAsyncKeyState('S')) e.tran.pos.y += 64 * DT;
		};
		obj.OnClick = func_{
			L"Snd/general_ok_button_click.wav"_snd.Play();
		};
	};

	void OnEnter() override {
		for (int x = 0; x < map_size.w; ++x)
			for (int y = 0; y < map_size.h; ++y)
				map[x][y] = Tile(x, y);
		CreateEntity(player);
	}
	void OnLeave()override {
	}
	void OnStep() override {
		Room::OnStep();
	}
	void OnRender() override {
		S.depth = 1;
		for (int x = 0; x < map_size.w; ++x)
			for (int y = 0; y < map_size.h; ++y)
			{
				auto& tile = map[x][y];

				if (auto player = "player"_ent; player != nullptr && tile.box & player->hitb.getBox())
					S.Render(tile.box.pos, tile_size, f4{ 0.6, 0.3, 0.3, 1 });
				S.RenderLine(tile.box.pos, tile_size, f4{ 0.3, 0.3, 0.3, 1 });
			}
		S.depth = 0;
		Room::OnRender();
		S.RenderText(Font("Consolas", 32), L"Hello World", f2());
	}
	void OnMsg(UINT Msg, WPARAM wParam, LPARAM lParam) override {
		if (Msg == WM_LBUTTONDOWN) {
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(DXUTGetHWND(), &p);

			Box cursor(f2(p.x, p.y), f2(1, 1));
			Entity* re = nullptr;

			std::for_each(entities.begin(), entities.end(), [&](Entity* e) {
				if (e->hasProp<Sprite>() && e->hasProp<Hitbox>() && cursor & e->hitb.getBox())
					if (re == nullptr || re->prop<Sprite>().depth < e->prop<Sprite>().depth)
						re = e;
				});
			if (re != nullptr) re->EventClick();
		}
	}
};

void Sys::SetStartRoom() {
	next_room = new RStart;
	L = "[Sys SetStartRoom]", next_room == nullptr;
}