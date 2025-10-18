﻿#pragma once

namespace apn::dark
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : IOBase
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto module_file_path = my::get_module_file_name(hive.instance);
			auto config_folder_path = module_file_path.parent_path() / L"config";

			path = hive.config_file_name = config_folder_path /
				module_file_path.filename().replace_extension(L".json");
			MY_TRACE_STR(hive.config_file_name);

			// コンフィグフォルダを作成しておきます。
			try { std::filesystem::create_directories(config_folder_path); } catch (...) {}

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
		// 更新処理を実行します。
		//
		BOOL update()
		{
			MY_TRACE_FUNC("");

			return config_dialog.to_ui();
		}

		//
		// ノードからダークモード化の設定を読み込みます。
		//
		BOOL read_jd(n_json& root)
		{
			MY_TRACE_FUNC("");

//			read_string(root, "jd.custom_color_file_name", hive.jd.custom_color_file_name);
			read_bool(root, "jd.exclude_comdlg32", hive.jd.exclude_comdlg32);
			read_bool(root, "jd.use_d2d", hive.jd.use_d2d);

			return TRUE;
		}

		//
		// ノードにダークモード化の設定を書き込みます。
		//
		BOOL write_jd(n_json& root)
		{
			MY_TRACE_FUNC("");

//			write_string(root, "jd.custom_color_file_name", hive.jd.custom_color_file_name);
			write_bool(root, "jd.exclude_comdlg32", hive.jd.exclude_comdlg32);
			write_bool(root, "jd.use_d2d", hive.jd.use_d2d);

			return TRUE;
		}

		//
		// ノードからスクロールバーの設定を読み込みます。
		//
		BOOL read_scrollbar(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_int(root, "scrollbar.reduction", hive.scrollbar.reduction);
			read_bool(root, "scrollbar.arrow_as_button", hive.scrollbar.arrow_as_button);
			read_bool(root, "scrollbar.has_gripper", hive.scrollbar.has_gripper);

			return TRUE;
		}

		//
		// ノードにスクロールバーの設定を書き込みます。
		//
		BOOL write_scrollbar(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_int(root, "scrollbar.reduction", hive.scrollbar.reduction);
			write_bool(root, "scrollbar.arrow_as_button", hive.scrollbar.arrow_as_button);
			write_bool(root, "scrollbar.has_gripper", hive.scrollbar.has_gripper);

			return TRUE;
		}

		//
		// ノードからスリムバーの設定を読み込みます。
		//
		BOOL read_slimbar(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "slimbar.flag_use", my::slimbar_t::config.flag_use);
			read_bool(root, "slimbar.flag_whole_title", my::slimbar_t::config.flag_whole_title);
			read_string(root, "slimbar.title_format", my::slimbar_t::config.title_format);
			read_int(root, "slimbar.button_width", my::slimbar_t::config.button_width);

			return TRUE;
		}

		//
		// ノードにスリムバーの設定を書き込みます。
		//
		BOOL write_slimbar(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "slimbar.flag_use", my::slimbar_t::config.flag_use);
			write_bool(root, "slimbar.flag_whole_title", my::slimbar_t::config.flag_whole_title);
			write_string(root, "slimbar.title_format", my::slimbar_t::config.title_format);
			write_int(root, "slimbar.button_width", my::slimbar_t::config.button_width);

			return TRUE;
		}

		//
		// ノードから丸みの設定を読み込みます。
		//
		BOOL read_round(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "round.flag_use", hive.round.flag_use);
			read_int(root, "round.size", hive.round.size);

			return TRUE;
		}

		//
		// ノードに丸みの設定を書き込みます。
		//
		BOOL write_round(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "round.flag_use", hive.round.flag_use);
			write_int(root, "round.size", hive.round.size);

			return TRUE;
		}

		//
		// ノードから縁の設定を読み込みます。
		//
		BOOL read_border(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "border.flag_3d_edge", hive.border.flag_3d_edge);
			read_int(root, "border.width", hive.border.width);

			return TRUE;
		}

		//
		// ノードに縁の設定を書き込みます。
		//
		BOOL write_border(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "border.flag_3d_edge", hive.border.flag_3d_edge);
			write_int(root, "border.width", hive.border.width);

			return TRUE;
		}

		//
		// ノードからグラデーションの設定を読み込みます。
		//
		BOOL read_gradient(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "gradient.flag_use", hive.gradient.flag_use);
			read_bool(root, "gradient.flag_end_alpha", hive.gradient.flag_end_alpha);
			read_int(root, "gradient.end_alpha", hive.gradient.end_alpha);

			return TRUE;
		}

		//
		// ノードにグラデーションの設定を書き込みます。
		//
		BOOL write_gradient(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "gradient.flag_use", hive.gradient.flag_use);
			write_bool(root, "gradient.flag_end_alpha", hive.gradient.flag_end_alpha);
			write_int(root, "gradient.end_alpha", hive.gradient.end_alpha);

			return TRUE;
		}

		//
		// ノードから影の設定を読み込みます。
		//
		BOOL read_shadow(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "shadow.flag_use", hive.shadow.flag_use);
			read_bool(root, "shadow.flag_blur", hive.shadow.flag_blur);
			read_point(root, "shadow.offset", hive.shadow.offset);
			read_int(root, "shadow.size", hive.shadow.size);

			return TRUE;
		}

		//
		// ノードに影の設定を書き込みます。
		//
		BOOL write_shadow(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "shadow.flag_use", hive.shadow.flag_use);
			write_bool(root, "shadow.flag_blur", hive.shadow.flag_blur);
			write_point(root, "shadow.offset", hive.shadow.offset);
			write_int(root, "shadow.size", hive.shadow.size);

			return TRUE;
		}

		//
		// ノードからその他の設定を読み込みます。
		//
		BOOL read_etc(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_window_pos(root, "config_dialog", config_dialog);

			return TRUE;
		}

		//
		// ノードにその他の設定を書き込みます。
		//
		BOOL write_etc(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_window_pos(root, "config_dialog", config_dialog);

			return TRUE;
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_jd(root);
			read_scrollbar(root);
			read_slimbar(root);
			read_round(root);
			read_border(root);
			read_gradient(root);
			read_shadow(root);
			read_etc(root);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_jd(root);
			write_scrollbar(root);
			write_slimbar(root);
			write_round(root);
			write_border(root);
			write_gradient(root);
			write_shadow(root);
			write_etc(root);

			return TRUE;
		}
	} config_io;
}
