/*********************************************************************\
*	Author:			Red_Calcifer
*	Projekt:		Display-Symbols-HeaderFile									
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio 4.18 mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	This File contains all Font and Symbol Data
* ------------------------------------------------------------------														
*	Date:			13.07.2011 	
* 	lastChanges:										
\**********************************************************************/


/* ===================================================================*
 * 						Fonts
 * ===================================================================*/

#define Font_6X8_LEN  584
unsigned char Font_6X8[Font_6X8_LEN] =
{
    6,  1, 
    
     //-------------------------------------------------------Positiv----
    0,  0,  0,  0,  0,  0,
    0,  0, 95,  0,  0,  0,
    0,  7,  0,  7,  0,  0,
   20,127, 20,127, 20,  0,
   36, 42,127, 42, 18,  0,
   35, 19,  8,100, 98,  0,
   54, 73, 86, 32, 80,  0,
    0,  8,  7,  3,  0,  0,
    0, 28, 34, 65,  0,  0,
    0, 65, 34, 28,  0,  0,
   42, 28,127, 28, 42,  0,
    8,  8, 62,  8,  8,  0,
    0,128,112, 48,  0,  0,
    8,  8,  8,  8,  8,  0,
    0,  0, 96, 96,  0,  0,
   32, 16,  8,  4,  2,  0,
   62, 81, 73, 69, 62,  0,
    0, 66,127, 64,  0,  0,
   66, 97, 81, 73, 70,  0,
   33, 65, 73, 77, 51,  0,
   24, 20, 18,127, 16,  0,
   39, 69, 69, 69, 57,  0,
   60, 74, 73, 73, 48,  0,
   65, 33, 17,  9,  7,  0,
   54, 73, 73, 73, 54,  0,
    6, 73, 73, 41, 30,  0,
    0,  0, 20,  0,  0,  0,
    0, 64, 52,  0,  0,  0,
    0,  8, 20, 34, 65,  0,
   20, 20, 20, 20, 20,  0,
    0, 65, 34, 20,  8,  0,
    2,  1, 81,  9,  6,  0,
   62, 65, 93, 89, 78,  0,
  124, 18, 17, 18,124,  0,
  127, 73, 73, 73, 54,  0,
   62, 65, 65, 65, 34,  0,
  127, 65, 65, 65, 62,  0,
  127, 73, 73, 73, 65,  0,
  127,  9,  9,  9,  1,  0,
   62, 65, 73, 73,122,  0,
  127,  8,  8,  8,127,  0,
    0, 65,127, 65,  0,  0,
   32, 64, 65, 63,  1,  0,
  127,  8, 20, 34, 65,  0,
  127, 64, 64, 64, 64,  0,
  127,  2, 28,  2,127,  0,
  127,  4,  8, 16,127,  0,
   62, 65, 65, 65, 62,  0,
  127,  9,  9,  9,  6,  0,
   62, 65, 81, 33, 94,  0,
  127,  9, 25, 41, 70,  0,
   38, 73, 73, 73, 50,  0,
    1,  1,127,  1,  1,  0,
   63, 64, 64, 64, 63,  0,
   31, 32, 64, 32, 31,  0,
   63, 64, 56, 64, 63,  0,
   99, 20,  8, 20, 99,  0,
    3,  4,120,  4,  3,  0,
   97, 81, 73, 69, 67,  0,
    0,127, 65, 65, 65,  0,
    2,  4,  8, 16, 32,  0,
    0, 65, 65, 65,127,  0,
    4,  2,  1,  2,  4,  0,
   64, 64, 64, 64, 64,  0,
    0,  3,  7,  8,  0,  0,
   32, 84, 84, 84,120,  0,
  127, 40, 68, 68, 56,  0,
   56, 68, 68, 68, 40,  0,
   56, 68, 68, 40,127,  0,
   56, 84, 84, 84, 24,  0,
    0,  8,126,  9,  2,  0,
   24,164,164,164,124,  0,
  127,  8,  4,  4,120,  0,
    0, 68,125, 64,  0,  0,
   32, 64, 64, 61,  0,  0,
  127, 16, 40, 68,  0,  0,
    0, 65,127, 64,  0,  0,
  124,  4,120,  4,120,  0,
  124,  8,  4,  4,120,  0,
   56, 68, 68, 68, 56,  0,
  252, 24, 36, 36, 24,  0,
   24, 36, 36, 24,252,  0,
  124,  8,  4,  4,  8,  0,
   72, 84, 84, 84, 36,  0,
    4,  4, 63, 68, 36,  0,
   60, 64, 64, 32,124,  0,
   28, 32, 64, 32, 28,  0,
   60, 64, 48, 64, 60,  0,
   68, 40, 16, 40, 68,  0,
   76,144,144,144,124,  0,
   68,100, 84, 76, 68,  0,
    0,  8, 54, 65,  0,  0,
    0,  0,119,  0,  0,  0,
    0, 65, 54,  8,  0,  0,
    2,  1,  2,  4,  2,  0,
    6, 15,  9, 15,  6,  0		//Grad
};

#define Font_6X8_Neg_LEN  578
unsigned char Font_6X8_Neg[Font_6X8_Neg_LEN] =
{
    6,  1, 

	//-------------------------------------------------------Negativ----
  255,255,255,255,255,255,
  255,255,160,255,255,255,
  255,248,255,248,255,255,
  235,128,235,128,235,255,
  219,213,128,213,237,255,
  220,236,247,155,157,255,
  201,182,169,223,175,255,
  255,247,248,252,255,255,
  255,227,221,190,255,255,
  255,190,221,227,255,255,
  213,227,128,227,213,255,
  247,247,193,247,247,255,
  255,127,143,207,255,255,
  247,247,247,247,247,255,
  255,255,159,159,255,255,
  223,239,247,251,253,255,
  193,174,182,186,193,255,
  255,189,128,191,255,255,
  189,158,174,182,185,255,
  222,190,182,178,204,255,
  231,235,237,128,239,255,
  216,186,186,186,198,255,
  195,181,182,182,207,255,
  190,222,238,246,248,255,
  201,182,182,182,201,255,
  249,182,182,214,225,255,
  255,255,235,255,255,255,
  255,191,203,255,255,255,
  255,247,235,221,190,255,
  235,235,235,235,235,255,
  255,190,221,235,247,255,
  253,254,174,246,249,255,
  193,190,162,166,177,255,
  131,237,238,237,131,255,
  128,182,182,182,201,255,
  193,190,190,190,221,255,
  128,190,190,190,193,255,
  128,182,182,182,190,255,
  128,246,246,246,254,255,
  193,190,182,182,133,255,
  128,247,247,247,128,255,
  255,190,128,190,255,255,
  223,191,190,192,254,255,
  128,247,235,221,190,255,
  128,191,191,191,191,255,
  128,253,227,253,128,255,
  128,251,247,239,128,255,
  193,190,190,190,193,255,
  128,246,246,246,249,255,
  193,190,174,222,161,255,
  128,246,230,214,185,255,
  217,182,182,182,205,255,
  254,254,128,254,254,255,
  192,191,191,191,192,255,
  224,223,191,223,224,255,
  192,191,199,191,192,255,
  156,235,247,235,156,255,
  252,251,135,251,252,255,
  158,174,182,186,188,255,
  255,128,190,190,190,255,
  253,251,247,239,223,255,
  255,190,190,190,128,255,
  251,253,254,253,251,255,
  191,191,191,191,191,255,
  255,252,248,247,255,255,
  223,171,171,171,135,255,
  128,215,187,187,199,255,
  199,187,187,187,215,255,
  199,187,187,215,128,255,
  199,171,171,171,231,255,
  255,247,129,246,253,255,
  231, 91, 91, 91,131,255,
  128,247,251,251,135,255,
  255,187,130,191,255,255,
  223,191,191,194,255,255,
  128,239,215,187,255,255,
  255,190,128,191,255,255,
  131,251,135,251,135,255,
  131,247,251,251,135,255,
  199,187,187,187,199,255,
    3,231,219,219,231,255,
  231,219,219,231,  3,255,
  131,247,251,251,247,255,
  183,171,171,171,219,255,
  251,251,192,187,219,255,
  195,191,191,223,131,255,
  227,223,191,223,227,255,
  195,191,207,191,195,255,
  187,215,239,215,187,255,
  179,111,111,111,131,255,
  187,155,171,179,187,255,
  255,247,201,190,255,255,
  255,255,136,255,255,255,
  255,190,201,247,255,255,
  253,254,253,251,253,255
};



#define FontNumbers_4X6_Neg_LEN  63
unsigned char FontNumbers_4X6_Neg[FontNumbers_4X6_Neg_LEN] =
{
    4,  1,  
   32, 46, 32, 63,
   45, 32, 47, 63,
   34, 42, 40, 63,
   42, 42, 32, 63,
   56, 59, 32, 63,
   40, 42, 34, 63,
   32, 42, 34, 63,
   62, 38, 56, 63,
   32, 42, 32, 63,
   40, 42, 32, 63,
   63, 53, 63, 63,			//:
   59, 59, 59, 63,			//-
   63, 32, 63, 63,			//|
   255,255,255,255			//Leerzeichen
};

#define FontNumbers_4X6_LEN  82
unsigned char FontNumbers_4X6[FontNumbers_4X6_LEN] =
{
    4,  1,  
   31, 17, 31,  0,
   18, 31, 16,  0,
   29, 21, 23,  0,
   21, 21, 31,  0,
    7,  4, 31,  0,
   23, 21, 29,  0,
   31, 21, 29,  0,
    1, 25,  7,  0,
   31, 21, 31,  0,
   23, 21, 31,  0,
    0, 10,  0,  0,			//:
    4,  4,  4,  0,			//-
    0, 31,  0,  0,			//|
   28,  8, 28,  0,			//m
   31, 20, 28,  0,			//b
   24, 20, 28,  0,			//a
   0, 28,  4,   0,			//r
   31,  4, 28,  0,			//h
    0,  0,  0,  0,			//Leerzeichen
	9,  4, 18,  0			//% 20
};



#define Font_Numbers_8X16_LEN  322
unsigned char Font_Numbers_8X16[Font_Numbers_8X16_LEN] =
{
    8, 2, 
    
    //-------------------------------------------------------Negativ----
	255,  7,  3,243,115,  3,  7,255,255,224,192,206,207,192,224,255,
	255,255,207,  3,  3,255,255,255,255,207,207,192,192,207,207,255,
	255,199,195,243, 51,  3,199,255,255,207,195,192,204,207,207,255,
	255,243,243, 51,  3,195,243,255,255,227,195,207,204,192,227,255,
	255,255, 63, 15,  3,  3,255,255,255,240,240,243,192,192,243,255,
	255,  3,  3, 51, 51, 51,115,255,255,231,199,207,207,192,224,255,
	255, 15,  7, 99,115,115,255,255,255,224,192,206,206,192,224,255,
	255,243,243,243, 51,  3,195,255,255,255,195,192,252,255,255,255,
	255,199,  3, 51, 51,  3,199,255,255,224,192,207,207,192,224,255,
	255,135,  3, 51, 51,  3,  7,255,255,255,207,207,199,224,240,255,
    
    //-------------------------------------------------------Positiv----
    0,248,252, 12,140,252,248,  0,  0, 31, 63, 49, 48, 63, 31,  0,
    0,  0, 48,252,252,  0,  0,  0,  0, 48, 48, 63, 63, 48, 48,  0,
    0, 56, 60, 12,204,252, 56,  0,  0, 48, 60, 63, 51, 48, 48,  0,
    0, 12, 12,204,252, 60, 12,  0,  0, 28, 60, 48, 51, 63, 28,  0,
    0,  0,192,240,252,252,  0,  0,  0, 15, 15, 12, 63, 63, 12,  0,
    0,252,252,204,204,204,140,  0,  0, 24, 56, 48, 48, 63, 31,  0,
    0,240,248,156,140,140,  0,  0,  0, 31, 63, 49, 49, 63, 31,  0,
    0, 12, 12, 12,204,252, 60,  0,  0,  0, 60, 63,  3,  0,  0,  0,
    0, 56,252,204,204,252, 56,  0,  0, 31, 63, 48, 48, 63, 31,  0,
    0,120,252,204,204,252,248,  0,  0,  0, 48, 48, 56, 31, 15,  0
};




/* ===================================================================*
 * 						Symbols1
 * ===================================================================*/
/* ===================================================================*					
 * len= 105 * sym + 2
 * ===================================================================*/


#define Symbols_35x23_bmp_LEN  842		
unsigned char Symbols_35x23_bmp[Symbols_35x23_bmp_LEN] =
{
   35, 3,
   
   //-------------------------------------------------------Negativ-----
   //pumpOff
    192, 64,192,192,192,192,192,192,192,192,192,192,192,192,192,192,
  192,192,192,192,192,224, 48, 24,136,140,196, 68,196,199,  1,131,
  198,108, 56,255,  0,239,239,239,239,239,239,239,239,239,239,239,
  239,239,239,239,239,239,239,239,236,238,239,239,239,239,  0,255,
    1,  1,  1,  0,  0,  0,127, 64, 95, 95, 95, 95, 95, 95, 95, 95,
   95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,
   95, 64,127,  0,  0,  0,  0,  0,  0,
  
	//mud
	192, 64,192,192,192,192,192,192,192,192,192,192,248, 12,  4,  4,
  196,199,  1,131,198,236,248,192,192,192,192, 64,192,  0,  0,  0,
    0,  0,  0,255,  0,251,251,251,251,251,251,251,251,251,251,251,
    0,  0,  0,251,251,251,251,251,251,251,251,251,251,251,  0,255,
    0,  0,  0,  0,  0,  0,127, 64, 95, 85, 95, 90, 95, 85, 95, 90,
   95, 85, 95, 90, 86, 90, 95, 85, 95, 90, 95, 85, 95, 90, 95, 85,
   95, 64,127,  0,  0,  0,  0,  0,  0,
   
    //inflowPump
      128,  0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
  128,128,128,128,184,236,198,131,  1,199,196, 68,196,140,136, 24,
   48,224,128,255,  0,251,251,247,239,239,239,247,251,251,251,247,
  239,239,239,247,251,251,251,247,239,239,239,247,251,251,  0,255,
    0,  1,  3,  6,  4,  7,127, 64, 95, 95, 95, 95, 95, 95, 95, 95,
   95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95, 95,
   95, 64,127,  0,  0,  0,  0,  0,  0,
   
	//pump2
       0,  0,254,254,254,  6,242,114,242, 50,242,114,242,  6,254,254,
  254,  0,254,254,254,  6,242,114,242, 50,242,114,242,  6,254,254,
  254,  0,  0,  0,  0,255,255,127,  0,127, 96,127, 96,127, 96,127,
    0,127,255,255,  0,255,255,127,  0,127, 96,127, 96,127, 96,127,
    0,127,255,255,  0,  0,  0,  0,127, 96, 64, 78, 78, 78, 78, 78,
   78, 78, 78, 78, 64, 96,127,  0,127, 96, 64, 78, 78, 78, 78, 78,
   78, 78, 78, 78, 64, 96,127,  0,  0,
   
   //-------------------------------------------------------Positiv----
   //pumpOff
    0,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,192,224,112,112, 56,184, 56, 56,254,124,
   56, 16,  0,  0,255, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
   16, 16, 16, 16, 16, 16, 16, 16, 19, 17, 16, 16, 16, 16,255,  0,
    0,  0,  0,  0,  0,  0,  0, 63, 32, 32, 32, 32, 32, 32, 32, 32,
   32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
   32, 63,  0,  0,  0,  0,  0,  0,  0,

  
	//mud
	    0,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,240,248,248,
   56, 56,254,124, 56, 16,  0,  0,  0,  0,  0,128,  0,  0,  0,  0,
    0,  0,  0,  0,255,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
  255,255,255,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,255,  0,
    0,  0,  0,  0,  0,  0,  0, 63, 32, 42, 32, 37, 32, 42, 32, 37,
   32, 42, 32, 37, 41, 37, 32, 42, 32, 37, 32, 42, 32, 37, 32, 42,
   32, 63,  0,  0,  0,  0,  0,  0,  0,

   
    //inflowPump
        0,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0, 16, 56,124,254, 56, 56,184, 56,112,112,224,
  192,  0,  0,  0,255,  4,  4,  8, 16, 16, 16,  8,  4,  4,  4,  8,
   16, 16, 16,  8,  4,  4,  4,  8, 16, 16, 16,  8,  4,  4,255,  0,
    0,  0,  0,  1,  3,  0,  0, 63, 32, 32, 32, 32, 32, 32, 32, 32,
   32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
   32, 63,  0,  0,  0,  0,  0,  0,  0,
   
   //pump2
       0,  0,  0,  0,  0,248, 12,140, 12,204, 12,140, 12,248,  0,  0,
    0,  0,  0,  0,  0,248, 12,140, 12,204, 12,140, 12,248,  0,  0,
    0,  0,  0,  0,  0,  0,  0,128,255,128,159,128,159,128,159,128,
  255,128,  0,  0,  0,  0,  0,128,255,128,159,128,159,128,159,128,
  255,128,  0,  0,  0,  0,  0,  0,  0, 31, 63, 49, 49, 49, 49, 49,
   49, 49, 49, 49, 63, 31,  0,  0,  0, 31, 63, 49, 49, 49, 49, 49,
   49, 49, 49, 49, 63, 31,  0,  0,  0
};



/* ===================================================================*
 * 						Symbols2
 * ===================================================================*/
/* ===================================================================*					
 * len= 87 * sym + 2
 * ===================================================================*/


#define Symbols_29x17_bmp_LEN		1742
unsigned char Symbols_29x17_bmp[Symbols_29x17_bmp_LEN] =
{
   29, 3,
   
   //-------------------------------------------------------Negativ-----
   //setDown
  255,  1,247,247,247,247,247,247,247,247,247,119,119, 23, 23, 23,
  119,119,247,247,247,247,247,247,247,247,247,  1,255,255,  0,127,
   87,127,111,127, 87,127,111,127, 95,126,124, 88,124,126, 95,127,
  111,127, 87,127,111,127, 87,127,  0,255,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,
    
    //alarm
    0,  0,248,252, 60,220,108,188,252,252,252, 60, 12,  4,  4, 12,
   60,252,252,252,188,108,220, 60,252,252,248,  0,  0,  0,  0, 63,
  127,124,123,118,125,127,127,124,120,120, 72, 72,120,120,124,127,
  127,125,118,123,124,127,127, 63,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,
    
    //air
  255,  1,239,239,223,191,191,191,223,239,239,239,223,191,191,191,
  223,239,239,239,223,191,191,191,223,239,239,  1,255,255,  0,127,
  127,111,125,127,127, 94,127,127, 97, 94, 94, 97,127, 75, 83,127,
  127,126,127,111,127,125, 95,127,  0,255,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,
    
    //sensor
      255, 31,239,239,225,239,239, 31,255,255,255,255, 63,191,191,255,
   63,191,191,255, 63,127,255, 63,255, 63,191,191,255,255,224,223,
  223, 31,223,223,224,255,255,255,255,250,250,248,255,248,250,250,
  255,248,255,254,248,255,250,250,248,255,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,
    
    //watch
        0,  0,248,252,252,252,252,252,252, 60, 28, 12,  4,  4,244,  4,
    4, 12, 28, 60,252,252,252,252,252,252,248,  0,  0,  0,  0, 63,
  127,127,127,127,127,127,120,112, 96, 64, 65, 67, 65, 65, 97,112,
  120,127,127,127,127,127,127, 63,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,
    
    //compressor
      255, 15,247,247,  7,247,247,183,183,183,183,183,183,183,183,183,
  247,247,  3,  1,249,249,249,249,249,  1,  3,255,255,255,224,223,
  223,192,223,223,218,218,218,218,218,218,218,218,218,223,223,128,
    0, 63, 63, 63, 63, 63,  0,128,199,255,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,
    
    //circulate
      255,  1,247,247,247,247,247, 55, 23, 23, 23, 23, 23, 23, 23, 23,
  247,247,119,247,247,247,247,247,247,247,247,  1,255,255,  0,127,
  127,127,127,127,120,112,112,113,113,113,113,113,113,113,113, 64,
   96,113,123,127,127,127,127,127,  0,255,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,
    
    //cal
        0,  0,248,252,252,252,252,252, 60,188,188,252, 60,188,188, 60,
  252, 60,252,252,252,252,252,252,252,252,248,  0,  0,  0,  0, 63,
  127,127,127,127,127,120,123,123,127,120,126,126,120,127,120,123,
  123,127,127,127,127,127,127, 63,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,
   
    //Zone
  	255, 1,235,235,235,235,235,235,235,235,235,235, 43, 43, 43, 43,
   43,235,235,235,235,235,235,235,235,235,235,  1,255,255,  0,127,
  127,127,127,127,127,127,127,127,127, 71, 67, 73, 76, 78,127,127,
  127,127,127,127,127,127,127,127,  0,255,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,
    
	//level
  255,  7,255,255,255,127,127, 31, 31, 31,127,127,255,255,255,255,
  255,255,255,243, 49, 25,129,195,255,255,255,  7,255,255,  0,239,
  239,239,239,238,236,232,236,238,239,239,239,239,239,239,239,239,
  239,233,233,239,239,239,239,239,  0,255,  7,  4,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  4,  7,


	//-------------------------------------------------------Positiv----
	//setDown
       0,254,  8,  8,  8,  8,  8,  8,  8,  8,  8,136,136,232,232,232,
  136,136,  8,  8,  8,  8,  8,  8,  8,  8,  8,254,  0,  0,255,128,
  168,128,144,128,168,128,144,128,160,129,131,167,131,129,160,128,
  144,128,168,128,144,128,168,128,255,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,

    
    //alarm
        0,248,  4,  2,194, 34,146, 66,  2,  2,  2,194,242,250,250,242,
  194,  2,  2,  2, 66,146, 34,194,  2,  2,  4,248,  0,  0, 63, 64,
  128,131,132,137,130,128,128,131,135,135,183,183,135,135,131,128,
  128,130,137,132,131,128,128, 64, 63,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,
 
    
    //air
        0,254, 16, 16, 32, 64, 64, 64, 32, 16, 16, 16, 32, 64, 64, 64,
   32, 16, 16, 16, 32, 64, 64, 64, 32, 16, 16,254,  0,  0,255,128,
  128,144,130,128,128,161,128,128,158,161,161,158,128,180,172,128,
  128,129,128,144,128,130,160,128,255,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,
 
    
    //sensor
        0,224, 16, 16, 30, 16, 16,224,  0,  0,  0,  0,192, 64, 64,  0,
  192, 64, 64,  0,192,128,  0,192,  0,192, 64, 64,  0,  0, 31, 32,
   32,224, 32, 32, 31,  0,  0,  0,  0,  5,  5,  7,  0,  7,  5,  5,
    0,  7,  0,  1,  7,  0,  5,  5,  7,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,

    
    //watch
        0,248,  4,  2,  2,  2,  2,  2,  2,194,226,242,250,250, 10,250,
  250,242,226,194,  2,  2,  2,  2,  2,  2,  4,248,  0,  0, 63, 64,
  128,128,128,128,128,128,135,143,159,191,190,188,190,190,158,143,
  135,128,128,128,128,128,128, 64, 63,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,

    
    //compressor
        0,240,  8,  8,248,  8,  8, 72, 72, 72, 72, 72, 72, 72, 72, 72,
    8,  8,252,254,  6,  6,  6,  6,  6,254,252,  0,  0,  0, 31, 32,
   32, 63, 32, 32, 37, 37, 37, 37, 37, 37, 37, 37, 37, 32, 32,127,
  255,192,192,192,192,192,255,127, 56,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,
 
    
    //circulate
        0,254,  8,  8,  8,  8,  8,200,232,232,232,232,232,232,232,232,
    8,  8,136,  8,  8,  8,  8,  8,  8,  8,  8,254,  0,  0,255,128,
  128,128,128,128,135,143,143,142,142,142,142,142,142,142,142,191,
  159,142,132,128,128,128,128,128,255,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,

    
    //cal
        0,248,  4,  2,  2,  2,  2,  2,194, 66, 66,  2,194, 66, 66,194,
    2,194,  2,  2,  2,  2,  2,  2,  2,  2,  4,248,  0,  0, 63, 64,
  128,128,128,128,128,135,132,132,128,135,129,129,135,128,135,132,
  132,128,128,128,128,128,128, 64, 63,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,
    
      //Zone
      0,254, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,212,212,212,212,
  212, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,254,  0,  0,255,128,
  128,128,128,128,128,128,128,128,128,184,188,182,179,177,128,128,
  128,128,128,128,128,128,128,128,255,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,

	//level
	 0, 248,  0,  0,  0,128,128,224,224,224,128,128,  0,  0,  0,  0,
    0,  0,  0, 12,206,230,126, 60,  0,  0,  0,248,  0,  0,255, 16,
   16, 16, 16, 17, 19, 23, 19, 17, 16, 16, 16, 16, 16, 16, 16, 16,
   16, 22, 22, 16, 16, 16, 16, 16,255,  0,  0,  3,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  3,  0
};





/* ===================================================================*
 * 						Symbols3
 * ===================================================================*/
/* ===================================================================*					
 * len= 57 * sym + 2
 * ===================================================================*/

#define Symbols_19x19_bmp_LEN		1085
unsigned char Symbols_19x19_bmp[Symbols_19x19_bmp_LEN] =
{
   19, 3,
   
	//-------------------------------------------------------Negativ----
	//Phosphor
  252,  6,251,253,253,253, 13, 13,205,205,205, 13, 29,253,253,253,
  251,  6,252,255,  0,255,255,255,255,128,128,252,252,252,252,254,
  255,255,255,255,  0,255,  1,  3,  6,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  6,  3,  1,
    
	//pump
    0,  0,252,252,252, 12,228,228,228,100,228,228,228, 12,252,252,
  252,  0,  0,  0,  0,255,255,255,  0,255,192,255,192,255,192,255,
    0,255,255,255,  0,  0,  0,  0,255,193,128,156,156,156,156,156,
  156,156,156,156,128,193,255,  0,  0,
  
	//ESC
      0,  0,248,252,124,124,124,252,124,124,124,252,124,124,124,252,
  248,  0,  0,  0,  0,255,255,240,245,245,255,244,245,241,255,240,
  247,247,255,255,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  0,  0,  0,
    
    //Plus
      252,  6,251,253,253,253,253,253, 13, 13, 13,253,253,253,253,253,
  251,  6,252,255,  0,255,255,248,248,248,248,128,128,128,248,248,
  248,248,255,255,  0,255,  1,  3,  6,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  6,  3,  1,
    
	//Minus
      252,  6,251,253,253,253,253,253,253,253,253,253,253,253,253,253,
  251,  6,252,255,  0,255,255,248,248,248,248,248,248,248,248,248,
  248,248,255,255,  0,255,  1,  3,  6,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  6,  3,  1,
    
    //arrowUp
      252,  6,251,253,253,253,253,125, 61, 29, 61,125,253,253,253,253,
  251,  6,252,255,  0,255,247,243,241,240,240,240,240,240,240,240,
  241,243,247,255,  0,255,  1,  3,  6,  5,  5,  5,  5,  5,253,253,
  253,  5,  5,  5,  5,  5,  6,  3,  1,
  
	//arrowDown
    128,192, 96,160,160,160,160,160,191,191,191,160,160,160,160,160,
   96,192,128,255,  0,255,239,207,143, 15, 15, 15, 15, 15, 15, 15,
  143,207,239,255,  0,255, 63, 96,223,191,191,191,191,190,188,184,
  188,190,191,191,191,191,223, 96, 63,
  
	//OK
	0,  0,248,252,252,124,124,124,124,252,124,252,252,124,252,252,
  248,  0,  0,  0,  0,255,255,255,240,247,247,240,255,240,253,250,
  247,255,255,255,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  0,  0,  0,
    
    //Grad 
      252,  6,251,253, 29, 93, 29,253, 61, 29, 29, 29, 29, 29, 61,253,
  251,  6,252,255,  0,255,255,255,255,255,255,192,128,128,143,143,
  143,207,255,255,  0,255,  1,  3,  6,  5,  5,  5,  5,  5,  5,  5,
    5,  5,  5,  5,  5,  5,  6,  3,  1,
    
    
    //-------------------------------------------------------Positiv----
    //Phosphor
	    0,248,  4,  2,  2,  2,242,242, 50, 50, 50,242,226,  2,  2,  2,
    4,248,  0,  0,255,  0,  0,  0,  0,127,127,  3,  3,  3,  3,  1,
    0,  0,  0,  0,255,  0,  0,  0,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  1,  0,  0,
    
	//pump
	    0,  0,  0,  0,  0,240, 24, 24, 24,152, 24, 24, 24,240,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,255,  0, 63,  0, 63,  0, 63,  0,
  255,  0,  0,  0,  0,  0,  0,  0,  0, 62,127, 99, 99, 99, 99, 99,
   99, 99, 99, 99,127, 62,  0,  0,  0,
  
	//ESC
	0,248,  4,  2,130,130,130,  2,130,130,130,  2,130,130,130,  2,
    4,248,  0,  0,255,  0,  0, 15, 10, 10,  0, 11, 10, 14,  0, 15,
    8,  8,  0,  0,255,  0,  0,  0,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  1,  0,  0,
    
    //Plus
        0,248,  4,  2,  2,  2,  2,  2,242,242,242,  2,  2,  2,  2,  2,
    4,248,  0,  0,255,  0,  0,  7,  7,  7,  7,127,127,127,  7,  7,
    7,  7,  0,  0,255,  0,  0,  0,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  1,  0,  0,

    
	//Minus
	    0,248,  4,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
    4,248,  0,  0,255,  0,  0,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    7,  7,  0,  0,255,  0,  0,  0,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  1,  0,  0,

    
    //arrowUp
        0,248,  4,  2,  2,  2,  2,130,194,226,194,130,  2,  2,  2,  2,
    4,248,  0,  0,255,  0,  8, 12, 14, 15, 15, 15, 15, 15, 15, 15,
   14, 12,  8,  0,255,  0,  0,  0,  1,  2,  2,  2,  2,  2,250,250,
  250,  2,  2,  2,  2,  2,  1,  0,  0,

  
	//arrowDown
	    0,  0,128, 64, 64, 64, 64, 64, 95, 95, 95, 64, 64, 64, 64, 64,
	128,  0,  0,  0,255,  0, 16, 48,112,240,240,240,240,240,240,240,
	112, 48, 16,  0,255,  0,  0, 31, 32, 64, 64, 64, 64, 65, 67, 71,
	67, 65, 64, 64, 64, 64, 32, 31,  0,
 
  
	//OK
	    0,248,  4,  2,  2,130,130,130,130,  2,130,  2,  2,130,  2,  2,
    4,248,  0,  0,255,  0,  0,  0, 15,  8,  8, 15,  0, 15,  2,  5,
    8,  0,  0,  0,255,  0,  0,  0,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  1,  0,  0,
	
	//Line
	    0,  0,  0,  0,  0,  0,  0,  0,255,255,255,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,255,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,
	255,  0,  0,  0,  0,  0,  0,  0,  0,
	
	//Grad
	    0,248,  4,  2,226,162,226,  2,194,226,226,226,226,226,194,  2,
    4,248,  0,  0,255,  0,  0,  0,  0,  0,  0, 63,127,127,112,112,
  112, 48,  0,  0,255,  0,  0,  0,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  2,  2,  2,  2,  2,  1,  0,  0
};




/* ===================================================================*
 * 						PIN-SYMBOLS					
 * ===================================================================*/
/* ===================================================================*					
 * len= 102 * sym + 2
 * ===================================================================*/

#define Pin_34x21_bmp_LEN  614
unsigned char Pin_34x21_bmp[Pin_34x21_bmp_LEN] =
{
   34, 3,
   
   //---------------------------------------------------------Positiv---
   //Frame
  255,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,255, 31, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
   16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
   16, 16, 16, 16, 16, 31,
   
   //ESC
     255,  1,  1,  1,193,193,193,193,193,193,193,  1,  1,193,193,193,
  193,193,193,193,  1,  1,193,193,193,193,193,193,193,  1,  1,  1,
    1,255,255,  0,  0,  0,255,255,204,204,204,204,204,  0,  0,207,
  207,204,204,204,252,252,  0,  0,255,255,192,192,192,192,192,  0,
    0,  0,  0,255, 31, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
   16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
   16, 16, 16, 16, 16, 31,
   
   //DEL
     255,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,129,193,225,241,
  241,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  4, 14, 31, 63,127,
  255,255,255, 31, 31, 31, 31, 31, 31,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,255, 31, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
   16, 16, 16, 17, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
   16, 16, 16, 16, 16, 31,
   
   //---------------------------------------------------------Negativ---
   //BlackBox
     255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
   31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
   31, 31, 31, 31, 31, 31,
   
   //ESC
     255,255,255,255, 63, 63, 63, 63, 63, 63, 63,255,255, 63, 63, 63,
   63, 63, 63, 63,255,255, 63, 63, 63, 63, 63, 63, 63,255,255,255,
  255,255,255,255,255,255,  0,  0, 51, 51, 51, 51, 51,255,255, 48,
   48, 51, 51, 51,  3,  3,255,255,  0,  0, 63, 63, 63, 63, 63,255,
  255,255,255,255, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
   31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
   31, 31, 31, 31, 31, 31,
   
   //DEL
     255,255,255,255,255,255,255,255,255,255,255,255,127, 63, 31, 15,
   15,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,251,241,224,192,128,
    0,  0,  0,224,224,224,224,224,224,255,255,255,255,255,255,255,
  255,255,255,255, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
   31, 31, 31, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
   31, 31, 31, 31, 31, 31
};


/* ===================================================================*
 * 						Text-SYMBOLS					
 * ===================================================================*/
/* ===================================================================*					
 * len= 78 * sym + 2
 * ===================================================================*/

#define textButton_39x16_bmp_LEN  158
unsigned char textButton_39x16_bmp[textButton_39x16_bmp_LEN] =
{
   39, 2,
   
	//textBlame
 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   
   //textFrame
  255,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,255,255,128,128,128,128,128,128,128,128,
  128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
  128,128,128,128,128,128,128,128,128,128,128,128,128,255
};



/* ===================================================================*
 * 						HECS Logo					
 * ===================================================================*/

#define Text_HECS_LEN  78
unsigned char Text_HECS[Text_HECS_LEN] =
{
   38, 2,
  252,252,252,128,128,128,128,252,252,252,  0,  0,248,252,252,156,
  156,156,156,156,  0,  0,248,252,252, 28, 28, 28, 28,  0,  0,248,
  252,252,156,156,156,156,127,127,127,  3,  3,  3,  3,127,127,127,
    0,  0, 63,127,127,115,115,115,115,115,  0,  0, 63,127,127,112,
  112,112,112,  0,  0,113,115,115,115,127,127, 63
};


/* ===================================================================*
 * 						Purator Logo					
 * ===================================================================*/

#define Text_Purator_LEN  158
unsigned char Text_Purator[Text_Purator_LEN] =
{
   78, 2,
  192,224, 96,224,192,  0,224,224,  0,224,224,  0,192,224, 96, 96,
    0,192,224, 96,224,192,  0,  0,192,224, 96, 96,  0,252,252, 96,
  224,192,  0,192,224, 96, 96,  0,192,224, 96,224,192,  0,192,224,
   96,224,192,  0,128,128,128,  0,252,252, 96,224,192,  0,252,252,
    0,  0,  0,240,240,  0,240,240,  0,224,240,176,240,224, 31, 31,
    3,  3,  1,  0,  1,  3,  3,  3,  1,  0,  3,  3,  0,  0,  0,  1,
    3,  3,  3,  3,  2,  0,  1,  3,  3,  3,  0,  3,  3,  0,  3,  3,
    0,  3,  3,  0,  0,  0,  1,  3,  3,  3,  1,  0,  3,  3,  0,  3,
    3,  0,  0,  0,  0,  0,  1,  3,  3,  3,  1,  0,  1,  3,  3,  3,
    0,  1,  3,  3,  3,  1,  0,  1,  3,  2,  2,  2
};







/**********************************************************************\
 * End of sym.h
\**********************************************************************/

