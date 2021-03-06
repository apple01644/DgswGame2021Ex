constexpr struct {
	int w = 64;
	int h = 64;
} map_size;

constexpr f2 tile_size(Window.w / (float)map_size.w, Window.h / (float)map_size.h);
struct Tile {
	Tile() {}
	Tile(int x, int y) : box(Box(f2(x* tile_size.x, y* tile_size.y), tile_size)) {}
	bool painted = false;
	bool healed = false;
	bool blocked = false;

	char flag = 'N';
	Box box;
};
static Tile map[map_size.w][map_size.h];