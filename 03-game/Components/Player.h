#include "Stage.h"

struct Player : Prop {
	def_c(Player);

	POINT curr{};
	POINT prev{};
	POINT entry1, entry2;

	float move_p = 0;
	char mode = 'N';
	int life = 5;
	float hit_time = 1;

	int item_speed = 3;
	int item_defense = 3;
	int item_superman = 1;
	int item_health = 2;
	int item_random = 5;
	std::string item_text = "";

	float superman_time = 0;

	enum ItemState {
		Null,
		Speed,
		Defense,
		Superman,
		Health,
		Random
	};

	ItemState item_state = ItemState::Null;

	f2 item_get_pos{};

	void SetItemEffect(ItemState is) {
		item_get_pos.x = curr.x * tile_size.x - 50;
		item_get_pos.y = curr.y * tile_size.y;
		if (is == ItemState::Random) {
			item_text = "∑£¥˝æ∆¿Ã≈€ »πµÊ";
			if (int k = rand() % 4; k == 0) is = ItemState::Speed;
			else if (k == 1) is = ItemState::Defense;
			else if (k == 2) is = ItemState::Superman;
			else is = ItemState::Health;
		}
		else if (is == ItemState::Speed) item_text = "º”µµæ∆¿Ã≈€ »πµÊ";
		else if (is == ItemState::Defense) item_text = "πÊæÓæ∆¿Ã≈€ »πµÊ";
		else if (is == ItemState::Superman) item_text = "π´¿˚æ∆¿Ã≈€ »πµÊ";
		else if (is == ItemState::Health) item_text = "√º∑¬æ∆¿Ã≈€ »πµÊ";
		if (item_state != is) {
			if (item_state == ItemState::Superman) superman_time = 0;
			if (is == ItemState::Superman) superman_time = 5;

			else if (is == ItemState::Health)
				if (life < 5) ++life;
				else S.global_variables.hp_score += 500;
			item_state = is;
		}
	}

	int FillEntry(list<POINT> start_p, char flag) {
		int result = 1;

		list<POINT> p_list = start_p;
		list<POINT> p_buffer;

		do {
			for (const POINT& p : p_list) {
				if (p.x - 1 >= 1)
					if (auto& o = map[p.x - 1][p.y]; !o.healed && o.flag == 'N' && !o.blocked) o.flag = 'X';
				if (p.x + 1 <= map_size.w - 2)
					if (auto& o = map[p.x + 1][p.y];  !o.healed && o.flag == 'N' && !o.blocked) o.flag = 'X';
				if (p.y - 1 >= 1)
					if (auto& o = map[p.x][p.y - 1];  !o.healed && o.flag == 'N' && !o.blocked) o.flag = 'X';
				if (p.y + 1 <= map_size.h - 2)
					if (auto& o = map[p.x][p.y + 1];  !o.healed && o.flag == 'N' && !o.blocked)  o.flag = 'X';
			}
			for (int x = 1; x < map_size.w - 1; ++x)
				for (int y = 1; y < map_size.h - 1; ++y)
					if (map[x][y].flag == 'X')
					{
						map[x][y].flag = flag;
						result += 1;
						p_buffer.push_back({ x, y });
					}
			p_list.swap(p_buffer);
			p_buffer.clear();
		} while (p_list.size());

		return result;
	}

	void FillArea() {
		list<POINT> entries1;
		list<POINT> entries2;

		for (int x = 0; x < map_size.w; ++x)
			for (int y = 0; y < map_size.h; ++y)
				if (map[x][y].painted) {
					map[x][y].healed = true;
				}
				else if (map[x][y].flag == '1') entries1.push_back({ x, y });
				else if (map[x][y].flag == '2') entries2.push_back({ x, y });

		int count1 = FillEntry(entries1, '1');
		int count2 = FillEntry(entries2, '2');

		if (count1 < count2)
		{
			for (int x = 0; x < map_size.w; ++x)
				for (int y = 0; y < map_size.h; ++y)

					if (map[x][y].flag == '1') map[x][y].healed = true;
		}
		else {
			for (int x = 0; x < map_size.w; ++x)
				for (int y = 0; y < map_size.h; ++y)

					if (map[x][y].flag == '2')map[x][y].healed = true;
		}
		for (int x = 0; x < map_size.w; ++x)
			for (int y = 0; y < map_size.h; ++y) {
				map[x][y].flag = 'N';
				map[x][y].painted = false;
			}
	}

	void OnRender() override {
		S.RenderText(Font("Consolas", 25), item_text, f2(item_get_pos.x + 1, item_get_pos.y + 1), f2{ 100, 30 });
		item_get_pos.y -= 10 * DT * 32;
		S.RenderText(Font("Consolas", 25), item_text, f2(item_get_pos.x, item_get_pos.y), f2{ 100, 30 }, f4{ 1,1,1,1 });
	}

	void OnMsg(UINT Msg, WPARAM wParam, LPARAM lParam) override {
		if (Msg == WM_KEYDOWN) {
			if (wParam == VK_F2)
				SetItemEffect(ItemState::Random);
		}
	}

	void OnStep() override {
		if (life <= 0) return;
		if (superman_time >= 0) {
			superman_time -= DT;
			if (superman_time < 0) {
				item_state = ItemState::Null;
			}
		}
		float X = (curr.x * move_p + prev.x * (1 - move_p)) * tile_size.x;
		float Y = (curr.y * move_p + prev.y * (1 - move_p)) * tile_size.y;

		e.tran.pos.x = X - e.tran.siz.x / 2;
		e.tran.pos.y = Y - e.tran.siz.y / 2;

		/**/ if (GetAsyncKeyState('A'))  mode = 'A';
		else if (GetAsyncKeyState('D'))  mode = 'D';
		else if (GetAsyncKeyState('W'))  mode = 'W';
		else if (GetAsyncKeyState('S'))  mode = 'S';
		else mode = 'N';

		if (mode == 'A' && curr.x > 0 ||
			mode == 'D' && curr.x + 1 < map_size.w ||
			mode == 'W' && curr.y > 0 ||
			mode == 'S' && curr.y + 1 < map_size.h
			)
		{
			move_p += DT * 50;
			if (item_state == ItemState::Speed)
				move_p += DT * 50;
		}
		while (move_p > 1) {
			move_p -= 1;
			prev = curr;

			POINT next{};
			if (mode == 'A') next = { curr.x - 1, curr.y };
			else if (mode == 'D') next = { curr.x + 1, curr.y };
			else if (mode == 'W') next = { curr.x, curr.y - 1 };
			else if (mode == 'S') next = { curr.x, curr.y + 1 };

			if (next.x >= 0 && next.x < map_size.w && next.y >= 0 && next.y < map_size.h &&
				!map[next.x][next.y].painted
				) {
				if (!map[next.x][next.y].blocked)
					curr = next;
				else Hitted();
			}

			if (!map[prev.x][prev.y].healed) {
				map[prev.x][prev.y].painted = true;
				/**/ if (mode == 'A') {
					if (curr.y + 1 <= map_size.h - 1) {
						entry1 = { curr.x, curr.y + 1 };
						map[entry1.x][entry1.y].flag = '1';
					}
					if (curr.y - 1 >= 0) {
						entry2 = { curr.x, curr.y - 1 };
						map[entry2.x][entry2.y].flag = '2';
					}
				}
				else if (mode == 'D') {
					if (curr.y - 1 >= 0) {
						entry1 = { curr.x, curr.y - 1 };
						map[entry1.x][entry1.y].flag = '1';
					};
					if (curr.y + 1 <= map_size.h - 1) {
						entry2 = { curr.x, curr.y + 1 };
						map[entry2.x][entry2.y].flag = '2';
					};
				}
				else if (mode == 'W') {
					if (curr.x - 1 >= 0) {
						entry1 = { curr.x - 1 , curr.y };
						map[entry1.x][entry1.y].flag = '1';
					};
					if (curr.x + 1 <= map_size.w - 1) {
						entry2 = { curr.x + 1 , curr.y };
						map[entry2.x][entry2.y].flag = '2';
					};
				}
				else if (mode == 'S') {
					if (curr.x + 1 <= map_size.w - 1) {
						entry1 = { curr.x + 1 , curr.y };
						map[entry1.x][entry1.y].flag = '1';
					};
					if (curr.x - 1 >= 0) {
						entry2 = { curr.x - 1 , curr.y };
						map[entry2.x][entry2.y].flag = '2';
					};
				}
			}

			if (!map[prev.x][prev.y].healed && map[curr.x][curr.y].healed) {
				FillArea();
				while (item_speed + item_defense + item_superman + item_health + item_random > 0) {
					if (int idx = rand() % 5; idx == 0 && item_speed > 0) {
						item_speed -= 1;
						SetItemEffect(ItemState::Speed);
						break;
					}
					else if (idx == 1 && item_defense > 0) {
						item_defense -= 1;
						SetItemEffect(ItemState::Defense);
						break;
					}
					else if (idx == 2 && item_superman > 0) {
						item_superman -= 1;
						SetItemEffect(ItemState::Superman);
						break;
					}
					else if (idx == 3 && item_health > 0) {
						item_health -= 1;
						SetItemEffect(ItemState::Health);
						break;
					}
					else if (item_random > 0) {
						item_random -= 1;
						SetItemEffect(ItemState::Random);
						break;
					}
				}
			}
		}
		float colorg = life / 5.f;
		if (superman_time > 0) {
			e.prop<Sprite>().color = f4(
				0.5, 0.5, 1, 0.75 + 0.25 * sinf(T * 60 / XM_2PI)
			);
		}
		else if (hit_time < 1) {
			hit_time += DT * 0.5;
			e.prop<Sprite>().color = f4(
				1, colorg, colorg, 0.5 + 0.5 * sinf(hit_time * 60 / XM_2PI)
			);
		}
		else e.prop<Sprite>().color = f4(1, colorg, colorg, 1);
	}
	void OnClick() override {
		L"Snd/general_ok_button_click.wav"_snd.Play();
	}

	void Hitted() {
		if (hit_time >= 1) {
			if (superman_time > 0);
			else  if (item_state == ItemState::Defense) {
				item_state = ItemState::Null;
			}
			else life -= 1;
			hit_time = 0;
		}
	}
};