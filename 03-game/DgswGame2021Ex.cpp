#include "DXUT.h"

Room* curr_room = nullptr;
Room* next_room = nullptr;
float T = 0;
float DT = 0;
Sys* sys = nullptr;
size_t RI = 0;

CSoundManager SND{};
unordered_map<std::wstring, CSound*> SoundMap;

HRESULT CALLBACK OnD3D10CreateDevice(ID3D10Device* D, const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
	void* pUserContext)
{
	sys = new Sys;
	sys->D = D;
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
	HRESULT	hr = D3DX10CreateEffectFromFile(L"DgswGame2021Ex.fx", NULL, NULL, "fx_4_0", dwShaderFlags, 0, D, NULL,
		NULL, S.effect.GetAddressOf(), NULL, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL,
			L"The FX file cannot be located.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
		V_RETURN(hr);
	}
	sys->EventInit();

	return S_OK;
}

HRESULT CALLBACK OnD3D10ResizedSwapChain(ID3D10Device* D, IDXGISwapChain* pSwapChain, const DXGI_SURFACE_DESC* Surf, void* pUserContext)
{
	S.P = XMMatrixScaling(2.f / Surf->Width, -2.f / Surf->Height, 0) * XMMatrixTranslation(-1, 1, 0);
	S.ProjVar->SetMatrix(S.P.r->m128_f32);
	return S_OK;
}

void CALLBACK OnD3D10FrameRender(ID3D10Device* D, double fTime, float fElapsedTime, void* pUserContext)
{
	T = (float)fTime;
	DT = fElapsedTime;
	S.EventRender();
}

void CALLBACK OnD3D10ReleasingSwapChain(void* pUserContext) {}

void CALLBACK OnD3D10DestroyDevice(void* pUserContext) { delete sys; }

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	T = (float)fTime;
	DT = fElapsedTime;
	S.EventStep();
}

LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{
	S.EventMsg(uMsg, wParam, lParam);
	return 0;
}

void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
	if (bKeyDown)
	{
		switch (nChar)
		{
		case VK_F1: break;
		}
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	DXUTSetCallbackD3D10DeviceCreated(OnD3D10CreateDevice);
	DXUTSetCallbackD3D10SwapChainResized(OnD3D10ResizedSwapChain);
	DXUTSetCallbackD3D10SwapChainReleasing(OnD3D10ReleasingSwapChain);
	DXUTSetCallbackD3D10DeviceDestroyed(OnD3D10DestroyDevice);
	DXUTSetCallbackD3D10FrameRender(OnD3D10FrameRender);

	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackKeyboard(OnKeyboard);
	DXUTSetCallbackFrameMove(OnFrameMove);

	DXUTInit(true, true, NULL);     DXUTSetCursorSettings(true, true);     DXUTCreateWindow(L"DgswGame2021Ex");
	E, SND.Initialize(DXUTGetHWND(), DSSCL_PRIORITY);
	DXUTCreateDevice(true, Window.w, Window.h);
	DXUTMainLoop();
	for (const auto& pair : SoundMap) delete pair.second;
	SND.~CSoundManager();
	return DXUTGetExitCode();
}

void Sys::EventInit() {
	L = "[Sys Start]";

	{
		Technique = S.effect->GetTechniqueByName("Render");
		WorldVar = S.effect->GetVariableByName("World")->AsMatrix();
		ViewVar = S.effect->GetVariableByName("View")->AsMatrix();
		ProjVar = S.effect->GetVariableByName("Projection")->AsMatrix();
		ColorVar = S.effect->GetVariableByName("vMeshColor")->AsVector();
		TextureVar = S.effect->GetVariableByName("txDiffuse")->AsShaderResource();
		V = XMMatrixIdentity();
		ViewVar->SetMatrix(V.r->m128_f32);
		TextureVar->SetResource("Img/solid.dds"_tex);
	}
	{
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT numElements = sizeof(layout) / sizeof(layout[0]);

		D3D10_PASS_DESC PassDesc;
		Technique->GetPassByIndex(0)->GetDesc(&PassDesc);
		eD->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize, S.VL.GetAddressOf());

		D->IASetInputLayout(S.VL.Get());
	}

	D3D10_BUFFER_DESC bd = {};
	D3D10_SUBRESOURCE_DATA InitData{};
	{
		vector<SimpleVertex> vertices =
		{
			{ D3DXVECTOR3(0, 0, 0), D3DXVECTOR2(0, 0) },
			{ D3DXVECTOR3(0, 1, 0), D3DXVECTOR2(0, 1) },
			{ D3DXVECTOR3(1, 0, 0), D3DXVECTOR2(1, 0) },
			{ D3DXVECTOR3(1, 1, 0), D3DXVECTOR2(1, 1) },
		};
		bd.ByteWidth = sizeof(SimpleVertex) * vertices.size();
		bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;

		InitData.pSysMem = vertices.data();
		eD->CreateBuffer(&bd, &InitData, S.VB.GetAddressOf());
	}
	{
		vector<DWORD> indices = { 0,1,2, 2,1,3 };
		bd = {};
		bd.ByteWidth = sizeof(DWORD) * indices.size();
		bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		InitData.pSysMem = indices.data();
		eD->CreateBuffer(&bd, &InitData, S.IB_FILL_RECT.GetAddressOf());
	}
	{
		vector<DWORD> indices = { 0,1,3,2,0 };
		bd = {};
		bd.ByteWidth = sizeof(DWORD) * indices.size();
		bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		InitData.pSysMem = indices.data();
		eD->CreateBuffer(&bd, &InitData, S.IB_DRAW_RECT.GetAddressOf());
	}
	{
		D3D10_RASTERIZER_DESC rdesc = {};
		rdesc.FillMode = D3D10_FILL_SOLID;
		rdesc.CullMode = D3D10_CULL_NONE;
		eD->CreateRasterizerState(&rdesc, S.RS.GetAddressOf());
	}
	{
		D3D10_DEPTH_STENCIL_DESC dsdesc = {};
		eD->CreateDepthStencilState(&dsdesc, S.DSS.GetAddressOf());
	}
	{
		D3D10_BLEND_DESC bdesc = {};
		bdesc.BlendEnable[0] = TRUE;
		bdesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
		bdesc.SrcBlend = D3D10_BLEND::D3D10_BLEND_SRC_ALPHA;
		bdesc.DestBlend = D3D10_BLEND::D3D10_BLEND_INV_SRC_ALPHA;
		bdesc.BlendOp = D3D10_BLEND_OP::D3D10_BLEND_OP_ADD;
		bdesc.SrcBlendAlpha = D3D10_BLEND::D3D10_BLEND_ZERO;
		bdesc.DestBlendAlpha = D3D10_BLEND::D3D10_BLEND_ZERO;
		bdesc.BlendOpAlpha = D3D10_BLEND_OP::D3D10_BLEND_OP_ADD;
		eD->CreateBlendState(&bdesc, S.BS.GetAddressOf());
	}
}

void Sys::EventStep() {
	if (curr_room != nullptr) {
		R.OnStep();
	}
}

void Sys::EventRender() {
	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
	D->ClearRenderTargetView(pRTV, ClearColor);

	ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
	D->ClearDepthStencilView(pDSV, D3D10_CLEAR_DEPTH, 1.0, 0);
	RI = 1;

	D->RSSetState(S.RS.Get());
	D->OMSetBlendState(S.BS.Get(), nullptr, 1);
	D->OMSetDepthStencilState(S.DSS.Get(), NULL);

	SetWindowTextA(DXUTGetHWND(), ("DgswGame2021Ex fps:" + std::to_string((int)floorf(1 / DT))).c_str());;

	if (curr_room != nullptr) R.OnRender();

	if (next_room != nullptr) {
		if (curr_room != nullptr)
		{
			L = "Leave", R.GetName();
			R.OnLeave();
			delete curr_room;
		}
		curr_room = next_room;
		next_room = nullptr;
		L = "Enter", R.GetName();
		R.OnEnter();
	}
}