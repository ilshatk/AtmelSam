#ifndef CUSTOM_PDO_NAME_H
#define CUSTOM_PDO_NAME_H

//-------------------------------------------------------------------//
//                                                                   //
//     This file has been created by the Easy Configurator tool      //
//                                                                   //
//     Easy Configurator project EasyIO.prj
//                                                                   //
//-------------------------------------------------------------------//


#define CUST_BYTE_NUM_OUT	8
#define CUST_BYTE_NUM_IN	13
#define TOT_BYTE_NUM_ROUND_OUT	8
#define TOT_BYTE_NUM_ROUND_IN	16


typedef union												//---- output buffer ----
{
	uint8_t  Byte [TOT_BYTE_NUM_ROUND_OUT];
	struct
	{
		uint16_t    AnaOut_0;
		uint16_t    AnaOut_1;
		uint16_t    DigiOut;
		uint16_t    Output1;
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
		uint8_t     OutFault;
		int8_t     Input1;
		int8_t     Input2;
	}Cust;
} PROCBUFFER_IN;

#endif