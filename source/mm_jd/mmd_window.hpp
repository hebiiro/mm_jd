#pragma once

namespace apn::dark
{
	//
	// このクラスはmmdウィンドウを管理します。
	//
	inline struct mmd_window_t
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.theme_window = getHWND();
			hive.show_config_id = createWM_APP_ID();
			hive.slimbar.subclass(hive.theme_window);

			// メインメニューを取得します。
			auto menu = ::GetMenu(hive.theme_window);

			// メインメニューの項目数を取得します。
			auto c = ::GetMenuItemCount(menu);

			// メインメニューの項目を走査します。
			for (decltype(c) i = 0; i < c; i++)
			{
				// メニュー項目のテキストを取得します。
				auto text = my::get_menu_item_text(menu, i, MF_BYPOSITION);

				// メニュー項目が操作対象ではない場合は何もしません。
				if (text != L"MMDPlugin") continue;

				// サブメニューを取得します。
				auto sub_menu = ::GetSubMenu(menu, i);

				// サブメニューの項目数を取得します。
				auto c = ::GetMenuItemCount(sub_menu);

				// サブメニューの項目を走査します。
				for (decltype(c) i = 0; i < c; i++)
				{
					// メニュー項目のテキストを取得します。
					auto text = my::get_menu_item_text(sub_menu, i, MF_BYPOSITION);

					// メニュー項目が操作対象ではない場合は何もしません。
					if (text.find(L"mm_jd") == text.npos) continue;

					// バージョン情報を取得します。
					auto item_name = my::ws(version_info.name);

					// エキストラメニューを作成します。
					auto extra_menu = ::CreatePopupMenu();

					// エキストラメニューにメニュー項目を追加します。
					::AppendMenuW(extra_menu, MF_STRING, hive.show_config_id, L"設定");

					// メニュー項目にバージョン情報とエキストラメニューをセットします。
					MENUITEMINFOW mii = { sizeof(mii) };
					mii.fMask = MIIM_STRING | MIIM_SUBMENU;
					mii.dwTypeData = item_name.data();
					mii.hSubMenu = extra_menu;
					::SetMenuItemInfoW(sub_menu, i, TRUE, &mii);

					// ループを終了します。
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

			hive.slimbar.unsubclass();

			return TRUE;
		}
	} mmd_window;
}
