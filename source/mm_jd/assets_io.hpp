#pragma once

namespace apn::dark
{
	//
	// このクラスはアセットの入出力を担当します。
	//
	inline struct AssetsIO : IOBase
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// アセットフォルダのパスを取得します。
			auto module_file_path = my::get_module_file_name(hive.instance);
			hive.assets_folder_path = module_file_path.parent_path() / L"assets";

			// アセットファイルのパスを取得します。
			path = hive.assets_file_name = hive.assets_folder_path /
				module_file_path.filename().replace_extension(L".json");
			MY_TRACE_STR(hive.assets_file_name);

			// アセットフォルダを作成しておきます。
			try { std::filesystem::create_directories(hive.assets_folder_path); } catch (...) {}

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

			return TRUE;
		}

		//
		// ノードからアセットを読み込みます。
		//
		virtual BOOL read_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ノードにアセットを書き込みます。
		//
		virtual BOOL write_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} assets_io;
}
