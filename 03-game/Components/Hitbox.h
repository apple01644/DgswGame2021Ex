struct Hitbox : Prop {
	Hitbox(ent, f2 pos_d, f2 siz) :Prop(e), pos_d(pos_d), siz(siz) {}
	f2 pos_d = {};
	f2 siz = {};
	bool draw_hitbox = false;
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
