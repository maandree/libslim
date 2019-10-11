/**
 * ISC License
 * 
 * © 2019 Mattias Andrée <maandree@kth.se>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef LIBSLIM_H
#define LIBSLIM_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>


struct libslim_image_meta {
	size_t width;
	size_t height;
	size_t hblank;
};

#define LIBSLIM_DECLARE_FORMAT(SUFFIX, PIXEL)\
	struct libslim_pixel_##SUFFIX PIXEL;\
	struct libslim_image_##SUFFIX {\
		struct libslim_image_meta meta;\
		struct libslim_pixel_##SUFFIX *data;\
	}

LIBSLIM_DECLARE_FORMAT(xyza_f, { float x, y, z, a; });
LIBSLIM_DECLARE_FORMAT(xyza_d, { double x, y, z, a; });
LIBSLIM_DECLARE_FORMAT(xyza_ld, { long double x, y, z, a; });

LIBSLIM_DECLARE_FORMAT(xyz_f, { float x, y, z; });
LIBSLIM_DECLARE_FORMAT(xyz_d, { double x, y, z; });
LIBSLIM_DECLARE_FORMAT(xyz_ld, { long double x, y, z; });

LIBSLIM_DECLARE_FORMAT(rgba_f, { float r, g, b, a; });
LIBSLIM_DECLARE_FORMAT(rgba_d, { double r, g, b, a; });
LIBSLIM_DECLARE_FORMAT(rgba_ld, { long double r, g, b, a; });

LIBSLIM_DECLARE_FORMAT(rgb_f, { float r, g, b; });
LIBSLIM_DECLARE_FORMAT(rgb_d, { double r, g, b; });
LIBSLIM_DECLARE_FORMAT(rgb_ld, { long double r, g, b; });



/* Replace an entire row, of an image, with a single colour */
#define libslim_set_colour_row(OUT, COLOUR)\
	do {\
		size_t x__, y__;\
		size_t w__ = (OUT)->meta.width;\
		for (x__ = 0; x__ < w__; x__++)\
			(OUT)->data[x__] = *(COLOUR);\
	} while (0)


/* Replace an entire image with a single colour */
#define libslim_set_colour(OUT, COLOUR)\
	do {\
		size_t x__, y__;\
		size_t h__ = (OUT)->meta.height;\
		size_t w__ = (OUT)->meta.width;\
		void *out__ = (OUT)->data;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++)\
				(OUT)->data[x__] = (COLOUR);\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(OUT)->data = out__;\
	} while (0)


/* Horizontally flip a row of an image */
#define libslim_flop_row(OUT, IN)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width - 1;\
		for (i__ = 0; i__ < n__; i__++)\
			(OUT)->data[n__ - i__] = (IN)->data[i__];\
	} while (0)


/* Horizontally flip an image */
#define libslim_flop(OUT, IN)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width - 1;\
		size_t h__ = (IN)->meta.height;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.width = (IN)->meta.width;\
		(OUT)->meta.height = (IN)->meta.height;\
		while (h__--) {\
			for (i__ = 0; i__ < n__; i__++)\
				(OUT)->data[n__ - i__] = (IN)->data[i__];\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Vertically flip an image */
#define libslim_flip(OUT, IN)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.height - 1;\
		size_t w__ = (IN)->meta.width * sizeof(*(IN)->data);\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.width = (IN)->meta.width;\
		(OUT)->meta.height = (IN)->meta.height;\
		(OUT)->data += n__ * ((OUT)->meta.width + (OUT)->meta.hblank);\
		for (i__ = 0; i__ < n__; i__++) {\
			memcpy((OUT)->data, (IN)->data, w__);\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data -= (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Transpose an image */
#define libslim_transpose(OUT, IN)\
	do {\
		size_t x__, y__, i__;\
		size_t rw__ = (OUT)->meta.width + (OUT)->meta.hblank;\
		size_t w__ = (IN)->meta.width;\
		size_t h__ = (IN)->meta.height;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.width = (IN)->meta.height;\
		(OUT)->meta.height = (IN)->meta.width;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = i__ = 0; x__ < w__; x__++, i__ += rw__)\
				(OUT)->data[i__] = (IN)->data[x__];\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += 1;\
		}\
		(IN)->data = in__;\
		(OUT)->data = in__;\
	} while (0)


/* Rotate an image 90 degrees clockwise */
#define libslim_rotate_90(OUT, IN)\
	do {\
		libslim_transpose((IN), (OUT));\
		libslim_flop((IN), (OUT));\
	} while (0)


/* Rotate an image 180 degrees */
#define libslim_rotate_180(OUT, IN)\
	do {\
		libslim_flip((IN), (OUT));\
		libslim_flop((IN), (OUT));\
	} while (0)


/* Rotate an image 270 degrees clockwise */
#define libslim_rotate_270(OUT, IN)\
	do {\
		libslim_flop((IN), (OUT));\
		libslim_transpose((IN), (OUT));\
	} while (0)


/* Swap channels in an image with 4 channels */
#define libslim_swap_channels_4(OUT, IN, IN_CH1, OUT_CH1, IN_CH2, OUT_CH2, IN_CH3, OUT_CH3, IN_CH4, OUT_CH4)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__].OUT_CH1 = (IN)->data[x__].IN_CH1;\
				(OUT)->data[x__].OUT_CH2 = (IN)->data[x__].IN_CH2;\
				(OUT)->data[x__].OUT_CH3 = (IN)->data[x__].IN_CH3;\
				(OUT)->data[x__].OUT_CH4 = (IN)->data[x__].IN_CH4;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Swap channels in a row of an image with 4 channels */
#define libslim_swap_channels_4_row(OUT, IN, IN_CH1, OUT_CH1, IN_CH2, OUT_CH2, IN_CH3, OUT_CH3, IN_CH4, OUT_CH4)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__].OUT_CH1 = (IN)->data[i__].IN_CH1;\
			(OUT)->data[i__].OUT_CH2 = (IN)->data[i__].IN_CH2;\
			(OUT)->data[i__].OUT_CH3 = (IN)->data[i__].IN_CH3;\
			(OUT)->data[i__].OUT_CH4 = (IN)->data[i__].IN_CH4;\
		}\
	} while (0)


/* Swap channels in an image with 3 channels */
#define libslim_swap_channels_3(OUT, IN, IN_CH1, OUT_CH1, IN_CH2, OUT_CH2, IN_CH3, OUT_CH3)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__].OUT_CH1 = (IN)->data[x__].IN_CH1;\
				(OUT)->data[x__].OUT_CH2 = (IN)->data[x__].IN_CH2;\
				(OUT)->data[x__].OUT_CH3 = (IN)->data[x__].IN_CH3;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Swap channels in a row of an image with 3 channels */
#define libslim_swap_channels_3_row(OUT, IN, IN_CH1, OUT_CH1, IN_CH2, OUT_CH2, IN_CH3, OUT_CH3)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__].OUT_CH1 = (IN)->data[i__].IN_CH1;\
			(OUT)->data[i__].OUT_CH2 = (IN)->data[i__].IN_CH2;\
			(OUT)->data[i__].OUT_CH3 = (IN)->data[i__].IN_CH3;\
		}\
	} while (0)


/* Swap channels in an image with 2 channels */
#define libslim_swap_channels_2(OUT, IN, IN_CH1, OUT_CH1, IN_CH2, OUT_CH2)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__].OUT_CH1 = (IN)->data[x__].IN_CH1;\
				(OUT)->data[x__].OUT_CH2 = (IN)->data[x__].IN_CH2;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Swap channels in a row of an image with 2 channels */
#define libslim_swap_channels_2_row(OUT, IN, IN_CH1, OUT_CH1, IN_CH2, OUT_CH2)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__].OUT_CH1 = (IN)->data[i__].IN_CH1;\
			(OUT)->data[i__].OUT_CH2 = (IN)->data[i__].IN_CH2;\
		}\
	} while (0)


/* Set the values of 3 channels in all pixels in an image */
#define libslim_set_3_channels(OUT, IN, COLOUR, CH1, CH2, CH3)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__] = (IN)->data[x__];\
				(OUT)->data[x__].CH1 = (COLOUR)->CH1;\
				(OUT)->data[x__].CH2 = (COLOUR)->CH2;\
				(OUT)->data[x__].CH3 = (COLOUR)->CH3;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Set the values of 3 channels in all pixels in a row of an image */
#define libslim_set_3_channels_row(OUT, IN, COLOUR, CH1, CH2, CH3)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__] = (IN)->data[i__];\
			(OUT)->data[i__].CH1 = (COLOUR)->CH1;\
			(OUT)->data[i__].CH2 = (COLOUR)->CH2;\
			(OUT)->data[i__].CH3 = (COLOUR)->CH3;\
		}\
	} while (0)


/* Set the values of 2 channels in all pixels in an image */
#define libslim_set_2_channels(OUT, IN, COLOUR, CH1, CH2)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__] = (IN)->data[x__];\
				(OUT)->data[x__].CH1 = (COLOUR)->CH1;\
				(OUT)->data[x__].CH2 = (COLOUR)->CH2;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Set the values of 2 channels in all pixels in a row of an image */
#define libslim_set_2_channels_row(OUT, IN, COLOUR, CH1, CH2)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__] = (IN)->data[i__];\
			(OUT)->data[i__].CH1 = (COLOUR)->CH1;\
			(OUT)->data[i__].CH2 = (COLOUR)->CH2;\
		}\
	} while (0)


/* Set the values of 1 channel in all pixels in an image */
#define libslim_set_1_channel(OUT, IN, COLOUR, CH)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__] = (IN)->data[x__];\
				(OUT)->data[x__].CH = (COLOUR)->CH;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Set the values of 1 channel in all pixels in a row of an image */
#define libslim_set_1_channel_row(OUT, IN, COLOUR, CH)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__] = (IN)->data[i__];\
			(OUT)->data[i__].CH = (COLOUR)->CH;\
		}\
	} while (0)


/* Crop an image */
#define libslim_crop(OUT, IN, LEFT, TOP, WIDTH, HEIGHT)\
	do {\
		size_t i__;\
		size_t w__ = (WIDTH);\
		size_t h__ = (HEIGHT);\
		size_t rw__ = w__ * sizeof(*(IN)->data);\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.width = w__;\
		(OUT)->meta.height = h__;\
		(IN)->data += (LEFT);\
		(IN)->data += (TOP) * ((IN)->meta.width + (IN)->meta.hblank);\
		for (i__ = 0; i__ < h__; i__++) {\
			memcpy((OUT)->data, (IN)->data, rw__);\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Premultiply 3 channels in all pixels in an image */
#define libslim_premultiply_3_channels(OUT, IN, CH1, CH2, CH3)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__].CH1 = (IN)->data[x__].CH1 * (IN)->data[x__].a;\
				(OUT)->data[x__].CH2 = (IN)->data[x__].CH2 * (IN)->data[x__].a;\
				(OUT)->data[x__].CH3 = (IN)->data[x__].CH3 * (IN)->data[x__].a;\
				(OUT)->data[x__].a = (IN)->data[x__].a;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Premultiply 3 channels in all pixels in a row of an image */
#define libslim_premultiply_3_channels_row(OUT, IN, CH1, CH2, CH3)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__].CH1 = (IN)->data[i__].CH1 * (IN)->data[i__].a;\
			(OUT)->data[i__].CH2 = (IN)->data[i__].CH2 * (IN)->data[i__].a;\
			(OUT)->data[i__].CH3 = (IN)->data[i__].CH3 * (IN)->data[i__].a;\
			(OUT)->data[i__].a = (IN)->data[i__].a;\
		}\
	} while (0)


/* Premultiply 2 channels in all pixels in an image */
#define libslim_premultiply_2_channels(OUT, IN, CH1, CH2)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__].CH1 = (IN)->data[x__].CH1 * (IN)->data[x__].a;\
				(OUT)->data[x__].CH2 = (IN)->data[x__].CH2 * (IN)->data[x__].a;\
				(OUT)->data[x__].a = (IN)->data[x__].a;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Premultiply 2 channels in all pixels in a row of an image */
#define libslim_premultiply_2_channels_row(OUT, IN, CH1, CH2)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__].CH1 = (IN)->data[i__].CH1 * (IN)->data[i__].a;\
			(OUT)->data[i__].CH2 = (IN)->data[i__].CH2 * (IN)->data[i__].a;\
			(OUT)->data[i__].a = (IN)->data[i__].a;\
		}\
	} while (0)


/* Premultiply 1 channel in all pixels in an image */
#define libslim_premultiply_1_channel(OUT, IN, CH)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__].CH = (IN)->data[x__].CH * (IN)->data[x__].a;\
				(OUT)->data[x__].a = (IN)->data[x__].a;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Premultiply 1 channel in all pixels in a row of an image */
#define libslim_premultiply_1_channel_row(OUT, IN, CH)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__].CH = (IN)->data[i__].CH * (IN)->data[i__].a;\
			(OUT)->data[i__].a = (IN)->data[i__].a;\
		}\
	} while (0)


/* Unpremultiply 3 channels in all pixels in an image */
#define libslim_unpremultiply_3_channels(OUT, IN, CH1, CH2, CH3)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__] = (IN)->data[x__];\
				if ((IN)->data[i__].a) {\
					(OUT)->data[x__].CH1 /= (IN)->data[x__].a;\
					(OUT)->data[x__].CH2 /= (IN)->data[x__].a;\
					(OUT)->data[x__].CH3 /= (IN)->data[x__].a;\
				}\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Unpremultiply 3 channels in all pixels in a row of an image */
#define libslim_unpremultiply_3_channels_row(OUT, IN, CH1, CH2, CH3)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__] = (IN)->data[i__];\
			if ((IN)->data[i__].a) {\
				(OUT)->data[i__].CH1 /= (IN)->data[i__].a;\
				(OUT)->data[i__].CH2 /= (IN)->data[i__].a;\
				(OUT)->data[i__].CH3 /= (IN)->data[i__].a;\
			}\
		}\
	} while (0)


/* Unpremultiply 2 channels in all pixels in an image */
#define libslim_unpremultiply_2_channels(OUT, IN, CH1, CH2)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__] = (IN)->data[x__];\
				if ((IN)->data[i__].a) {\
					(OUT)->data[x__].CH1 /= (IN)->data[x__].a;\
					(OUT)->data[x__].CH2 /= (IN)->data[x__].a;\
				}\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Unpremultiply 2 channels in all pixels in a row of an image */
#define libslim_unpremultiply_2_channels_row(OUT, IN, CH1, CH2)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__] = (IN)->data[i__];\
			if ((IN)->data[i__].a) {\
				(OUT)->data[i__].CH1 /= (IN)->data[i__].a;\
				(OUT)->data[i__].CH2 /= (IN)->data[i__].a;\
			}\
		}\
	} while (0)


/* Unpremultiply 1 channel in all pixels in an image */
#define libslim_unpremultiply_1_channel(OUT, IN, CH)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__] = (IN)->data[x__];\
				if ((IN)->data[i__].a)\
					(OUT)->data[x__].CH /= (IN)->data[x__].a;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Unpremultiply 1 channel in all pixels in a row of an image */
#define libslim_unpremultiply_1_channel_row(OUT, IN, CH)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__] = (IN)->data[i__];\
			if ((IN)->data[i__].a)\
				(OUT)->data[i__].CH /= (IN)->data[i__].a;\
		}\
	} while (0)


/* Unpremultiply 3 channels in all pixels in an image */
#define libslim_unpremultiply_3_channels_zero(OUT, IN, ZERO, CH1, CH2, CH3)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__] = (IN)->data[x__];\
				if ((IN)->data[i__].a) {\
					(OUT)->data[x__].CH1 /= (IN)->data[x__].a;\
					(OUT)->data[x__].CH2 /= (IN)->data[x__].a;\
					(OUT)->data[x__].CH3 /= (IN)->data[x__].a;\
				} else {\
					(OUT)->data[x__].CH1 = (ZERO)->CH1;\
					(OUT)->data[x__].CH2 = (ZERO)->CH2;\
					(OUT)->data[x__].CH3 = (ZERO)->CH3;\
				}
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Unpremultiply 3 channels in all pixels in a row of an image */
#define libslim_unpremultiply_3_channels_zero_row(OUT, IN, ZERO, CH1, CH2, CH3)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__] = (IN)->data[i__];\
			if ((IN)->data[i__].a) {\
				(OUT)->data[i__].CH1 /= (IN)->data[i__].a;\
				(OUT)->data[i__].CH2 /= (IN)->data[i__].a;\
				(OUT)->data[i__].CH3 /= (IN)->data[i__].a;\
			} else {\
				(OUT)->data[x__].CH1 = (ZERO)->CH1;\
				(OUT)->data[x__].CH2 = (ZERO)->CH2;\
				(OUT)->data[x__].CH3 = (ZERO)->CH3;\
			}\
		}\
	} while (0)


/* Unpremultiply 2 channels in all pixels in an image */
#define libslim_unpremultiply_2_channels_zero(OUT, IN, ZERO, CH1, CH2)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__] = (IN)->data[x__];\
				if ((IN)->data[i__].a) {\
					(OUT)->data[x__].CH1 /= (IN)->data[x__].a;\
					(OUT)->data[x__].CH2 /= (IN)->data[x__].a;\
				} else {\
					(OUT)->data[x__].CH1 = (ZERO)->CH1;\
					(OUT)->data[x__].CH2 = (ZERO)->CH2;\
				}\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Unpremultiply 2 channels in all pixels in a row of an image */
#define libslim_unpremultiply_2_channels_zero_row(OUT, IN, ZERO, CH1, CH2)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__] = (IN)->data[i__];\
			if ((IN)->data[i__].a) {\
				(OUT)->data[i__].CH1 /= (IN)->data[i__].a;\
				(OUT)->data[i__].CH2 /= (IN)->data[i__].a;\
			} else {\
				(OUT)->data[x__].CH1 = (ZERO)->CH1;\
				(OUT)->data[x__].CH2 = (ZERO)->CH2;\
			}\
		}\
	} while (0)


/* Unpremultiply 1 channel in all pixels in an image */
#define libslim_unpremultiply_1_channel_zero(OUT, IN, ZERO, CH)\
	do {\
		size_t x__, y__;\
		size_t h__ = (IN)->meta.height;\
		size_t w__ = (IN)->meta.width;\
		void *in__ = (IN)->data;\
		void *out__ = (OUT)->data;\
		(OUT)->meta.height = h__;\
		(OUT)->meta.width = w__;\
		for (y__ = 0; y__ < h__; y__++) {\
			for (x__ = 0; x__ < w__; x__++) {\
				(OUT)->data[x__] = (IN)->data[x__];\
				if ((IN)->data[i__].a)\
					(OUT)->data[x__].CH /= (IN)->data[x__].a;\
				else\
					(OUT)->data[x__].CH = (ZERO)->CH;\
			}\
			(IN)->data += (IN)->meta.width + (IN)->meta.hblank;\
			(OUT)->data += (OUT)->meta.width + (OUT)->meta.hblank;\
		}\
		(IN)->data = in__;\
		(OUT)->data = out__;\
	} while (0)


/* Unpremultiply 1 channel in all pixels in a row of an image */
#define libslim_unpremultiply_1_channel_zero_row(OUT, IN, ZERO, CH)\
	do {\
		size_t i__;\
		size_t n__ = (IN)->meta.width;\
		for (i__ = 0; i__ < n__; i__++) {\
			(OUT)->data[i__] = (IN)->data[i__];\
			if ((IN)->data[i__].a)\
				(OUT)->data[i__].CH /= (IN)->data[i__].a;\
			else\
				(OUT)->data[x__].CH = (ZERO)->CH;\
		}\
	} while (0)


#endif
