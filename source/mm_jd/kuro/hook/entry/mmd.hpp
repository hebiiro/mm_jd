#pragma once

namespace apn::dark::kuro::hook
{
	//
	// このクラスはmmd関係のフックを担当します。
	//
	inline struct mmd_t : Entry
	{
		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			auto mmd = ::GetModuleHandleW(nullptr);
			MY_TRACE_HEX(mmd);

			my::hook::attach_import(TextOutA, mmd, "TextOutA");
			my::hook::attach_import(GradientFill, mmd, "GradientFill");
			my::hook::attach_import(BitBlt, mmd, "BitBlt");

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
		// 初期化後処理を実行します。
		//
		BOOL on_post_init()
		{
			MY_TRACE_FUNC("");

			if (hive.etc.flag_arrange)
			{
				auto mmd = ::GetModuleHandleW(nullptr);
				MY_TRACE_HEX(mmd);

				my::hook::attach_import(TextOutA, mmd, "TextOutA");
				my::hook::attach_import(MoveWindow, mmd, "MoveWindow");
				my::hook::attach_import(CreateWindowExA, mmd, "CreateWindowExA");
			}

			return TRUE;
		}

		//
		// このクラスは::TextOutA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, LPCSTR text, int c)
			{
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
		// このクラスは::GradientFill()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(
				HDC dc, PTRIVERTEX vertex, ULONG vertex_count,
				PVOID mesh, ULONG mesh_count, ULONG mode)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/hex} : {/}",
					ret_addr(&dc), dc, vertex_count, mesh_count, mode, ::GetCurrentThreadId());

				if (mesh_count == 1 && vertex_count == 2 && mode == GRADIENT_FILL_RECT_V)
				{
					auto rc = RECT { vertex[0].x, vertex[0].y, vertex[1].x, vertex[1].y };
					auto w = my::get_width(rc);
					auto h = my::get_height(rc);

					if (h == 24 || h == 32)
					{
						// ボタンのパレットを使用します。
						const auto& palette = paint::button_material.palette;

						auto part_id = BP_PUSHBUTTON;
						auto state_id = PBS_NORMAL;

						if (auto pigment = palette.get(part_id, state_id))
						{
							COLORREF color[2] = {
								pigment->border.get_win32_color(),
								pigment->background.get_win32_color(),
							};

							for (size_t i = 0; i < std::size(color); i++)
							{
								vertex[i].Red = GetRValue(color[i]) << 8;
								vertex[i].Green = GetGValue(color[i]) << 8;
								vertex[i].Blue = GetBValue(color[i]) << 8;
							}
						}
					}
				}

				return orig_proc(dc, vertex, vertex_count, mesh, mesh_count, mode);
			}
			inline static decltype(&hook_proc) orig_proc = ::GradientFill;
		} GradientFill;

		//
		// このクラスは::BitBlt()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(
				HDC dc, int x, int y, int cx, int cy,
				HDC src_dc, int src_x, int src_y, DWORD rop)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}, {/}, {/} : {/}",
					ret_addr(&dc), dc, x, y, cx, cy, src_dc, src_x, src_y, ::GetCurrentThreadId());

				if (cx == 49 && cy == 24)
				{
					// 描画領域を取得します。
					auto rc = RECT { x, y, x + cx, y + cy - 3 };

					// アイコン描画用のピグメントを作成します。
					auto pigment = paint::Pigment {
						{},
						{},
						{ RGB(0, 255, 0) },
					};

					// アイコンの属性をセットします。
					paint::IconAttribute icon_attribute(dc, &rc, paint::c_pseudo, 0);

					// 虫眼鏡アイコンを描画します。
					{
						auto s = std::wstring(L"🔍");
						auto text_flags = DT_LEFT | DT_VCENTER | DT_SINGLELINE;

						paint::d2d::Texter(dc, s.c_str(), (int)s.length(), &rc, text_flags, &pigment).draw_text();
					}

					// 十字アイコンを描画します。
					{
						auto s = std::wstring(L"✚");
						auto text_flags = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;

						paint::d2d::Texter(dc, s.c_str(), (int)s.length(), &rc, text_flags, &pigment).draw_text();
					}

					return TRUE;
				}

				return orig_proc(dc, x, y, cx, cy, src_dc, src_x, src_y, rop);
			}
			inline static decltype(&hook_proc) orig_proc = ::BitBlt;
		} BitBlt;

		//
		// このクラスは::MoveWindow()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd, int x, int y, int w, int h, BOOL redraw)
			{
#ifdef _DEBUG
				auto class_name = my::get_class_name(hwnd);

				MY_TRACE_FUNC("{/}, {/}, {/}, {/}, {/}, {/}", class_name, x, y, w, h, redraw);
#endif
				{
					auto control_id = ::GetWindowLongPtr(hwnd, GWLP_ID);
					switch (control_id)
					{
					case 0x000001A1: // フレーム操作
						break;
					case 0x000001A2: // フレーム操作 => ＜
					case 0x000001A3: // フレーム操作 => ＞
						y -= 1; h += 0; break;
					case 0x00000215: // フレーム操作 => .<
					case 0x00000214: // フレーム操作 => >.
					case 0x0000022E: // フレーム操作 => |<
					case 0x0000022F: // フレーム操作 => >|
						y -= 3; h += 2; break;
					case 0x000001CD: // 照明操作 => 赤
					case 0x000001CE: // 照明操作 => 緑
					case 0x000001CF: // 照明操作 => 青
						x -= 4; w += 4;
						[[fallthrough]]; // このまま処理を続けます。
					case 0x000001A9: // 左側ペイン => 開始範囲
					case 0x000001AA: // 左側ペイン => 終了範囲
					case 0x000001C0: // カメラ操作 => 視野角
					case 0x000001D0: // 照明操作 => X
					case 0x000001D1: // 照明操作 => Y
					case 0x000001D2: // 照明操作 => Z
					case 0x00000231: // セルフ影操作 => 影範囲
					case 0x000001DE: // アクセサリ操作 => X
					case 0x000001DF: // アクセサリ操作 => Y
					case 0x000001E0: // アクセサリ操作 => Z
					case 0x000001E1: // アクセサリ操作 => Rx
					case 0x000001E2: // アクセサリ操作 => Ry
					case 0x000001E3: // アクセサリ操作 => Rz
					case 0x000001E4: // アクセサリ操作 => Si
					case 0x000001E5: // アクセサリ操作 => Tr
					case 0x00000199: // 再生 => 開始範囲
					case 0x0000019A: // 再生 => 終了範囲
						y -= 1; h += 2; break;
					case 0x0000022A: // カメラ・証明・アクセサリ(セットの右)
						y -= 3; h += 6; break;
					case 0x00000220: // カメラ中心 => X
					case 0x00000221: // カメラ中心 => Y
					case 0x00000222: // カメラ中心 => Z
					case 0x00000223: // 角度 => X
					case 0x00000224: // 角度 => Y
					case 0x00000225: // 角度 => Z
					case 0x00000226: // 距離
						y -= 2; h += 3; break;
					}
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
	} mmd;
}
