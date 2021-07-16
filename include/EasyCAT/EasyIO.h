#ifndef CUSTOM_PDO_NAME_H
#define CUSTOM_PDO_NAME_H

//-------------------------------------------------------------------//
//                                                                   //
//     This file has been created by the Easy Configurator tool      //
//                                                                   //
//     Easy Configurator project EasyIO.prj
//                                                                   //
//-------------------------------------------------------------------//


#define CUST_BYTE_NUM_OUT	15
#define CUST_BYTE_NUM_IN	17
#define TOT_BYTE_NUM_ROUND_OUT	16
#define TOT_BYTE_NUM_ROUND_IN	20


typedef union												//---- output buffer ----
{
	uint8_t  Byte [TOT_BYTE_NUM_ROUND_OUT];
	struct
	{
		uint16_t    AnaOut_0;
		uint16_t    AnaOut_1;
		uint16_t    DigiOut;
		uint16_t    Output1;
		uint16_t    ReqPosition;
		uint8_t     DriversReset;
		uint8_t     SensSignalFromPrevBarda;
		uint8_t     TookProduct;
		uint8_t     Flags;
		uint8_t     NextConvReadySignal;
	}Cust;
} PROCBUFFER_OUT;


typedef union												//---- input buffer ----
{
	uint8_t  Byte [TOT_BYTE_NUM_ROUND_IN];
	struct
	{
		uint16_t    AnaIn_0;
		uint16_t    AnaIn_1;
		uint16_t    AnaIn_2;
		uint16_t    AnaIn_3;
		uint16_t    DigiIn;
		uint16_t    OutState;
		uint8_t     OutFault;
		uint8_t     ConvReadySignal;
		uint8_t     SensSignalOnNextBarda;
		uint8_t     StackReady;
		uint8_t     GaveStack;
	}Cust;
} PROCBUFFER_IN;

#endif