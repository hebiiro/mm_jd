#pragma once

namespace apn::dark::kuro::paint::d2d
{
	//
	// このクラスはD2Dのコアです。
	//
	inline struct core_t
	{
		ComPtr<IDWriteFactory> dw_factory;
		ComPtr<ID2D1Factory1> d2d_factory;
		ComPtr<IWICImagingFactory> wic_factory;
		ComPtr<ID2D1DCRenderTarget> render_target;

		//
		// インターフェイスを再作成中の場合はTRUEになります。
		//
		BOOL flag_recreating = FALSE;

		//
		// 共通して使用されるピクセルフォーマットです。
		//
		inline static constexpr auto c_pixel_format = D2D1_PIXEL_FORMAT {
			DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED,
//			DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE,
		};

		//
		// 共通して使用されるレンダーターゲットプロパティです。
		//
		inline static constexpr auto c_render_target_props = D2D1_RENDER_TARGET_PROPERTIES {
			D2D1_RENDER_TARGET_TYPE_DEFAULT, c_pixel_format,
		};

		//
		// 各種ファクトリを作成します。
		//
		HRESULT create_factory()
		{
			dw_factory = nullptr;
			d2d_factory = nullptr;
			wic_factory = nullptr;

			// DWriteファクトリを作成します。
			auto hr = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
				__uuidof(IDWriteFactory), (IUnknown**)dw_factory.GetAddressOf());
			if (!dw_factory) return hr;

			// WICファクトリを作成します。
			hr = ::CoCreateInstance(CLSID_WICImagingFactory,
				nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wic_factory));
			if (!wic_factory) return hr;

			// D2Dファクトリを作成します。
			hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2d_factory.GetAddressOf());
			if (!d2d_factory) return hr;

			return hr;
		}

		//
		// レンダーターゲットを作成します。
		//
		HRESULT create_render_target()
		{
			if (!d2d_factory) return E_FAIL;

			// DCレンダーターゲットを作成します。
			auto props = D2D1::RenderTargetProperties(
				D2D1_RENDER_TARGET_TYPE_DEFAULT,
				c_pixel_format,
				0, 0,
				D2D1_RENDER_TARGET_USAGE_NONE,
				D2D1_FEATURE_LEVEL_DEFAULT);
			auto hr = d2d_factory->CreateDCRenderTarget(
				&props, render_target.ReleaseAndGetAddressOf());
			if (!render_target) return hr;

			return hr;
		}

		//
		// インターフェイスを再作成します。
		//
		BOOL recreate()
		{
			MY_TRACE_FUNC("");

			// ファクトリを作成します。
			if (FAILED(create_factory())) return FALSE;

			// レンダーターゲットを作成します。
			if (FAILED(create_render_target())) return FALSE;

			// フラグをリセットします。
			flag_recreating = FALSE;

			return TRUE;
		}

		//
		// 描画の準備をします。
		//
		BOOL prepare()
		{
			MY_TRACE_FUNC("");

			// インターフェイスを再作成中の場合は何もしません。
			if (flag_recreating) return FALSE;

			// ファクトリを作成します。
			if (!d2d_factory)
				if (FAILED(create_factory())) return FALSE;

			// レンダーターゲットを作成します。
			if (!render_target)
				if (FAILED(create_render_target())) return FALSE;

			return TRUE;
		}

		//
		// このクラスはレンダーターゲットとデバイスコンテキストをバインドします。
		// 内部的に使用されます。
		//
		struct binder_t
		{
			//
			// コンストラクタです。
			//
			binder_t(HDC dc, LPCRECT rc)
			{
				// デバイスコンテキストにバインドします。
				core.render_target->BindDC(dc, rc);

				// 描画を開始します。
				core.render_target->BeginDraw();
			}

			//
			// デストラクタです。
			//
			~binder_t()
			{
				// 描画を終了します。
				if (core.render_target->EndDraw() == D2DERR_RECREATE_TARGET)
				{
					// インターフェイスを再作成中ではない場合は
					if (!core.flag_recreating)
					{
						// フラグをセットします。
						core.flag_recreating = TRUE;

						// インターフェイスを再作成する必要があることを通知します。
						::PostMessage(hive.theme_window, hive.c_message.c_recreate, 0, 0);
					}
				}
			}
		};
	} core;
}
