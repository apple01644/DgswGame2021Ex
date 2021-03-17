#pragma once
#include <wrl/client.h>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <DirectXMath.h>
using namespace DirectX;
using std::string;
using namespace std::string_literals;
using std::vector;
using std::list;
using std::unordered_map;
using Microsoft::WRL::ComPtr;

extern float T;
extern float DT;
extern CSoundManager SND;
extern unordered_map<std::wstring, CSound*> SoundMap;
extern size_t RI;

struct RStart;
struct RStage1;
struct RStage2;
struct RIntro;
struct RGameRule;
struct RGameInfo;
struct RCredit;
struct RRanking;

template <class T>
T* newRoom();
#define asr(classRoom) template<> classRoom* newRoom () { classRoom* ptr = new classRoom; next_room = ptr; return ptr; }

constexpr static struct {
	int w = 640;
	int h = 640;
}Window;

#include "Macros.h"

struct SimpleVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};

static struct Logger {
	string buffer{};
	const char* toCharArray(const char* s) {
		return (buffer = s).c_str();
	}
	const char* toCharArray(const string s) {
		return (buffer = s).c_str();
	}
	const char* toCharArray(const bool s) {
		return s ? "TRUE" : "FALSE";
	}
	template <class T>
	const char* toCharArray(const T s) {
		return (buffer = std::to_string(s)).c_str();
	}

	template <class T>
	Logger& operator= (const T s) {
		OutputDebugStringA("\n>>> ");
		OutputDebugStringA(toCharArray(s));
		return *this;
	}
	template <class T>
	Logger& operator, (const T s) {
		OutputDebugStringA(", ");
		OutputDebugStringA(toCharArray(s));
		return *this;
	}
} L;
static struct ErrorChecker {
	void operator, (HRESULT hr) {
		if (FAILED(hr)) {
			L = "Errror", DXGetErrorStringA(hr);
		}
	}
} E;

struct Entity;
struct Prop {
	virtual void OnBorn() {};
	virtual void OnStep() {};
	virtual void OnRender() {};
	virtual void OnClick() {};
	virtual void OnDie() {};
	virtual void OnMsg(UINT Msg, WPARAM wParam, LPARAM lParam) {};
	Entity& e;
protected:
	Prop(Entity& e) : e(e) {}
};

struct Sys;
struct Room;
struct Entity {
	bool live = true;
	string name = "<unnamed>";
	std::function<void(Entity&)> OnBorn = func_{};
	std::function<void(Entity&)> OnStep = func_{};
	std::function<void(Entity&)> OnRender = func_{};
	std::function<void(Entity&)> OnClick = func_{};
	std::function<void(Entity&)> OnDie = func_{};
	std::function<void(Entity&, UINT, WPARAM, LPARAM)> OnMsg = func_msg{};

	inline Entity(std::function<void(Entity&)> func) {
		func(*this);
	}
	Entity(Entity&) = delete;
	Entity(Entity&&) = delete;
	virtual ~Entity() {
		L = "On Dest", prop_events["Dest"].size();
		for (auto prop_event : prop_events["Dest"]) prop_event();
	}
	template <class T, class... Args>
	T& newProp(Args ... args) {
		auto prop_name = typeid(T).name();
		T* pointer = new T(*this, args...);
		assert(raw_props.find(prop_name) == raw_props.end());

		raw_props[prop_name] = pointer;
		prop_events["OnBorn"].push_back([&]() { prop<T>().OnBorn(); });
		prop_events["OnStep"].push_back([&]() { prop<T>().OnStep(); });
		prop_events["OnRender"].push_back([&]() { prop<T>().OnRender(); });
		prop_events["OnClick"].push_back([&]() { prop<T>().OnClick(); });
		prop_events["OnDie"].push_back([&]() { prop<T>().OnDie(); });
		prop_msg_events.push_back([&](UINT Msg, WPARAM wParam, LPARAM lParam) { prop<T>().OnMsg(Msg, wParam, lParam); });
		prop_events["Dest"].push_back([&]() {
			L = "destroy", typeid(T).name();
			delete (T*)raw_props[typeid(T).name()];
			});
		return *pointer;
	}

	template <class T>
	T& prop() {
		auto prop_name = typeid(T).name();
		assert(raw_props.find(prop_name) != raw_props.end());
		return *(T*)raw_props[prop_name];
	}

	template <class T>
	bool hasProp() {
		return raw_props.find(typeid(T).name()) != raw_props.end();
	}

	void die() {
		this->live = false;
	}
	void EventClick() {
		OnClick(*this);
		for (auto prop_event : prop_events["OnClick"]) prop_event();
	}

private:
	friend Room;
	void EventBorn() {
		OnBorn(*this);
		for (auto prop_event : prop_events["OnBorn"]) prop_event();
	}
	void EventStep() {
		OnStep(*this);
		for (auto prop_event : prop_events["OnStep"]) prop_event();
	}
	void EventRender() {
		OnRender(*this);
		for (auto prop_event : prop_events["OnRender"]) prop_event();
	}
	void EventDie() {
		OnDie(*this);
		for (auto prop_event : prop_events["OnDie"]) prop_event();
	}
	void EventMsg(UINT Msg, WPARAM wParam, LPARAM lParam) {
		OnMsg(*this, Msg, wParam, lParam);
		for (auto prop_event : prop_msg_events) prop_event(Msg, wParam, lParam);
	}
private:
	unordered_map<string, void*> raw_props{};
	unordered_map<string, list<std::function<void()>>> prop_events{};
	list<std::function<void(UINT Msg, WPARAM wParam, LPARAM lParam)>> prop_msg_events{};
};

struct Sys;
struct Room {
	virtual void OnEnter() = 0;
	virtual void OnLeave() = 0;
	virtual void OnStep() {
		std::list<Entity*> old_entities{};
		old_entities.swap(entities);
		std::for_each(old_entities.begin(), old_entities.end(), [&](Entity* e) {
			if (e->live) entities.push_back(e);
			else {
				e->EventDie();
				delete e;
			}});

		std::for_each(future_entities.begin(), future_entities.end(), [&](Entity* e) {  entities.push_back(e); e->EventBorn(); });
		future_entities.clear();

		std::for_each(entities.begin(), entities.end(), [&](Entity* e) {e->EventStep(); });
	}
	virtual void OnRender() {
		std::for_each(entities.begin(), entities.end(), [&](Entity* e) {e->EventRender(); });
	};
	virtual void OnMsg(UINT Msg, WPARAM wParam, LPARAM lParam) {
		CheatCallback(Msg, wParam, lParam);
		std::for_each(entities.begin(), entities.end(), [&](Entity* e) {e->EventMsg(Msg, wParam, lParam); });
	}
	virtual const char* GetName() { return "[Room empty]"; }
	virtual ~Room() {
		for (auto& entity : entities) {
			entity->EventDie();
			delete entity;
		}
		for (auto& entity : future_entities) {
			entity->EventDie();
			delete entity;
		}
	}

	inline Entity& CreateEntity(std::function<void(Entity&)> func) {
		auto* entity = new Entity(func);
		future_entities.push_back(entity);
		return *entity;
	}

	list<Entity*> entities{};
	list<Entity*> future_entities{};
	void CheatCallback(UINT Msg, WPARAM wParam, LPARAM lParam);
};
extern Room* curr_room;
extern Room* next_room;
typedef XMFLOAT2 f2;
typedef XMFLOAT3 f3;
typedef XMFLOAT4 f4;
typedef ID3D10ShaderResourceView* Texture;
struct Box {
	Box() {}
	Box(f2 pos, f2 siz) : pos(pos), siz(siz) {}
	f2 pos{};
	f2 siz{};
};
inline bool operator& (const Box a, const Box b) {
	RECT r1 = { LONG(a.pos.x), LONG(a.pos.y), LONG(a.pos.x + a.siz.x), LONG(a.pos.y + a.siz.y) };
	RECT r2 = { LONG(b.pos.x), LONG(b.pos.y), LONG(b.pos.x + b.siz.x), LONG(b.pos.y + b.siz.y) };
	RECT r3;
	return IntersectRect(&r3, &r1, &r2) != FALSE;
}

inline Texture operator"" _tex(const char* raw_path, size_t n);
struct Sys {
	ID3D10Device* D = nullptr;
	ComPtr<ID3D10Effect> effect = nullptr;
	ComPtr<ID3D10InputLayout> VL = nullptr;
	ComPtr<ID3D10Buffer> VB = nullptr;
	ComPtr<ID3D10Buffer> IB_FILL_RECT = nullptr;
	ComPtr<ID3D10Buffer> IB_DRAW_RECT = nullptr;

	unordered_map<string, ComPtr<ID3D10ShaderResourceView>> SRV{};
	unordered_map<string, unordered_map<int, ComPtr<ID3DX10Font>>> FontMap{};

	ComPtr<ID3D10RasterizerState> RS = nullptr;
	ComPtr<ID3D10BlendState> BS = nullptr;
	ComPtr<ID3D10DepthStencilState> DSS = nullptr;

	ID3D10EffectTechnique* Technique = NULL;
	ID3D10EffectMatrixVariable* WorldVar = NULL;
	ID3D10EffectMatrixVariable* ViewVar = NULL;
	ID3D10EffectMatrixVariable* ProjVar = NULL;
	ID3D10EffectVectorVariable* ColorVar = NULL;
	ID3D10EffectShaderResourceVariable* TextureVar = NULL;

	XMMATRIX W = XMMatrixIdentity();
	XMMATRIX V = XMMatrixIdentity();
	XMMATRIX P = XMMatrixIdentity();
	float depth = 0;
	float SURF_W = 1920;
	float SURF_H = 1080;
	DXGI_SURFACE_DESC SRD;

	struct {
		bool superman = false;
		bool write_score = false;
		int score = 0;
		int hp_score = 0;
	} global_variables;

	void SetStartRoom();

	void EventInit();
	void EventStep();
	void EventRender();
	inline void EventMsg(UINT Msg, WPARAM wParam, LPARAM lParam) {
		if (curr_room != nullptr)
			R.OnMsg(Msg, wParam, lParam);
	};
	inline Sys() {
		SetStartRoom();
	}
	inline ~Sys() {
		L = "[Sys End]";
		if (curr_room != nullptr)
		{
			R.OnLeave();
			delete curr_room;
		}
	}

	inline void _RenderFillRect(f2 pos, f2 siz) {
		D->VSSetShader(nullptr);
		D->PSSetShader(nullptr);
		D->GSSetShader(nullptr);
		D->IASetInputLayout(VL.Get());

		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		D->IASetVertexBuffers(0, 1, VB.GetAddressOf(), &stride, &offset);
		D->IASetIndexBuffer(IB_FILL_RECT.Get(), DXGI_FORMAT_R32_UINT, 0);
		D->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		W = XMMatrixScaling(siz.x, siz.y, 1) * XMMatrixTranslation(pos.x, pos.y, depth);
		WorldVar->SetMatrix(W.r->m128_f32);
		Technique->GetPassByName("main")->Apply(0);
		D->DrawIndexed(6, 0, 0);
	}
	inline void _RenderDrawRect(f2 pos, f2 siz) {
		D->VSSetShader(nullptr);
		D->PSSetShader(nullptr);
		D->GSSetShader(nullptr);
		D->IASetInputLayout(VL.Get());

		UINT stride = sizeof(SimpleVertex);
		UINT offset = 0;
		D->IASetVertexBuffers(0, 1, VB.GetAddressOf(), &stride, &offset);
		D->IASetIndexBuffer(IB_DRAW_RECT.Get(), DXGI_FORMAT_R32_UINT, 0);
		D->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);

		W = XMMatrixScaling(siz.x, siz.y, 1) * XMMatrixTranslation(pos.x, pos.y, depth);
		WorldVar->SetMatrix(W.r->m128_f32);
		Technique->GetPassByName("main")->Apply(0);
		D->DrawIndexed(5, 0, 0);
	}

	inline void Render(f2 pos, f2 siz, f4 color = { 1,1,1,1 }) {
		ColorVar->SetFloatVector(&color.x);
		TextureVar->SetResource("Img/solid.dds"_tex);
		_RenderFillRect(pos, siz);
	}
	inline void Render(Texture tex, f2 pos, f2 siz, f4 color = { 1,1,1,1 }) {
		ColorVar->SetFloatVector(&color.x);
		TextureVar->SetResource(tex);
		_RenderFillRect(pos, siz);
	}
	inline void RenderLine(f2 pos, f2 siz, f4 color = { 1,1,1,1 }) {
		ColorVar->SetFloatVector(&color.x);
		TextureVar->SetResource("Img/solid.dds"_tex);
		_RenderDrawRect(pos, siz);
	}

	inline void RenderText(ID3DX10Font* font, std::string text, f2 pos, f4 color = { 0,0,0,1 }) {
		RECT rect = {
			((pos.x * 1080.f / 1920 + (1920 - 1080) / 1080.f * 180) * SRD.Width) / 640 , pos.y * SRD.Height / 640 ,
			0, 0
		};
		font->DrawTextA(nullptr, text.c_str(), -1, &rect, DT_NOCLIP, D3DXCOLOR((float*)&color));
	}
	inline void RenderText(ID3DX10Font* font, std::string text, f2 pos, f2 siz, f4 color = { 0,0,0,1 }) {
		RECT rect = {
			((pos.x * 1080.f / 1920 + (1920 - 1080) / 1080.f * 180) * SRD.Width) / 640 , pos.y * SRD.Height / 640 ,
			(((pos.x + siz.x) * 1080.f / 1920 + (1920 - 1080) / 1080.f * 180) * SRD.Width) / 640,  (pos.y + siz.y) * SRD.Height / 640
		};
		font->DrawTextA(nullptr, text.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_NOCLIP, D3DXCOLOR((float*)&color));
	}
};
extern Sys* sys;
inline Texture operator"" _tex(const char* raw_path, size_t n) {
	string path{ raw_path };
	if (S.SRV[path].Get() == nullptr)
		E, D3DX10CreateShaderResourceViewFromFileA(S.D, path.c_str(), NULL, NULL, S.SRV[path].GetAddressOf(), NULL);
	return S.SRV[path].Get();
}
inline CSound& operator"" _snd(const wchar_t* raw_path, size_t n) {
	std::wstring path{ raw_path };
	if (SoundMap[path] == nullptr)
		E, SND.Create(&SoundMap[path], path.data(), DSBCAPS_GLOBALFOCUS);

	return *SoundMap[path];
}

inline Entity* operator"" _ent(const char* raw_name, size_t n) {
	string target_name{ raw_name };
	auto& itr = std::find_if(R.entities.begin(), R.entities.end(), [=](Entity* e) {return e->name == target_name; });
	if (itr == None)
		return nullptr;
	else
		return *itr;
}

inline size_t  operator"" _ent_count(const char* raw_name, size_t n) {
	string target_name{ raw_name };
	size_t result = 0;
	for (const auto& e : R.entities) {
		if (e->name == target_name)
			result += 1;
	}
	return result;
}

inline ID3DX10Font* Font(string font_name, int size) {
	auto& Fonts = S.FontMap[font_name];
	if (Fonts.find(size) == Fonts.end())
		E, D3DX10CreateFontA(S.D, size, 0, 0, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, font_name.c_str(), Fonts[size].GetAddressOf());
	return Fonts[size].Get();
}