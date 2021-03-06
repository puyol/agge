#pragma once

#include <agge/pixel.h>

struct HBITMAP__;
typedef struct HBITMAP__ *HBITMAP;

struct HDC__;
typedef struct HDC__ *HDC;

namespace agge
{
	namespace platform
	{
		class raw_bitmap : noncopyable
		{
		public: // General
			raw_bitmap(count_t width, count_t height, bits_per_pixel bpp, count_t row_extra_bytes = 0);
			~raw_bitmap();

			void resize(count_t width, count_t height);

			count_t width() const;
			count_t height() const;

			void *row_ptr(count_t y);
			const void *row_ptr(count_t y) const;

		public: // Win32
			HBITMAP native() const;
			void blit(HDC hdc, int x, int y, count_t width, count_t height) const;

		private:
			static count_t calculate_stride(count_t width, bits_per_pixel bpp);

		private:
			void *_memory;
			count_t _stride;
			count_t _width, _max_width, _height, _max_height;
			const bits_per_pixel _bpp;
			HBITMAP _native;
			const count_t _extra_pixels;
		};



		inline count_t raw_bitmap::width() const
		{	return _width;	}

		inline count_t raw_bitmap::height() const
		{	return _height;	}

		inline void *raw_bitmap::row_ptr(count_t y)
		{	return static_cast<uint8_t *>(_memory) + y * _stride;	}

		inline const void *raw_bitmap::row_ptr(count_t y) const
		{	return static_cast<const uint8_t *>(_memory) + y * _stride;	}

		inline HBITMAP raw_bitmap::native() const
		{	return _native;	}
	}
}
