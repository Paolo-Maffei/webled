/*
 * Author: zhouqiang
 * Ver: 1.0
 * */
#include "enc28j60.h"
#include "uip.h"
#include "uip_arp.h"

uint8_t Enc28j60Bank;
uint16_t NextPacketPtr;
uint8_t  ENCRevID;

void SetMacAddress(const uint8_t *macaddr)
{
  // write MAC address
  // NOTE: MAC address in ENC28J60 is byte-backward
  enc28j60Write(MAADR5, *macaddr++);
  enc28j60Write(MAADR4, *macaddr++);
  enc28j60Write(MAADR3, *macaddr++);
  enc28j60Write(MAADR2, *macaddr++);
  enc28j60Write(MAADR1, *macaddr++);
  enc28j60Write(MAADR0, *macaddr++);
}

void GetMacAddress(uint8_t *macaddr)
{
  // read MAC address registers
  // NOTE: MAC address in ENC28J60 is byte-backward
  *macaddr++ = enc28j60Read(MAADR5);
  *macaddr++ = enc28j60Read(MAADR4);
  *macaddr++ = enc28j60Read(MAADR3);
  *macaddr++ = enc28j60Read(MAADR2);
  *macaddr++ = enc28j60Read(MAADR1);
  *macaddr++ = enc28j60Read(MAADR0);
}

static uint8_t enc28j60ReadOp(uint8_t op, uint8_t address)
{

    uint8_t data;
    // assert CS
    SPI_CS_Low();

    data = op | (address & ADDR_MASK);
    SPI_SendByte(data);
    data = SPI_ReadByte(0xff);

    // do dummy read if needed
    if(address & 0x80)
      data = SPI_ReadByte(0xff);
    
    SPI_CS_High(); //CS auf High
    return data;
}

static void enc28j60WriteOp(uint8_t op, uint8_t address, uint8_t data)
{
    uint16_t senddata;

    // assert CS
    SPI_CS_Low();
    
    senddata= op | (address & ADDR_MASK);
    SPI_SendByte(senddata);

    SPI_SendByte(data);

    // release CS
    SPI_CS_High(); //CS auf High
}

static void enc28j60ReadBuffer(uint16_t len, uint8_t *data)
{
	// assert CS
    SPI_CS_Low();
	// issue read command
    SPI_SendByte(ENC28J60_READ_BUF_MEM);
    while(len--)
    {
      *data++ = SPI_ReadByte(0);
    }	
	// release CS
    SPI_CS_High();
}

static void enc28j60WriteBuffer(uint16_t len, uint8_t* data)
{
	// assert CS
  SPI_CS_Low(); //CS auf Low
	// issue write command
  SPI_SendByte(ENC28J60_WRITE_BUF_MEM);
  while(len--)
  {
    // write data
    SPI_SendByte(*data++);
  }	
	// release CS
  SPI_CS_High();
}

static void enc28j60SetBank(uint8_t address)
{
	// set the bank (if needed)
	if((address & BANK_MASK) != Enc28j60Bank)
	{
		// set the bank
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
		Enc28j60Bank = (address & BANK_MASK);
	}
}

static uint8_t enc28j60Read(uint8_t address)
{
	// set the bank
	enc28j60SetBank(address);
	// do the read
	return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}

static void enc28j60Write(uint8_t address, uint8_t data)
{
	// set the bank
	enc28j60SetBank(address);
	// do the write
	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

static void enc28j60PhyWrite(uint8_t address, uint16_t data)
{
	// set the PHY register address
	enc28j60Write(MIREGADR, address);
	
	// write the PHY data
	enc28j60Write(MIWRL, data);	
	enc28j60Write(MIWRH, data>>8);

	// wait until the PHY write completes
	while(enc28j60Read(MISTAT) & MISTAT_BUSY);
}

void enc28j60Init(uint8_t *mymac)
{
    uint32_t timeout;
    
    // 初始化 SPI0 接口
    SPI_MasterInit();      
    // 拉高RESET脚               
    LPC_IOCON->PIO2_10 |= 0x00;                                          //RESET gpio
    LPC_GPIO2->DIR    |= 0x0400;                                         // direction:output 
    LPC_GPIO2->DATA   |= 0x0400;                                         // init high level
    
    // perform system reset
    SPI_CS_High();    
    timeout = 100000;
    enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
    while(timeout--);
    // check CLKRDY bit to see if reset is complete
    // Errata workaround #2, CLKRDY check is unreliable, delay 1 mS instead
    while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY))
    {
      timeout++;
      if (timeout > 100000)
        //debug_printf("enc28j60Read timeout");
        break;
    };
    
    // do bank 0 stuff
    // initialize receive buffer
    // 16-bit transfers, must write low byte first
    // set receive buffer start address
    NextPacketPtr = RXSTART_INIT;
    // Rx start
    enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);
    enc28j60Write(ERXSTH, RXSTART_INIT>>8);
    // set receive pointer address
    enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);
    enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);
    // RX end
    enc28j60Write(ERXNDL, RXSTOP_INIT&0xFF);
    enc28j60Write(ERXNDH, RXSTOP_INIT>>8);
    // TX start
    enc28j60Write(ETXSTL, TXSTART_INIT&0xFF);
    enc28j60Write(ETXSTH, TXSTART_INIT>>8);
    // TX end
    enc28j60Write(ETXNDL, TXSTOP_INIT&0xFF);
    enc28j60Write(ETXNDH, TXSTOP_INIT>>8);
    // do bank 1 stuff, packet filter:
    // For broadcast packets we allow only ARP packtets
    // All other packets should be unicast only for our mac (MAADR)
    //
    // The pattern to match on is therefore
    // Type     ETH.DST
    // ARP      BROADCAST
    // 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
    // in binary these poitions are:11 0000 0011 1111
    // This is hex 303F->EPMM0=0x3f,EPMM1=0x30
    enc28j60Write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
    enc28j60Write(EPMM0, 0x3f);
    enc28j60Write(EPMM1, 0x30);
    enc28j60Write(EPMCSL, 0xf9);
    enc28j60Write(EPMCSH, 0xf7);
    //
    //
    // do bank 2 stuff
    // enable MAC receive
    enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    // bring MAC out of reset
    enc28j60Write(MACON2, 0x00); 
    
    // enable automatic padding to 60bytes and CRC operations
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
    
    // set inter-frame gap (non-back-to-back)
    enc28j60Write(MAIPGL, 0x12);
    enc28j60Write(MAIPGH, 0x0C);
    
    // set inter-frame gap (back-to-back)
    enc28j60Write(MABBIPG, 0x12);
    
    // Set the maximum packet size which the controller will accept
    // Do not send packets longer than MAX_FRAMELEN:
    enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);//enc28j60Write(0x0A|0x40|0x80, 1500&0xFF);	
    enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);//enc28j60Write(0x0B|0x40|0x80, 1500>>8)
    
    // do bank 3 stuff
    SetMacAddress(mymac);
       
    enc28j60PhyWrite(PHCON1, PHCON1_PDPXMD);//enc28j60PhyWrite(0x00, 0x0100);
    // no loopback of transmitted frames
    enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);
    
    ENCRevID = enc28j60Read(EREVID);
    // switch to bank 0
    enc28j60SetBank(ECON1);
    // enable interrutps
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
    // enable packet reception
    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
    
/*    printf("%02x-",enc28j60Read(MAADR5));
    printf("%02x-",enc28j60Read(MAADR4));
    printf("%02x-",enc28j60Read(MAADR3));
    printf("%02x-",enc28j60Read(MAADR2));
    printf("%02x-",enc28j60Read(MAADR1));
    printf("%02x",enc28j60Read(MAADR0));
    */
}

void enc28j60clkout(unsigned char clk)
{
  //setup clkout: 2 is 12.5MHz:
    enc28j60Write(ECOCON, clk & 0x7);
}

/************************************************
 * enc28j60 Powersave Mode
 * mode 1 = powerdown
 * mode 0 = wakeup
 * usage:
 * char result enc28j60_powersave(1);
 ************************************************/
char enc28j60_powersave(uint8_t mode)
{
    char result = 0;
    uint32_t timeout = 0;
    switch(mode)
    {
        case 1:
            enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);              
            while(!(enc28j60Read(ESTAT) & ESTAT_RXBUSY))
            {//Wait for any in-progress packets to finish being received by polling
                timeout++;
                if (timeout > 100000)
                    break;
            };
            timeout = 0;
            while(!(enc28j60Read(ECON1) & ECON1_TXRTS))
            {//Wait for any current transmissions to end by confirming
                timeout++;
                if (timeout > 100000)
                    break;
            };
            enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_VRPS);
            enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PWRSV);//Enter Sleep
            result = 0;
        break;
        case 0:
            enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_PWRSV);//Enter Sleep
            timeout = 0;
            while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY))
            {//Wait for PHY to stabilize (wait 300us)
                timeout++;
                if (timeout > 100000)
                    break;
            };
            enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
            //set maybee 12.1.5 Link Change Interrupt Flag (LINKIF)
            result = 1;
        break;
    }
    return result;
}

void enc28j60PacketSend(void)
{
    // Set the write pointer to start of transmit buffer area
	enc28j60Write(EWRPTL, (TXSTART_INIT)&0xFF);
	enc28j60Write(EWRPTH, (TXSTART_INIT)>>8);
	// Set the TXND pointer to correspond to the packet size given
	enc28j60Write(ETXNDL, (TXSTART_INIT+uip_len)&0xFF);
	enc28j60Write(ETXNDH, (TXSTART_INIT+uip_len)>>8);
	// write per-packet control byte (0x00 means use macon3 settings)
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x02);
	// copy the packet into the transmit buffer
	enc28j60WriteBuffer(uip_len, uip_buf);
	// send the contents of the transmit buffer onto the network
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
/*       if(ENCRevID == 0x05u || ENCRevID == 0x06u)
	{
          uint16_t AttemptCounter = 0x0000;
	  while((enc28j60Read(EIR) & (EIR_TXERIF | EIR_TXIF)) && (++AttemptCounter < 1000));
	  if((enc28j60Read(EIR) & EIR_TXERIF) || (AttemptCounter >= 1000))
	  {
	      WORD_VAL ReadPtrSave;
	      WORD_VAL TXEnd;
	      TXSTATUS TXStatus;
	      uint8_t i;

			// Cancel the previous transmission if it has become stuck set
			//BFCReg(ECON1, ECON1_TXRTS);
              enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
			// Save the current read pointer (controlled by application)
	      ReadPtrSave.v[0] = enc28j60Read(ERDPTL);
              ReadPtrSave.v[1] = enc28j60Read(ERDPTH);
			// Get the location of the transmit status vector
	      TXEnd.v[0] = enc28j60Read(ETXNDL);
	      TXEnd.v[1] = enc28j60Read(ETXNDH);
	      TXEnd.Val++;	
			// Read the transmit status vector
	      enc28j60Write(ERDPTL, TXEnd.v[0]);
	      enc28j60Write(ERDPTH, TXEnd.v[1]);

	      enc28j60ReadBuffer(sizeof(TXStatus) ,(uint8_t*)&TXStatus);

			// Implement retransmission if a late collision occured (this can 
			// happen on B5 when certain link pulses arrive at the same time 
			// as the transmission)
	      for(i = 0; i < 16u; i++)
	      {
                if((enc28j60Read(EIR) & EIR_TXERIF) && TXStatus.bits.LateCollision)
		{
					// Reset the TX logic
                    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
                    enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
                    enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF | EIR_TXIF);
					// Transmit the packet again
		    enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
		    while(!(enc28j60Read(EIR) & (EIR_TXERIF | EIR_TXIF)));
					// Cancel the previous transmission if it has become stuck set
		    enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
					// Read transmit status vector
		    enc28j60Write(ERDPTL, TXEnd.v[0]);
		    enc28j60Write(ERDPTH, TXEnd.v[1]);
                    enc28j60ReadBuffer(sizeof(TXStatus) ,(uint8_t*)&TXStatus);
		}
		else
		{
		    break;
		}
	      }
			// Restore the current read pointer
	      enc28j60Write(ERDPTL, ReadPtrSave.v[0]);
	      enc28j60Write(ERDPTH, ReadPtrSave.v[1]);
          }
	}
 */
       // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
    if( (enc28j60Read(EIR) & EIR_TXERIF) )
    {
        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
    }
}

uint16_t enc28j60PacketReceive(void)
{
	uint16_t ret_len,pkg_status;
        uint16_t rs,re;
      
	// check if a packet has been received and buffered
	if( enc28j60Read(EPKTCNT) ==0 )
          return 0;
	enc28j60Write(ERDPTL, (NextPacketPtr));
	enc28j60Write(ERDPTH, (NextPacketPtr)>>8);
	// read the next packet pointer
	NextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	NextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the packet length
	ret_len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	ret_len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
        ret_len -= 4; //remove the CRC count
        
        // read the packet status
	pkg_status  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	pkg_status |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
        
        if(0x80&pkg_status)
        {
          // read the packet from the receive buffer
	  enc28j60ReadBuffer(UIP_BUFSIZE, uip_buf);
        }
        else
        {
          ret_len = 0;
        }
          
        rs = enc28j60Read(ERXSTH);
        rs <<= 8;
        rs |= enc28j60Read(ERXSTL);
        re = enc28j60Read(ERXNDH);
        re <<= 8;
        re |= enc28j60Read(ERXNDL);
        if (NextPacketPtr - 1 < rs || NextPacketPtr - 1 > re)
        {
          enc28j60Write(ERXRDPTL, (re));
          enc28j60Write(ERXRDPTH, (re)>>8);
        }
        else
        {
          enc28j60Write(ERXRDPTL, (NextPacketPtr-1));
          enc28j60Write(ERXRDPTH, (NextPacketPtr-1)>>8);
        }

	// decrement the packet counter indicate we are done with this packet
	// clear the PKTIF: Receive Packet Pending Interrupt Flag bit
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
        
        return ret_len;
}



