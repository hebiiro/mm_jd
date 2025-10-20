#pragma once

namespace apn::dark::kuro::gdi
{
	//
	// このクラスはGDIレンダラーを管理します。
	//
	inline struct Manager
	{
		//
		// 指定されたウィンドウに対応するGDIレンダラーを返します。
		//
		std::shared_ptr<Renderer> find_renderer(
			HWND hwnd, const Renderer::NormalizedClassName& class_name)
		{
//			if (class_name == L"#32768") return std::make_shared<MenuRenderer>();
			if (class_name == L"#32770")
			{
				auto instance = (HINSTANCE)::GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
				auto comdlg32 = ::GetModuleHandleW(L"comdlg32.dll");
//				auto style = my::get_style(hwnd);

				// コモンダイアログの場合は
				if (instance == comdlg32)
//				if (instance == comdlg32 && style & WS_THICKFRAME)
					return std::make_shared<comdlg32::DialogRenderer>();

				return std::make_shared<DialogRenderer>();
			}

			if (class_name == WC_STATIC) return std::make_shared<StaticRenderer>();
			if (class_name == WC_BUTTON) return std::make_shared<ButtonRenderer>();
			if (class_name == WC_EDIT) return std::make_shared<EditBoxRenderer>();
//			if (class_name == RICHEDIT_CLASS) return std::make_shared<RichEditRenderer>();
			if (class_name == WC_LISTBOX) return std::make_shared<ListBoxRenderer>();
			if (class_name == L"ListviewPopup") return std::make_shared<ListBoxRenderer>();
			if (class_name == L"ComboLBox") return std::make_shared<ListBoxRenderer>();
			if (class_name == WC_COMBOBOX) return std::make_shared<ComboBoxRenderer>();
			if (class_name == WC_COMBOBOXEX) return std::make_shared<ComboBoxExRenderer>();
			if (class_name == TOOLTIPS_CLASS) return std::make_shared<ToolTipRenderer>();
			if (class_name == TRACKBAR_CLASS) return std::make_shared<TrackBarRenderer>();
			if (class_name == UPDOWN_CLASS) return std::make_shared<SpinRenderer>();
			if (class_name == WC_TABCONTROL) return std::make_shared<TabRenderer>();
			if (class_name == WC_HEADER) return std::make_shared<HeaderRenderer>();
			if (class_name == WC_LISTVIEW) return std::make_shared<ListViewRenderer>();
			if (class_name == WC_TREEVIEW)
			{
				auto instance = (HINSTANCE)::GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
				auto ExplorerFrame = ::GetModuleHandleW(L"ExplorerFrame.dll");

				// エクスプローラのツリービューの場合は
				if (instance == ExplorerFrame)
				{
					// ウィンドウの縁にボーダーを追加します。
					my::modify_style(hwnd, 0, WS_BORDER);
//					my::modify_ex_style(hwnd, 0, WS_EX_CLIENTEDGE);
					::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
						SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
				}

				return std::make_shared<TreeViewRenderer>();
			}

			if (class_name == TOOLBARCLASSNAME) return std::make_shared<ToolBarRenderer>();

			// mmdのメインウィンドウです。
			if (class_name == L"Polygon Movie Maker")
				return std::make_shared<AviUtl2Renderer>();

			if (hive.comdlg32_visible_count)
			{
				// ファイル選択ダイアログのリストビューなどの親ウィンドウです。
//				if (class_name == L"DirectUIHWND") return std::make_shared<comdlg32::DirectUIHWNDRenderer>();

				// DirectUIHWNDの親ウィンドウです。
				if (class_name == L"DUIViewWndClassName") return std::make_shared<comdlg32::DUIViewRenderer>();

				// ファイル選択ダイアログのツリービューの親ウィンドウです。
				if (class_name == L"NamespaceTreeControl") return std::make_shared<Renderer>();

				// ファイル選択ダイアログのコンボボックスの親ウィンドウです。
//				if (class_name == L"FloatNotifySink") return std::make_shared<Renderer>();

				// コマンドモジュールがドロップダウン表示するポップアップメニューのようなウィンドウです。
//				if (class_name == L"ViewControlClass") return std::make_shared<Renderer>();

				// ファイル選択ダイアログのリストビューの場合は
				if (class_name == L"SHELLDLL_DefView")
				{
#if 0
					auto parent = ::GetParent(hwnd);
					auto parent_class_name = my::get_class_name(parent);

					if (parent_class_name == L"#32770")
#endif
					{
						// ウィンドウの縁にボーダーを追加します。
						my::modify_style(hwnd, 0, WS_BORDER);
//						my::modify_ex_style(hwnd, 0, WS_EX_CLIENTEDGE);
						::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
							SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
					}
				}

				// フォルダ選択ダイアログのツリービューの親ウィンドウです。
				if (class_name == L"SHBrowseForFolder ShellNameSpace Control") return std::make_shared<Renderer>();
			}

			if (0) // テスト用コードです。
			{
				return std::make_shared<Renderer>();
			}

			return nullptr;
		}

		//
		// GDIレンダラーをウィンドウに関連付けされます。
		//
		void attach_renderer(HWND hwnd)
		{
			auto class_name = my::get_class_name(hwnd);

			MY_TRACE_FUNC("{/hex}, {/}", hwnd, class_name);

			if (auto renderer = find_renderer(hwnd, class_name))
				renderer->attach(hwnd);
		}
		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND hwnd)
		{
			MY_TRACE_FUNC("");

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
	} manager;
}
