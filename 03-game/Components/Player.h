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

	void OnStep() override {
		if (life <= 0) return;
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
			move_p += DT * 50;
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
			}
		}
		float colorg = life / 5.f;
		if (hit_time < 1) {
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
			life -= 1;
			hit_time = 0;
		}
	}
};