/*
This is the core GUI library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

MIT License
*/

#include "LCDWIKI_font.c"
#include "LCDWIKI_GUI.h"

#ifndef WITH_ILI9431_FONTS
	// prh - already defined in Paul's stuff
	#define swap(a, b) { int16_t t = a; a = b; b = t; }
#endif

//Constructor to set text color 
LCDWIKI_GUI::LCDWIKI_GUI(void)
{
	text_bgcolor = 0xF800; //default red
	text_color = 0x07E0; //default green
	draw_color = 0xF800; //default red
	text_size = 1;
	text_mode = 0;
	
	#if WITH_ILI9431_FONTS
		font = 0;
	#endif	
	
}

//set 16bits draw color
void LCDWIKI_GUI::Set_Draw_color(uint16_t color)
{
	draw_color = color;
}

//set 8bits r,g,b color
void LCDWIKI_GUI::Set_Draw_color(uint8_t r, uint8_t g, uint8_t b)
{
	draw_color = Color_To_565(r, g, b);
}

//get draw color
uint16_t LCDWIKI_GUI::Get_Draw_color(void) const
{
	return draw_color;
}

//draw a pixel point
void LCDWIKI_GUI::Draw_Pixel(int16_t x, int16_t y)
{
	Draw_Pixe(x, y, draw_color);
}

//read color data for point(x,y)
uint16_t LCDWIKI_GUI::Read_Pixel(int16_t x, int16_t y)
{
	uint16_t colour;
	Read_GRAM(x, y, &colour, 1, 1);
	return colour;
}

//fill a rectangle
void LCDWIKI_GUI::Fill_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	int w = x2 - x1 + 1, h = y2 - y1 + 1;
   	if (w < 0) 
	{ 
		x1 = x2; 
		w = -w; 
	}
	if (h < 0) 
	{ 
		y1 = y2; 
		h = -h; 
	}
	Fill_Rect(x1, y1, w, h, draw_color);
}

//draw a vertical line 
void LCDWIKI_GUI::Draw_Fast_VLine(int16_t x, int16_t y, int16_t h)
{
	Fill_Rect(x, y, 1, h, draw_color);
}

//draw a horizontal line
void LCDWIKI_GUI::Draw_Fast_HLine(int16_t x, int16_t y, int16_t w)
{
	Fill_Rect(x, y, w, 1, draw_color);
}

//Fill the full screen with color
void LCDWIKI_GUI::Fill_Screen(uint16_t color)
{
	Fill_Rect(0, 0, Get_Width(), Get_Height(), color);
}

//Fill the full screen with r,g,b
void LCDWIKI_GUI::Fill_Screen(uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t color = Color_To_565(r, g, b);
	Fill_Rect(0, 0, Get_Width(), Get_Height(), color);
}

//draw an arbitrary line from (x1,y1) to (x2,y2)
void LCDWIKI_GUI::Draw_Line(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	int16_t steep = abs(y2 - y1) > abs(x2 - x1);
  	if (steep) 
	{
    	swap(x1, y1);
    	swap(x2, y2);
	}
	if (x1 > x2) 
	{
    	swap(x1, x2);
    	swap(y1, y2);
  	}
	
  	int16_t dx, dy;
  	dx = x2 - x1;
  	dy = abs(y2 - y1);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y1 < y2) 
	{
    	ystep = 1;
  	} 
	else 
	{
    	ystep = -1;
	}

	for (; x1<=x2; x1++) 
	{
    	if (steep) 
		{
      		Draw_Pixel(y1, x1);
    	} 
		else 
		{
      		Draw_Pixel(x1, y1);
    	}
    	err -= dy;
    	if (err < 0) 
		{
			y1 += ystep;
			err += dx;
    	}
  	}
}

//draw a rectangle
void LCDWIKI_GUI::Draw_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{ 
	int16_t w = x2 - x1 + 1, h = y2 - y1 + 1;
	if (w < 0) 
	{ 
		x1 = x2; 
		w = -w; 
	}
	if (h < 0) 
	{ 
		y1 = y2; 
		h = -h; 
	}
	Draw_Fast_HLine(x1, y1, w);
  	Draw_Fast_HLine(x1, y2, w);
	Draw_Fast_VLine(x1, y1, h);
	Draw_Fast_VLine(x2, y1, h);
}

//draw a round rectangle
void LCDWIKI_GUI::Draw_Round_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t radius)
{
	int w = x2 - x1 + 1, h = y2 - y1 + 1;
	if (w < 0) 
	{ 
		x1 = x2; 
		w = -w; 
	}
 	if (h < 0) 
	{ 
		y1 = y2; 
		h = -h; 
	}
	Draw_Fast_HLine(x1+radius, y1, w-2*radius); 
	Draw_Fast_HLine(x1+radius, y1+h-1, w-2*radius); 
	Draw_Fast_VLine(x1, y1+radius, h-2*radius); 
  	Draw_Fast_VLine(x1+w-1, y1+radius, h-2*radius);
	Draw_Circle_Helper(x1+radius, y1+radius, radius, 1);
	Draw_Circle_Helper(x1+w-radius-1, y1+radius, radius, 2);
	Draw_Circle_Helper(x1+w-radius-1, y1+h-radius-1, radius, 4);
	Draw_Circle_Helper(x1+radius, y1+h-radius-1, radius, 8);
}

//fill a round rectangle
void LCDWIKI_GUI::Fill_Round_Rectangle(int16_t x1, int16_t y1, int16_t x2,int16_t y2, int16_t radius)
{
	int w = x2 - x1 + 1, h = y2 - y1 + 1;
	if (w < 0) 
	{ 
		x1 = x2; 
		w = -w; 
	}
	if (h < 0) 
	{ 
		y1 = y2; 
		h = -h; 
	}
	Fill_Rect(x1+radius, y1, w-2*radius, h, draw_color);
	Fill_Circle_Helper(x1+w-radius-1, y1+radius, radius, 1, h-2*radius-1);
	Fill_Circle_Helper(x1+radius, y1+radius, radius, 2, h-2*radius-1);	
}

//draw a circle
void LCDWIKI_GUI::Draw_Circle(int16_t x, int16_t y, int16_t radius)
{
	int16_t f = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t x1= 0;
	int16_t y1= radius;

	Draw_Pixel(x, y+radius);
 	Draw_Pixel(x, y-radius);
	Draw_Pixel(x+radius, y);
	Draw_Pixel(x-radius, y);

	while (x1<y1) 
	{
    	if (f >= 0) 
		{
      		y1--;
      		ddF_y += 2;
      		f += ddF_y;
    	}
    	x1++;
    	ddF_x += 2;
    	f += ddF_x;
  
		Draw_Pixel(x + x1, y + y1);
    	Draw_Pixel(x - x1, y + y1);
		Draw_Pixel(x + x1, y - y1);
		Draw_Pixel(x - x1, y - y1);
		Draw_Pixel(x + y1, y + x1);
		Draw_Pixel(x - y1, y + x1);
		Draw_Pixel(x + y1, y - x1);
		Draw_Pixel(x - y1, y - x1);
 	}
}

//draw a circular bead
void LCDWIKI_GUI::Draw_Circle_Helper(int16_t x0, int16_t y0, int16_t radius, uint8_t cornername)
{
	int16_t f     = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t x     = 0;
	int16_t y     = radius;
	while (x<y) 
	{
    	if (f >= 0) 
		{
      		y--;
      		ddF_y += 2;
      		f += ddF_y;
    	}
	    x++;
	    ddF_x += 2;
	    f += ddF_x;
	    if (cornername & 0x4) 
		{
			Draw_Pixel(x0 + x, y0 + y);
			Draw_Pixel(x0 + y, y0 + x);
	    } 
	    if (cornername & 0x2) 
		{
			Draw_Pixel(x0 + x, y0 - y);
			Draw_Pixel(x0 + y, y0 - x);
	    }
	    if (cornername & 0x8) 
		{
			Draw_Pixel(x0 - y, y0 + x);
			Draw_Pixel(x0 - x, y0 + y);
	    }
	    if (cornername & 0x1)
		{
			Draw_Pixel(x0 - y, y0 - x);
	 		Draw_Pixel(x0 - x, y0 - y);
	    }
  	}
}

//fill a circle
void LCDWIKI_GUI::Fill_Circle(int16_t x, int16_t y, int16_t radius)
{
	Draw_Fast_VLine(x, y-radius, 2*radius+1);
	Fill_Circle_Helper(x, y, radius, 3, 0);
}

//fill a semi-circle
void LCDWIKI_GUI::Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta)
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) 
	{
    	if (f >= 0) 
		{
      		y--;
      		ddF_y += 2;
      		f += ddF_y;
    	}
    	x++;
    	ddF_x += 2;
    	f += ddF_x;

    	if (cornername & 0x1) 
		{
      		Draw_Fast_VLine(x0+x, y0-y, 2*y+1+delta);
      		Draw_Fast_VLine(x0+y, y0-x, 2*x+1+delta);
    	}
    	if (cornername & 0x2) 
		{
      		Draw_Fast_VLine(x0-x, y0-y, 2*y+1+delta);
      		Draw_Fast_VLine(x0-y, y0-x, 2*x+1+delta);
    	}
  	}
}

//draw a triangle
void LCDWIKI_GUI::Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2)
{
	Draw_Line(x0, y0, x1, y1);
	Draw_Line(x1, y1, x2, y2);
  	Draw_Line(x2, y2, x0, y0);
}

//fill a triangle
void LCDWIKI_GUI::Fill_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2)
{
	int16_t a, b, y, last;
  	if (y0 > y1) 
	{
    	swap(y0, y1); 
		swap(x0, x1);
  	}
  	if (y1 > y2) 
	{
    	swap(y2, y1); 
		swap(x2, x1);
  	}
  	if (y0 > y1) 
	{
    	swap(y0, y1); 
		swap(x0, x1);
  	}

	if(y0 == y2) 
	{ 
    	a = b = x0;
    	if(x1 < a)
    	{
			a = x1;
    	}
    	else if(x1 > b)
    	{
			b = x1;
    	}
    	if(x2 < a)
    	{
			a = x2;
    	}
    	else if(x2 > b)
    	{
			b = x2;
    	}
    	Draw_Fast_HLine(a, y0, b-a+1);
    	return;
	}
  	int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0, dx12 = x2 - x1, dy12 = y2 - y1;
	int32_t sa = 0, sb = 0;
	if(y1 == y2)
	{
		last = y1; 
	}
  	else
  	{
		last = y1-1; 
  	}

  	for(y=y0; y<=last; y++) 
	{
    	a   = x0 + sa / dy01;
    	b   = x0 + sb / dy02;
    	sa += dx01;
    	sb += dx02;
    	if(a > b)
    	{
			swap(a,b);
    	}
    	Draw_Fast_HLine(a, y, b-a+1);
	}
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) 
	{
    	a   = x1 + sa / dy12;
    	b   = x0 + sb / dy02;
    	sa += dx12;
    	sb += dx02;
    	if(a > b)
    	{
			swap(a,b);
    	}
		Draw_Fast_HLine(a, y, b-a+1);
	}
}

//uint8_t LCDKIWI_GUI::Get_Screen_Rotation(void) const
//{
//	return LCDKIWI_KBV::Get_Rotation();
//}

//void LCDKIWI_GUI::Push_Colors(uint16_t * block, int16_t n, boolean first, uint8_t flags)
//{
//	LCDKIWI_KBV::Push_Any_Color(block, n, first, flags);
//}

//void LCDKIWI_GUI::Push_Colors(uint8_t * block, int16_t n, boolean first, uint8_t flags)
//{
//	LCDKIWI_KBV::Push_Any_Color(block, n, first, flags);
//}

//draw a bit map
void LCDWIKI_GUI::Draw_Bit_Map(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale)
{
	int16_t color;
	Set_Addr_Window(x, y, x + sx*scale - 1, y + sy*scale - 1); 
	if(1 == scale)
	{

		Push_Any_Color((uint16_t *)data, sx * sy, 1, 0);
	}
	else 
	{
		for (int16_t row = 0; row < sy; row++) 
		{
			for (int16_t col = 0; col < sx; col++) 
			{
				color = *(data + (row*sx + col)*1);//pgm_read_word(data + (row*sx + col)*1);
				Fill_Rect(x+col*scale, y+row*scale, scale, scale, color);
			}
		}
	}
}

//set text coordinate
void LCDWIKI_GUI::Set_Text_Cursor(int16_t x, int16_t y)
{
	text_x = x;
	text_y = y;
}

//get text x coordinate
int16_t LCDWIKI_GUI::Get_Text_X_Cursor(void) const
{
	return text_x;
}

//get text y coordinate
int16_t LCDWIKI_GUI::Get_Text_Y_Cursor(void) const
{
	return text_y;
}

//set text colour with 16bit color
void LCDWIKI_GUI::Set_Text_colour(uint16_t color)
{
	text_color = color;
}

//set text colour with 8bits r,g,b
void LCDWIKI_GUI::Set_Text_colour(uint8_t r, uint8_t g, uint8_t b)
{
	text_color = Color_To_565(r, g, b);
}

//get text colour
uint16_t LCDWIKI_GUI::Get_Text_colour(void) const
{
	return text_color;
}

//set text background colour with 16bits color
void LCDWIKI_GUI::Set_Text_Back_colour(uint16_t color)
{
	text_bgcolor = color;	
}

//set text background colour with 8bits r,g,b
void LCDWIKI_GUI::Set_Text_Back_colour(uint8_t r, uint8_t g, uint8_t b)
{
	text_bgcolor = Color_To_565(r, g, b);
}

//get text background colour
uint16_t LCDWIKI_GUI::Get_Text_Back_colour(void) const
{
	return text_bgcolor;
}

//set text size
void LCDWIKI_GUI::Set_Text_Size(uint8_t s)
{
	text_size = s;
}

//get text size
uint8_t LCDWIKI_GUI::Get_Text_Size(void) const
{
	return text_size;
}

//set text mode
void LCDWIKI_GUI::Set_Text_Mode(boolean mode)
{
	text_mode = mode;
}

//get text mode
boolean LCDWIKI_GUI::Get_Text_Mode(void) const
{
	return text_mode;
}



// draw a char
// - assumes a 6x8 full box font
// - font's are stored as bytes containing columns
// - every 6 bytes is another character
// - size is a simple integer multiplier
// - background only drawn IFF bg is different than color
// - "mode" forces background drawing off


void LCDWIKI_GUI::Draw_Char(
	int16_t x,
	int16_t y,
	uint8_t c,
	uint16_t color,
	uint16_t bg,
	uint8_t size,
	boolean mode)
{
	if((x >= Get_Width()) || (y >= Get_Height()) || ((x + 6 * size - 1) < 0) || ((y + 8 * size - 1) < 0))
	{
    	return;
	}		
  	if(c >= 176) // prh huh?
  	{
		c++; 
  	}
	
	for (int8_t i=0; i<6; i++) 		// for each column
	{
    	uint8_t line;
    	if (i == 5)
    	{
      		line = 0x0;
    	}
    	else
    	{
      		line = pgm_read_byte(lcd_font+(c*5)+i);
    	}
		
    	for (int8_t j = 0; j<8; j++) 	// for each row
		{
      		if (line & 0x1) 
			{
        		if (size == 1)
        		{
        			Draw_Pixe(x+i, y+j, color);
        		}
        		else 
				{  
					Fill_Rect(x+(i*size), y+(j*size), size, size, color);
        		}
        	} 
			else if (bg != color) 				
			{
				if (!mode)
				{
	        		if (size == 1) 
	        		{
	        			Draw_Pixe(x+i, y+j, bg);
	        		}
	        		else 
					{  
						Fill_Rect(x+i*size, y+j*size, size, size, bg);
					}
				}
			}
      		line >>= 1;
    	}
    }
}

//print string
size_t LCDWIKI_GUI::Print(uint8_t *st, int16_t x, int16_t y)
{
	int16_t pos;
	uint16_t len;
	const char * p = (const char *)st;
	size_t n = 0;
	if (x == CENTER || x == RIGHT) 
	{
		len = strlen((const char *)st) * 6 * text_size;		
		pos = (Get_Display_Width() - len); 
		if (x == CENTER)
		{
			x = pos/2;
		}
		else
		{
			x = pos - 1;
		}
	}
    Set_Text_Cursor(x, y);
	while(1)
	{
		unsigned char ch = *(p++);//pgm_read_byte(p++);
		if(ch == 0)
		{
			break;
		}
		if(write(ch))
		{
			n++;
		}
		else
		{
			break;
		}
	}	
	return n;
}

//print string
void LCDWIKI_GUI::Print_String(const uint8_t *st, int16_t x, int16_t y)
{
	Print((uint8_t *)st, x, y);
}

//print string
void LCDWIKI_GUI::Print_String(uint8_t *st, int16_t x, int16_t y)
{
	Print(st, x, y);
}

//print string
void LCDWIKI_GUI::Print_String(String st, int16_t x, int16_t y)
{
	Print((uint8_t *)(st.c_str()), x, y);
}

//print int number
void LCDWIKI_GUI::Print_Number_Int(long num, int16_t x, int16_t y, int16_t length, uint8_t filler, int16_t system)
{
	uint8_t st[27] = {0};
	uint8_t *p = st+26;
	boolean flag = false;
	int16_t len = 0,nlen = 0,left_len = 0,i = 0;
	*p = '\0';
	if(0 == num)
	{
		*(--p) = '0';
		len = 1;
	}
	else
	{
		if(num < 0)
		{
			num = -num;
			flag = true;
		}		
	}
	while((num > 0) && (len < 10))
	{
		if(num%system > 9)
		{
			*(--p) = 'A' + (num%system-10);
		}
		else
		{
			*(--p) = '0' + num%system;
		}
		num = num/system;
		len++;
	}
	if(flag)
	{
		*(--p) = '-';
	}
	if(length > (len + flag + 1))
	{
		if(length > (int16_t) sizeof(st))
		{
			nlen = sizeof(st) - len - flag - 1;
		}
		else
		{
			nlen = length - len - flag - 1;
		}
		for(i = 0;i< nlen;i++)
		{
			*(--p) = filler;
		}
		left_len = sizeof(st) - nlen - len - flag - 1;
	}	
	else
	{
		left_len = sizeof(st) - len - flag - 1;
	}
	for(i = 0; i < (int16_t)(sizeof(st) - left_len);i++)
	{
		st[i] = st[left_len + i];
	}
	st[i] = '\0';
	Print(st, x, y);
}

//print float number
void LCDWIKI_GUI::Print_Number_Float(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler)
{
	uint8_t st[27] = {0};
	uint8_t * p = st;
	boolean flag = false;
	int16_t i = 0;
	if(dec<1)
	{
		dec=1;
	}
	else if(dec>5)
	{
		dec=5;
	}
	if(num<0)
	{
		flag = true;
	}
	dtostrf(num, length, dec, (char *)st);
	if(divider != '.')
	{
		while(i < (int16_t)sizeof(st))
		{
			if('.' == *(p+i))
			{
				*(p+i) = divider;
			}
			i++;
		}	
	}
	if(filler != ' ')
	{
		if(flag)
		{
			*p = '-';
			i = 1;
			while(i < (int16_t)sizeof(st))
			{
				if((*(p+i) == ' ') || (*(p+i) == '-'))
				{
					*(p+i) = filler;
				}
				i++;
			}
		}
		else
		{
			i = 0;
			while(i < (int16_t) sizeof(st))
			{
				if(' ' == *(p+i))
				{
					*(p+i) = filler;
				}
			}
		}
	}
	Print(st, x, y);
}




//write a char
size_t LCDWIKI_GUI::write(uint8_t c) 
{
	#if WITH_ILI9431_FONTS
		if (font)
		{
			if (c == '\n')
			{
				text_y += font->line_space; // Fix linefeed. Added by T.T., SoftEgg
				text_x = 0;
			}
			else if(c == '\r')
			{
			}
			else
			{
				drawFontChar(c);
			}
			return 1;
		}
	#endif
	
	if (c == '\n') 
	{
    	text_y += text_size*8;
    	text_x  = 0;
 	} 
	else if(c == '\r')
	{
	}
	else 
	{
    	Draw_Char(text_x, text_y, c, text_color, text_bgcolor, text_size,text_mode);
    	text_x += text_size*6;		
    }	
  	return 1;	
}



#if WITH_ILI9431_FONTS
	#include <myDebug.h>

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


	void LCDWIKI_GUI::drawFontChar(unsigned int c)
	{
		uint32_t bitoffset;
		const uint8_t *data;
	
		//Serial.printf("drawFontChar %d\n", c);
	
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
		
		//Serial.printf("  index =  %d\n", fetchbits_unsigned(font->index, bitoffset, font->bits_index));
		
		data = font->data + fetchbits_unsigned(font->index, bitoffset, font->bits_index);
		uint32_t encoding = fetchbits_unsigned(data, 0, 3);
		if (encoding != 0) return;
		
		uint32_t width = fetchbits_unsigned(data, 3, font->bits_width);
		bitoffset = font->bits_width + 3;
		uint32_t height = fetchbits_unsigned(data, bitoffset, font->bits_height);
		bitoffset += font->bits_height;
		//Serial.printf("  size =   %d,%d\n", width, height);
	
		int32_t xoffset = fetchbits_signed(data, bitoffset, font->bits_xoffset);
		bitoffset += font->bits_xoffset;
		int32_t yoffset = fetchbits_signed(data, bitoffset, font->bits_yoffset);
		bitoffset += font->bits_yoffset;
		//Serial.printf("  offset = %d,%d\n", xoffset, yoffset);
	
		uint32_t delta = fetchbits_unsigned(data, bitoffset, font->bits_delta);
		bitoffset += font->bits_delta;
		//Serial.printf("  delta =  %d\n", delta);
	
		//Serial.printf("  cursor = %d,%d\n", cursor_x, cursor_y);
	
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
		//Serial.printf("  origin = %d,%d\n", origin_x, origin_y);
	
		// TODO: compute top skip and number of lines
		int32_t linecount = height;
		//uint32_t loopcount = 0;
		uint32_t y = origin_y;
		while (linecount)
		{
			//Serial.printf("    linecount = %d\n", linecount);
			uint32_t b = fetchbit(data, bitoffset++);
			if (b == 0)
			{
				//Serial.println("    single line");
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
					//Serial.printf("    multi line %d\n", n);
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
						//Serial.printf("        pixel at %d,%d\n", x1, y);
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


// #define LCD_JUST_LEFT    0
// #define LCD_JUST_CENTER  1 
// #define LCD_JUST_RIGHT   2

int LCDWIKI_GUI::getFontHeight()
{
	#if WITH_ILI9431_FONTS
		if (font)
			return font->line_space;
	#endif
	
	return text_size * 8;
}

int LCDWIKI_GUI::getTextExtent(const char *text)
{
	int len = strlen(text);

	#if WITH_ILI9431_FONTS
		if (font)
		{
			int strlen = 0;
			for (int i=0; i<len; i++)
			{
				unsigned int c = text[i];
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
					continue;
				}

				const uint8_t *data = font->data + fetchbits_unsigned(font->index, bitoffset, font->bits_index);
				uint32_t encoding = fetchbits_unsigned(data, 0, 3);
				if (encoding != 0)
				{
					warning(0,"WARNING: chr(%d)=%c bad encoding(%d)",c,c,encoding);
					continue;
				}

				bitoffset = font->bits_width + 3;
				bitoffset += font->bits_height;
				bitoffset += font->bits_xoffset;
				bitoffset += font->bits_yoffset;
				
				uint32_t delta = fetchbits_unsigned(data, bitoffset, font->bits_delta);
				strlen += (int) delta;				
			}
			return strlen;
		}
	#endif
	
	return len * text_size * 6;
}


#define MAX_PRINTF_STRING  255


void LCDWIKI_GUI::printf_justified(
	int x,
	int y,
	int w,
	int h,
	int just,
	uint16_t fc,			// calls Set_Text_colour
	uint16_t bc,			// draws the rectangle for you if font*
	const char *format,
	...)
{
	unsigned char display_buffer[MAX_PRINTF_STRING+1];
	
	va_list var;
	va_start(var, format);
        
	// allows for 2 times expansion, but no telling it may just crash
	
	if (strlen(format) >= MAX_PRINTF_STRING/2)
	{
		my_error("error - MAX_PRINTF_STRING overflow",0);
		return;
	}
    
	vsprintf((char *)display_buffer,format,var);

	#if WITH_ILI9431_FONTS
		// draw the background rectangle
		// some characters (S) are drawn one pixel above the 1 position
		if (font)
			Fill_Rect(x,y-1,w,h+1,bc);
		else
	#endif	
		Set_Text_Back_colour(bc);
		
	if (just != LCD_JUST_LEFT)
	{
		int width = getTextExtent((char *)display_buffer);
		
		int xoffset = (w - width);
		if (xoffset < 0) xoffset = 0;
		if (just == LCD_JUST_CENTER)
			xoffset /= 2;
			
		x += xoffset;
	}
	
	Set_Text_colour(fc);
	Print_String(display_buffer,x,y);
}



void LCDWIKI_GUI::drawBorderedRectangle(int x, int y, int w, int h, int b, int color)
{
	Fill_Rect(x,		y,		b,		h,	color);
	Fill_Rect(x+w-b,	y,		b,		h,	color);
	Fill_Rect(x,		y,		w,		b,	color);
	Fill_Rect(x,		y+h-b,	w,		b,	color);
}






//get lcd width
int16_t LCDWIKI_GUI::Get_Display_Width(void) const
{
	return Get_Width();
}

//get lcd height 
int16_t LCDWIKI_GUI::Get_Display_Height(void) const
{
	return Get_Height();
}

//uint16_t LCDKIWI_GUI::Change_To_565(uint8_t r, uint8_t g, uint8_t b)
//{
//	return LCDKIWI_KBV::Color_To_565(r, g, b);
//}

//uint16_t LCDKIWI_GUI::Read_Dev_ID(void)
//{
//	return LCDKIWI_KBV::Read_ID();

//}

//void LCDKIWI_GUI::Dev_Invert_Display(boolean i)
//{
//	LCDKIWI_KBV::Invert_Display(i);
//}

//void LCDKIWI_GUI::Dev_Vert_Scroll(int16_t top, int16_t scrollines, int16_t offset)
//{
//	LCDKIWI_KBV::Vert_Scroll(top, scrollines, offset);
//}
