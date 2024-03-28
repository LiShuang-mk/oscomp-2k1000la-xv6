
./kernel/kernel:     file format elf64-loongarch


Disassembly of section .text:

9000000000200000 <_ZN7ConsoleC1Ev>:
9000000000200000:	02ffc063 	addi.d      	$sp, $sp, -16
9000000000200004:	29c02061 	st.d        	$ra, $sp, 8
9000000000200008:	27000076 	stptr.d     	$fp, $sp, 0
900000000020000c:	02c04076 	addi.d      	$fp, $sp, 16
9000000000200010:	54006400 	bl          	100	# 9000000000200074 <_ZN4LockC1Ev>
9000000000200014:	28c02061 	ld.d        	$ra, $sp, 8
9000000000200018:	26000076 	ldptr.d     	$fp, $sp, 0
900000000020001c:	02c04063 	addi.d      	$sp, $sp, 16
9000000000200020:	4c000020 	ret         

9000000000200024 <_ZN7Console4initEPKc>:
9000000000200024:	02ffc063 	addi.d      	$sp, $sp, -16
9000000000200028:	29c02061 	st.d        	$ra, $sp, 8
900000000020002c:	27000076 	stptr.d     	$fp, $sp, 0
9000000000200030:	02c04076 	addi.d      	$fp, $sp, 16
9000000000200034:	54005800 	bl          	88	# 900000000020008c <_ZN4Lock4initEPKc>
9000000000200038:	28c02061 	ld.d        	$ra, $sp, 8
900000000020003c:	26000076 	ldptr.d     	$fp, $sp, 0
9000000000200040:	02c04063 	addi.d      	$sp, $sp, 16
9000000000200044:	4c000020 	ret         

9000000000200048 <_GLOBAL__sub_I_k_console>:
9000000000200048:	02ffc063 	addi.d      	$sp, $sp, -16
900000000020004c:	29c02061 	st.d        	$ra, $sp, 8
9000000000200050:	27000076 	stptr.d     	$fp, $sp, 0
9000000000200054:	02c04076 	addi.d      	$fp, $sp, 16
9000000000200058:	1a000024 	pcalau12i   	$a0, 1
900000000020005c:	02c4c084 	addi.d      	$a0, $a0, 304
9000000000200060:	57ffa3ff 	bl          	-96	# 9000000000200000 <_ZN7ConsoleC1Ev>
9000000000200064:	28c02061 	ld.d        	$ra, $sp, 8
9000000000200068:	26000076 	ldptr.d     	$fp, $sp, 0
900000000020006c:	02c04063 	addi.d      	$sp, $sp, 16
9000000000200070:	4c000020 	ret         

9000000000200074 <_ZN4LockC1Ev>:
9000000000200074:	02ffc063 	addi.d      	$sp, $sp, -16
9000000000200078:	29c02076 	st.d        	$fp, $sp, 8
900000000020007c:	02c04076 	addi.d      	$fp, $sp, 16
9000000000200080:	28c02076 	ld.d        	$fp, $sp, 8
9000000000200084:	02c04063 	addi.d      	$sp, $sp, 16
9000000000200088:	4c000020 	ret         

900000000020008c <_ZN4Lock4initEPKc>:
900000000020008c:	02ffc063 	addi.d      	$sp, $sp, -16
9000000000200090:	29c02076 	st.d        	$fp, $sp, 8
9000000000200094:	02c04076 	addi.d      	$fp, $sp, 16
9000000000200098:	27000085 	stptr.d     	$a1, $a0, 0
900000000020009c:	29802080 	st.w        	$zero, $a0, 8
90000000002000a0:	29803080 	st.w        	$zero, $a0, 12
90000000002000a4:	28c02076 	ld.d        	$fp, $sp, 8
90000000002000a8:	02c04063 	addi.d      	$sp, $sp, 16
90000000002000ac:	4c000020 	ret         

90000000002000b0 <main>:
90000000002000b0:	02ffc063 	addi.d      	$sp, $sp, -16
90000000002000b4:	29c02061 	st.d        	$ra, $sp, 8
90000000002000b8:	27000076 	stptr.d     	$fp, $sp, 0
90000000002000bc:	02c04076 	addi.d      	$fp, $sp, 16
90000000002000c0:	1a000025 	pcalau12i   	$a1, 1
90000000002000c4:	02c000a5 	addi.d      	$a1, $a1, 0
90000000002000c8:	1a000024 	pcalau12i   	$a0, 1
90000000002000cc:	28c48084 	ld.d        	$a0, $a0, 288
90000000002000d0:	57ff57ff 	bl          	-172	# 9000000000200024 <_ZN7Console4initEPKc>
90000000002000d4:	50000000 	b           	0	# 90000000002000d4 <main+0x24>

90000000002000d8 <_entry>:
90000000002000d8:	1a00002c 	pcalau12i   	$t0, 1
90000000002000dc:	28c4418c 	ld.d        	$t0, $t0, 272
90000000002000e0:	4c000180 	jr          	$t0
90000000002000e4:	0380440c 	li.w        	$t0, 0x11
90000000002000e8:	0324018c 	lu52i.d     	$t0, $t0, -1792
90000000002000ec:	0406002c 	csrwr       	$t0, 0x180
90000000002000f0:	0015000c 	move        	$t0, $zero
90000000002000f4:	0406042c 	csrwr       	$t0, 0x181
90000000002000f8:	0406082c 	csrwr       	$t0, 0x182
90000000002000fc:	04060c2c 	csrwr       	$t0, 0x183
9000000000200100:	0402202c 	csrwr       	$t0, 0x88
9000000000200104:	0382c00c 	li.w        	$t0, 0xb0
9000000000200108:	0400002c 	csrwr       	$t0, 0x0
900000000020010c:	06498000 	invtlb      	0x0, $zero, $zero
9000000000200110:	18008183 	pcaddi      	$sp, 1036
9000000000200114:	14000024 	lu12i.w     	$a0, 1
9000000000200118:	04008005 	csrrd       	$a1, 0x20
900000000020011c:	02c000a2 	addi.d      	$tp, $a1, 0
9000000000200120:	02c004a5 	addi.d      	$a1, $a1, 1
9000000000200124:	001d9484 	mul.d       	$a0, $a0, $a1
9000000000200128:	00109063 	add.d       	$sp, $sp, $a0
900000000020012c:	57ff87ff 	bl          	-124	# 90000000002000b0 <main>

9000000000200130 <spin>:
9000000000200130:	50000000 	b           	0	# 9000000000200130 <spin>
	...

Disassembly of section .rodata:

9000000000201000 <.rodata>:
9000000000201000:	736e6f63 	vssrarni.du.q	$vr3, $vr27, 0x1b
9000000000201004:	00656c6f 	bstrins.w   	$t3, $sp, 0x5, 0x1b

Disassembly of section .eh_frame:

9000000000201008 <.eh_frame>:
9000000000201008:	00000010 	.word		0x00000010
900000000020100c:	00000000 	.word		0x00000000
9000000000201010:	00527a01 	.word		0x00527a01
9000000000201014:	01017c01 	fadd.d      	$fa1, $fa0, $fs7
9000000000201018:	00030d1b 	.word		0x00030d1b
900000000020101c:	00000020 	.word		0x00000020
9000000000201020:	00000018 	.word		0x00000018
9000000000201024:	ffffefdc 	.word		0xffffefdc
9000000000201028:	00000024 	.word		0x00000024
900000000020102c:	100e4400 	addu16i.d   	$zero, $zero, 913
9000000000201030:	96028148 	.word		0x96028148
9000000000201034:	160c4404 	lu32i.d     	$a0, 25120
9000000000201038:	44c14800 	bnez        	$zero, 49480	# 900000000020d180 <stack0+0xc040>
900000000020103c:	030d44d6 	lu52i.d     	$fp, $a2, 849
9000000000201040:	00000020 	.word		0x00000020
9000000000201044:	0000003c 	.word		0x0000003c
9000000000201048:	ffffefdc 	.word		0xffffefdc
900000000020104c:	00000024 	.word		0x00000024
9000000000201050:	100e4400 	addu16i.d   	$zero, $zero, 913
9000000000201054:	96028148 	.word		0x96028148
9000000000201058:	160c4404 	lu32i.d     	$a0, 25120
900000000020105c:	44c14800 	bnez        	$zero, 49480	# 900000000020d1a4 <stack0+0xc064>
9000000000201060:	030d44d6 	lu52i.d     	$fp, $a2, 849
9000000000201064:	00000020 	.word		0x00000020
9000000000201068:	00000060 	.word		0x00000060
900000000020106c:	ffffefdc 	.word		0xffffefdc
9000000000201070:	0000002c 	.word		0x0000002c
9000000000201074:	100e4400 	addu16i.d   	$zero, $zero, 913
9000000000201078:	96028148 	.word		0x96028148
900000000020107c:	160c4404 	lu32i.d     	$a0, 25120
9000000000201080:	44c15000 	bnez        	$zero, 49488	# 900000000020d1d0 <stack0+0xc090>
9000000000201084:	030d44d6 	lu52i.d     	$fp, $a2, 849
9000000000201088:	0000001c 	.word		0x0000001c
900000000020108c:	00000084 	.word		0x00000084
9000000000201090:	ffffefe4 	.word		0xffffefe4
9000000000201094:	00000018 	.word		0x00000018
9000000000201098:	100e4400 	addu16i.d   	$zero, $zero, 913
900000000020109c:	44029644 	bnez        	$t6, 1049236	# 9000000000301330 <stack0+0x1001f0>
90000000002010a0:	4400160c 	bnez        	$t4, 3145748	# 90000000005010b4 <stack0+0x2fff74>
90000000002010a4:	030d44d6 	lu52i.d     	$fp, $a2, 849
90000000002010a8:	0000001c 	.word		0x0000001c
90000000002010ac:	000000a4 	.word		0x000000a4
90000000002010b0:	ffffefdc 	.word		0xffffefdc
90000000002010b4:	00000024 	.word		0x00000024
90000000002010b8:	100e4400 	addu16i.d   	$zero, $zero, 913
90000000002010bc:	44029644 	bnez        	$t6, 1049236	# 9000000000301350 <stack0+0x100210>
90000000002010c0:	5000160c 	b           	-131071980	# 8ffffffff85010d4 <_ZN7ConsoleC1Ev-0x7cfef2c>
90000000002010c4:	030d44d6 	lu52i.d     	$fp, $a2, 849
90000000002010c8:	0000001c 	.word		0x0000001c
90000000002010cc:	000000c4 	.word		0x000000c4
90000000002010d0:	ffffefe0 	.word		0xffffefe0
90000000002010d4:	00000028 	.word		0x00000028
90000000002010d8:	100e4400 	addu16i.d   	$zero, $zero, 913
90000000002010dc:	96028148 	.word		0x96028148
90000000002010e0:	160c4404 	lu32i.d     	$a0, 25120
90000000002010e4:	00000000 	.word		0x00000000

Disassembly of section .rela.dyn:

90000000002010e8 <.rela.dyn>:
	...

Disassembly of section .init_array:

9000000000201100 <.init_array>:
9000000000201100:	00200048 	div.w       	$a4, $tp, $zero
9000000000201104:	90000000 	.word		0x90000000

Disassembly of section .got:

9000000000201108 <_GLOBAL_OFFSET_TABLE_>:
	...
9000000000201110:	002000e4 	div.w       	$a0, $a3, $zero
9000000000201114:	90000000 	.word		0x90000000
	...
9000000000201120:	00201130 	div.w       	$t4, $a5, $a0
9000000000201124:	90000000 	.word		0x90000000

Disassembly of section .debug_info:

0000000000000000 <.debug_info>:
   0:	0000026d 	.word		0x0000026d
   4:	08010005 	.word		0x08010005
   8:	00000000 	.word		0x00000000
   c:	0000000c 	.word		0x0000000c
  10:	00002100 	clo.d       	$zero, $a4
  14:	000b0000 	.word		0x000b0000
  18:	00000000 	.word		0x00000000
  1c:	00000020 	.word		0x00000020
  20:	00749000 	bstrpick.w  	$zero, $zero, 0x14, 0x4
	...
  2c:	f50d0000 	.word		0xf50d0000
  30:	04000000 	csrrd       	$zero, 0x0
  34:	003a070b 	.word		0x003a070b
  38:	04010000 	csrrd       	$zero, 0x40
  3c:	00011807 	.word		0x00011807
  40:	07020100 	.word		0x07020100
  44:	0000018b 	.word		0x0000018b
  48:	61080101 	blt         	$a4, $ra, 67584	# 10848 <_ZN7ConsoleC1Ev-0x90000000001ef7b8>
  4c:	01000001 	.word		0x01000001
  50:	01130708 	fcopysign.d 	$ft0, $fs0, $fa1
  54:	9e070000 	.word		0x9e070000
  58:	02000001 	slti        	$ra, $zero, 0
  5c:	000000be 	.word		0x000000be
  60:	00012504 	.word		0x00012504
  64:	0e100200 	.word		0x0e100200
  68:	000000be 	.word		0x000000be
  6c:	016f0400 	.word		0x016f0400
  70:	11020000 	addu16i.d   	$zero, $zero, 16512
  74:	00002e08 	ctz.d       	$a4, $t4
  78:	630e0800 	bltz        	$zero, -61944	# ffffffffffff0e80 <stack0+0x6fffffffffdefd40>
  7c:	02007570 	slti        	$t4, $a7, 29
  80:	002e0812 	.word		0x002e0812
  84:	0f0c0000 	.word		0x0f0c0000
  88:	0000019e 	.word		0x0000019e
  8c:	d0021402 	.word		0xd0021402
  90:	01000000 	.word		0x01000000
  94:	0000009d 	.word		0x0000009d
  98:	0000a300 	.word		0x0000a300
  9c:	00cf0200 	bstrpick.d  	$zero, $t4, 0xf, 0x0
  a0:	08000000 	.word		0x08000000
  a4:	000000cb 	.word		0x000000cb
  a8:	00e31502 	bstrpick.d  	$tp, $a4, 0x23, 0x5
  ac:	00b20000 	bstrins.d   	$zero, $zero, 0x32, 0x0
  b0:	cf020000 	.word		0xcf020000
  b4:	09000000 	.word		0x09000000
  b8:	000000be 	.word		0x000000be
  bc:	ca050000 	.word		0xca050000
  c0:	01000000 	.word		0x01000000
  c4:	016a0601 	.word		0x016a0601
  c8:	c3060000 	.word		0xc3060000
  cc:	05000000 	gcsrrd      	$zero, 0x0
  d0:	00000056 	.word		0x00000056
  d4:	0000cf06 	.word		0x0000cf06
  d8:	01b30700 	.word		0x01b30700
  dc:	26030000 	ldptr.d     	$zero, $zero, 768
  e0:	04000001 	csrrd       	$ra, 0x0
  e4:	000000dd 	.word		0x000000dd
  e8:	56071003 	bl          	919312	# e07f8 <_ZN7ConsoleC1Ev-0x900000000011f808>
  ec:	00000000 	.word		0x00000000
  f0:	0001b310 	.word		0x0001b310
  f4:	02130300 	slti        	$zero, $s1, 1216
  f8:	0000017b 	.word		0x0000017b
  fc:	00010501 	.word		0x00010501
 100:	00010b00 	asrtle.d    	$s1, $tp
 104:	01260200 	.word		0x01260200
 108:	08000000 	.word		0x08000000
 10c:	000000cb 	.word		0x000000cb
 110:	00b61403 	bstrins.d   	$sp, $zero, 0x36, 0x5
 114:	011a0000 	.word		0x011a0000
 118:	26020000 	ldptr.d     	$zero, $zero, 512
 11c:	09000001 	.word		0x09000001
 120:	000000be 	.word		0x000000be
 124:	d9050000 	.word		0xd9050000
 128:	06000000 	cacop       	0x0, $zero, 0
 12c:	00000126 	.word		0x00000126
 130:	00010911 	.word		0x00010911
 134:	10170300 	addu16i.d   	$zero, $s1, 1472
 138:	000000d9 	.word		0x000000d9
 13c:	00013012 	.word		0x00013012
 140:	090b0100 	.word		0x090b0100
 144:	11300309 	addu16i.d   	$a5, $s1, 19456
 148:	00000020 	.word		0x00000020
 14c:	87139000 	.word		0x87139000
 150:	2a000000 	ld.bu       	$zero, $zero, 0
 154:	5f000001 	bne         	$zero, $ra, -65536	# ffffffffffff0154 <stack0+0x6fffffffffdef014>
 158:	69000001 	bltu        	$zero, $ra, 65536	# 10158 <_ZN7ConsoleC1Ev-0x90000000001efea8>
 15c:	0a000001 	.word		0x0a000001
 160:	00000176 	.word		0x00000176
 164:	000000d4 	.word		0x000000d4
 168:	00fa1400 	bstrpick.d  	$zero, $zero, 0x3a, 0x5
 16c:	00480000 	.word		0x00480000
 170:	00000020 	.word		0x00000020
 174:	002c9000 	alsl.d      	$zero, $zero, $a0, 0x2
 178:	00000000 	.word		0x00000000
 17c:	9c010000 	.word		0x9c010000
 180:	000001b5 	.word		0x000001b5
 184:	0001b515 	.word		0x0001b515
 188:	20005800 	ll.w        	$zero, $zero, 88
 18c:	00000000 	.word		0x00000000
 190:	00000c90 	.word		0x00000c90
 194:	00000000 	.word		0x00000000
 198:	01130100 	fcopysign.d 	$fa0, $ft0, $fa0
 19c:	20006416 	ll.w        	$fp, $zero, 100
 1a0:	00000000 	.word		0x00000000
 1a4:	54010390 	bl          	-29359872	# fffffffffe4002a4 <stack0+0x6ffffffffe1ff164>
 1a8:	11300309 	addu16i.d   	$a5, $s1, 19456
 1ac:	00000020 	.word		0x00000020
 1b0:	00009000 	.word		0x00009000
 1b4:	01371700 	.word		0x01371700
 1b8:	18010000 	pcaddi      	$zero, 2048
 1bc:	0000010b 	.word		0x0000010b
 1c0:	dd060f01 	.word		0xdd060f01
 1c4:	24000001 	ldptr.w     	$ra, $zero, 0
 1c8:	00002000 	clo.d       	$zero, $zero
 1cc:	24900000 	ldptr.w     	$zero, $zero, -28672
 1d0:	00000000 	.word		0x00000000
 1d4:	01000000 	.word		0x01000000
 1d8:	0002179c 	.word		0x0002179c
 1dc:	01761900 	.word		0x01761900
 1e0:	012b0000 	.word		0x012b0000
 1e4:	000c0000 	bytepick.d  	$zero, $zero, $zero, 0x0
 1e8:	251a0000 	stptr.w     	$zero, $zero, 6656
 1ec:	01000001 	.word		0x01000001
 1f0:	00be210f 	bstrins.d   	$t3, $a4, 0x3e, 0x8
 1f4:	00360000 	.word		0x00360000
 1f8:	380b0000 	.word		0x380b0000
 1fc:	00002000 	clo.d       	$zero, $zero
 200:	a3900000 	.word		0xa3900000
 204:	03000000 	lu52i.d     	$zero, $zero, 0
 208:	a3035401 	.word		0xa3035401
 20c:	01035401 	fsub.d      	$fa1, $fa0, $ft13
 210:	01a30355 	.word		0x01a30355
 214:	1b000055 	pcalau12i   	$r21, -524286
 218:	000000f0 	.word		0x000000f0
 21c:	28010d01 	ld.b        	$ra, $a4, 67
 220:	00000002 	.word		0x00000002
 224:	00000232 	.word		0x00000232
 228:	0001760a 	.word		0x0001760a
 22c:	00012b00 	asrtle.d    	$s1, $a6
 230:	171c0000 	lu32i.d     	$zero, -466944
 234:	a3000002 	.word		0xa3000002
 238:	51000001 	b           	327680	# 50238 <_ZN7ConsoleC1Ev-0x90000000001afdc8>
 23c:	00000002 	.word		0x00000002
 240:	00002000 	clo.d       	$zero, $zero
 244:	24900000 	ldptr.w     	$zero, $zero, -28672
 248:	00000000 	.word		0x00000000
 24c:	01000000 	.word		0x01000000
 250:	02281d9c 	slti        	$s5, $t0, -1529
 254:	00600000 	bstrins.w   	$zero, $zero, 0x0, 0x0
 258:	140b0000 	lu12i.w     	$zero, 22528
 25c:	00002000 	clo.d       	$zero, $zero
 260:	4e900000 	jirl        	$zero, $zero, -94208
 264:	03000001 	lu52i.d     	$ra, $zero, 0
 268:	a3035401 	.word		0xa3035401
 26c:	00005401 	bitrev.d    	$ra, $zero
 270:	00015600 	asrtle.d    	$t4, $r21
 274:	01000500 	.word		0x01000500
 278:	0001ba08 	.word		0x0001ba08
 27c:	00000600 	.word		0x00000600
 280:	62210000 	bltz        	$zero, -122624	# fffffffffffe2380 <stack0+0x6fffffffffde1240>
 284:	0b000000 	.word		0x0b000000
 288:	74000000 	xvseq.b     	$xr0, $xr0, $xr0
 28c:	00002000 	clo.d       	$zero, $zero
 290:	3c900000 	.word		0x3c900000
 294:	00000000 	.word		0x00000000
 298:	aa000000 	.word		0xaa000000
 29c:	07000000 	.word		0x07000000
 2a0:	000000f5 	.word		0x000000f5
 2a4:	3a070b03 	.word		0x3a070b03
 2a8:	01000000 	.word		0x01000000
 2ac:	01180704 	.word		0x01180704
 2b0:	02010000 	slti        	$zero, $zero, 64
 2b4:	00018b07 	.word		0x00018b07
 2b8:	08010100 	.word		0x08010100
 2bc:	00000161 	.word		0x00000161
 2c0:	13070801 	addu16i.d   	$ra, $zero, -15934
 2c4:	08000001 	.word		0x08000001
 2c8:	0000019e 	.word		0x0000019e
 2cc:	070d0210 	.word		0x070d0210
 2d0:	000000c0 	.word		0x000000c0
 2d4:	00012502 	.word		0x00012502
 2d8:	c00e1000 	.word		0xc00e1000
 2dc:	00000000 	.word		0x00000000
 2e0:	00016f02 	.word		0x00016f02
 2e4:	2e081100 	ldl.w       	$zero, $a4, 516
 2e8:	08000000 	.word		0x08000000
 2ec:	75706309 	.word		0x75706309
 2f0:	08120200 	fmadd.s     	$fa0, $ft8, $fa0, $fa4
 2f4:	0000002e 	.word		0x0000002e
 2f8:	019e0a0c 	.word		0x019e0a0c
 2fc:	14020000 	lu12i.w     	$zero, 4096
 300:	0000d002 	.word		0x0000d002
 304:	009d0100 	bstrins.d   	$zero, $a4, 0x1d, 0x0
 308:	00a30000 	bstrins.d   	$zero, $zero, 0x23, 0x0
 30c:	d1030000 	.word		0xd1030000
 310:	00000000 	.word		0x00000000
 314:	0000cb0b 	.word		0x0000cb0b
 318:	07150200 	.word		0x07150200
 31c:	000000e3 	.word		0x000000e3
 320:	0000b401 	.word		0x0000b401
 324:	00d10300 	bstrpick.d  	$zero, $s1, 0x11, 0x0
 328:	c00c0000 	.word		0xc00c0000
 32c:	00000000 	.word		0x00000000
 330:	00cc0400 	bstrpick.d  	$zero, $zero, 0xc, 0x1
 334:	01010000 	fadd.d      	$fa0, $fa0, $fa0
 338:	00016a06 	.word		0x00016a06
 33c:	00c50500 	bstrpick.d  	$zero, $a4, 0x5, 0x1
 340:	56040000 	bl          	132096	# 20740 <_ZN7ConsoleC1Ev-0x90000000001df8c0>
 344:	05000000 	gcsrrd      	$zero, 0x0
 348:	000000d1 	.word		0x000000d1
 34c:	0000a30d 	.word		0x0000a30d
 350:	060d0100 	cacop       	0x0, $a4, 832
 354:	000000fd 	.word		0x000000fd
 358:	0020008c 	div.w       	$t0, $a0, $zero
 35c:	90000000 	.word		0x90000000
 360:	00000024 	.word		0x00000024
 364:	00000000 	.word		0x00000000
 368:	01179c01 	.word		0x01179c01
 36c:	760e0000 	.word		0x760e0000
 370:	d6000001 	.word		0xd6000001
 374:	01000000 	.word		0x01000000
 378:	01250f54 	.word		0x01250f54
 37c:	0d010000 	fsel        	$fa0, $fa0, $fa0, $fcc2
 380:	0000c01e 	.word		0x0000c01e
 384:	00550100 	.word		0x00550100
 388:	00008810 	.word		0x00008810
 38c:	010b0100 	fmin.d      	$fa0, $ft0, $fa0
 390:	00000128 	.word		0x00000128
 394:	00013200 	asrtle.d    	$t4, $t0
 398:	01761100 	.word		0x01761100
 39c:	00d60000 	bstrpick.d  	$zero, $zero, 0x16, 0x0
 3a0:	12000000 	addu16i.d   	$zero, $zero, -32768
 3a4:	00000117 	.word		0x00000117
 3a8:	000001bb 	.word		0x000001bb
 3ac:	00000151 	.word		0x00000151
 3b0:	00200074 	div.w       	$t8, $sp, $zero
 3b4:	90000000 	.word		0x90000000
 3b8:	00000018 	.word		0x00000018
 3bc:	00000000 	.word		0x00000000
 3c0:	28139c01 	ld.b        	$ra, $zero, 1255
 3c4:	01000001 	.word		0x01000001
 3c8:	97000054 	.word		0x97000054
 3cc:	05000001 	gcsrrd      	$ra, 0x0
 3d0:	de080100 	.word		0xde080100
 3d4:	09000002 	.word		0x09000002
 3d8:	00000000 	.word		0x00000000
 3dc:	00006a21 	rdtime.d    	$ra, $t5
 3e0:	00000b00 	movgr2scr   	$scr0, $s1
 3e4:	2000b000 	ll.w        	$zero, $zero, 176
 3e8:	00000000 	.word		0x00000000
 3ec:	00002890 	cto.d       	$t4, $a0
 3f0:	00000000 	.word		0x00000000
 3f4:	00015300 	asrtle.d    	$s1, $t8
 3f8:	00f50a00 	bstrpick.d  	$zero, $t4, 0x35, 0x2
 3fc:	0b040000 	.word		0x0b040000
 400:	00003a07 	revb.2w     	$a3, $t4
 404:	07040100 	.word		0x07040100
 408:	00000118 	.word		0x00000118
 40c:	8b070201 	.word		0x8b070201
 410:	01000001 	.word		0x01000001
 414:	01610801 	.word		0x01610801
 418:	08010000 	.word		0x08010000
 41c:	00011307 	.word		0x00011307
 420:	019e0500 	.word		0x019e0500
 424:	bb020000 	.word		0xbb020000
 428:	03000000 	lu52i.d     	$zero, $zero, 0
 42c:	00000125 	.word		0x00000125
 430:	bb0e1002 	.word		0xbb0e1002
 434:	00000000 	.word		0x00000000
 438:	00016f03 	.word		0x00016f03
 43c:	08110200 	fmadd.s     	$fa0, $ft8, $fa0, $fa2
 440:	0000002e 	.word		0x0000002e
 444:	70630b08 	vabsd.wu    	$vr8, $vr24, $vr2
 448:	12020075 	addu16i.d   	$r21, $sp, -32640
 44c:	00002e08 	ctz.d       	$a4, $t4
 450:	9e060c00 	.word		0x9e060c00
 454:	02000001 	slti        	$ra, $zero, 0
 458:	0000d014 	.word		0x0000d014
 45c:	00009a00 	.word		0x00009a00
 460:	0000a000 	.word		0x0000a000
 464:	00cc0200 	bstrpick.d  	$zero, $t4, 0xc, 0x0
 468:	07000000 	.word		0x07000000
 46c:	000000cb 	.word		0x000000cb
 470:	00e31502 	bstrpick.d  	$tp, $a4, 0x23, 0x5
 474:	00af0000 	bstrins.d   	$zero, $zero, 0x2f, 0x0
 478:	cc020000 	.word		0xcc020000
 47c:	08000000 	.word		0x08000000
 480:	000000bb 	.word		0x000000bb
 484:	c7040000 	.word		0xc7040000
 488:	01000000 	.word		0x01000000
 48c:	016a0601 	.word		0x016a0601
 490:	c00c0000 	.word		0xc00c0000
 494:	04000000 	csrrd       	$zero, 0x0
 498:	00000056 	.word		0x00000056
 49c:	0001b305 	.word		0x0001b305
 4a0:	011c0300 	.word		0x011c0300
 4a4:	dd030000 	.word		0xdd030000
 4a8:	03000000 	lu52i.d     	$zero, $zero, 0
 4ac:	00560710 	.word		0x00560710
 4b0:	06000000 	cacop       	0x0, $zero, 0
 4b4:	000001b3 	.word		0x000001b3
 4b8:	017b1303 	.word		0x017b1303
 4bc:	00fb0000 	bstrpick.d  	$zero, $zero, 0x3b, 0x0
 4c0:	01010000 	fadd.d      	$fa0, $fa0, $fa0
 4c4:	1c020000 	pcaddu12i   	$zero, 4096
 4c8:	00000001 	.word		0x00000001
 4cc:	0000cb07 	.word		0x0000cb07
 4d0:	b6140300 	.word		0xb6140300
 4d4:	10000000 	addu16i.d   	$zero, $zero, 0
 4d8:	02000001 	slti        	$ra, $zero, 0
 4dc:	0000011c 	.word		0x0000011c
 4e0:	0000bb08 	.word		0x0000bb08
 4e4:	04000000 	csrrd       	$zero, 0x0
 4e8:	000000d1 	.word		0x000000d1
 4ec:	0001090d 	.word		0x0001090d
 4f0:	10170300 	addu16i.d   	$zero, $s1, 1472
 4f4:	000000d1 	.word		0x000000d1
 4f8:	0000c00e 	.word		0x0000c00e
 4fc:	00013e00 	asrtle.d    	$t4, $t3
 500:	004f0f00 	.word		0x004f0f00
 504:	0fff0000 	.word		0x0fff0000
 508:	01c81000 	.word		0x01c81000
 50c:	06010000 	cacop       	0x0, $zero, 64
 510:	00012d29 	.word		0x00012d29
 514:	03091000 	lu52i.d     	$zero, $zero, 580
 518:	00201140 	div.w       	$zero, $a6, $a0
 51c:	90000000 	.word		0x90000000
 520:	0001cf11 	.word		0x0001cf11
 524:	05080100 	gcsrxchg    	$zero, $a4, 0x200
 528:	00000193 	.word		0x00000193
 52c:	002000b0 	div.w       	$t4, $a1, $zero
 530:	90000000 	.word		0x90000000
 534:	00000028 	.word		0x00000028
 538:	00000000 	.word		0x00000000
 53c:	01939c01 	.word		0x01939c01
 540:	d4120000 	.word		0xd4120000
 544:	00002000 	clo.d       	$zero, $zero
 548:	01900000 	.word		0x01900000
 54c:	13000001 	addu16i.d   	$ra, $zero, -16384
 550:	03095501 	lu52i.d     	$ra, $a4, 597
 554:	00201000 	div.w       	$zero, $zero, $a0
 558:	90000000 	.word		0x90000000
 55c:	04140000 	csrrd       	$zero, 0x500
 560:	746e6905 	.word		0x746e6905
	...

Disassembly of section .debug_abbrev:

0000000000000000 <.debug_abbrev>:
   0:	0b002401 	.word		0x0b002401
   4:	030b3e0b 	lu52i.d     	$a7, $t4, 719
   8:	0200000e 	slti        	$t2, $zero, 0
   c:	13490005 	addu16i.d   	$a1, $zero, -11712
  10:	00001934 	cto.w       	$t8, $a5
  14:	02004903 	slti        	$sp, $a4, 18
  18:	00187e18 	sra.w       	$s1, $t4, $s8
  1c:	000d0400 	bytepick.d  	$zero, $zero, $ra, 0x2
  20:	0b3a0e03 	.word		0x0b3a0e03
  24:	0b390b3b 	.word		0x0b390b3b
  28:	0b381349 	.word		0x0b381349
  2c:	0f050000 	.word		0x0f050000
  30:	08210b00 	fmadd.d     	$fa0, $fs0, $fa2, $fa2
  34:	00001349 	clo.w       	$a5, $s3
  38:	49002606 	jiscr0      	1638436
  3c:	07000013 	.word		0x07000013
  40:	0e030102 	.word		0x0e030102
  44:	3a10210b 	.word		0x3a10210b
  48:	0d213b0b 	xvbitsel.v  	$xr11, $xr24, $xr14, $xr2
  4c:	01072139 	fdiv.d      	$fs1, $ft1, $ft0
  50:	08000013 	.word		0x08000013
  54:	193f012e 	pcaddi      	$t2, -395255
  58:	0b3a0e03 	.word		0x0b3a0e03
  5c:	21390b3b 	sc.w        	$s4, $s2, 14600
  60:	320e6e07 	.word		0x320e6e07
  64:	193c0121 	pcaddi      	$ra, -401399
  68:	00001364 	clo.w       	$a0, $s4
  6c:	49000509 	bcnez       	$fcc0, 2424836	# 250070 <_ZN7ConsoleC1Ev-0x8ffffffffffaff90>
  70:	0a000013 	.word		0x0a000013
  74:	0e030005 	.word		0x0e030005
  78:	19341349 	pcaddi      	$a5, -417638
  7c:	480b0000 	bceqz       	$fcc0, 2816	# b7c <_ZN7ConsoleC1Ev-0x90000000001ff484>
  80:	7f017d01 	.word		0x7f017d01
  84:	0c000013 	.word		0x0c000013
  88:	0e250111 	.word		0x0e250111
  8c:	1f030b13 	pcaddu18i   	$t7, -518056
  90:	01111f1b 	fscaleb.d   	$fs3, $fs0, $fa7
  94:	17100712 	lu32i.d     	$t6, -491464
  98:	160d0000 	lu32i.d     	$zero, 26624
  9c:	3a0e0300 	.word		0x3a0e0300
  a0:	390b3b0b 	.word		0x390b3b0b
  a4:	0013490b 	maskeqz     	$a7, $a4, $t6
  a8:	000d0e00 	bytepick.d  	$zero, $t4, $sp, 0x2
  ac:	0b3a0803 	.word		0x0b3a0803
  b0:	0b390b3b 	.word		0x0b390b3b
  b4:	0b381349 	.word		0x0b381349
  b8:	2e0f0000 	ldl.w       	$zero, $zero, 960
  bc:	03193f01 	lu52i.d     	$ra, $s1, 1615
  c0:	3b0b3a0e 	.word		0x3b0b3a0e
  c4:	6e0b390b 	bgeu        	$a4, $a7, -128200	# fffffffffffe0bfc <stack0+0x6fffffffffddfabc>
  c8:	3c0b320e 	.word		0x3c0b320e
  cc:	20136419 	ll.w        	$s2, $zero, 4964
  d0:	0013010b 	maskeqz     	$a7, $a4, $zero
  d4:	012e1000 	.word		0x012e1000
  d8:	0e03193f 	.word		0x0e03193f
  dc:	0b3b0b3a 	.word		0x0b3b0b3a
  e0:	0e6e0b39 	.word		0x0e6e0b39
  e4:	193c0b32 	pcaddi      	$t6, -401319
  e8:	13011364 	addu16i.d   	$a0, $s4, -16316
  ec:	34110000 	.word		0x34110000
  f0:	3a0e0300 	.word		0x3a0e0300
  f4:	390b3b0b 	.word		0x390b3b0b
  f8:	3f13490b 	.word		0x3f13490b
  fc:	00193c19 	srl.d       	$s2, $zero, $t3
 100:	00341200 	rcr.b       	$zero, $t4, $a0
 104:	0b3a1347 	.word		0x0b3a1347
 108:	0b390b3b 	.word		0x0b390b3b
 10c:	00001802 	cto.w       	$tp, $zero
 110:	31012e13 	.word		0x31012e13
 114:	640e6e13 	bge         	$t4, $t7, 3692	# f80 <_ZN7ConsoleC1Ev-0x90000000001ff080>
 118:	00130113 	maskeqz     	$t7, $a4, $zero
 11c:	012e1400 	.word		0x012e1400
 120:	19340e03 	pcaddi      	$sp, -417680
 124:	07120111 	.word		0x07120111
 128:	197a1840 	pcaddi      	$zero, -274238
 12c:	00001301 	clo.w       	$ra, $s1
 130:	31011d15 	.word		0x31011d15
 134:	12011113 	addu16i.d   	$t7, $a4, -32700
 138:	590b5807 	beq         	$zero, $a3, 68440	# 10c90 <_ZN7ConsoleC1Ev-0x90000000001ef370>
 13c:	000b570b 	.word		0x000b570b
 140:	01481600 	.word		0x01481600
 144:	0000017d 	.word		0x0000017d
 148:	03002e17 	lu52i.d     	$s0, $t4, 11
 14c:	2019340e 	ll.w        	$t2, $zero, 6452
 150:	1800000b 	pcaddi      	$a7, 0
 154:	1347012e 	addu16i.d   	$t2, $a5, -11840
 158:	0b3b0b3a 	.word		0x0b3b0b3a
 15c:	13640b39 	addu16i.d   	$s2, $s2, -9982
 160:	07120111 	.word		0x07120111
 164:	197a1840 	pcaddi      	$zero, -274238
 168:	00001301 	clo.w       	$ra, $s1
 16c:	03000519 	lu52i.d     	$s2, $a4, 1
 170:	3413490e 	.word		0x3413490e
 174:	00170219 	sll.w       	$s2, $t4, $zero
 178:	00051a00 	alsl.w      	$zero, $t4, $a2, 0x3
 17c:	0b3a0e03 	.word		0x0b3a0e03
 180:	0b390b3b 	.word		0x0b390b3b
 184:	17021349 	lu32i.d     	$a5, -520038
 188:	2e1b0000 	ldl.w       	$zero, $zero, 1728
 18c:	3a134701 	.word		0x3a134701
 190:	390b3b0b 	.word		0x390b3b0b
 194:	2013640b 	ll.w        	$a7, $zero, 4964
 198:	0013010b 	maskeqz     	$a7, $a4, $zero
 19c:	012e1c00 	.word		0x012e1c00
 1a0:	0e6e1331 	.word		0x0e6e1331
 1a4:	01111364 	fscaleb.d   	$fa4, $fs3, $fa4
 1a8:	18400712 	pcaddi      	$t6, 131128
 1ac:	0000197a 	cto.w       	$s3, $a7
 1b0:	3100051d 	.word		0x3100051d
 1b4:	00170213 	sll.w       	$t7, $t4, $zero
 1b8:	24010000 	ldptr.w     	$zero, $zero, 256
 1bc:	3e0b0b00 	.word		0x3e0b0b00
 1c0:	000e030b 	bytepick.d  	$a7, $s1, $zero, 0x4
 1c4:	000d0200 	bytepick.d  	$zero, $t4, $zero, 0x2
 1c8:	213a0e03 	sc.w        	$sp, $t4, 14860
 1cc:	390b3b02 	.word		0x390b3b02
 1d0:	3813490b 	.word		0x3813490b
 1d4:	0300000b 	lu52i.d     	$a7, $zero, 0
 1d8:	13490005 	addu16i.d   	$a1, $zero, -11712
 1dc:	00001934 	cto.w       	$t8, $a5
 1e0:	0b000f04 	.word		0x0b000f04
 1e4:	13490821 	addu16i.d   	$ra, $ra, -11710
 1e8:	26050000 	ldptr.d     	$zero, $zero, 1280
 1ec:	00134900 	maskeqz     	$zero, $a4, $t6
 1f0:	01110600 	fscaleb.d   	$fa0, $ft8, $fa1
 1f4:	0b130e25 	.word		0x0b130e25
 1f8:	1f1b1f03 	pcaddu18i   	$sp, -468744
 1fc:	07120111 	.word		0x07120111
 200:	00001710 	clz.w       	$t4, $s1
 204:	03001607 	lu52i.d     	$a3, $t4, 5
 208:	3b0b3a0e 	.word		0x3b0b3a0e
 20c:	490b390b 	bcnez       	$fcc0, 2951992	# 2d0d44 <_ZN7ConsoleC1Ev-0x8ffffffffff2f2bc>
 210:	08000013 	.word		0x08000013
 214:	0e030102 	.word		0x0e030102
 218:	0b3a0b0b 	.word		0x0b3a0b0b
 21c:	0b390b3b 	.word		0x0b390b3b
 220:	00001301 	clo.w       	$ra, $s1
 224:	03000d09 	lu52i.d     	$a5, $a4, 3
 228:	3b0b3a08 	.word		0x3b0b3a08
 22c:	490b390b 	bcnez       	$fcc0, 2951992	# 2d0d64 <_ZN7ConsoleC1Ev-0x8ffffffffff2f29c>
 230:	000b3813 	.word		0x000b3813
 234:	012e0a00 	.word		0x012e0a00
 238:	0e03193f 	.word		0x0e03193f
 23c:	0b3b0b3a 	.word		0x0b3b0b3a
 240:	0e6e0b39 	.word		0x0e6e0b39
 244:	193c0b32 	pcaddi      	$t6, -401319
 248:	13011364 	addu16i.d   	$a0, $s4, -16316
 24c:	2e0b0000 	ldl.w       	$zero, $zero, 704
 250:	03193f01 	lu52i.d     	$ra, $s1, 1615
 254:	3b0b3a0e 	.word		0x3b0b3a0e
 258:	6e0b390b 	bgeu        	$a4, $a7, -128200	# fffffffffffe0d90 <stack0+0x6fffffffffddfc50>
 25c:	3c0b320e 	.word		0x3c0b320e
 260:	00136419 	maskeqz     	$s2, $zero, $s2
 264:	00050c00 	alsl.w      	$zero, $zero, $sp, 0x3
 268:	00001349 	clo.w       	$a5, $s3
 26c:	47012e0d 	bnez        	$t4, 3604780	# 370398 <_ZN7ConsoleC1Ev-0x8fffffffffe8fc68>
 270:	3b0b3a13 	.word		0x3b0b3a13
 274:	640b390b 	bge         	$a4, $a7, 2872	# dac <_ZN7ConsoleC1Ev-0x90000000001ff254>
 278:	12011113 	addu16i.d   	$t7, $a4, -32700
 27c:	7a184007 	.word		0x7a184007
 280:	00130119 	maskeqz     	$s2, $a4, $zero
 284:	00050e00 	alsl.w      	$zero, $t4, $sp, 0x3
 288:	13490e03 	addu16i.d   	$sp, $t4, -11709
 28c:	18021934 	pcaddi      	$t8, 4297
 290:	050f0000 	gcsrrd      	$zero, 0x3c0
 294:	3a0e0300 	.word		0x3a0e0300
 298:	390b3b0b 	.word		0x390b3b0b
 29c:	0213490b 	slti        	$a7, $a4, 1234
 2a0:	10000018 	addu16i.d   	$s1, $zero, 0
 2a4:	1347012e 	addu16i.d   	$t2, $a5, -11840
 2a8:	0b3b0b3a 	.word		0x0b3b0b3a
 2ac:	13640b39 	addu16i.d   	$s2, $s2, -9982
 2b0:	13010b20 	addu16i.d   	$zero, $s2, -16318
 2b4:	05110000 	gcsrrd      	$zero, 0x440
 2b8:	490e0300 	jiscr1      	69120
 2bc:	00193413 	srl.d       	$t7, $zero, $t1
 2c0:	012e1200 	.word		0x012e1200
 2c4:	0e6e1331 	.word		0x0e6e1331
 2c8:	01111364 	fscaleb.d   	$fa4, $fs3, $fa4
 2cc:	18400712 	pcaddi      	$t6, 131128
 2d0:	0000197a 	cto.w       	$s3, $a7
 2d4:	31000513 	.word		0x31000513
 2d8:	00180213 	sra.w       	$t7, $t4, $zero
 2dc:	24010000 	ldptr.w     	$zero, $zero, 256
 2e0:	3e0b0b00 	.word		0x3e0b0b00
 2e4:	000e030b 	bytepick.d  	$a7, $s1, $zero, 0x4
 2e8:	00050200 	alsl.w      	$zero, $t4, $zero, 0x3
 2ec:	19341349 	pcaddi      	$a5, -417638
 2f0:	0d030000 	fsel        	$fa0, $fa0, $fa0, $fcc6
 2f4:	3a0e0300 	.word		0x3a0e0300
 2f8:	390b3b0b 	.word		0x390b3b0b
 2fc:	3813490b 	.word		0x3813490b
 300:	0400000b 	csrrd       	$a7, 0x0
 304:	210b000f 	sc.w        	$t3, $zero, 2816
 308:	00134908 	maskeqz     	$a4, $a4, $t6
 30c:	01020500 	.word		0x01020500
 310:	210b0e03 	sc.w        	$sp, $t4, 2828
 314:	3b0b3a10 	.word		0x3b0b3a10
 318:	21390d21 	sc.w        	$ra, $a5, 14604
 31c:	00130107 	maskeqz     	$a3, $a4, $zero
 320:	012e0600 	.word		0x012e0600
 324:	0e03193f 	.word		0x0e03193f
 328:	0b3b0b3a 	.word		0x0b3b0b3a
 32c:	6e022139 	bgeu        	$a5, $s2, -130528	# fffffffffffe054c <stack0+0x6fffffffffddf40c>
 330:	0121320e 	.word		0x0121320e
 334:	1364193c 	addu16i.d   	$s5, $a5, -9978
 338:	00001301 	clo.w       	$ra, $s1
 33c:	3f012e07 	.word		0x3f012e07
 340:	3a0e0319 	.word		0x3a0e0319
 344:	390b3b0b 	.word		0x390b3b0b
 348:	0e6e0721 	.word		0x0e6e0721
 34c:	3c012132 	.word		0x3c012132
 350:	00136419 	maskeqz     	$s2, $zero, $s2
 354:	00050800 	alsl.w      	$zero, $zero, $tp, 0x3
 358:	00001349 	clo.w       	$a5, $s3
 35c:	25011109 	stptr.w     	$a5, $a4, 272
 360:	030b130e 	lu52i.d     	$t2, $s1, 708
 364:	111f1b1f 	addu16i.d   	$s8, $s1, 18374
 368:	10071201 	addu16i.d   	$ra, $t4, 452
 36c:	0a000017 	.word		0x0a000017
 370:	0e030016 	.word		0x0e030016
 374:	0b3b0b3a 	.word		0x0b3b0b3a
 378:	13490b39 	addu16i.d   	$s2, $s2, -11710
 37c:	0d0b0000 	.word		0x0d0b0000
 380:	3a080300 	.word		0x3a080300
 384:	390b3b0b 	.word		0x390b3b0b
 388:	3813490b 	.word		0x3813490b
 38c:	0c00000b 	.word		0x0c00000b
 390:	13490026 	addu16i.d   	$a2, $ra, -11712
 394:	340d0000 	.word		0x340d0000
 398:	3a0e0300 	.word		0x3a0e0300
 39c:	390b3b0b 	.word		0x390b3b0b
 3a0:	3f13490b 	.word		0x3f13490b
 3a4:	00193c19 	srl.d       	$s2, $zero, $t3
 3a8:	01010e00 	fadd.d      	$fa0, $ft8, $fa3
 3ac:	13011349 	addu16i.d   	$a5, $s3, -16316
 3b0:	210f0000 	sc.w        	$zero, $zero, 3840
 3b4:	2f134900 	stl.w       	$zero, $a4, 1234
 3b8:	10000005 	addu16i.d   	$a1, $zero, 0
 3bc:	0e030034 	.word		0x0e030034
 3c0:	0b3b0b3a 	.word		0x0b3b0b3a
 3c4:	13490b39 	addu16i.d   	$s2, $s2, -11710
 3c8:	0188193f 	.word		0x0188193f
 3cc:	0018020b 	sra.w       	$a7, $t4, $zero
 3d0:	012e1100 	.word		0x012e1100
 3d4:	0e03193f 	.word		0x0e03193f
 3d8:	0b3b0b3a 	.word		0x0b3b0b3a
 3dc:	13490b39 	addu16i.d   	$s2, $s2, -11710
 3e0:	07120111 	.word		0x07120111
 3e4:	197a1840 	pcaddi      	$zero, -274238
 3e8:	00001301 	clo.w       	$ra, $s1
 3ec:	7d014812 	.word		0x7d014812
 3f0:	00137f01 	maskeqz     	$ra, $s1, $s8
 3f4:	00491300 	srai.d      	$zero, $s1, 0x4
 3f8:	187e1802 	pcaddi      	$tp, 258240
 3fc:	24140000 	ldptr.w     	$zero, $zero, 5120
 400:	3e0b0b00 	.word		0x3e0b0b00
 404:	0008030b 	bytepick.w  	$a7, $s1, $zero, 0x0
	...

Disassembly of section .debug_loclists:

0000000000000000 <.debug_loclists>:
   0:	00000086 	.word		0x00000086
   4:	00080005 	bytepick.w  	$a1, $zero, $zero, 0x0
   8:	00000000 	.word		0x00000000
   c:	20002407 	ll.w        	$a3, $zero, 36
  10:	00000000 	.word		0x00000000
  14:	20003790 	ll.w        	$t4, $s5, 52
  18:	00000000 	.word		0x00000000
  1c:	07540190 	.word		0x07540190
  20:	00200037 	div.w       	$s0, $ra, $zero
  24:	90000000 	.word		0x90000000
  28:	00200048 	div.w       	$a4, $tp, $zero
  2c:	90000000 	.word		0x90000000
  30:	5401a304 	bl          	-66059872	# fffffffffc1001d0 <stack0+0x6ffffffffbeff090>
  34:	2407009f 	ldptr.w     	$s8, $a0, 1792
  38:	00002000 	clo.d       	$zero, $zero
  3c:	37900000 	.word		0x37900000
  40:	00002000 	clo.d       	$zero, $zero
  44:	01900000 	.word		0x01900000
  48:	00370755 	armadd.w    	$s3, $ra, 0x5
  4c:	00000020 	.word		0x00000020
  50:	00489000 	srai.w      	$zero, $zero, 0x4
  54:	00000020 	.word		0x00000020
  58:	a3049000 	.word		0xa3049000
  5c:	009f5501 	bstrins.d   	$ra, $a4, 0x1f, 0x15
  60:	20000007 	ll.w        	$a3, $zero, 0
  64:	00000000 	.word		0x00000000
  68:	20001390 	ll.w        	$t4, $s5, 16
  6c:	00000000 	.word		0x00000000
  70:	07540190 	.word		0x07540190
  74:	00200013 	div.w       	$t7, $zero, $zero
  78:	90000000 	.word		0x90000000
  7c:	00200024 	div.w       	$a0, $ra, $zero
  80:	90000000 	.word		0x90000000
  84:	5401a304 	bl          	-66059872	# fffffffffc100224 <stack0+0x6ffffffffbeff0e4>
  88:	Address 0x88 is out of bounds.


Disassembly of section .debug_aranges:

0000000000000000 <.debug_aranges>:
   0:	0000002c 	.word		0x0000002c
   4:	00000002 	.word		0x00000002
   8:	00080000 	bytepick.w  	$zero, $zero, $zero, 0x0
   c:	00000000 	.word		0x00000000
  10:	00200000 	div.w       	$zero, $zero, $zero
  14:	90000000 	.word		0x90000000
  18:	00000074 	.word		0x00000074
	...
  30:	0000002c 	.word		0x0000002c
  34:	02710002 	sltui       	$tp, $zero, -960
  38:	00080000 	bytepick.w  	$zero, $zero, $zero, 0x0
  3c:	00000000 	.word		0x00000000
  40:	00200074 	div.w       	$t8, $sp, $zero
  44:	90000000 	.word		0x90000000
  48:	0000003c 	.word		0x0000003c
	...
  60:	0000002c 	.word		0x0000002c
  64:	03cb0002 	xori        	$tp, $zero, 0x2c0
  68:	00080000 	bytepick.w  	$zero, $zero, $zero, 0x0
  6c:	00000000 	.word		0x00000000
  70:	002000b0 	div.w       	$t4, $a1, $zero
  74:	90000000 	.word		0x90000000
  78:	00000028 	.word		0x00000028
	...

Disassembly of section .debug_line:

0000000000000000 <.debug_line>:
   0:	000000a6 	.word		0x000000a6
   4:	00080005 	bytepick.w  	$a1, $zero, $zero, 0x0
   8:	0000003d 	.word		0x0000003d
   c:	fb010101 	.word		0xfb010101
  10:	01000d0e 	.word		0x01000d0e
  14:	00010101 	.word		0x00010101
  18:	00010000 	asrtle.d    	$zero, $zero
  1c:	01010100 	fadd.d      	$fa0, $ft0, $fa0
  20:	000b021f 	.word		0x000b021f
  24:	003c0000 	.word		0x003c0000
  28:	01020000 	.word		0x01020000
  2c:	050f021f 	gcsrxchg    	$s8, $t4, 0x3c0
	...
  38:	00460000 	.word		0x00460000
  3c:	4e010000 	jirl        	$zero, $zero, -130816
  40:	01000000 	.word		0x01000000
  44:	00000059 	.word		0x00000059
  48:	00010501 	.word		0x00010501
  4c:	00000209 	.word		0x00000209
  50:	00000020 	.word		0x00000020
  54:	0c039000 	.word		0x0c039000
  58:	06120501 	cacop       	0x1, $a4, 1153
  5c:	10090003 	addu16i.d   	$sp, $zero, 576
  60:	15050100 	lu12i.w     	$zero, -514040
  64:	04090003 	csrrd       	$sp, 0x240
  68:	01050100 	fmul.d      	$fa0, $ft0, $fa0
  6c:	09030306 	.word		0x09030306
  70:	05010010 	gcsrrd      	$t4, 0x40
  74:	09010302 	.word		0x09010302
  78:	05010010 	gcsrrd      	$t4, 0x40
  7c:	0003060c 	.word		0x0003060c
  80:	01000009 	.word		0x01000009
  84:	02030105 	slti        	$a1, $a4, 192
  88:	01000409 	.word		0x01000409
  8c:	09000306 	.word		0x09000306
  90:	05010010 	gcsrrd      	$t4, 0x40
  94:	78030609 	.word		0x78030609
  98:	01001009 	.word		0x01001009
  9c:	08030105 	.word		0x08030105
  a0:	01000c09 	.word		0x01000c09
  a4:	00001009 	clo.w       	$a5, $zero
  a8:	00a50101 	bstrins.d   	$ra, $a4, 0x25, 0x0
  ac:	00050000 	alsl.w      	$zero, $zero, $zero, 0x3
  b0:	00380008 	.word		0x00380008
  b4:	01010000 	fadd.d      	$fa0, $fa0, $fa0
  b8:	0d0efb01 	.word		0x0d0efb01
  bc:	01010100 	fadd.d      	$fa0, $ft0, $fa0
  c0:	00000001 	.word		0x00000001
  c4:	01000001 	.word		0x01000001
  c8:	021f0101 	slti        	$ra, $a4, 1984
  cc:	0000000b 	.word		0x0000000b
  d0:	0000003c 	.word		0x0000003c
  d4:	021f0102 	slti        	$tp, $a4, 1984
  d8:	0062040f 	bstrins.w   	$t3, $zero, 0x2, 0x1
  dc:	62000000 	bltz        	$zero, -131072	# fffffffffffe00dc <stack0+0x6fffffffffddef9c>
  e0:	00000000 	.word		0x00000000
  e4:	00000046 	.word		0x00000046
  e8:	00005901 	ext.w.h     	$ra, $a4
  ec:	01050100 	fmul.d      	$fa0, $ft0, $fa0
  f0:	74020900 	xvsle.b     	$xr0, $xr8, $xr2
  f4:	00002000 	clo.d       	$zero, $zero
  f8:	03900000 	li.w        	$zero, 0x400
  fc:	0f05010a 	.word		0x0f05010a
 100:	09000306 	.word		0x09000306
 104:	0501000c 	gcsrrd      	$t0, 0x40
 108:	03030601 	lu52i.d     	$ra, $t4, 193
 10c:	01000c09 	.word		0x01000c09
 110:	01030205 	fsub.d      	$fa5, $ft8, $fa0
 114:	01000c09 	.word		0x01000c09
 118:	03060d05 	lu52i.d     	$a1, $a4, 387
 11c:	00000900 	movgr2scr   	$scr0, $a4
 120:	06020501 	cacop       	0x1, $a4, 129
 124:	04090103 	csrxchg     	$sp, $a4, 0x240
 128:	0f050100 	.word		0x0f050100
 12c:	09000306 	.word		0x09000306
 130:	05010000 	gcsrrd      	$zero, 0x40
 134:	01030602 	fsub.d      	$fa2, $ft8, $fa1
 138:	01000409 	.word		0x01000409
 13c:	03060c05 	lu52i.d     	$a1, $zero, 387
 140:	00000900 	movgr2scr   	$scr0, $a4
 144:	03010501 	lu52i.d     	$ra, $a4, 65
 148:	00040901 	alsl.w      	$ra, $a4, $tp, 0x1
 14c:	000c0901 	bytepick.d  	$ra, $a4, $tp, 0x0
 150:	79010100 	.word		0x79010100
 154:	05000000 	gcsrrd      	$zero, 0x0
 158:	3d000800 	.word		0x3d000800
 15c:	01000000 	.word		0x01000000
 160:	0efb0101 	.word		0x0efb0101
 164:	0101000d 	fadd.d      	$ft5, $fa0, $fa0
 168:	00000101 	.word		0x00000101
 16c:	00000100 	.word		0x00000100
 170:	1f010101 	pcaddu18i   	$ra, -522232
 174:	00000b02 	movgr2scr   	$scr2, $s1
 178:	00003c00 	revb.d      	$zero, $zero
 17c:	1f010200 	pcaddu18i   	$zero, -522224
 180:	6a050f02 	bltu        	$s1, $tp, -129780	# fffffffffffe068c <stack0+0x6fffffffffddf54c>
 184:	00000000 	.word		0x00000000
 188:	0000006a 	.word		0x0000006a
 18c:	00004600 	revh.d      	$zero, $t4
 190:	004e0100 	.word		0x004e0100
 194:	59010000 	beq         	$zero, $zero, 65792	# 10294 <_ZN7ConsoleC1Ev-0x90000000001efd6c>
 198:	01000000 	.word		0x01000000
 19c:	09000105 	.word		0x09000105
 1a0:	2000b002 	ll.w        	$tp, $zero, 176
 1a4:	00000000 	.word		0x00000000
 1a8:	02051a90 	slti        	$t4, $t8, 326
 1ac:	10090103 	addu16i.d   	$sp, $a4, 576
 1b0:	10050100 	addu16i.d   	$zero, $a4, 320
 1b4:	09000306 	.word		0x09000306
 1b8:	05010000 	gcsrrd      	$zero, 0x40
 1bc:	01030602 	fsub.d      	$fa2, $ft8, $fa1
 1c0:	01001409 	.word		0x01001409
 1c4:	00090003 	bytepick.w  	$sp, $zero, $zero, 0x2
 1c8:	04090100 	csrxchg     	$zero, $a4, 0x240
 1cc:	01010000 	fadd.d      	$fa0, $fa0, $fa0

Disassembly of section .debug_str:

0000000000000000 <.debug_str>:
   0:	20554e47 	ll.w        	$a3, $t6, 21836
   4:	312b2b43 	vstelm.w    	$vr3, $s3, -216, 0x2
   8:	33312037 	xvstelm.w   	$xr23, $ra, 288, 0x4
   c:	302e322e 	vldrepl.w   	$vr14, $t5, -464
  10:	616d2d20 	bltz        	$a5, 93484	# 16d3c <_ZN7ConsoleC1Ev-0x90000000001e92c4>
  14:	6c3d6962 	bgeu        	$a7, $tp, 15720	# 3d7c <_ZN7ConsoleC1Ev-0x90000000001fc284>
  18:	73343670 	vsrai.b     	$vr16, $vr19, 0x5
  1c:	616d2d20 	bltz        	$a5, 93484	# 16d48 <_ZN7ConsoleC1Ev-0x90000000001e92b8>
  20:	3d686372 	.word		0x3d686372
  24:	6e6f6f6c 	bgeu        	$s4, $t0, -102548	# fffffffffffe6f90 <stack0+0x6fffffffffde5e50>
  28:	63726167 	blt         	$a7, $a3, -36256	# ffffffffffff7288 <stack0+0x6fffffffffdf6148>
  2c:	20343668 	ll.w        	$a4, $t7, 13364
  30:	70666d2d 	vavg.bu     	$vr13, $vr9, $vr27
  34:	34363d75 	.word		0x34363d75
  38:	636d2d20 	bltz        	$a5, -37588	# ffffffffffff6d64 <stack0+0x6fffffffffdf5c24>
  3c:	65646f6d 	bge         	$s4, $t1, 91244	# 164a8 <_ZN7ConsoleC1Ev-0x90000000001e9b58>
  40:	6f6e3d6c 	bgeu        	$a7, $t0, -37316	# ffffffffffff6e7c <stack0+0x6fffffffffdf5d3c>
  44:	6c616d72 	bgeu        	$a7, $t6, 24940	# 61b0 <_ZN7ConsoleC1Ev-0x90000000001f9e50>
  48:	746d2d20 	.word		0x746d2d20
  4c:	3d656e75 	.word		0x3d656e75
  50:	6e6f6f6c 	bgeu        	$s4, $t0, -102548	# fffffffffffe6fbc <stack0+0x6fffffffffde5e7c>
  54:	63726167 	blt         	$a7, $a3, -36256	# ffffffffffff72b4 <stack0+0x6fffffffffdf6174>
  58:	20343668 	ll.w        	$a4, $t7, 13364
  5c:	6467672d 	bge         	$s2, $t1, 26468	# 67c0 <_ZN7ConsoleC1Ev-0x90000000001f9840>
  60:	4f2d2062 	jirl        	$tp, $sp, -53984
  64:	6e662d20 	bgeu        	$a5, $zero, -104916	# fffffffffffe6690 <stack0+0x6fffffffffde5550>
  68:	6d6f2d6f 	bgeu        	$a7, $t3, 93996	# 16f94 <_ZN7ConsoleC1Ev-0x90000000001e906c>
  6c:	662d7469 	bge         	$sp, $a5, -119436	# fffffffffffe2de0 <stack0+0x6fffffffffde1ca0>
  70:	656d6172 	bge         	$a7, $t6, 93536	# 16dd0 <_ZN7ConsoleC1Ev-0x90000000001e9230>
  74:	696f702d 	bltu        	$ra, $t1, 94064	# 16fe4 <_ZN7ConsoleC1Ev-0x90000000001e901c>
  78:	7265746e 	.word		0x7265746e
  7c:	66662d20 	bgez        	$a5, -104916	# fffffffffffe66a8 <stack0+0x6fffffffffde5568>
  80:	73656572 	vssrani.wu.d	$vr18, $vr11, 0x19
  84:	646e6174 	bge         	$a7, $t8, 28256	# 6ee4 <_ZN7ConsoleC1Ev-0x90000000001f911c>
  88:	20676e69 	ll.w        	$a5, $t7, 26476
  8c:	6f6e662d 	bgeu        	$t5, $t1, -37276	# ffffffffffff6ef0 <stack0+0x6fffffffffdf5db0>
  90:	6d6f632d 	bgeu        	$s2, $t1, 94048	# 16ff0 <_ZN7ConsoleC1Ev-0x90000000001e9010>
  94:	206e6f6d 	ll.w        	$t1, $s4, 28268
  98:	6f6e662d 	bgeu        	$t5, $t1, -37276	# ffffffffffff6efc <stack0+0x6fffffffffdf5dbc>
  9c:	6174732d 	blt         	$s2, $t1, 95344	# 1750c <_ZN7ConsoleC1Ev-0x90000000001e8af4>
  a0:	702d6b63 	.word		0x702d6b63
  a4:	65746f72 	bge         	$s4, $t6, 95340	# 17510 <_ZN7ConsoleC1Ev-0x90000000001e8af0>
  a8:	726f7463 	.word		0x726f7463
  ac:	6e662d20 	bgeu        	$a5, $zero, -104916	# fffffffffffe66d8 <stack0+0x6fffffffffde5598>
  b0:	69702d6f 	bltu        	$a7, $t3, 94252	# 170dc <_ZN7ConsoleC1Ev-0x90000000001e8f24>
  b4:	5a5f0065 	beq         	$sp, $a1, -106752	# fffffffffffe5fb4 <stack0+0x6fffffffffde4e74>
  b8:	6f43374e 	bgeu        	$s3, $t2, -48332	# ffffffffffff43ec <stack0+0x6fffffffffdf32ac>
  bc:	6c6f736e 	bgeu        	$s4, $t2, 28528	# 702c <_ZN7ConsoleC1Ev-0x90000000001f8fd4>
  c0:	6e693465 	bgeu        	$sp, $a1, -104140	# fffffffffffe69f4 <stack0+0x6fffffffffde58b4>
  c4:	50457469 	b           	27542900	# 1a44638 <_ZN7ConsoleC1Ev-0x8ffffffffe7bb9c8>
  c8:	6900634b 	bltu        	$s3, $a7, 65632	# 10128 <_ZN7ConsoleC1Ev-0x90000000001efed8>
  cc:	0074696e 	bstrins.w   	$t2, $a7, 0x14, 0x1a
  d0:	344e5a5f 	.word		0x344e5a5f
  d4:	6b636f4c 	bltu        	$s3, $t0, -40084	# ffffffffffff6440 <stack0+0x6fffffffffdf5300>
  d8:	76453443 	.word		0x76453443
  dc:	6f6c5f00 	bgeu        	$s1, $zero, -37796	# ffffffffffff6d38 <stack0+0x6fffffffffdf5bf8>
  e0:	5f006b63 	bne         	$s4, $sp, -65432	# ffffffffffff0148 <stack0+0x6fffffffffdef008>
  e4:	4c344e5a 	jirl        	$s3, $t6, 13388
  e8:	346b636f 	.word		0x346b636f
  ec:	74696e69 	xvavgr.w    	$xr9, $xr19, $xr27
  f0:	634b5045 	blt         	$tp, $a1, -46256	# ffffffffffff4c40 <stack0+0x6fffffffffdf3b00>
  f4:	6e697500 	bgeu        	$a4, $zero, -104076	# fffffffffffe6a68 <stack0+0x6fffffffffde5928>
  f8:	475f0074 	bnez        	$sp, -2924800	# ffffffffffd35ff8 <stack0+0x6fffffffffb34eb8>
  fc:	41424f4c 	beqz        	$s3, 3228236	# 314348 <_ZN7ConsoleC1Ev-0x8fffffffffeebcb8>
 100:	735f5f4c 	vsrarni.d.q 	$vr12, $vr26, 0x57
 104:	495f6275 	.word		0x495f6275
 108:	635f6b5f 	blt         	$s3, $s8, -41112	# ffffffffffff6070 <stack0+0x6fffffffffdf4f30>
 10c:	6f736e6f 	bgeu        	$t7, $t3, -35988	# ffffffffffff7478 <stack0+0x6fffffffffdf6338>
 110:	6c00656c 	bgeu        	$a7, $t0, 100	# 174 <_ZN7ConsoleC1Ev-0x90000000001ffe8c>
 114:	20676e6f 	ll.w        	$t3, $t7, 26476
 118:	69736e75 	bltu        	$t7, $r21, 95084	# 17484 <_ZN7ConsoleC1Ev-0x90000000001e8b7c>
 11c:	64656e67 	bge         	$t7, $a3, 25964	# 6688 <_ZN7ConsoleC1Ev-0x90000000001f9978>
 120:	746e6920 	.word		0x746e6920
 124:	6d616e00 	bgeu        	$t4, $zero, 90476	# 16290 <_ZN7ConsoleC1Ev-0x90000000001e9d70>
 128:	5a5f0065 	beq         	$sp, $a1, -106752	# fffffffffffe6028 <stack0+0x6fffffffffde4ee8>
 12c:	6f4c344e 	bgeu        	$tp, $t2, -46028	# ffffffffffff4d60 <stack0+0x6fffffffffdf3c20>
 130:	31436b63 	vstelm.h    	$vr3, $s4, -76, 0x0
 134:	5f007645 	bne         	$t6, $a1, -65420	# ffffffffffff01a8 <stack0+0x6fffffffffdef068>
 138:	6174735f 	blt         	$s3, $s8, 95344	# 175a8 <_ZN7ConsoleC1Ev-0x90000000001e8a58>
 13c:	5f636974 	bne         	$a7, $t8, -40088	# ffffffffffff64a4 <stack0+0x6fffffffffdf5364>
 140:	74696e69 	xvavgr.w    	$xr9, $xr19, $xr27
 144:	696c6169 	bltu        	$a7, $a5, 93280	# 16da4 <_ZN7ConsoleC1Ev-0x90000000001e925c>
 148:	6974617a 	bltu        	$a7, $s3, 95328	# 175a8 <_ZN7ConsoleC1Ev-0x90000000001e8a58>
 14c:	615f6e6f 	blt         	$t7, $t3, 89964	# 160b8 <_ZN7ConsoleC1Ev-0x90000000001e9f48>
 150:	645f646e 	bge         	$sp, $t2, 24420	# 60b4 <_ZN7ConsoleC1Ev-0x90000000001f9f4c>
 154:	72747365 	.word		0x72747365
 158:	69746375 	bltu        	$s4, $r21, 95328	# 175b8 <_ZN7ConsoleC1Ev-0x90000000001e8a48>
 15c:	305f6e6f 	vldrepl.h   	$vr15, $t7, -74
 160:	736e7500 	vssrarni.du.q	$vr0, $vr8, 0x1d
 164:	656e6769 	bge         	$s4, $a5, 93796	# 16fc8 <_ZN7ConsoleC1Ev-0x90000000001e9038>
 168:	68632064 	bltu        	$sp, $a0, 25376	# 6488 <_ZN7ConsoleC1Ev-0x90000000001f9b78>
 16c:	6c007261 	bgeu        	$t7, $ra, 112	# 1dc <_ZN7ConsoleC1Ev-0x90000000001ffe24>
 170:	656b636f 	bge         	$s4, $t3, 93024	# 16cd0 <_ZN7ConsoleC1Ev-0x90000000001e9330>
 174:	68740064 	bltu        	$sp, $a0, 29696	# 7574 <_ZN7ConsoleC1Ev-0x90000000001f8a8c>
 178:	5f007369 	bne         	$s4, $a5, -65424	# ffffffffffff01e8 <stack0+0x6fffffffffdef0a8>
 17c:	43374e5a 	beqz        	$t6, -1362100	# ffffffffffeb38c8 <stack0+0x6fffffffffcb2788>
 180:	6f736e6f 	bgeu        	$t7, $t3, -35988	# ffffffffffff74ec <stack0+0x6fffffffffdf63ac>
 184:	3443656c 	.word		0x3443656c
 188:	73007645 	.word		0x73007645
 18c:	74726f68 	xvmin.b     	$xr8, $xr27, $xr27
 190:	736e7520 	vssrarni.du.q	$vr0, $vr9, 0x1d
 194:	656e6769 	bge         	$s4, $a5, 93796	# 16ff8 <_ZN7ConsoleC1Ev-0x90000000001e9008>
 198:	6e692064 	bgeu        	$sp, $a0, -104160	# fffffffffffe6ab8 <stack0+0x6fffffffffde5978>
 19c:	6f4c0074 	bgeu        	$sp, $t8, -46080	# ffffffffffff4d9c <stack0+0x6fffffffffdf3c5c>
 1a0:	5f006b63 	bne         	$s4, $sp, -65432	# ffffffffffff0208 <stack0+0x6fffffffffdef0c8>
 1a4:	43374e5a 	beqz        	$t6, -1362100	# ffffffffffeb38f0 <stack0+0x6fffffffffcb27b0>
 1a8:	6f736e6f 	bgeu        	$t7, $t3, -35988	# ffffffffffff7514 <stack0+0x6fffffffffdf63d4>
 1ac:	3243656c 	xvldrepl.h  	$xr12, $a7, 434
 1b0:	43007645 	beqz        	$t6, 1507444	# 170224 <_ZN7ConsoleC1Ev-0x900000000008fddc>
 1b4:	6f736e6f 	bgeu        	$t7, $t3, -35988	# ffffffffffff7520 <stack0+0x6fffffffffdf63e0>
 1b8:	5f00656c 	bne         	$a7, $t0, -65436	# ffffffffffff021c <stack0+0x6fffffffffdef0dc>
 1bc:	4c344e5a 	jirl        	$s3, $t6, 13388
 1c0:	436b636f 	beqz        	$s4, 4156256	# 3f6d20 <_ZN7ConsoleC1Ev-0x8fffffffffe092e0>
 1c4:	00764532 	bstrins.w   	$t6, $a5, 0x16, 0x11
 1c8:	63617473 	blt         	$sp, $t7, -40588	# ffffffffffff633c <stack0+0x6fffffffffdf51fc>
 1cc:	6d00306b 	bgeu        	$sp, $a7, 65584	# 101fc <_ZN7ConsoleC1Ev-0x90000000001efe04>
 1d0:	006e6961 	bstrins.w   	$ra, $a7, 0xe, 0x1a

Disassembly of section .debug_line_str:

0000000000000000 <.debug_line_str>:
   0:	736e6f63 	vssrarni.du.q	$vr3, $vr27, 0x1b
   4:	2e656c6f 	ldr.w       	$t3, $sp, -1701
   8:	2f006363 	stl.w       	$sp, $s4, 24
   c:	656d6f68 	bge         	$s4, $a4, 93548	# 16d78 <_ZN7ConsoleC1Ev-0x90000000001e9288>
  10:	73696c2f 	vssrarni.w.d	$vr15, $vr1, 0x1b
  14:	6e617568 	bgeu        	$a7, $a4, -106124	# fffffffffffe6188 <stack0+0x6fffffffffde5048>
  18:	76782f67 	.word		0x76782f67
  1c:	6f6c2d36 	bgeu        	$a5, $fp, -37844	# ffffffffffff6c48 <stack0+0x6fffffffffdf5b08>
  20:	61676e6f 	blt         	$t7, $t3, 92012	# 1678c <_ZN7ConsoleC1Ev-0x90000000001e9874>
  24:	2f686372 	str.w       	$t6, $s4, -1512
  28:	30316b32 	.word		0x30316b32
  2c:	616c3030 	blt         	$ra, $t4, 93232	# 16c5c <_ZN7ConsoleC1Ev-0x90000000001e93a4>
  30:	3676782d 	.word		0x3676782d
  34:	72656b2f 	.word		0x72656b2f
  38:	006c656e 	bstrins.w   	$t2, $a7, 0xc, 0x19
  3c:	6e692f2e 	bgeu        	$s2, $t2, -104148	# fffffffffffe6968 <stack0+0x6fffffffffde5828>
  40:	64756c63 	bge         	$sp, $sp, 30060	# 75ac <_ZN7ConsoleC1Ev-0x90000000001f8a54>
  44:	6f6c0065 	bgeu        	$sp, $a1, -37888	# ffffffffffff6c44 <stack0+0x6fffffffffdf5b04>
  48:	682e6b63 	bltu        	$s4, $sp, 11880	# 2eb0 <_ZN7ConsoleC1Ev-0x90000000001fd150>
  4c:	6f630068 	bgeu        	$sp, $a4, -40192	# ffffffffffff634c <stack0+0x6fffffffffdf520c>
  50:	6c6f736e 	bgeu        	$s4, $t2, 28528	# 6fc0 <_ZN7ConsoleC1Ev-0x90000000001f9040>
  54:	68682e65 	bltu        	$t7, $a1, 26668	# 6880 <_ZN7ConsoleC1Ev-0x90000000001f9780>
  58:	70797400 	.word		0x70797400
  5c:	682e7365 	bltu        	$s4, $a1, 11888	# 2ecc <_ZN7ConsoleC1Ev-0x90000000001fd134>
  60:	6f6c0068 	bgeu        	$sp, $a4, -37888	# ffffffffffff6c60 <stack0+0x6fffffffffdf5b20>
  64:	632e6b63 	blt         	$s4, $sp, -53656	# ffffffffffff2ecc <stack0+0x6fffffffffdf1d8c>
  68:	616d0063 	blt         	$sp, $sp, 93440	# 16d68 <_ZN7ConsoleC1Ev-0x90000000001e9298>
  6c:	632e6e69 	blt         	$t7, $a5, -53652	# ffffffffffff2ed8 <stack0+0x6fffffffffdf1d98>
  70:	Address 0x70 is out of bounds.


Disassembly of section .comment:

0000000000000000 <.comment>:
   0:	3a434347 	.word		0x3a434347
   4:	4e472820 	jirl        	$zero, $ra, -112856
   8:	31202955 	vstelm.w    	$vr21, $a6, 40, 0x0
   c:	2e322e33 	ldl.w       	$t7, $t5, -885
  10:	Address 0x10 is out of bounds.

