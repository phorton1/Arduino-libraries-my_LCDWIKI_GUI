#include "LCDWIKI_font.c"
#include "LCDWIKI_GUI.h"
#include <myDebug.h>

// Miscellaneous

void LCDWIKI_GUI::drawBorder(int x, int y, int w, int h, int b, int color)
	// draw a frigin border
{
	Fill_Rect(x,		y,		b,		h,	color);
	Fill_Rect(x+w-b,	y,		b,		h,	color);
	Fill_Rect(x,		y,		w,		b,	color);
	Fill_Rect(x,		y+h-b,	w,		b,	color);
}


//------------------------------------------------------
// ILI9431 FONT STUFF
//------------------------------------------------------

#ifndef WITH_ILI9431_FONTS
	// prh - already defined in Paul's stuff
	#define swap(a, b) { int16_t t = a; a = b; b = t; }
#endif


#if WITH_ILI9431_FONTS


	static uint32_t fetchbit(const uint8_t *p, uint32_t index)
	{
		if (p[index >> 3] & (1 << (7 - (index & 7)))) return 1;
		return 0;
	}

	static uint32_t fetchbits_unsigned(const uint8_t *p, uint32_t index, uint32_t required)
	{
		uint32_t val = 0;
		do {
			uint8_t b = p[index >> 3];
			uint32_t avail = 8 - (index & 7);
			if (avail <= required) {
				val <<= avail;
				val |= b & ((1 << avail) - 1);
				index += avail;
				required -= avail;
			} else {
				b >>= avail - required;
				val <<= required;
				val |= b & ((1 << required) - 1);
				break;
			}
		} while (required);
		return val;
	}

	static uint32_t fetchbits_signed(const uint8_t *p, uint32_t index, uint32_t required)
	{
		uint32_t val = fetchbits_unsigned(p, index, required);
		if (val & (1 << (required - 1))) {
			return (int32_t)val - (1 << required);
		}
		return (int32_t)val;
	}


	#define DEBUG_DRAW_FONT_CHAR   0


	void LCDWIKI_GUI::drawFontChar(unsigned int c)
	{
		uint32_t bitoffset;
		const uint8_t *data;

		#if DEBUG_DRAW_FONT_CHAR
			Serial.printf("drawFontChar %d at x=%d y=%d\n", c,text_x,text_y);
		#endif

		if (c >= font->index1_first && c <= font->index1_last)
		{
			bitoffset = c - font->index1_first;
			bitoffset *= font->bits_index;
		}
		else if (c >= font->index2_first && c <= font->index2_last)
		{
			bitoffset = c - font->index2_first + font->index1_last - font->index1_first + 1;
			bitoffset *= font->bits_index;
		}
		else if (font->unicode)
		{
			return; // TODO: implement sparse unicode
		}
		else
		{
			return;
		}

		#if DEBUG_DRAW_FONT_CHAR
			Serial.printf("  index =  %d\n", fetchbits_unsigned(font->index, bitoffset, font->bits_index));
		#endif

		data = font->data + fetchbits_unsigned(font->index, bitoffset, font->bits_index);
		uint32_t encoding = fetchbits_unsigned(data, 0, 3);
		if (encoding != 0) return;

		uint32_t width = fetchbits_unsigned(data, 3, font->bits_width);
		bitoffset = font->bits_width + 3;
		uint32_t height = fetchbits_unsigned(data, bitoffset, font->bits_height);
		bitoffset += font->bits_height;

		#if DEBUG_DRAW_FONT_CHAR
			Serial.printf("  size =   %d,%d\n", width, height);
		#endif

		int32_t xoffset = fetchbits_signed(data, bitoffset, font->bits_xoffset);
		bitoffset += font->bits_xoffset;
		int32_t yoffset = fetchbits_signed(data, bitoffset, font->bits_yoffset);
		bitoffset += font->bits_yoffset;
		#if DEBUG_DRAW_FONT_CHAR
			Serial.printf("  offset = %d,%d\n", xoffset, yoffset);
		#endif

		uint32_t delta = fetchbits_unsigned(data, bitoffset, font->bits_delta);
		bitoffset += font->bits_delta;

		#if DEBUG_DRAW_FONT_CHAR
			Serial.printf("  delta =  %d\n", delta);
			// Serial.printf("  cursor = %d,%d\n", cursor_x, cursor_y);
		#endif

		// horizontally, we draw every pixel, or none at all

		if (text_x < 0) text_x = 0;
		int32_t origin_x = text_x + xoffset;
		if (origin_x < 0)
		{
			text_x -= xoffset;
			origin_x = 0;
		}
		if (origin_x + (int)width > Get_Width())
		{
			// if (!wrap) return;  prh - wrap

			origin_x = 0;
			if (xoffset >= 0) {
				text_x = 0;
			} else {
				text_x = -xoffset;
			}
			text_y += font->line_space;
		}
		if (text_y >= Get_Height()) return;
		text_x += delta;

		// vertically, the top and/or bottom can be clipped
		int32_t origin_y = text_y + font->cap_height - height - yoffset;
		#if DEBUG_DRAW_FONT_CHAR
			Serial.printf("  origin = %d,%d\n", origin_x, origin_y);
		#endif

		// TODO: compute top skip and number of lines
		int32_t linecount = height;
		//uint32_t loopcount = 0;
		uint32_t y = origin_y;
		while (linecount)
		{
			#if DEBUG_DRAW_FONT_CHAR
				Serial.printf("    linecount = %d\n", linecount);
			#endif

			uint32_t b = fetchbit(data, bitoffset++);
			if (b == 0)
			{
				#if DEBUG_DRAW_FONT_CHAR
					Serial.println("    single line");
				#endif

				uint32_t x = 0;
				do
				{
					uint32_t xsize = width - x;
					if (xsize > 32) xsize = 32;
					uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
					drawFontBits(bits, xsize, origin_x + x, y, 1);
					bitoffset += xsize;
					x += xsize;
				} while (x < width);
				y++;
				linecount--;
			}
			else
			{
				uint32_t n = fetchbits_unsigned(data, bitoffset, 3) + 2;
				bitoffset += 3;
				uint32_t x = 0;
				do
				{
					uint32_t xsize = width - x;
					if (xsize > 32) xsize = 32;
					#if DEBUG_DRAW_FONT_CHAR
						Serial.printf("    multi line %d\n", n);
					#endif
					uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
					drawFontBits(bits, xsize, origin_x + x, y, n);
					bitoffset += xsize;
					x += xsize;
				} while (x < width);
				y += n;
				linecount -= n;
			}
			//if (++loopcount > 100) {
				//Serial.println("     abort draw loop");
				//break;
			//}
		}
	}



	void LCDWIKI_GUI::drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat)
	{
		#if 1
			// TODO: replace this *slow* code with something fast...
			//Serial.printf("      %d bits at %d,%d: %X\n", numbits, x, y, bits);
			if (bits == 0) return;
			do {
				uint32_t x1 = x;
				uint32_t n = numbits;
				do
				{
					n--;
					if (bits & (1 << n))
					{
						Draw_Pixe(x1, y, text_color);
						#if DEBUG_DRAW_FONT_CHAR > 1
							Serial.printf("        pixel at %d,%d\n", x1, y);
						#endif
					}
					x1++;
				} while (n > 0);
				y++;
				repeat--;
			} while (repeat);
		#endif



		#if 0
			if (bits == 0) return;
			beginSPITransaction();
			int w = 0;
			do {
				uint32_t x1 = x;
				uint32_t n = numbits;

				writecommand_cont(ILI9341_PASET); // Row addr set
				writedata16_cont(y);   // YSTART
				writedata16_cont(y);   // YEND

				do {
					n--;
					if (bits & (1 << n)) {
						w++;
					}
					else if (w > 0) {
						// "drawFastHLine(x1 - w, y, w, textcolor)"
						writecommand_cont(ILI9341_CASET); // Column addr set
						writedata16_cont(x1 - w);   // XSTART
						writedata16_cont(x1);   // XEND
						writecommand_cont(ILI9341_RAMWR);
						while (w-- > 1) { // draw line
							writedata16_cont(textcolor);
						}
						writedata16_last(textcolor);
					}

					x1++;
				} while (n > 0);

				if (w > 0) {
						// "drawFastHLine(x1 - w, y, w, textcolor)"
						writecommand_cont(ILI9341_CASET); // Column addr set
						writedata16_cont(x1 - w);   // XSTART
						writedata16_cont(x1);   // XEND
						writecommand_cont(ILI9341_RAMWR);
						while (w-- > 1) { //draw line
							writedata16_cont(textcolor);
						}
						writedata16_last(textcolor);
				}

				y++;
				repeat--;
			} while (repeat);
			endSPITransaction();
		#endif
	}

#endif	// WITH_ILI9431_FONTS



//------------------------------------------
// text extents
//-----------------------------------------


int LCDWIKI_GUI::getFontHeight()
{
	#if WITH_ILI9431_FONTS
		if (font)
			return font->line_space;
	#endif

	return text_size * 8;
}


int LCDWIKI_GUI::getCharWidth(unsigned int c)
{
	#if WITH_ILI9431_FONTS
		if (font)
		{
			uint32_t bitoffset;
			if (c >= font->index1_first && c <= font->index1_last)
			{
				bitoffset = c - font->index1_first;
				bitoffset *= font->bits_index;
			}
			else if (c >= font->index2_first && c <= font->index2_last)
			{
				bitoffset = c - font->index2_first + font->index1_last - font->index1_first + 1;
				bitoffset *= font->bits_index;
			}
			else
			{
				warning(0,"WARNING: chr(%d)=%c cannot be mapped",c,c);
				return 0;
			}

			const uint8_t *data = font->data + fetchbits_unsigned(font->index, bitoffset, font->bits_index);
			uint32_t encoding = fetchbits_unsigned(data, 0, 3);
			if (encoding != 0)
			{
				warning(0,"WARNING: chr(%d)=%c bad encoding(%d)",c,c,encoding);
				return 0;
			}

			bitoffset = font->bits_width + 3;
			bitoffset += font->bits_height;
			bitoffset += font->bits_xoffset;
			bitoffset += font->bits_yoffset;

			uint32_t delta = fetchbits_unsigned(data, bitoffset, font->bits_delta);
			return (int) delta;
		}
	#endif
	return text_size * 6;
}




int LCDWIKI_GUI::getTextExtent(const char *text)
{
	int len = strlen(text);

	#if WITH_ILI9431_FONTS
		if (font)
		{
			int strlen = 0;
			for (int i=0; i<len; i++)
				strlen += getCharWidth(text[i]);
			return strlen;
		}
	#endif

	return len * text_size * 6;
}



//-----------------------------------------
// printf_justified()
//-----------------------------------------
// prints multi-line strings justified within the bounding box
// without clipping.  clears the entire bounding box if use_bc.


#define MAX_PRINTF_STRING  1024

void LCDWIKI_GUI::printf_justified(
	int x,
	int y,
	int w,
	int h,
	int just,
	uint16_t fc,
	uint16_t bc,
	bool use_bc,
	const char *format,
	...)
{
	va_list args;
	va_start(args, format);
	printfv_justified(x,y,w,h,just,fc,bc,use_bc,format,args);
}


void LCDWIKI_GUI::printfv_justified(
	int x,
	int y,
	int w,
	int h,
	int just,
	uint16_t fc,
	uint16_t bc,
	bool use_bc,
	const char *format,
	va_list args)
{
	char display_buffer[MAX_PRINTF_STRING+1];
	if (strlen(format) >= MAX_PRINTF_STRING/2)
	{
		my_error("error - MAX_PRINTF_STRING overflow",0);
		return;
	}
	vsprintf(display_buffer,format,args);
	print_justified(x,y,w,h,just,fc,bc,use_bc,display_buffer);
}



#define MAX_PRINT_LEN  255
char print_j_buf[MAX_PRINT_LEN+1];



void LCDWIKI_GUI::print_justified(
	int x,
	int start_y,
	int w,
	int h,
	int just,
	uint16_t fc,
	uint16_t bc,
	bool use_bc,
	const char *text)
{
	#if DEBUG_DRAW_FONT_CHAR
		Serial.printf("print_justified(%d,%d,%d,%d,  %d, 0x%04x, 0x%04x, %d, \"%s\")\n", x, start_y,w,h,just,fc,bc,use_bc,text);
	#endif

	Set_Text_colour(fc);
	if (use_bc)
		Fill_Rect(x,start_y,w,h,bc);

	const char *start = text;
	int y = start_y + 1;
	int yoffset = getFontHeight();

	while (*start) // && y+yoffset-1<start_y+h-1)
	{
		int len = 0;
		int pixel_len = 0;

		while (*start)
		{
			char c = *start;
			if (c == 13)
			{
				start++;
				break;
			}
			else if (len>=MAX_PRINT_LEN)
			{
				break;
			}
			else
			{
				int pix = getCharWidth(c);
				if (pixel_len + pix > w)
				{
					break;
				}

				start++;
				pixel_len += pix;
				print_j_buf[len++] = c;
			}
		}	// while 1

		print_j_buf[len++] = 0;

		int use_x = x;
		if (just != LCD_JUST_LEFT)
		{
			int xoffset = (w - pixel_len);
			if (xoffset < 0) xoffset = 0;
			if (just == LCD_JUST_CENTER)
				xoffset /= 2;
			use_x += xoffset;
		}

		Print_String(print_j_buf,use_x,y);
		y += yoffset;
	}
}
