struct Sprite : Prop {
	Sprite(ent, Texture tex) : Prop(e), tex(tex) {}
	Texture tex = nullptr;
	XMFLOAT4 color = { 1,1,1,1 };
	float depth = 0.5f;
	void OnRender() override {
		if (tex != nullptr) S.Render(tex, e.tran.pos, e.tran.siz, color);
	}
};
