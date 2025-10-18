﻿#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはメニューのテーマをフックしてカスタム描画を実行します。
	//
	inline struct MenuRenderer : base::MenuRenderer
	{
		//
		// ポップアップメニューのガーターの右側の座標です。
		//
		int gutter_right = {};

		//
		// ポップアップアイテムの矩形です。
		//
		RECT popup_item_rect = {};

		virtual HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			{
				// クリップ矩形を使用します。
				Clipper clipper(dc, rc, rc_clip);

				switch (part_id)
				{
				case MENU_BARBACKGROUND:
					{
						// メニューバーの背景を描画します。

						auto rc2 = *rc;
						rc2.bottom += 1; // クリップ矩形を使用すると、この1ピクセルが上書き描画できるようになります。

						if (paint::stylus.draw_rect(dc, &rc2, palette, part_id, state_id))
							return S_OK;

						break;
					}
				case MENU_BARITEM:
					{
						// メニューバー項目の背景を描画します。

						// 項目が選択状態のときだけ
						if (state_id == MPI_HOT || state_id == MBI_PUSHED)
						{
							// 背景を描画します。
							if (paint::stylus.draw_round_rect(dc, rc, palette, part_id, state_id))
								return S_OK;
						}
						// それ以外の場合は
						else
						{
							// 背景を描画しません。
							return S_OK;
						}

						break;
					}
#if 0 // テスト用コードです。
				case MENU_POPUPBACKGROUND:
					{
						// ポップアップメニューの背景を描画します。

						MY_TRACE("MENU_POPUPBACKGROUND\n");

						auto hwnd = ::WindowFromDC(dc);
						MY_TRACE_HWND(hwnd);

						break;
					}
				case MENU_POPUPBORDERS:
					{
						// ポップアップメニューのボーダーを描画します。

						MY_TRACE("MENU_POPUPBORDERS\n");

						auto rc2 = *rc;

						if (paint::stylus.draw_rect(dc, &rc2, palette, part_id, state_id))
							return S_OK;

						break;
					}
#endif
				case MENU_POPUPGUTTER:
					{
						// ポップアップメニューの背景左側を描画します。

						auto rc2 = *rc;

						// 後で使うので、ガーターの位置を記憶しておきます。
						gutter_right = rc2.right;

						if (paint::stylus.draw_rect(dc, &rc2, palette, part_id, state_id))
							return S_OK;

						break;
					}
				case MENU_POPUPSEPARATOR:
					{
						// ポップアップメニューのセパレータを描画します。

						auto rc2 = *rc;
						::InflateRect(&rc2, 0, -2);

						if (paint::stylus.draw_rect(dc, &rc2, palette, part_id, state_id))
							return S_OK;

						break;
					}
				case MENU_POPUPCHECKBACKGROUND:
					{
						// ポップアップメニューのアイテムを描画します。

						auto rc2 = *rc;

						// ガーターが上書きされないように描画矩形を縮小します。
						rc2.left = gutter_right;

						if (paint::stylus.draw_rect(dc, &rc2, palette, part_id, state_id))
							return S_OK;

						break;
					}
				case MENU_POPUPITEM:
				case MENU_POPUPITEMFOCUSABLE:
					{
						// ポップアップメニューのアイテムを描画します。

						// ここでポップアップアイテムの矩形を取得しておきます。
						// (後でon_theme_draw_text()内で使用します)
						popup_item_rect = *rc;

						auto rc2 = *rc;

						// ホット状態以外の場合は
						if (state_id != MPI_HOT)
						{
							// ガーターが上書きされないように描画矩形を縮小します。
							rc2.left = gutter_right;

							if (paint::stylus.draw_rect(dc, &rc2, palette, part_id, state_id))
								return S_OK;
						}
						else
						{
							if (paint::stylus.draw_round_rect(dc, &rc2, palette, part_id, state_id))
								return S_OK;
						}

						break;
					}
				case MENU_POPUPCHECK:
					{
						switch (state_id)
						{
						case MC_CHECKMARKNORMAL:
						case MC_CHECKMARKDISABLED:
							{
//								if (paint::stylus.d2d_draw_icon(dc, rc, palette, part_id, state_id, paint::c_pseudo, L'✔')) // D2Dだと紫
//								if (paint::stylus.d2d_draw_icon(dc, rc, palette, part_id, state_id, paint::c_pseudo, L'☑')) // GDIだと背景が白、D2Dだと紫
								if (paint::stylus.d2d_draw_icon(dc, rc, palette, part_id, state_id, paint::c_pseudo, L'✅')) // GDIだと背景が透明、D2Dだと緑
									return S_OK;

								break;
							}
						case MC_BULLETNORMAL:
						case MC_BULLETDISABLED:
							{
								if (paint::stylus.d2d_draw_icon(dc, rc, palette, part_id, state_id, paint::c_pseudo, L'⬤'))
									return S_OK;

								break;
							}
						}

						break;
					}
				case MENU_POPUPSUBMENU:
					{
						auto rc2 = *rc;
						::InflateRect(&rc2, 6, 6);
						::OffsetRect(&rc2, 0, -2);

						if (paint::stylus.d2d_draw_icon(dc, &rc2, palette, part_id, state_id, paint::c_symbol, 0xE013))
							return S_OK;

						break;
					}
				case MENU_SYSTEMCLOSE:
					{
						if (paint::stylus.d2d_draw_icon(dc, rc, palette, part_id, state_id, L"Webdings", 0x0072))
							return S_OK;

						break;
					}
				case MENU_SYSTEMMAXIMIZE:
					{
						if (paint::stylus.d2d_draw_icon(dc, rc, palette, part_id, state_id, L"Webdings", 0x0031))
							return S_OK;

						break;
					}
				case MENU_SYSTEMMINIMIZE:
					{
						if (paint::stylus.d2d_draw_icon(dc, rc, palette, part_id, state_id, L"Webdings", 0x0030))
							return S_OK;

						break;
					}
				case MENU_SYSTEMRESTORE:
					{
						if (paint::stylus.d2d_draw_icon(dc, rc, palette, part_id, state_id, L"Webdings", 0x0032))
							return S_OK;

						break;
					}
				default:
					{
						if (paint::stylus.draw_rect(dc, rc, palette, part_id, state_id))
							return S_OK;
					}
				}
			}

			return __super::on_draw_theme_background(theme, dc, part_id, state_id, rc, rc_clip);
		}

		virtual HRESULT on_draw_theme_background_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(rc), options);

			return __super::on_draw_theme_background_ex(theme, dc, part_id, state_id, rc, options);
		}

		virtual HRESULT on_draw_theme_text(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, ({/})", theme, dc, part_id, state_id, safe_string(text, c), text_flags, text_flags2, safe_string(rc));

			if (!(text_flags & DT_CALCRECT))
			{
				// メニュー文字列を描画するとき
				// 背景を塗りつぶさないようにします。
				auto opaque = FALSE;

				if (paint::stylus.d2d_draw_text(dc, rc, text, c, text_flags, palette, part_id, state_id, opaque))
					return S_OK;
			}

			return __super::on_draw_theme_text(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);
		}

		virtual HRESULT on_draw_theme_text_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(text, c), text_flags, safe_string(rc), options);

			return __super::on_draw_theme_text_ex(theme, dc, part_id, state_id, text, c, text_flags, rc, options);
		}

		virtual HRESULT on_draw_theme_icon(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, HIMAGELIST image_list, int image_index) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/}", theme, dc, part_id, state_id, safe_string(rc), image_list, image_index);

			return __super::on_draw_theme_icon(theme, dc, part_id, state_id, rc, image_list, image_index);
		}

		virtual HRESULT on_draw_theme_edge(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT dest_rect, UINT edge, UINT flags, LPRECT content_rect) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/hex}", theme, dc, part_id, state_id, safe_string(dest_rect), edge, flags);

			return __super::on_draw_theme_edge(theme, dc, part_id, state_id, dest_rect, edge, flags, content_rect);
		}
	} menu_renderer;
}
