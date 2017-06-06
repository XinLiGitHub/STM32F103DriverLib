﻿#include "W25QXX.h"

u16 W25QXX_TYPE = W25Q32;	//??????W25Q32

//??????SPI FLASH??IO??
W25QXX::W25QXX(SPI& SPIx, GPIO_TypeDef* CSPortx, u16 CSPinx) :mSPI(SPIx)
{
	mCSPort = CSPortx;
	mCSPin = CSPinx;
	GPIO_InitTypeDef  GPIO_InitStructure;
	if (mCSPort == GPIOA)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//Ê¹ÄÜGPIOAÊ±ÖÓ
	else if (mCSPort == GPIOB)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//Ê¹ÄÜGPIOBÊ±ÖÓ
	else if (mCSPort == GPIOC)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//Ê¹ÄÜGPIOCÊ±ÖÓ
	else if (mCSPort == GPIOD)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);//Ê¹ÄÜGPIODÊ±ÖÓ
	//	else if (mCSPort == GPIOE)
	//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOBE, ENABLE);//Ê¹ÄÜGPIOEÊ±ÖÓ
	//	else if (mCSPort == GPIOF)
	//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOBF ENABLE);//Ê¹ÄÜGPIOFÊ±ÖÓ

	GPIO_InitStructure.GPIO_Pin = mCSPin;									//PB0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;					//Êä³ö
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//100MHz
	GPIO_Init(mCSPort, &GPIO_InitStructure);							//³õÊ¼»¯

	GPIO_SetBits(mCSPort, mCSPin);
	mSPI.SetSpeed(SPI_BaudRatePrescaler_2);
	W25QXX_TYPE = W25QXX_ReadID();												//¶ÁÈ¡FLASH ID.
}

//????W25QXX??×????????÷
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:????0,×????????÷±??¤??,????WP????
//TB,BP2,BP1,BP0:FLASH???ò??±??¤?è??
//WEL:?????????¨
//BUSY:??±ê????(1,??;0,????)
//????:0x00
u8 W25QXX::W25QXX_ReadSR(void)
{
	u8 byte = 0;
	GPIO_ResetBits(mCSPort, mCSPin);         //?????÷??   
	mSPI.SPI_RW(W25X_ReadStatusReg); 				 //·???????×????????÷?ü??  
	byte = mSPI.SPI_RW(0xFF);         			 //????????×??? 
	GPIO_SetBits(mCSPort, mCSPin);					 //????????  
	return byte;
}
//??W25QXX×????????÷
//????SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)??????!!!
void W25QXX::W25QXX_Write_SR(u8 sr)
{
	GPIO_ResetBits(mCSPort, mCSPin);         //?????÷??   
	mSPI.SPI_RW(W25X_WriteStatusReg);  			 //·???????×????????÷?ü??    
	mSPI.SPI_RW(sr);               					 //????????×???  
	GPIO_SetBits(mCSPort, mCSPin);					 //????????  
}
//W25QXX??????	
//??WEL????   
void W25QXX::W25QXX_Write_Enable(void)
{
	GPIO_ResetBits(mCSPort, mCSPin);         //?????÷??   
	mSPI.SPI_RW(W25X_WriteEnable);     			 //·?????????
	GPIO_SetBits(mCSPort, mCSPin);					 //????????  	      
}
//W25QXX??????	
//??WEL????  
void W25QXX::W25QXX_Write_Disable(void)
{
	GPIO_ResetBits(mCSPort, mCSPin);         //?????÷??   
	mSPI.SPI_RW(W25X_WriteDisable);    			 //·?????????????
	GPIO_SetBits(mCSPort, mCSPin);					 //????????    	      
}
//????????ID
//·?????????:				   
//0XEF13,±í????????????W25Q80  
//0XEF14,±í????????????W25Q16    
//0XEF15,±í????????????W25Q32  
//0XEF16,±í????????????W25Q64 
//0XEF17,±í????????????W25Q128 	  
u16 W25QXX::W25QXX_ReadID(void)
{
	u16 Temp = 0;
	GPIO_ResetBits(mCSPort, mCSPin);         //?????÷??   			    
	mSPI.SPI_RW(0x90);//·???????ID?ü??	    
	mSPI.SPI_RW(0x00);
	mSPI.SPI_RW(0x00);
	mSPI.SPI_RW(0x00);
	Temp |= mSPI.SPI_RW(0xFF) << 8;
	Temp |= mSPI.SPI_RW(0xFF);
	GPIO_SetBits(mCSPort, mCSPin);					 //????????  			    
	return Temp;
}
//????SPI FLASH  
//?????¨???·???????????¨?¤????????
//pBuffer:??????????
//ReadAddr:?????????????·(24bit)
//NumByteToRead:????????×?????(×??ó65535)
void W25QXX::W25QXX_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
	u16 i;
	GPIO_ResetBits(mCSPort, mCSPin);         //?????÷??   
	mSPI.SPI_RW(W25X_ReadData);         		 //·????????ü??   
	mSPI.SPI_RW((u8)((ReadAddr) >> 16)); 		 //·???24bit???·    
	mSPI.SPI_RW((u8)((ReadAddr) >> 8));
	mSPI.SPI_RW((u8)ReadAddr);
	for (i = 0; i < NumByteToRead; i++)
	{
		pBuffer[i] = mSPI.SPI_RW(0XFF);  			 //???·????  
	}
	GPIO_SetBits(mCSPort, mCSPin);					 //????????  				    	      
}
//SPI??????(0~65535)??????????256??×?????????
//?????¨???·????????×??ó256×?????????
//pBuffer:??????????
//WriteAddr:?????????????·(24bit)
//NumByteToWrite:????????×?????(×??ó256),???????????????????????à×?????!!!	 
void W25QXX::W25QXX_Write_Page(const u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 i;
	W25QXX_Write_Enable();                  //SET WEL
	GPIO_ResetBits(mCSPort, mCSPin);        //?????÷??  
	mSPI.SPI_RW(W25X_PageProgram);      		//·????????ü??   
	mSPI.SPI_RW((u8)((WriteAddr) >> 16));   //·???24bit???·
	mSPI.SPI_RW((u8)((WriteAddr) >> 8));
	mSPI.SPI_RW((u8)WriteAddr);
	for (i = 0; i < NumByteToWrite; i++)
		mSPI.SPI_RW(pBuffer[i]);							//???·????
	GPIO_SetBits(mCSPort, mCSPin);					//????????  	
	W25QXX_Wait_Busy();					  				  //?????????á??
}
//???ì?é??SPI FLASH 
//±????·±??ù???????··??§??????????????0XFF,·??ò??·?0XFF???????????????§°?!
//????×??????????? 
//?????¨???·???????????¨?¤????????,???????·±????·??????!
//pBuffer:??????????
//WriteAddr:?????????????·(24bit)
//NumByteToWrite:????????×?????(×??ó65535)
//CHECK OK
void W25QXX::W25QXX_Write_NoCheck(const u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u16 pageremain;
	pageremain = 256 - WriteAddr % 256;							//???????à??×?????		 	    
	if (NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite;									//???ó??256??×???
	while (1)
	{
		W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
		if (NumByteToWrite == pageremain)
			break;											 								//?????á????
		else																				  //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;
			NumByteToWrite -= pageremain;			  				//????????????????×?????
			if (NumByteToWrite > 256)
				pageremain = 256; 											  //????????????256??×???
			else
				pageremain = NumByteToWrite; 	 			 		  //????256??×?????
		}
	};
}
//??SPI FLASH  
//?????¨???·???????????¨?¤????????
//??????????????×÷!
//pBuffer:??????????
//WriteAddr:?????????????·(24bit)						
//NumByteToWrite:????????×?????(×??ó65535)   
u8 W25QXX_BUFFER[4096];
void W25QXX::W25QXX_Write(const u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
	u32 secpos;
	u16 secoff;
	u16 secremain;
	u16 i;
	u8 * W25QXX_BUF;
	W25QXX_BUF = W25QXX_BUFFER;
	secpos = WriteAddr / 4096;													//???????·  
	secoff = WriteAddr % 4096;													//??????????????
	secremain = 4096 - secoff;													//???????à?????ó??   
	if (NumByteToWrite <= secremain)
		secremain = NumByteToWrite;												//???ó??4096??×???
	while (1)
	{
		W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096);								 //??????????????????
		for (i = 0; i < secremain; i++)															 //???é????
		{
			if (W25QXX_BUF[secoff + i] != 0XFF)
				break;									 																 //?è??????  	  
		}
		if (i < secremain)																					 //?è??????
		{
			W25QXX_Erase_Sector(secpos);										    			 //????????????
			for (i = 0; i < secremain; i++)	   												 //????
			{
				W25QXX_BUF[i + secoff] = pBuffer[i];
			}
			W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096);	   //????????????  
		}
		else
			W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain);			 //??????????????,?±?????????????à????. 				   
		if (NumByteToWrite == secremain)
			break;																										 //?????á????
		else																												 //???????á??
		{
			secpos++;																	 //???????·??1
			secoff = 0;																 //??????????0 	 
			pBuffer += secremain;  										 //????????
			WriteAddr += secremain;										 //?????·????	   
			NumByteToWrite -= secremain;							 //×?????????
			if (NumByteToWrite > 4096)
				secremain = 4096;											   //???????????????????ê
			else
				secremain = NumByteToWrite;					 		 //?????????????????ê??
		}
	};
}
//????????????		  
//?????±?????¤...
void W25QXX::W25QXX_Erase_Chip(void)
{
	W25QXX_Write_Enable();                   //SET WEL 
	W25QXX_Wait_Busy();
	GPIO_ResetBits(mCSPort, mCSPin);         //?????÷??  
	mSPI.SPI_RW(W25X_ChipErase);     				 //·??????????ü??  
	GPIO_SetBits(mCSPort, mCSPin);					 //????????     	      
	W25QXX_Wait_Busy();   				  				 //?????????????á??
}
//????????????
//Dst_Addr:???????· ?ù???????????è??
//??????????????×????±??:150ms
void W25QXX::W25QXX_Erase_Sector(u32 Dst_Addr)
{
	//?à??falsh?????é??,??????    
	Dst_Addr *= 4096;
	W25QXX_Write_Enable();                	   //SET WEL 	 
	W25QXX_Wait_Busy();
	GPIO_ResetBits(mCSPort, mCSPin);        	 //?????÷??  
	mSPI.SPI_RW(W25X_SectorErase);     				 //·??????????????? 
	mSPI.SPI_RW((u8)((Dst_Addr) >> 16));  		 //·???24bit???·    
	mSPI.SPI_RW((u8)((Dst_Addr) >> 8));
	mSPI.SPI_RW((u8)Dst_Addr);
	GPIO_SetBits(mCSPort, mCSPin);						 //????????  
	W25QXX_Wait_Busy();   				  					 //?????????ê??
}
//????????
void W25QXX::W25QXX_Wait_Busy(void)
{
	while ((W25QXX_ReadSR() & 0x01) == 0x01);  // ????BUSY??????
}
//????????????
void W25QXX::W25QXX_PowerDown(void)
{
	GPIO_ResetBits(mCSPort, mCSPin);        	 //?????÷??   
	mSPI.SPI_RW(W25X_PowerDown);      	 	  	 //·????????ü??
	GPIO_SetBits(mCSPort, mCSPin);						 //????????
}
//????
void W25QXX::W25QXX_WAKEUP(void)
{
	GPIO_ResetBits(mCSPort, mCSPin);        	 //?????÷??
	mSPI.SPI_RW(W25X_ReleasePowerDown);  	  	 //send W25X_PowerDown command 0xAB
	GPIO_SetBits(mCSPort, mCSPin);						 //????????
}