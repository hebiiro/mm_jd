#pragma once

namespace apn::dark::kuro
{
#pragma pack(push)
#pragma pack(1)
	//
	// このクラスはRGBAを保持します。
	//
	union RGBA {
		uint32_t value; // 0xrrggbbaa;
		uint8_t vec[4];
		struct { uint8_t a, b, g, r; };
	};
#pragma pack(pop)

	//
	// このクラスは配色データを保持します。
	//
	struct Color
	{
		RGBA rgba;
		COLORREF win32;

		//
		// win32カラーをrgbaに変換して返します。
		//
		inline static constexpr RGBA win32_to_rgba(COLORREF win32)
		{
			return { .a = 0xff, .b = GetBValue(win32), .g = GetGValue(win32), .r = GetRValue(win32) };
		}

		//
		// デフォルトコンストラクタです。
		//
		constexpr Color()
			: rgba(0)
			, win32(CLR_NONE)
		{
		}

		//
		// コンストラクタです。
		//
		constexpr Color(const RGBA& rgba)
			: rgba(rgba)
			, win32(RGB(rgba.r, rgba.g, rgba.b))
		{
		}

		//
		// コンストラクタです。
		//
		constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			: rgba { .a = a, .b = b, .g = g, .r = r }
			, win32(RGB(r, g, b))
		{
		}

		//
		// コンストラクタです。
		//
		constexpr Color(COLORREF win32_color)
			: rgba((win32_color != CLR_NONE) ? win32_to_rgba(win32_color) : RGBA {})
			, win32(win32_color)
		{
		}

		//
		// 配色データが有効の場合はTRUEを返します。
		//
		constexpr BOOL is_valid() const
		{
//			return !!rgba.a;
			return win32 != CLR_NONE;
		}

		//
		// 各配色成分を実数で返します。
		//
		constexpr float red() const { return rgba.r / 255.0f; }
		constexpr float green() const { return rgba.g / 255.0f; }
		constexpr float blue() const { return rgba.b / 255.0f; }
		constexpr float alpha() const { return rgba.a / 255.0f; }
	};

	//
	// このクラスはカラーエントリです。
	//
	struct ColorEntry
	{
		//
		// カラーパーツの最大数です。
		//
		inline static constexpr size_t c_max_size = 2;

		//
		// カラーパーツです。
		//
		Color parts[c_max_size] = {};
	};
}
