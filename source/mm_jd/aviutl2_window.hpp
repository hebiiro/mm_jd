#pragma once

namespace apn::dark
{
	//
	// このクラスはaviutl2ウィンドウを管理します。
	//
	inline struct AviUtl2Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.theme_window = getHWND();
			hive.show_config_id = createWM_APP_ID();

			auto menu = ::GetMenu(hive.theme_window);
			auto sub_menu = ::GetSubMenu(menu, 8);
			auto c = ::GetMenuItemCount(sub_menu);

			for (decltype(c) i = 0; i < c; i++)
			{
				auto text = my::get_menu_item_text(sub_menu, i, MF_BYPOSITION);
				if (text.find(L"mm_jd") != text.npos)
				{
					auto extra_menu = ::CreatePopupMenu();
					::AppendMenuW(extra_menu, MF_STRING, hive.show_config_id, L"設定");

					MENUITEMINFOW mii = { sizeof(mii) };
					mii.fMask = MIIM_SUBMENU;
					mii.hSubMenu = extra_menu;
					::SetMenuItemInfoW(sub_menu, i, TRUE, &mii);

					break;
				}
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// aviutl2ウィンドウを取得します。
		//
		BOOL get_aviutl2_window()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンフィグをaviutl2ウィンドウに適用します。
		//
		BOOL apply_config()
		{
			MY_TRACE_FUNC("");

			// aviutl2ウィンドウを最前面にします。
//			::SetForegroundWindow(hive.theme_window);

			return TRUE;
		}

		//
		// スリムメニューバーの設定をaviutl2ウィンドウに適用します。
		//
		BOOL apply_slim_menubar()
		{
//			return TRUE;

			// メニューバーとタイトルバーを一体化する場合は
			if (hive.slimbar.flag_use)
			{
//				my::modify_style(hive.theme_window, WS_CAPTION, 0);
//				my::modify_style(hive.theme_window, WS_DLGFRAME, 0);
				my::modify_style(hive.theme_window, WS_BORDER, 0);
			}
			// メニューバーとタイトルバーを一体化しない場合は
			else
			{
				my::modify_style(hive.theme_window, 0, WS_CAPTION);
			}

			return ::SetWindowPos(hive.theme_window, nullptr, 0, 0, 0, 0,
				SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		}
	} aviutl2_window;
}
