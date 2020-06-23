#ifndef _LCDWIKI_GUI_H_
#define _LCDWIKI_GUI_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(__MK66FX1M0__)	// prh - eliminate teensy warnings
 #include <pgmspace.h>
#else
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif


// prh addition WITH_ILI9431_FONTS
// added ability to use fonts from Paul's ILI9431_t3 library

#define WITH_ILI9431_FONTS   1

#if WITH_ILI9431_FONTS
	#include <ILI9341_t3.h>
#endif



//#if !defined(AVR)
//#include <avr/dtostrf.h>
//#endif

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

class LCDWIKI_GUI : public Print
{
	public:
	LCDWIKI_GUI(void); // Constructor

    //These are defined by the subclass:
	virtual uint16_t Color_To_565(uint8_t r, uint8_t g, uint8_t b)=0;
	virtual void Draw_Pixe(int16_t x, int16_t y, uint16_t color)=0;
	virtual void Fill_Rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)=0;
	virtual void Set_Addr_Window(int16_t x1, int16_t y1, int16_t x2, int16_t y2)=0;
	virtual void Push_Any_Color(uint16_t * block, int16_t n, bool first, uint8_t flags)=0;
	virtual int16_t Read_GRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h)=0;
	virtual int16_t Get_Height(void) const=0;
  	virtual int16_t Get_Width(void) const=0;

	//These exist only with LCDWIKI_GUI(no subclass overrides)
	void Set_Draw_color(uint16_t color);
	void Set_Draw_color(uint8_t r, uint8_t g, uint8_t b);
	uint16_t Get_Draw_color(void) const;
	void Draw_Pixel(int16_t x, int16_t y);
	uint16_t Read_Pixel(int16_t x, int16_t y);
	void Draw_Fast_VLine(int16_t x, int16_t y, int16_t h);
	void Draw_Fast_HLine(int16_t x, int16_t y, int16_t w);
	void Fill_Screen(uint16_t color);
	void Fill_Screen(uint8_t r, uint8_t g, uint8_t b);
	void Draw_Line(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	void Draw_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	void Fill_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
	void Draw_Round_Rectangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t radius);
	void Fill_Round_Rectangle(int16_t x1, int16_t y1, int16_t x2,int16_t y2, int16_t radius);
	void Draw_Circle(int16_t x, int16_t y, int16_t radius);
	void Draw_Circle_Helper(int16_t x0, int16_t y0, int16_t radius, uint8_t cornername);
	void Fill_Circle(int16_t x, int16_t y, int16_t radius);
	void Fill_Circle_Helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta);
	void Draw_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2);
	void Fill_Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2);
	void Draw_Bit_Map(int16_t x, int16_t y, int16_t sx, int16_t sy, const uint16_t *data, int16_t scale);

	void Set_Text_Cursor(int16_t x, int16_t y);
	int16_t Get_Text_X_Cursor(void) const;
	int16_t Get_Text_Y_Cursor(void) const;
		// prh fixed mispelling of "cursor"

	void Set_Text_colour(uint16_t color);
	void Set_Text_colour(uint8_t r, uint8_t g, uint8_t b);
	uint16_t Get_Text_colour(void) const;

	void Set_Text_Back_colour(uint16_t color);
	void Set_Text_Back_colour(uint8_t r, uint8_t g, uint8_t b);
	uint16_t Get_Text_Back_colour(void) const;

	void Set_Text_Size(uint8_t s);
	uint8_t Get_Text_Size(void) const;


	// prh - changes
	// void Set_Text_Mode(boolean mode);
	// boolean Get_Text_Mode(void) const;
	void setTextBackgroundOn(boolean on) 	{ m_use_bc = on; }
	boolean getTextBackgroundOn() 			{ return m_use_bc; }
		// prh - this whole "text_mode" thing is backwards and obtruse
		// it was only used in Draw_Char()
		//   IFF the background color != the font color ??!?!
		//   to "not" draw the background if set.
		//


	size_t Print(uint8_t *st, int16_t x, int16_t y);
	void Print_String(const uint8_t *st, int16_t x, int16_t y);
	void Print_String(uint8_t *st, int16_t x, int16_t y);
	void Print_String(String st, int16_t x, int16_t y);
	void Print_Number_Int(long num, int16_t x, int16_t y, int16_t length, uint8_t filler, int16_t system);
	void Print_Number_Float(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler);

    void Draw_Char(int16_t x, int16_t y, uint8_t c, uint16_t color,uint16_t bg, uint8_t size, boolean mode);

	int16_t Get_Display_Width(void) const;
	int16_t Get_Display_Height(void) const;

	size_t write(uint8_t c);

	// prh additions

	#if WITH_ILI9431_FONTS
		void setFont(const ILI9341_t3_font_t &f) { font = &f; }
		void setDefaultFont(void) { font = NULL; }
	#endif

	int getFontHeight();
	int getTextExtent(const char *text);
	void drawBorder(int x, int y, int w, int h, int b, int color);

	#define LCD_JUST_LEFT    0
	#define LCD_JUST_CENTER  1
	#define LCD_JUST_RIGHT   2

	void printf_justified(
		int x,
		int y,
		int w,
		int h,
		int just,
		uint16_t fc,
		uint16_t bc,
		bool use_bc,
		const char *format,
		...);
	void printfv_justified(
		int x,
		int y,
		int w,
		int h,
		int just,
		uint16_t fc,
		uint16_t bc,
		bool use_bc,
		const char *format,
		va_list args);
	void print_justified(
		int x,
		int y,
		int w,
		int h,
		int just,
		uint16_t fc,
		uint16_t bc,
		bool use_bc,
		char *text);

	// end prh additions

protected:

	int16_t text_x, text_y;
	uint16_t text_color, text_bgcolor,draw_color;
	uint8_t text_size;

	// prh - boolean text_mode became use_bc
	boolean m_use_bc;
		// if set, text_bgcolor is not used when writing characters
		// otherwise, it is.

	#if WITH_ILI9431_FONTS	// prh addition
		const ILI9341_t3_font_t *font;
		void drawFontChar(unsigned int c);
		void drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat);
	#endif
};

#endif
