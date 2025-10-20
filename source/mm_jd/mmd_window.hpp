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
					mii.fMask = MIIM_STRING | MIIM_SUBMENU;
					mii.dwTypeData = my::ws(version_info.name).data();
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

			hive.slimbar.unsubclass();

			return TRUE;
		}
	} mmd_window;
}
