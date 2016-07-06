Now the program proceeds to _bootmain_ in _main.c_.

From now I'll explain in C code, since explaining corresponding assembly line by line would not be very necessary and efficient.

###readsect
Start from the innermost function _readsect_ used by _readseg_
```  
readsect(void *dst, uint32_t offset)  
```  
_readsect_ falls into three portions: wait for disk, set read status, read from sector


###set read status
The main portion of _readsect_ is

```  
outb(0x1F2, 1);  
outb(0x1F3, offset);  
outb(0x1F4, offset >> 8);  
outb(0x1F5, offset >> 16);  
outb(0x1F6, (offset >> 24) | 0xE0);  
outb(0x1F7, 0x20);  
```  

First, macro _outb_ defined in _x86.h_  
```  
static __inline void  
outb(int port, uint8_t data)  
{  
	__asm __volatile("outb %0,%w1" : : "a" (data), "d" (port));  
}  
```  
Just a wrapper around assembly code. keyword _\_\_volatile_ tells the compiler take the assembly code as it is without any optimization.  


Then, read/write a hard disk. Most hdd today uses LBA (logical block addressing) instead of CHS (Cylinder, Head, Sector). A translation of two methods can be found [here](https://en.wikipedia.org/wiki/Logical_block_addressing).


[This source](ftp://ftp.seagate.com/acrobat/reference/111-1c.pdf) stated that

>• The logical address is a 28-bit unsigned binary number, which is placed into the command block as follows:  
>– bits 27–24 into the Drive/Head register bits 3–0  
>– bits 23–16 into the Cylinder High register  
>– bits 15–8 into the Cylinder Low register  
>– bits 7–0 into the Sector Number register  

And from [this source](http://wiki.osdev.org/ATA_PIO_Mode#Registers)

|Port Offset |Function |Description|
| --- | --- | --- |
|0    	     |Data Port				|Read/Write PIO data bytes on this port.
|1    	     |Features / Error Information   	|Usually used for ATAPI devices.
|2    	     |Sector Count     			|Number of sectors to read/write (0 is a special value).
|3    	     |Sector Number / LBAlo   		|This is CHS / LBA28 / LBA48 specific.
|4    	     |Cylinder Low / LBAmid   		|Partial Disk Sector address.
|5    	     |Cylinder High / LBAhi   		|Partial Disk Sector address.
|6    	     |Drive / Head Port	     		|Used to select a drive and/or head. May supports extra address/flag bits.
|7    	     |Command port / Regular Status port	    |Used to send commands or read the current status.

So line by line

```  
outb(0x1F2, 1);  
```  
Sector count, one sector.


```  
outb(0x1F3, offset);  
```    
0-7 bits goes to LBAlo. _outb_'s second argument is _uint8\_t_


```  
outb(0x1F4, offset >> 8);  
```  
8-15 bits goes to LBAmid  


```  
outb(0x1F5, offset >> 16);  
```  
16-23 bits goes to LBAhi  


```  
outb(0x1F6, (offset >> 24) | 0xE0);  
```  
24-27 goes to Drive/Head port. Drive/Head register is 8-bit, where in LBA mode only highest 3 bits are used. _0xE0_ is _1110 0000b_. The 7th and 5th bit are always 1. 6th bit is LBA status bit. See [Page 26](ftp://ftp.seagate.com/acrobat/reference/111-1c.pdf)  


```  
outb(0x1F7, 0x20);  
```  
Set read status to disk. _0x20_ is read sector.


###wait for disk
```  
while ((inb(0x1F7) & 0xC0) != 0x40)  
```  
When reading from 0x1F7, it is status register (when writing, it's command register).  
_0xC0_ is _1100 0000b_ and _0x40_ is _0100 0000b_. The highest bit is BSY (busy). The 6th bit is DRDY (disk ready). When disk is ready to write, it should be not busy and ready (0x40; other bits are ignored so masked with 0xC0), so this keeps checking if disk is avaible.


###read from sector
```  
static __inline void  
insl(int port, void *addr, int cnt)  
{  
	__asm __volatile("cld\n\trepne\n\tinsl"			:  
			 "=D" (addr), "=c" (cnt)		:  
			 "d" (port), "0" (addr), "1" (cnt)	:  
			 "memory", "cc");  
}  
```  
inline assembly follows this format

>asm ( "statements" : output_registers : input_registers : clobbered_registers);


and called by  

```  
insl(0x1F0, dst, SECTSIZE/4);  
```  
reads four byte a time (long type), so number of reads = SECTSIZE / 4


```  
cld
```    
clear data flag


```  
trepne  
tinsl 
```  
read _cx_ bytes from port _dx_ and save in _es:di_. Here _cx_ is _cnt_, _dx_ is _port_ and _di_ is _addr_.  

see (command description)[http://x86.renejeschke.de/html/file_module_x86_id_279.html]  



###(To be continued ...)

###readseg
```  
readseg(uint32_t pa, uint32_t count, uint32_t offset)  
```  

Takes three arguments  
pa: physical address
count: size of sector in bits
offset: 