#pragma once

namespace apn::dark::kuro::gdi
{
	struct ButtonRenderer : RendererNc
	{
		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, dc, control, brush);

			// ボタンの背景の色を変更します。
			// ボタンがダイアログに配置されることを前提に処理しています。
			return get_dialog_brush(hwnd, message, dc, control, brush);
		}

		virtual LRESULT on_custom_draw(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, wParam, lParam);

			return CDRF_DODEFAULT;

			// テスト用コードです。

			auto cd = (NMTVCUSTOMDRAW*)lParam;

			MY_TRACE_HEX(cd->nmcd.dwDrawStage);

			auto lr = __super::on_custom_draw(hwnd, message, wParam, lParam);

			MY_TRACE_HEX(lr);

			return lr;
		}
	};
}
