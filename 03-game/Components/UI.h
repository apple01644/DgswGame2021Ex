struct Widget : Prop {
	def_c(Widget);
	bool border = false;
	string text = "";

	void OnBorn() override {
		e.newProp<Hitbox>(f2{ 0,0 }, e.tran.siz);
	}

	void OnRender() override {
		if (border) S._RenderDrawRect(e.tran.pos, e.tran.siz);
		S.Render(e.tran.pos, e.tran.siz);
		S.RenderText(Font("Consolas", 25), text, e.tran.pos, e.tran.siz);
	}
};
