#ifndef CUSTOM_PDO_NAME_H
#define CUSTOM_PDO_NAME_H

//-------------------------------------------------------------------//
//                                                                   //
//     This file has been created by the Easy Configurator tool      //
//                                                                   //
//     Easy Configurator project EasyIO.prj
//                                                                   //
//-------------------------------------------------------------------//


#define CUST_BYTE_NUM_OUT	25
#define CUST_BYTE_NUM_IN	21
#define TOT_BYTE_NUM_ROUND_OUT	28
#define TOT_BYTE_NUM_ROUND_IN	24


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
		uint16_t    NextConvReadySignal1;
		uint16_t    NextConvReadySignal2;
		uint16_t    Flags;
		uint16_t    DrvRst;
		uint8_t     TookProduct;
		uint8_t     NextConvReadySignal;
		uint8_t     DriversReset;
		uint8_t     SensSignalFromPrevBarda;
		uint8_t     PallFull;
		uint8_t     DevReadyFromPrevBoard1;
		uint8_t     DevReadyFromPrevBoard2;
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
		uint16_t    SensSignalOnNextBarda;
		uint8_t     ConvReadySignal;
		uint8_t     StackReady;
		uint8_t     GaveStack;
		uint8_t     OutFault;
		uint8_t     DevReady;
		uint8_t     myInputVariable;
		uint8_t     TookProduct;
	}Cust;
} PROCBUFFER_IN;

#endif