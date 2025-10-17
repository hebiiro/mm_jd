#pragma once

namespace apn::dark::kuro::hook
{
	//
	// このクラスはMMD関係のフックを担当します。
	//
	inline struct mmd_t : Entry
	{
		inline static constexpr auto c_inflate = 1;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			auto mmd = ::GetModuleHandleW(nullptr);
			MY_TRACE_HEX(mmd);
/*
			auto font_name = (LPSTR)((size_t)mmd + 0x12B4F8);
			constexpr auto buffer_size = size_t { 32 };
			char new_font_name[buffer_size] = "HackGen35";
			auto old_protect = DWORD {};
			auto r0 = ::VirtualProtect(font_name, buffer_size, PAGE_READWRITE, &old_protect);
			auto write_size = SIZE_T {};
			auto r1 = ::WriteProcessMemory(::GetCurrentProcess(),
				font_name, new_font_name, buffer_size, &write_size);
			auto r2 = ::VirtualProtect(font_name, buffer_size, old_protect, &old_protect);
*/
//			my::hook::attach_import(CreateFontIndirectA, mmd, "CreateFontIndirectA");
			my::hook::attach_import(TextOutA, mmd, "TextOutA");
			my::hook::attach_import(MoveWindow, mmd, "MoveWindow");
			my::hook::attach_import(CreateWindowExA, mmd, "CreateWindowExA");
			my::hook::attach_import(CreateWindowExW, mmd, "CreateWindowExW");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは::CreateFontIndirectA()をフックします。
		//
		struct {
			inline static HFONT WINAPI hook_proc(const LOGFONTA* lf)
			{
				if (!lf) return orig_proc(lf);

				MY_TRACE_FUNC("{/}, {/}", lf->lfHeight, lf->lfFaceName);

				{
					// テスト用コードです。

					auto lf2 = *lf;
//					lf2.lfHeight = abs(lf2.lfHeight) + 4;
					strcpy_s(lf2.lfFaceName, "HackGen35");
					return orig_proc(&lf2);
				}

				return orig_proc(lf);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateFontIndirectA;
		} CreateFontIndirectA;

		//
		// このクラスは::TextOutA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, LPCSTR text, int c)
			{
				// ::ExtTextOut()のフックがロックされている場合は
				if (ext_text_out_lock.is_locked())
				{
//					MY_TRACE_FUNC("再帰呼び出し時はフックしません\n");

					return orig_proc(dc, x, y, text, c);
				}

				// ::ExtTextOut()のフックをロックします。
				Locker locker(&ext_text_out_lock);

				auto wcs = my::ws({ text, (size_t)c });

				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/} : {/}",
					ret_addr(&dc), dc, x, y, wcs, c, ::GetCurrentThreadId());

				{
					// ダイアログとして描画します。
					const auto& palette = paint::dialog_material.palette;

					auto part_id = WP_DIALOG;
					auto state_id = ETS_NORMAL;

					if (auto pigment = palette.get(part_id, state_id))
					{
						return paint::stylus.ext_text_out(dc, x, y, 0,
							nullptr, wcs.c_str(), (UINT)wcs.length(), nullptr, pigment);
					}
				}

				return orig_proc(dc, x, y, text, c);
			}
			inline static decltype(&hook_proc) orig_proc = ::TextOutA;
		} TextOutA;

		//
		// このクラスは::MoveWindow()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd, int x, int y, int w, int h, BOOL redraw)
			{
				auto class_name = my::get_class_name(hwnd);

				MY_TRACE_FUNC("{/}, {/}, {/}, {/}, {/}, {/}", class_name, x, y, w, h, redraw);

				if (::lstrcmpi(class_name.c_str(), WC_EDIT) == 0)
				{
					auto control_id = ::GetWindowLongPtr(hwnd, GWLP_ID);
#if 0
					switch (control_id)
					{
						case
カメラ・証明・アクセサリ	0000022A
フレーム操作				000001A1
左側ペインの何かの開始範囲	000001A9
左側ペインの何かの終了範囲	000001AA
カメラ中心のX				00000220
カメラ中心のY				00000221
カメラ中心のZ				00000222
角度のX						00000223
角度のY						00000224
角度のZ						00000225
距離						00000226

カメラ操作の視野角			000001C0

照明操作の赤				000001CD
照明操作の緑				000001CE
照明操作の青				000001CF
照明操作のX					000001D0
照明操作のY					000001D1
照明操作のZ					000001D2

セルフ影操作の影範囲		00000231
アクセサリ操作のX			000001DE
アクセサリ操作のY			000001DF
アクセサリ操作のZ			000001E0
アクセサリ操作のRx			000001E1
アクセサリ操作のRy			000001E2
アクセサリ操作のRz			000001E3
アクセサリ操作のSi			000001E4
アクセサリ操作のTr			000001E5
再生の開始範囲				00000199
再生の終了範囲				0000019A

					if (h == 14)
					{
//						y -= c_inflate;
//						h += c_inflate * 2;
					}
#endif
				}

				return orig_proc(hwnd, x, y, w, h, redraw);
			}
			inline static decltype(&hook_proc) orig_proc = ::MoveWindow;
		} MoveWindow;

		//
		// このクラスは::CreateWindowExA()をフックします。
		//
		struct {
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCSTR class_name, LPCSTR window_name, DWORD style,
				int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				auto safe_class_name = safe_string(class_name);
				auto safe_window_name = safe_string(window_name);

				MY_TRACE_FUNC("{/}, {/}", safe_class_name, safe_window_name);

				if (::lstrcmpiA(safe_class_name.c_str(), WC_EDITA) == 0)
				{
					ex_style &= ~WS_EX_STATICEDGE;
					style |= WS_BORDER;
				}

				return orig_proc(
					ex_style, class_name, window_name, style,
					x, y, w, h, parent, menu, instance, param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateWindowExA;
		} CreateWindowExA;

		//
		// このクラスは::CreateWindowExW()をフックします。
		//
		struct {
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCWSTR class_name, LPCWSTR window_name, DWORD style,
				int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				auto safe_class_name = safe_string(class_name);
				auto safe_window_name = safe_string(window_name);

				MY_TRACE_FUNC("{/}, {/}", safe_class_name, safe_window_name);

				if (::lstrcmpiW(safe_class_name.c_str(), WC_EDITW) == 0)
				{
					int n = 0;
				}

				return orig_proc(
					ex_style, class_name, window_name, style,
					x, y, w, h, parent, menu, instance, param);
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateWindowExW;
		} CreateWindowExW;
	} mmd;
}
