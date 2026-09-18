#include<regx52.h>
#include <INTRINS.H>
#include"lib\Fonts.c"
#include"lib\LCDG_S68063.c"
#include"Amlich.c"
//#include"Key.c"
#include"Ds18b20.h"
sbit sda =	P3^0;   // SDA
sbit scl =	P3^1;   // SCL


sbit menu_key=P3^7;
sbit up_key=P3^6;
sbit down_key=P3^5;

unsigned char gio,phut,giay,thang_al,ngay_al,mode = 0;
unsigned char time[7];
///////////////////////////////////////////////////////  
void delay(unsigned int i)
{unsigned int j;
	while(i-->0)
		{   
			for(j=0;j<400;j++)
			{
}
}
} 
void delay_ms(unsigned int t)
{
unsigned int x,y;
for(x=0; x<t; x++);
{
for(y=0; y<123; y++);   //delay 1ms
}
}


/*void delay(unsigned int i)
{unsigned int j;
	while(i-->0)
		{   
			for(j=0;j<400;j++)
			{
}
}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~// 
void nop(void)
{
	unsigned int j=3;
	while(j-->0) { }
}*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 //                    CAC CHUONG TRINH CON GIAO TIEP VOI RTC                   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void start(void) 
   {				
	scl=1;
	sda=1;	
	_nop_();
	_nop_();
	sda=0;
	_nop_();
	scl=0;
   }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void stop(void)	//dk stop: 1 su cdoi trang thai tu muc thap len cao tren duong sda trong khi duong				
    {				// slc dang o muc cao
	
	scl=0;
	_nop_();
	sda=0;
	_nop_();
	scl=1;
	_nop_();
	sda=1;
	}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
bit write(unsigned char x)
    {	
	unsigned char i;
	bit k;	
	for(i=0;i<8;i++)
	    {
		sda=(x&0x80);//? 1:0;  //dua bit du lieu ra chan SDA
		scl=1;					  
	   _nop_();
	   _nop_();			  
		scl=0;
		x<<=1;			   //bit co trong so lon hon dc truyen truoc
	    }
	sda=1;
	scl=1; 			   //nhan bit ACK tu SLAVER	bao hieu ket thuc mot byte du lieu
	_nop_();
	_nop_();
	k=sda ; // nhan gia tri ACK
	scl=0;
	_nop_();	 
	 return k ;
    }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
unsigned char read(bit ack)
{	
	unsigned char Data,i;
	sda=1;	
	for(i=0;i<8;i++) // nhan vao 8 bit
	  {
		scl=1; 
	 	Data<<=1;
		Data=Data|sda;
		scl=0;
     	}
	scl =0;	 	
	sda=ack;//master gui bit ACK
	scl=1;  //master nhan/gui bit du lieu(sda) khi scl o muc cao
	_nop_();
	_nop_();
	scl=0;
	_nop_();//du lieu(sda) thay doi khi scl muc thap/
	sda = 1;
	scl = 1;
	return Data;//tra gia tri cho ham
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void read_ds(void)
{		
	int i;
	start();
	write(0xd0);
	write(0x00);
	start();
	write(0xd1);
	for(i=0;i<6;i++)
    time[i] = read(0);	
	time[6] = read(1);
	stop(); 	  
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void write_ds(void)
     {
	unsigned char t;
	start();
	write(0xd0);
	write(0x0);
	for(t=0;t<7;t++)
		write(time[t]);
	stop();
     }
/**********************************************************************************/
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 //             CHUONG TRINH CON THUC HIEN TINH AM LICH                         //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void am_lich (bit k)
{	
	unsigned int t;
	if((time[2]==0 & time[1] ==0 & time[0] < 3) | (k==1) ) //Tinh lich 00h00p03s
	{
		t = tinhlich(time[4],time[5],time[6]);			
		ngay_al=t & 0xff;
		thang_al= t >> 8;	
	};		
}//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 //     CHUONG TRINH CON THUC HIEN VIEC CAI DAT NGAY GIO, THANG NAM             //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  unsigned char tang(unsigned char bcd,unsigned char min,unsigned char max)// tang so bcd	lên 1 don vi
       {		  // chinh tang so bcd len 1  trong gioi han min,max
	   if(bcd>=max) bcd = min;
	   else if ((bcd& 0xf)==9)  bcd = bcd + 0x10 - 0x9; // neu hang don vi = 9 thi tang hang chuc len 1	    
	   else bcd++;
       return bcd;
         }
  unsigned char giam(unsigned char bcd,unsigned char min,unsigned char max)// giam so bcd xuong 1 don vi
       {		  // giam so bcd di 1
	   if(bcd<=min) bcd = max;
	   else if ((bcd& 0xf)==0)  bcd = bcd - 0x10 + 0x9; // neu hang don vi = 9 thi tang hang chuc len 1	    
	   else bcd--;
       return bcd;
         }
//=======================================================================
void scan_key (void)
{
	static unsigned char l;
  if(menu_key==0) // nut menu
  {
  	delay(10);
	if(menu_key==0)
	{
		while(menu_key==0 & l < 20){ l++; delay(10);};
		delay(10);
		if(l<20) // Nhan de vao cai dat
		            {
		            mode++;
		            if( mode  > 7 )
		                          {
			                         mode = 0;
			                         write_ds(); 
                               am_lich (1);
		                          }
	              }
		if(l>=20) // O cai dat nhan giu thi se thoat vaf khong luu
		            {
								mode = 0; 
                am_lich (1);
								}
	}
  }
	/////////////////////////////////////////////////////////////////////
  else if(up_key==0) // nut down
  {
  	delay(10);
	if(up_key==0)
	{
		while(up_key==0 & l < 20){ l++; delay(10);};
		delay(10);
		if(l>=20) // Nhan giu de tang gia tri cai dat
		            {
	            	if	   (mode ==1 ) time[0] = tang(time[0],0   ,0x59);// tang giay
		          	else if(mode ==2 ) time[1] = tang(time[1],0   ,0x59);// tang phut
		          	else if(mode ==3 ) time[2] = tang(time[2],0   ,0x23);// tang gio
		          	else if(mode ==4 ) time[3] = tang(time[3],0x1 ,0x07);// tang thu
		          	else if(mode ==5 ) time[4] = tang(time[4],1   ,0x31);// tang ngay
		          	else if(mode ==6 ) time[5] = tang(time[5],1   ,0x12);// tang thang
		          	else if(mode ==7 ) time[6] = tang(time[6],0x1 ,0x20);// tang nam
		            }	
		if(l<20) // Nhan de tang
		            {
		          	if	   (mode ==1 ) time[0] = tang(time[0],0   ,0x59);// tang giay
		          	else if(mode ==2 ) time[1] = tang(time[1],0   ,0x59);// tang phut
		          	else if(mode ==3 ) time[2] = tang(time[2],0   ,0x23);// tang gio
		          	else if(mode ==4 ) time[3] = tang(time[3],0x1 ,0x07);// tang thu
		          	else if(mode ==5 ) time[4] = tang(time[4],1   ,0x31);// tang ngay
		          	else if(mode ==6 ) time[5] = tang(time[5],1   ,0x12);// tang thang
		          	else if(mode ==7 ) time[6] = tang(time[6],0x1 ,0x20);// tang nam
		            }	
	}
  }	
	/////////////////////////////////////////////////////////////////////
else if(down_key==0) // nut down
  {
  	delay(10);
	if(down_key==0)
	{
		while(down_key==0 & l < 20){ l++; delay(10);};
		delay(10);
		if(l>=20) // Nhan giu de tang gia tri cai dat
		            {
	            	if	   (mode ==1 ) time[0] = giam(time[0],0   ,0x59);// giam giay
		          	else if(mode ==2 ) time[1] = giam(time[1],0   ,0x59);// giam phut
		          	else if(mode ==3 ) time[2] = giam(time[2],0   ,0x23);// giam gio
		          	else if(mode ==4 ) time[3] = giam(time[3],0x1 ,0x07);// giam thu
		          	else if(mode ==5 ) time[4] = giam(time[4],1   ,0x31);// giam ngay
		          	else if(mode ==6 ) time[5] = giam(time[5],1   ,0x12);// giam thang
		          	else if(mode ==7 ) time[6] = giam(time[6],0x1 ,0x20);// giam nam
		            }	
		if(l<20) // Nhan de tang
		            {
		          	if	   (mode ==1 ) time[0] = giam(time[0],0   ,0x59);// giam giay
		          	else if(mode ==2 ) time[1] = giam(time[1],0   ,0x59);// giam phut
		          	else if(mode ==3 ) time[2] = giam(time[2],0   ,0x23);// giam gio
		          	else if(mode ==4 ) time[3] = giam(time[3],0x1 ,0x07);// giam thu
		          	else if(mode ==5 ) time[4] = giam(time[4],1   ,0x31);// giam ngay
		          	else if(mode ==6 ) time[5] = giam(time[5],1   ,0x12);// giam thang
		          	else if(mode ==7 ) time[6] = giam(time[6],0x1 ,0x20);// giam nam
		            }	
	}
  }
  else l = 0;		
}
//=======================================================================
void kiem_tra_ds (void)
{
  
  read_ds();
	if(time[0] & 0x80) // kiem tra xem ds1307 co chay khong
	{
		time[0] = 0x1;
		time[1] = 0x40;
		time[2] = 0x6;
		time[3] = 0x3;
		time[4] = 0x1;
		time[5] = 0x12;
		time[6] = 0x15;
		write_ds(); 
	}
}
//====================================================================
void main() //Chuong trinh chinh
{	
	kiem_tra_ds();
	am_lich (1);
	delay_ms(500);
	lcd_init();
	delay_ms(100);
	lcd_clear();
	delay_ms(300);
	while(1){
		
		
if(mode ==0)
		    {
        read_ds();
        ReadTemperature(); 
        am_lich (0);
				}
					
		
		
 
	
	      lcd_gotoxy(0,0);
	      lcd_puts_amban("  DAM CLOCK V1  ");
/////////////////////////////////////////////////////////////////		
		    lcd_gotoxy(0,1);
        lcd_putchar16_1(((time[2]%100)>> 4)+48);  //Chuc
        lcd_putchar16_2(((time[2]%100)>> 4)+48);  //Chuc
        lcd_putchar16_1((time[2]& 0xf)+48);	//Don vi
        lcd_putchar16_2((time[2]& 0xf)+48);	//Don vi
        lcd_putchar16_1(':');	//Don vi		
        lcd_putchar16_2(':');	//Don vi
        lcd_putchar16_1(((time[1]%100)>> 4)+48);  //Chuc
        lcd_putchar16_2(((time[1]%100)>> 4)+48);  //Chuc
        lcd_putchar16_1((time[1]& 0xf)+48);	//Don vi
        lcd_putchar16_2((time[1]& 0xf)+48);	//Don vi
        lcd_putchar16_1(':');	//Don vi		
        lcd_putchar16_2(':');	//Don vi
        lcd_putchar16_1(((time[0]%100)>> 4)+48);  //Chuc
        lcd_putchar16_2(((time[0]%100)>> 4)+48);  //Chuc
        lcd_putchar16_1((time[0]& 0xf)+48);	//Don vi
        lcd_putchar16_2((time[0]& 0xf)+48);	//Don vi
			
		    lcd_gotoxy(0,2);
        lcd_putchar16_3(((time[2]%100)>> 4)+48);  //Chuc
        lcd_putchar16_4(((time[2]%100)>> 4)+48);  //Chuc
        lcd_putchar16_3((time[2]& 0xf)+48);	//Don vi
        lcd_putchar16_4((time[2]& 0xf)+48);	//Don vi
        lcd_putchar16_3(':');	//Don vi		
        lcd_putchar16_4(':');	//Don vi
        lcd_putchar16_3(((time[1]%100)>> 4)+48);  //Chuc
        lcd_putchar16_4(((time[1]%100)>> 4)+48);  //Chuc
        lcd_putchar16_3((time[1]& 0xf)+48);	//Don vi
        lcd_putchar16_4((time[1]& 0xf)+48);	//Don vi
        lcd_putchar16_3(':');	//Don vi		
        lcd_putchar16_4(':');	//Don vi
        lcd_putchar16_3(((time[0]%100)>> 4)+48);  //Chuc
        lcd_putchar16_4(((time[0]%100)>> 4)+48);  //Chuc
        lcd_putchar16_3((time[0]& 0xf)+48);	//Don vi
        lcd_putchar16_4((time[0]& 0xf)+48);	//Don vi
////////////////////////////////////////////////////////////////////
	      lcd_gotoxy(0,4);
	      lcd_puts_amban(" 20");
		    lcd_gotoxy(3,4);
        lcd_putchar_amban(((time[6]%100)>> 4)+48);  //Chuc
        lcd_putchar_amban((time[6]& 0xf)+48);	//Don vi
		    lcd_gotoxy(5,4);
	      lcd_puts_amban(" ");
				
	      lcd_gotoxy(10,4);
	      lcd_puts_amban("  AL  ");
	
	      lcd_gotoxy(6,4);
				      if(time[3]==0x01) { lcd_putchar16_1('.');} // Chu C
         else                   { lcd_putchar16_1(',');} // Chu T
	      lcd_gotoxy(7,4);
				      if(time[3]==0x01) { lcd_putchar16_2('.');} // Chu C
         else                   { lcd_putchar16_2(',');} // Chu T
	      lcd_gotoxy(6,5);
				      if(time[3]==0x01) { lcd_putchar16_3('.');} // Chu N
         else                   { lcd_putchar16_3(',');} // Chu T
	      lcd_gotoxy(7,5);
				      if(time[3]==0x01) { lcd_putchar16_4('.');} // Chu N
         else                   { lcd_putchar16_4(',');} // Chu T
	      lcd_gotoxy(8,4);
				      if(time[3]==0x01) { lcd_putchar16_1('-');} // Chu N
         else                   { lcd_putchar16_1((time[3]& 0xf)+48);} // Chu THU		 
	      lcd_gotoxy(9,4);
				      if(time[3]==0x01) { lcd_putchar16_2('-');} // Chu N
         else                   { lcd_putchar16_2((time[3]& 0xf)+48);} // Chu THU				 
	      lcd_gotoxy(8,5);
				      if(time[3]==0x01) { lcd_putchar16_3('-');} // Chu N
         else                   { lcd_putchar16_3((time[3]& 0xf)+48);} // Chu THU		 
	      lcd_gotoxy(9,5);
				      if(time[3]==0x01) { lcd_putchar16_4('-');} // Chu N
         else                   { lcd_putchar16_4((time[3]& 0xf)+48);} // Chu THU
/////////////////////////////////////////////////////////////////////
				lcd_gotoxy(0,5);      // ngay dl
        lcd_putchar(((time[4]%100)>> 4)+48);  //Chuc
        lcd_putchar((time[4]& 0xf)+48);	//Don vi
		    lcd_putchar('/');
		    lcd_gotoxy(3,5);
        lcd_putchar(((time[5]%100)>> 4)+48);  //Chuc
        lcd_putchar((time[5]& 0xf)+48);	//Don vi
				 
		    lcd_gotoxy(11,5);
        lcd_putchar(((ngay_al%100)>> 4)+48);  //Chuc
        lcd_putchar((ngay_al& 0xf)+48);	//Don vi
		    lcd_putchar('/');
		    lcd_gotoxy(14,5);
        lcd_putchar((( thang_al%100)>> 4)+48);  //Chuc
        lcd_putchar(( thang_al& 0xf)+48);	//Don vi
////////////////////////////////////////////////////////////////////
	      lcd_gotoxy(0,6);
	      lcd_puts_4("++++++++++++++++");
/////////////////////// nhiet do /////////////////////////				


				          if ( mode == 0) {	      lcd_gotoxy(3,7);
	                                        lcd_puts_4("*)(");	
				
		                                      lcd_gotoxy(7,7);
                                          lcd_putchar(((temp1%100)/10)+48);  //Chuc
                                          lcd_putchar((temp1%10)+48);	//Don vi
		                                      lcd_putchar(',');
                                          lcd_putchar(((temp2%100)/10)+48);  //Chuc
				                                  lcd_puts("*C    ");	
                                   }
						 else if ( mode == 1) {   lcd_gotoxy(3,7); lcd_puts("SET Giay  ");}
						 else if ( mode == 2) {   lcd_gotoxy(3,7); lcd_puts("SET Phut  ");}
						 else if ( mode == 3) {   lcd_gotoxy(3,7); lcd_puts("SET Gio   ");}
						 else if ( mode == 4) {   lcd_gotoxy(3,7); lcd_puts("SET Thu   ");}
						 else if ( mode == 5) {   lcd_gotoxy(3,7); lcd_puts("SET Ngay  "); am_lich (1);}
						 else if ( mode == 6) {   lcd_gotoxy(3,7); lcd_puts("SET Thang "); am_lich (1);}
						 else if ( mode == 7) {   lcd_gotoxy(3,7); lcd_puts("SET Nam   "); am_lich (1);} 				
				
		    delay_ms(200);
			
				
////////////////////////////////////////////////////////////////////
				scan_key();
				
}
}