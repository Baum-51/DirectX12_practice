#include <Windows.h>
#include <vector>
#ifdef _DEBUG
#include <iostream>
#endif
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;

//@brief コンソール画面にフォーマット付き文字列を表示
//@param formatフォーマット(%dとか%fとかの)
//@param 可変長引数
//@remarks この関数はデバッグ用です。デバッグ時にしか動作しません

ID3D12Device* _dev = nullptr;
IDXGIFactory6* _dxgiFactory = nullptr;
IDXGISwapChain4* _swapchain = nullptr;

auto result = CreateDXGIFactory(IID_PPV_ARGS(&_dxgiFactory));

HRESULT D3D12CreateDevice(
	IUnknown* pAdapter,//ひとまずはnullptrでOK
	D3D_FEATURE_LEVEL MinimumFeatureaLevel,//最低限必要なフィーチャーレベル
	REFIID riid,
	void** ppDevice
);

D3D_FEATURE_LEVEL levels[] =
{
	D3D_FEATURE_LEVEL_12_1,
	D3D_FEATURE_LEVEL_12_0,
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
};

//Deirect3Dデバイスの初期化
D3D_FEATURE_LEVEL featureLevel;



void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf(format, valist);
	va_end(valist);
#endif
}

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//ウインドウが破棄されるときに呼び出される
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);//OSにアプリケーションが終了をすることを伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);//既定の処理を行う
}

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif

	for (auto lv : levels) {
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			break;//生成可能なバージョンを見つけたらループを打ち切り
		}
	}

	//アダプターの列挙用
	std::vector <IDXGIAdapter*> adapters;
	//ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0;
		_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		adapters.push_back(tmpAdapter);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);//アダプターの説明オブジェクト取得

		std::wstring strDesc = adesc.Description;

		//探したいアダプターの名前を確認
		if (strDesc.find(L"NVIDIA") != string::npos)
		{
			tmpAdapter = adpt;
			break;
		}

	}


	WNDCLASSEX winc = {};

	winc.cbSize = sizeof(WNDCLASSEX);
	winc.lpfnWndProc = (WNDPROC)WindowProcedure;//コールバック関数の指定
	winc.lpszClassName = TEXT("DX12Sample");//アプリケーションクラス名
	winc.hInstance = GetModuleHandle(nullptr);//ハンドルの取得



	RegisterClassEx(&winc);//アプリケーションクラス（ウィンドウクラスの指定をOSに伝える）

	RECT wrc = { 0, 0, 500, 500 };//ウインドウサイズを決める

	//関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(winc.lpszClassName,//クラス名指定
		TEXT("DX12テスト"),//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,//タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,//表示X座標はOSにお任せ
		CW_USEDEFAULT,//表示Y座標はOSにお任せ
		wrc.right - wrc.left,//ウィンドウ幅
		wrc.bottom - wrc.top,//ウィンドウ高
		nullptr,//親ウィンドウハンドル
		nullptr,//メニューハンドル
		winc.hInstance,//呼び出しアプリケーションハンドル
		nullptr);//追加パラメータ

	//ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {};

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//アプリケーションが終わるときにmessageがWM_QUITになる
		if (msg.message == WM_QUIT)
		{
			break;
		}
	}

	DebugOutputFormatString("Breaka Window.");
	getchar();
	return 0;
}
