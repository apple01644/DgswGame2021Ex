struct Virus :Prop {
	def_c(Virus);
	bool random_pos = true;
	POINT curr{};
	POINT prev{};
	POINT siz{};

	POINT delta{ rand() % 2 * 2 - 1 , rand() % 2 * 2 - 1 };
	float move_p = 0;
	float move_speed = 30;
	bool flip_x = true;
	int flag = 0;

	bool CanGo(POINT next) {
		bool result = true;
		for (int x = 0; x < siz.x && result; ++x)
			for (int y = 0; y < siz.y && result; ++y)
			{
				int rx = x + next.x;
				int ry = y + next.y;
				bool z =
					rx >= 1 && rx <= map_size.w - 2 &&
					ry >= 1 && ry <= map_size.h - 2 &&
					!map[rx][ry].blocked &&
					!map[rx][ry].healed;
				if (!z) result = false;
			}
		return result;
	}

	void Attack() {
		bool result = false;
		for (int x = 0; x < siz.x && !result; ++x)
			for (int y = 0; y < siz.y && !result; ++y)
			{
				int rx = x + curr.x;
				int ry = y + curr.y;
				bool z =
					rx >= 1 && rx <= map_size.w - 2 &&
					ry >= 1 && ry <= map_size.h - 2 &&
					map[rx][ry].painted;
				if (z) result = true;
			}
		if (result) "player"_ent->prop<Player>().Hitted();
	}

	void OnBorn() override {
		L = "OnBorn";
		if (random_pos)
			do {
				curr = {
					1 + rand() % (map_size.w - 1 - siz.y),
					1 + rand() % (map_size.h - 1 - siz.x)
				};
			} while (!CanGo(curr));
			;
			prev = curr;
	}

	void OnStep() override {
		move_p += DT * move_speed;
		if (delta.x == 0 && delta.y == 0) {
			if (move_p < 0.1)
				e.prop<Sprite>().color = f4(
					1, 1, 1, move_p / 0.1f
				);
			else
				e.prop<Sprite>().color = f4(
					1, 1, 1, 1.f - move_p / 0.9f
				);
		}
		if (move_p > 1) {
			POINT next = {
				curr.x + delta.x,
				curr.y + delta.y,
			};
			if (delta.x == 0 && delta.y == 0) {
				int org_size = siz.x;
				int new_size = rand() % 6 + 3;
				siz = {
					new_size,
					new_size
				};
				next = {
					1 + rand() % (map_size.w - 1 - new_size),
					1 + rand() % (map_size.h - 1 - new_size)
				};
				if (CanGo(next)) {
					curr = next;
					prev = curr;
					move_p = 0;
				}
				else {
					siz = {
						 org_size,
						 org_size
					};
				}
			}
			else {
				bool can_go = CanGo(next);
				if (can_go) {
					prev = curr;
					curr = next;
					move_p = 0;
				}
				else {
					if (flip_x) delta.x *= -1;
					else delta.y *= -1;
					if (rand() % 2 == 0)flip_x = !flip_x;
					prev = curr;
				}
			}
		}
		if (!CanGo(curr)) e.die();
		else Attack();

		float X = (curr.x * move_p + prev.x * (1 - move_p)) * tile_size.x;
		float Y = (curr.y * move_p + prev.y * (1 - move_p)) * tile_size.y;
		e.tran.pos.x = X;
		e.tran.pos.y = Y;
		e.tran.siz.x = tile_size.x * siz.x;
		e.tran.siz.y = tile_size.y * siz.y;
	}
};