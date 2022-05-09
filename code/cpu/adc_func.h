// --
// ADC Setup

// include guard
#ifndef ADC_FUNC_H   
#define ADC_FUNC_H

void ADC_Init(void);
void ADC_Touch_Ch(void);
void ADC_MPX_Ch(void);
void ADC_USV_Ch(void);
void ADC_USV_Check(void);

unsigned char ReadCalibrationByte(unsigned char index);

#endif