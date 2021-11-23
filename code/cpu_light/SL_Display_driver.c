/*********************************************************************\
*	Author:			Red_Calcifer
* ------------------------------------------------------------------
* 	Projekt:		Steuerung Light SL
*	Name:			Display-driver-SourceFile
* ------------------------------------------------------------------
*	µ-Controler:	ATxmega128A1													
*	Compiler:		AVR Studio mit avr-gcc (WINAVR 2010)													
*	Description:	
* ------------------------------------------------------------------	
	SourceCode of Display EADOGXL160-7 for SL (Steuerung light)
	SourceCode was partly copied from EADOGXL160-7 HalloWorld PRG from EA
* ------------------------------------------------------------------														
*	Date:			01.06.2011  	
* 	lastChanges:											
\**********************************************************************/

#include<avr/io.h>

#include "SL_Define_sym.h"
#include "SL_TWI_func.h"
#include "SL_Display_driver.h"
#include "SL_sym.h"


/* ===================================================================*
 * 						Makros 
 * ===================================================================*/
 
		/* DogXL160 Initialization Commands */
		
        uint8 LcdInitMacro[]={                                
                              Set_Com_End_H,                 
                              Set_Com_End_L,                 
                              Set_LCD_Mapping_Control,       
                              Set_Scroll_Line_LSB,           
                              Set_Scroll_Line_MSB,           
                              Set_Panel_Loading,             
                              Set_LCD_Bias_Ratio,            
                              Set_Vbias_Potentiometer_H,     
                              Set_Vbias_Potentiometer_L,
                              Set_RAM_Address_Control,
                              Set_Display_Enable,            
                              };

 
/* ===================================================================*
 * 						FUNCTIONS 		Config and Transfer
 * ===================================================================*/
 
/*--------------------------------------------------------------------*
 * 	LCD_Init
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Init Display, see LcdInitMacro for details 
 * -------------------------------------------------------------------*/

void LCD_Init(void)
{
	PORTC.DIRSET= 	PIN3_bm 	| 			//ResetLine Output
					PIN4_bm;				//DisplayOnOffLine Output
	
	PORTC.OUTSET= 	PIN3_bm;				//Reset off

	LCD_Rst();
	while(LCD_SendCmd(LcdInitMacro, 11));	//Load Init
}



/*--------------------------------------------------------------------*
 * 	LCD_Backlight
 * --------------------------------------------------------------
 * 	parameter:		t_Backlight	-	switch Backlight on or off
 * --------------------------------------------------------------
 * 	return:			t_Backlight -	State of Backlight
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set or Clr Backlight
 * -------------------------------------------------------------------*/

void LCD_Backlight(t_Backlight back)
{
	static t_Backlight 		bState= Backlight_OFF;
	static int 				bCount=0;
	static unsigned char 	err=0;

	switch(back)
	{
		case Backlight_ON:		PORTC.OUTSET= 	PIN4_bm;	
								bState= Backlight_ON;				break;

		case Backlight_OFF: 	PORTC.OUTCLR= 	PIN4_bm;	
								bState= Backlight_OFF;				break;

		case Backlight_Count: 	if(	(bState== Backlight_ON) &&
									(err)){		bCount++;
									if(bCount>20)	{	bCount=0;	 	//Error blink ON
										PORTC.OUTCLR= 	PIN4_bm;
										bState= Backlight_OFF;}		break;}
								
								if(	(bState== Backlight_OFF) &&
									(err)){		bCount++;
									if(bCount>20)	{	bCount=0;		//Error blink OFF
										PORTC.OUTSET= 	PIN4_bm;
										bState= Backlight_ON;}		break;}

								if((bState== Backlight_ON) &&
									(!err)){	bCount++;
									if(bCount>2000)	{	bCount=0;
										PORTC.OUTCLR= 	PIN4_bm;
										bState= Backlight_OFF;}		break;}	
								break;

		case Backlight_Reset:		bCount=0;						break;
		case Backlight_ErrorOn:		err= 1;							break;
		case Backlight_ErrorOff:	err= 0;							break;		

		default:													break;
	}
}




/*--------------------------------------------------------------------*
 * 	LCD_SendCmd
 * --------------------------------------------------------------
 * 	parameter:	unsigned char* 	SCmd	-	Pointer on Commands
 * 				unsigned char	i		-	Count of Bytes
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Send Commands over TWI to Display
 * -------------------------------------------------------------------*/
 
unsigned char LCD_SendCmd(uint8* SCmd, uint8 i)
{
	unsigned char twiErr=0;
	
	twiErr= TWI_Master_WriteString(	W_CMD, 			//Address
									SCmd,			//Command
									i );			//Count of Bytes

	if(	twiErr == E_TWI_NO_DATA ||
		twiErr == E_TWI_WAIT	||
		twiErr == E_TWI_ARBLOST	||
		twiErr == E_TWI_BUSERR	||
		twiErr == E_TWI_NO_SENT		) return 1;	
	
	return 0;
}



/*--------------------------------------------------------------------*
 * 	LCD_SendData
 * --------------------------------------------------------------
 * 	parameter:	unsigned char* 	SData	-	Pointer on Commands
 * 				unsigned char	i		-	Count of Bytes
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Send Data over TWI to Display
 * -------------------------------------------------------------------*/
 
void LCD_SendData(uint8* SData, uint8 i)
{
	TWI_Master_WriteString(	W_DATA, 		//Address
							SData,			//Command
							i );			//Count of Bytes
}



/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 * 						Set Addresses
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/*--------------------------------------------------------------------*
 * 	LCD_SetPageAddress
 * --------------------------------------------------------------
 * 	parameter:	unsigned char	PA	-	Page Address to be written
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Send Page Address to Display
 * -------------------------------------------------------------------*/
 
void LCD_SetPageAddress(uint8 PA)
{
	uint8 Cmd[] = {Page_Address + PA};  		//PageAdressCmd = PageAddressInitCmd + PA

	if((Cmd[0]<0x60) || (Cmd[0]>0x78)) return;	//Protection
	while(LCD_SendCmd(Cmd, 1));					//send PA-Cmd
}



/*--------------------------------------------------------------------*
 * 	LCD_SetColumnAddress
 * --------------------------------------------------------------
 * 	parameter:	unsigned char	CA	-	Column Address to be written
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Send Column Address to Display
 * -------------------------------------------------------------------*/
 
void LCD_SetColumnAdress(uint8 CA)
{
	uint8 H = 0x00;
	uint8 L = 0x00;
	uint8 ColumnAddress[]  = {Column_LSB0 , Column_MSB0 };
	
	if(CA>160) return;						//Protection

	L = (CA & 0x0F);                
	H = (CA & 0xF0);
	H = (H >> 4);
	
	ColumnAddress[0] = (Column_LSB0 + L );	//AddressLow
	ColumnAddress[1] = (Column_MSB0 + H ); 	//AddressHigh
	
	while(LCD_SendCmd(ColumnAddress, 2));	//Send CA-Cmd
}



/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*
 * 						Window Programming
 * <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

/*--------------------------------------------------------------------*
 * 	LCD_WP_Enable
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Enable Window Programming
 * -------------------------------------------------------------------*/

void LCD_WP_Enable(void)
{
	uint8 Cmd[] = {WPEN};  			//Command
	while(LCD_SendCmd(Cmd, 1));		//send CA-Cmd
}

void LCD_WP_Disable(void)
{
	uint8 Cmd[] = {WPDIS};  		//Command
	while(LCD_SendCmd(Cmd, 1));		//send CA-Cmd
}



/*--------------------------------------------------------------------*
 * 	LCD_WP_Page
 * --------------------------------------------------------------
 * 	parameter:	unsigned char	startPA	-	Start Page-Address
 *				unsigned char	endPA	-	End Page-Address	
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Window Programming, Set Start Page-Address and End Page-Address
 * -------------------------------------------------------------------*/

void LCD_WP_Page(unsigned char startPA, unsigned char endPA)
{
	uint8 Cmd[] = {WPP0, startPA, 	WPP1, endPA};  	//Command
	while(LCD_SendCmd(Cmd, 4));						//send PA-Cmd
}



/*--------------------------------------------------------------------*
 * 	LCD_WP_Column
 * --------------------------------------------------------------
 * 	parameter:	unsigned char	startCA	-	Start Column-Address
 *				unsigned char	endCA	-	End Column-Address	
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Window Programming, Set Start Column-Address and End Column-Address
 * -------------------------------------------------------------------*/

void LCD_WP_Column(unsigned char startCA, unsigned char endCA)
{
	uint8 Cmd[] = {WPC0, startCA, 	WPC1, endCA};  	//Command
	while(LCD_SendCmd(Cmd, 4));						//send CA-Cmd
}
 




/* ===================================================================*
 * 						FUNCTIONS 		Commands
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_AllPixelsON
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set all Pixel ON
 * -------------------------------------------------------------------*/
 
void LCD_AllPixelsOn(void)
{
	uint8 Cmd[] = {Set_All_Pixels_On};
	while(LCD_SendCmd(Cmd, 1));
}

void LCD_AllPixelsOnOff(void)
{
	uint8 Cmd[] = {Set_All_Pixels_On_Off};
	while(LCD_SendCmd(Cmd, 1));
}


/*--------------------------------------------------------------------*
 * 	LCD_Rst
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Reset Display
 * -------------------------------------------------------------------*/

void LCD_Rst(void)
{
	uint8 Cmd[] = {System_Reset};
	while(LCD_SendCmd(Cmd, 1));
}

void LCD_HardwareRst(void)
{
	PORTC.DIRSET= PIN3_bm;		//Output
	PORTC.OUTCLR= PIN3_bm;		//Reset
}


/*--------------------------------------------------------------------*
 * 	LCD_Clean
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Set all Pixel OFF
 * -------------------------------------------------------------------*/
 
void LCD_Clean(void)
{
	uint8 LcdData[80] = {0x00};

	LCD_SetPageAddress(0);		//PageAddress
	LCD_SetColumnAdress(0);		//ColumnAddress

	LCD_WP_Enable();			//Window Programm Enable
	LCD_WP_Page(0, 25);			//Page Frame
	LCD_WP_Column(0,159);		//Column Frame

	for(uint8 p=0; p<52; p++)				//25  Pages 
	{                 	
		for(uint8 c=0; c<2; c++)			//160 Columns
		{     	     
			LCD_SendData(LcdData, 80);
		}
	}
	LCD_WP_Disable();
}

void LCD_Fill(void)
{
	uint8 LcdData[20] = {0xFF};
	
	for(uint8 p=0; p<20; p++)	//Fill Array
	{                 	
		LcdData[p] = 0xFF;
	}

	LCD_SetPageAddress(0);		//PageAddress
	LCD_SetColumnAdress(0);		//ColumnAddress

	LCD_WP_Enable();			//Window Programm Enable
	LCD_WP_Page(0, 25);			//Page Frame
	LCD_WP_Column(0,159);		//Column Frame

	for(uint8 p=0; p<52; p++)				//25  Pages 
	{                 	
		for(uint8 c=0; c<8; c++)			//160 Columns
		{     	     
			LCD_SendData(LcdData, 20);
		}
	}
	LCD_WP_Disable();
}



/*--------------------------------------------------------------------*
 * 	LCD_Inverse
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Init Display, see LcdInitMacro for details 
 * -------------------------------------------------------------------*/

void LCD_Inverse(void)
{
	uint8 Cmd[] = {INVERSE};
	while(LCD_SendCmd(Cmd, 1));
}

void LCD_NotInverse(void)
{
	uint8 Cmd[] = {NOT_INVERSE};
	while(LCD_SendCmd(Cmd, 1));
}




/* ===================================================================*
 * 						FUNCTIONS 		Fill and Clear Space
 * ===================================================================*/
 
/*--------------------------------------------------------------------*
 * 	LCD_FillSpace
 * --------------------------------------------------------------
 * 	parameter:		unsigned char row
 *					unsigned char col
 *					unsigned char rowEnd	
 *					unsigned char colEnd						
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Fills Space
 * -------------------------------------------------------------------*/

void LCD_FillSpace(uint8 row, uint8 col, uint8 height, uint8 len)
{                                    	
	uint8 LcdData[160] = {0xFF};

	for(uint8 p=0; p<160; p++)	//Fill Array
	{                 	
		LcdData[p] = 0xFF;
	}

	LCD_WP_SetFrame(row, col, height, len);		//FrameSet

	for(uint8 p=0; p<(height); p++)				//25  Pages 
	{                 	
		LCD_SendData(LcdData, (len));
	}

	LCD_WP_Disable();
}


/*--------------------------------------------------------------------*
 * 	LCD_ClrSpace
 * --------------------------------------------------------------
 * 	parameter:		void
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 *	Init Display, see LcdInitMacro for details 
 * -------------------------------------------------------------------*/

void LCD_ClrSpace(uint8 row, uint8 col, uint8 height, uint8 len)
{                                    	
	uint8 LcdData[160] = {0x00};

	LCD_WP_SetFrame(row, col, height, len);		//FrameSet

	for(uint8 p=0; p<(height); p++)				//25  Pages 
	{                 	
		LCD_SendData(LcdData, (len));
		LCD_SendData(LcdData, (len));
	}

	LCD_WP_Disable();
}





/* ===================================================================*
 * 						Font
 * ===================================================================*/

/* -------------------------------------------------------------------*
 * 						Font 6x8
 * -------------------------------------------------------------------*/
/*--------------------------------------------------------------------*
 * 	LCD_WriteFont
 * --------------------------------------------------------------
 * 	parameter:	unsigned char	y		-	Start Page
 * 				unsigned char	x		-	Start Collumn
 * 				unsigend char	word	-	Word to write	
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 * 	Write Font with WP
 * -------------------------------------------------------------------*/
 
void LCD_WriteFont(uint8 row, uint8 col, uint16 word)
{                                    	
	uint8 LcdData[40]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	unsigned char	len= 		Font_6X8[0];		//Width  in Dots
  	unsigned char	height= 	Font_6X8[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet

	for(uint8 p=0; p<height; p++)			//Pages 
	{            	  	
		for(int c=0; c<len; c++)			//Columns
		{ 							   	
			L = (Font_6X8[8 + c + len*p + ((word)*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (Font_6X8[8 + a + len*p + ((word)*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}


/* -------------------------------------------------------------------*
 * 						Font 6x8 Neg
 * -------------------------------------------------------------------*/

void LCD_WriteFontNeg(uint8 row, uint8 col, uint16 word)
{                                    	
	uint8 LcdData[40]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	unsigned char	len= 		Font_6X8_Neg[0];		//Width  in Dots
  	unsigned char	height= 	Font_6X8_Neg[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet

	for(uint8 p=0; p<height; p++)			//Pages 
	{            	  	
		for(int c=0; c<len; c++)			//Columns
		{ 							   	
			L = (Font_6X8_Neg[8 + c + len*p + ((word)*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (Font_6X8_Neg[8 + a + len*p + ((word)*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}


/* -------------------------------------------------------------------*
 * 						Font 8x16 Num
 * -------------------------------------------------------------------*/

void LCD_WriteFontNum(uint8 row, uint8 col, unsigned char word)
{                                    
	uint8 LcdData[20]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	unsigned char	len= 		Font_Numbers_8X16[0];		//Width  in Dots
  	unsigned char	height= 	Font_Numbers_8X16[1];		//height in Bytes
	unsigned char	n=0;	
	
	if(word & 0xF0)	n=1;		//Write negative Numbers
	else n=0;					//Write positive Numbers

	word= word & 0x0F;

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet
	
	for(uint8 p=0; p<height; p++)			//Pages 
	{            	  	
		for(int c=0; c<len; c++)			//Columns
		{ 							   	
			L = (Font_Numbers_8X16[2 + c + len*p + ((word)*len*height) + (n*10*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (Font_Numbers_8X16[2 + a + len*p + ((word)*len*height) + (n*10*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}


/* -------------------------------------------------------------------*
 * 						Font 4x6 
 * -------------------------------------------------------------------*/

void LCD_WriteMyFont(uint8 row, uint8 col, unsigned char word)
{                                    
	uint8 LcdData[10]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	unsigned char	len= 		FontNumbers_4X6[0];		//Width  in Dots
  	unsigned char	height= 	FontNumbers_4X6[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet
	
	for(uint8 p=0; p<height; p++)			//Pages 
	{            	  	
		for(int c=0; c<len; c++)			//Columns
		{ 							   	
			L = (FontNumbers_4X6[2 + c + len*p + ((word)*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (FontNumbers_4X6[2 + a + len*p + ((word)*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}


/* -------------------------------------------------------------------*
 * 						Font 4x6 Neg 
 * -------------------------------------------------------------------*/

void LCD_WriteMyFontNeg(uint8 row, uint8 col, unsigned char word)
{                                    
	uint8 LcdData[10]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	unsigned char	len= 		FontNumbers_4X6_Neg[0];		//Width  in Dots
  	unsigned char	height= 	FontNumbers_4X6_Neg[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet
	
	for(uint8 p=0; p<height; p++)			//Pages 
	{            	  	
		for(int c=0; c<len; c++)			//Columns
		{ 							   	
			L = (FontNumbers_4X6_Neg[2 + c + len*p + ((word)*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (FontNumbers_4X6_Neg[2 + a + len*p + ((word)*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}




/* ===================================================================*
 * 						String Font
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_WriteStringFont
 * --------------------------------------------------------------
 * 	parameter:	unsigned char	y		-	Start Page
 * 				unsigned char	x		-	Start Collumn
 * 				char			word[]	-	String to write	
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 * 	Write String Font with WP
 * -------------------------------------------------------------------*/

void LCD_WriteStringFont(uint8 y, uint8 x, char word[])
{
	uint8 k=0;
	uint8 a=0;
	
	do
	{                     
		k = word[a];
		LCD_WriteFont( y, x, (k-33));
		x = x + Font_6X8[0];              
		a++;
		k = word[a];
	}
	while(k!=0);        
}


/* -------------------------------------------------------------------*
 * 						StringFont 6x8 Neg
 * -------------------------------------------------------------------*/

void LCD_WriteStringFontNeg(uint8 y, uint8 x, char word[])
{
	uint8 k=0;
	uint8 a=0;

	do
	{                     
		k = word[a];
		LCD_WriteFontNeg( y, x, (k-33));
		x = x + Font_6X8_Neg[0];              
		a++;
		k = word[a];
	}
	while(k!=0); 
} 


/* -------------------------------------------------------------------*
 * 						StringFont 4x6
 * -------------------------------------------------------------------*/

void LCD_WriteStringMyFont(uint8 y, uint8 x, char word[])
{
	uint8 k=0;
	uint8 a=0;
	
	do
	{                     
		k = word[a];
		LCD_WriteMyFont(y, x, k-48);
		x = x + FontNumbers_4X6[0];              
		a++;
		k = word[a];
	}
	while(k!=0);        
}


/* -------------------------------------------------------------------*
 * 						StringFont 4x6 Neg
 * -------------------------------------------------------------------*/

void LCD_WriteStringMyFontNeg(uint8 y, uint8 x, char word[])
{
	uint8 k=0;
	uint8 a=0;

	do
	{                     
		k = word[a];
		LCD_WriteMyFontNeg( y, x, k-48);
		x = x + FontNumbers_4X6_Neg[0];              
		a++;
		k = word[a];
	}
	while(k!=0); 
} 





/* ===================================================================*
 * 						Value 6x8
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_WriteValue
 * --------------------------------------------------------------
 * 	parameter:	unsigned char	y		-	Start Page
 * 				unsigned char	x		-	Start Collumn
 * 				int				value	-	Value to Convert
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 * 	Write Value of int variable with 3Positions: 000
 * -------------------------------------------------------------------*/

void LCD_WriteValue(uint8 y, uint8 x, int value)
{
	char 			cValue[10]={0};		
	int				con=0;
	unsigned char	i=0;
	
	con= value;

	if(con>999) con= 999;
	if(con<1)	con= 0;

	//--------------------------------------Hex-to-Bcd-for-1Byte-------
	cValue[0]= (con/100);
	cValue[1]= ((con - (100*cValue[0]))/10);
	cValue[2]= (con - ((cValue[0]*100) + (cValue[1]*10)));
	//-----------------------------------------------------------------
	
	//--------------------------------------AsciiConversion------------
	for(i=0; i<3; i++)
	{
		cValue[i]= cValue[i]+48;
	}
	//-----------------------------------------------------------------

	LCD_WriteStringFont(y, x, cValue);       
}


/* -------------------------------------------------------------------*
 * 						Value 6x8 Neg	3Pos
 * -------------------------------------------------------------------*/

void LCD_WriteValueNeg(uint8 y, uint8 x, int value)
{
	char 			cValue[10]={0x00};		
	int				con=0;
	unsigned char	i=0;
	
	con= value;
	
	if(con>999) con= 999;
	if(con<1)	con= 0;

	//--------------------------------------Hex-to-Bcd-for-1Byte-------
	cValue[0]= (con/100);
	cValue[1]= ((con - (100*cValue[0]))/10);
	cValue[2]= (con - ((cValue[0]*100) + (cValue[1]*10)));
	//-----------------------------------------------------------------
	
	//--------------------------------------AsciiConversion------------
	for(i=0; i<3; i++)
	{
		cValue[i]= cValue[i]+48;
	}
	//-----------------------------------------------------------------

	LCD_WriteStringFontNeg(y, x, cValue);       
}


/* -------------------------------------------------------------------*
 * 						Value 6x8 		2Pos
 * -------------------------------------------------------------------*/

void LCD_WriteValue2(uint8 y, uint8 x, int value)
{
	char 			cValue[10]={0x00};		
	int				con=0;
	unsigned char	i=0;
	
	con= value;
	
	if(con>99) con= 99;
	if(con<1)	con= 0;
	
	//--------------------------------------Hex-to-Bcd-for-1Byte-------
	cValue[0]= (con/10);
	cValue[1]= (con - (10*cValue[0]));
	//-----------------------------------------------------------------
	
	//--------------------------------------AsciiConversion------------
	for(i=0; i<2; i++)
	{
		cValue[i]= cValue[i]+48;
	}
	//-----------------------------------------------------------------

	LCD_WriteStringFont(y, x, cValue);       
}


/* -------------------------------------------------------------------*
 * 						Value 6x8 Neg		2Pos
 * -------------------------------------------------------------------*/

void LCD_WriteValueNeg2(uint8 y, uint8 x, int value)
{
	char 			cValue[10]={0x00};		
	int				con=0;
	unsigned char	i=0;
	
	con= value;
	
	if(con>99) con= 99;
	if(con<1)	con= 0;

	//--------------------------------------Hex-to-Bcd-for-1Byte-------
	cValue[0]= (con/10);
	cValue[1]= (con - (10*cValue[0]));
	//-----------------------------------------------------------------
	
	//--------------------------------------AsciiConversion------------
	for(i=0; i<2; i++)
	{
		cValue[i]= cValue[i]+48;
	}
	//-----------------------------------------------------------------

	LCD_WriteStringFontNeg(y, x, cValue);       
}




/* ===================================================================*
 * 						Value 4x6		3Pos
 * ===================================================================*/

void LCD_WriteValue_MyFont(uint8 y, uint8 x, int value)
{
	char 			cValue[10]={0x00};		
	int				con=0;
	unsigned char	i=0;

	con= value;
		
	if(con>999) con= 999;
	if(con<1)	con= 0;
	
	//--------------------------------------Hex-to-Bcd-for-1Byte-------
	cValue[0]= (con/100);
	cValue[1]= ((con - (100*cValue[0]))/10);
	cValue[2]= (con - ((cValue[0]*100) + (cValue[1]*10)));
	//-----------------------------------------------------------------

	//--------------------------------------AsciiConversion------------
	for(i=0; i<3; i++)
	{
		cValue[i]= cValue[i]+48;
	}
	//-----------------------------------------------------------------

	LCD_WriteStringMyFont(y, x, cValue);       
}



/* -------------------------------------------------------------------*
 * 						Value 4x6 		2Pos
 * -------------------------------------------------------------------*/

void LCD_WriteValue_MyFont2(uint8 y, uint8 x, int value)
{
	char 			cValue[10]={0x00};		
	int				con=0;
	unsigned char	i=0;

	con= value;

		
	if(con>99) con= 99;
	if(con<1)	con= 0;
	
	//--------------------------------------Hex-to-Bcd-for-1Byte-------
	cValue[0]= (con/10);
	cValue[1]= (con - (10*cValue[0]));
	//-----------------------------------------------------------------
	
	//--------------------------------------AsciiConversion------------
	for(i=0; i<2; i++)
	{
		cValue[i]= cValue[i]+48;
	}
	//-----------------------------------------------------------------

	LCD_WriteStringMyFont(y, x, cValue);       
}



/* -------------------------------------------------------------------*
 * 						Value 4x6 		5Pos
 * -------------------------------------------------------------------*/

void LCD_WriteValue_MyFont5(uint8 y, uint8 x, int value)
{
	char 			cValue[10]={0x00};		
	int				con=0;
	unsigned char	i=0;

	int				v0=0;
	int				v1=0;

	con= value;
		
	if(con>60000) con= 60000;
	if(con<1)	con= 0;
	
	//--------------------------------------Hex-to-Bcd-for-1Byte-------
	cValue[0]= (con/10000);
	v0= 10000 * cValue[0];

	cValue[1]= ((con - (v0))/1000);
	v1= 1000 * cValue[1];

	cValue[2]= ((con - (v0 + v1))/100);

	cValue[3]= ((con - (v0 + v1 + cValue[2]*100))/10);

	cValue[4]= (con -  (v0 + v1 + cValue[2]*100 + cValue[3]*10));
	//-----------------------------------------------------------------

	//--------------------------------------AsciiConversion------------
	for(i=0; i<5; i++)
	{
		cValue[i]= cValue[i]+48;
	}
	//-----------------------------------------------------------------

	LCD_WriteStringMyFont(y, x, cValue);       
}







/* ===================================================================*
 * 						Symbols
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_WriteSymbols
 * --------------------------------------------------------------
 * 	parameter:	unsigned char	y		-	Start Page
 * 				unsigned char	x		-	Start Collumn
 * 				t_Symbol		sym		-	Enum-Button to write	
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 * 	Writes a negative Symbol
 * -------------------------------------------------------------------*/


void LCD_Write_Symbol_1(uint8 row, uint8 col, t_Symbols_35x23 sym)
{                                    
	uint8 LcdData[40]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	int				len= 		Symbols_35x23_bmp[0];		//Width  in Dots
  	unsigned char	height= 	Symbols_35x23_bmp[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet

	for(uint8 p=0; p<height; p++)						//Pages 
	{            	  	
		for(int c=0; c<len; c++)						//Columns
		{ 							   	
			L = (Symbols_35x23_bmp[2  + c + len*p + ((sym)*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);						//Write Low Page
		
		for(int a=0; a<len; a++)						//160 Columns
		{ 							   	
			H = (Symbols_35x23_bmp[2  + a + len*p + ((sym)*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}


/* -------------------------------------------------------------------*
 * 						Symbols 2	29x17
 * -------------------------------------------------------------------*/

void LCD_Write_Symbol_2(uint8 row, uint8 col, t_Symbols_29x17 sym)
{                                    	
	uint8 LcdData[40]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	int				len= 		Symbols_29x17_bmp[0];		//Width  in Dots
  	unsigned char	height= 	Symbols_29x17_bmp[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);					//FrameSet

	for(uint8 p=0; p<height; p++)							//Pages 
	{            	  	
		for(int c=0; c<len; c++)							//Columns
		{ 							   	
			L = (Symbols_29x17_bmp[2  + c + len*p + ((sym)*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);							//Write Low Page
		
		for(int a=0; a<len; a++)							//160 Columns
		{ 							   	
			H = (Symbols_29x17_bmp[2  + a + len*p + ((sym)*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}


/* -------------------------------------------------------------------*
 * 						Symbols 3	19x19
 * -------------------------------------------------------------------*/

void LCD_Write_Symbol_3(uint8 row, uint8 col, t_Symbols_19x24 sym)
{                                    
	uint8 LcdData[40]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	int				len= 		Symbols_19x19_bmp[0];		//Width  in Dots
  	unsigned char	height= 	Symbols_19x19_bmp[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet

	for(uint8 p=0; p<height; p++)						//Pages 
	{            	  	
		for(int c=0; c<len; c++)						//Columns
		{ 							   	
			L = (Symbols_19x19_bmp[2  + c + len*p + ((sym)*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (Symbols_19x19_bmp[2  + a + len*p + ((sym)*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}






/* ===================================================================*
 * 						Pin
 * ===================================================================*/

void LCD_Write_Pin(uint8 row, uint8 col, t_pinSymbols sym, uint8 num)
{                                    
	uint8 LcdData[40]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	int				len= 		Pin_34x21_bmp[0];		//Width  in Dots
  	unsigned char	height= 	Pin_34x21_bmp[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet

	for(uint8 p=0; p<height; p++)						//Pages 
	{            	  	
		for(int c=0; c<len; c++)						//Columns
		{ 							   	
			L = (Pin_34x21_bmp[2  + c + len*p + ((sym)*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (Pin_34x21_bmp[2  + a + len*p + ((sym)*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	if(num<0x20)
	{
		LCD_WriteFontNum((row+1), (col+13), num);
	}

	LCD_WP_Disable();
}




/* ===================================================================*
 * 						Text
 * ===================================================================*/

void LCD_Write_TextButton(uint8 row, uint8 col, t_textSymbols text, uint8 pos)
{                                    
	uint8 LcdData[40]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	int				len= 		textButton_39x16_bmp[0];		//Width  in Dots
  	unsigned char	height= 	textButton_39x16_bmp[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet

	for(uint8 p=0; p<height; p++)						//Pages 
	{            	  	
		for(int c=0; c<len; c++)						//Columns
		{ 							   	
			L = (textButton_39x16_bmp[2  + c + len*p + ((pos)*len*height)] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (textButton_39x16_bmp[2  + a + len*p + ((pos)*len*height)] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}
	
	if(pos)									//positive Text
	{
		switch(text)
		{
			case Auto:		LCD_WriteStringFont((row+1), (col+8), "Auto"); break;
			case Manual:	LCD_WriteStringFont((row+1), (col+2), "Manual"); break;
			case Setup:		LCD_WriteStringFont((row+1), (col+5), "Setup"); break;
			case Data:		LCD_WriteStringFont((row+1), (col+8), "Data"); break;
		}
	}
	else									//negative Text
	{
		switch(text)
		{
			case Auto:		LCD_WriteStringFontNeg((row+1), (col+8), "Auto"); break;
			case Manual:	LCD_WriteStringFontNeg((row+1), (col+2), "Manual"); break;
			case Setup:		LCD_WriteStringFontNeg((row+1), (col+5), "Setup"); break;
			case Data:		LCD_WriteStringFontNeg((row+1), (col+8), "Data"); break;
		}
	}
	
	LCD_WP_Disable();
}


/* -------------------------------------------------------------------*
 * 						Text Purator
 * -------------------------------------------------------------------*/

void LCD_Write_Purator(uint8 row, uint8 col)
{                                    
	uint8 LcdData[160]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	int				len= 		Text_Purator[0];		//Width  in Dots
  	unsigned char	height= 	Text_Purator[1];		//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);					//FrameSet

	for(uint8 p=0; p<height; p++)							//Pages 
	{            	  	
		for(int c=0; c<len; c++)							//Columns
		{ 							   	
			L = (Text_Purator[2  + c + len*p] & 0x0F); 		//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (Text_Purator[2  + a + len*p] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}


/* -------------------------------------------------------------------*
 * 						Text HECS
 * -------------------------------------------------------------------*/

void LCD_Write_HECS(uint8 row, uint8 col)
{                                    
	uint8 LcdData[80]={0x00};
	uint8 H = 0x00;
    uint8 L = 0x00; 	
	
	int				len= 		Text_HECS[0];			//Width  in Dots
  	unsigned char	height= 	Text_HECS[1];			//height in Bytes

	LCD_WP_SetFrame(row, col, height, len);				//FrameSet

	for(uint8 p=0; p<height; p++)						//Pages 
	{            	  	
		for(int c=0; c<len; c++)						//Columns
		{ 							   	
			L = (Text_HECS[2  + c + len*p] & 0x0F); 	//LSB
			LcdData[c] = LCD_ConvertWP(L);
		}

		LCD_SendData(LcdData, len);			//Write Low Page
		
		for(int a=0; a<len; a++)			//160 Columns
		{ 							   	
			H = (Text_HECS[2  + a + len*p] & 0xF0);		//MSB 	
			H = (H >> 4);
			LcdData[a] = LCD_ConvertWP(H);	
		}

		LCD_SendData(LcdData, len);			//Write High Page
	}

	LCD_WP_Disable();
}



 
/* ===================================================================*
 * 						Built-in-Functions
 * ===================================================================*/

/*--------------------------------------------------------------------*
 * 	LCD_WP_SetFrame
 * --------------------------------------------------------------
 * 	parameter:	unsigned char	row		-	Start Page
 * 				unsigned char	col		-	Start Collumn
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 * 	Writes a negative Symbol
 * -------------------------------------------------------------------*/


void LCD_WP_SetFrame(uint8 row, uint8 col, uint8 height, uint8 len)
{                                    
	LCD_SetPageAddress(row);				//PageAddress
	LCD_SetColumnAdress(col);				//ColumnAddress

	LCD_WP_Enable();						//WP-
	LCD_WP_Page(row, (row+(height*2)));		//Frame-
	LCD_WP_Column(col, (col+len-1));		//Set
}



/*--------------------------------------------------------------------*
 * 	LCD_ConvertWP
 * --------------------------------------------------------------
 * 	parameter:		unsigned char	con	-	Byte to Convert	
 * --------------------------------------------------------------
 * 	return:			void
 * 	Descirption:	
 * --------------------------------------------------------------
 * 	Converts a Nibble to the LED Standards of the Display:
 * 	Each Pixel in the H is converted to 2 Bits, 11 Pixel On - 00 Pixel Off 
 * -------------------------------------------------------------------*/
 
unsigned char LCD_ConvertWP(unsigned char con)
{
	unsigned char convert=0;

	if((con & 0x01) == 0x01)
	{               
		convert = convert + 0x03;
	}
	
	if((con & 0x02) == 0x02)
	{
		convert = convert + 0x0C;
	}
	
	if((con & 0x04) == 0x04)
	{
		convert = convert + 0x30;
	}
	
	if((con & 0x08) == 0x08)
	{
		convert = convert + 0xC0;
	}

	return convert;
}





/**********************************************************************\
 * End of xma_Display_driver.c
\**********************************************************************/
