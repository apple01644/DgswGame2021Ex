#define func_ [&](Entity& e)
#define func_obj [&](Entity& obj)
#define func_msg [&](Entity& e, UINT Msg, WPARAM wParam, LPARAM lParam)
#define ent Entity& e
#define def_c(x)  x(Entity& e) : Prop(e) {}
#define R (*((Room*)curr_room))

#define tran prop<Tran>()
#define hitb prop<Hitbox>()
#define obj_ std::function<void(Entity&)>

#define eD E, D
#define S (*sys)
#define None R.entities.end()