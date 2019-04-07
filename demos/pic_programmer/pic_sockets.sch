EESchema Schematic File Version 5
LIBS:pic_programmer-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title "JDM - COM84 PIC Programmer with 13V DC/DC converter"
Date "Sun 22 Mar 2015"
Rev "3"
Comp "KiCad"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	3300 6600 3300 6500
Wire Wire Line
	2100 2400 3000 2400
Wire Wire Line
	9200 2000 10100 2000
Wire Wire Line
	9200 1900 10000 1900
Wire Wire Line
	7100 1900 8000 1900
Connection ~ 2800 1200
Wire Wire Line
	2800 1200 3000 1200
Connection ~ 2800 1300
Wire Wire Line
	2800 1100 2800 1200
Wire Wire Line
	2800 1100 3000 1100
Wire Wire Line
	3000 1300 2800 1300
Wire Wire Line
	4400 1500 5300 1500
Wire Wire Line
	4400 1400 5400 1400
Wire Wire Line
	10100 4000 9200 4000
Wire Wire Line
	10000 3900 9200 3900
Wire Wire Line
	7100 3900 8000 3900
Wire Wire Line
	2950 4600 2750 4600
Wire Wire Line
	5400 4900 4500 4900
Wire Wire Line
	5300 4750 4500 4750
Wire Wire Line
	2200 4450 2950 4450
Wire Wire Line
	5400 3100 4450 3100
Wire Wire Line
	5300 2700 4450 2700
Wire Wire Line
	3000 3400 2200 3400
Wire Wire Line
	2750 4600 2750 4650
Wire Wire Line
	5500 4600 4500 4600
Wire Wire Line
	10200 2700 9200 2700
Wire Wire Line
	10200 4700 9200 4700
Wire Wire Line
	8000 4900 7200 4900
Wire Wire Line
	3300 6500 2400 6500
Text HLabel 1900 6200 0    60   Input ~ 0
VCC_PIC
Text Label 2450 2400 0    60   ~ 0
VCC_PIC
Text Label 7200 4900 0    60   ~ 0
VCC_PIC
Text Label 9450 4700 0    60   ~ 0
VCC_PIC
Text Label 9450 2700 0    60   ~ 0
VCC_PIC
$Comp
L pic_programmer_schlib:GND #PWR036
U 1 1 442A8794
P 8000 5000
F 0 "#PWR036" H 8000 5000 30  0001 C CNN
F 1 "GND" H 8000 4930 30  0001 C CNN
F 2 "" H 8000 5000 60  0001 C CNN
F 3 "" H 8000 5000 60  0001 C CNN
	1    8000 5000
	0    1    1    0   
$EndComp
Text Label 4750 4600 0    60   ~ 0
VCC_PIC
$Comp
L pic_programmer_schlib:GND #PWR037
U 1 1 443CCA5D
P 8000 4600
F 0 "#PWR037" H 8000 4600 30  0001 C CNN
F 1 "GND" H 8000 4530 30  0001 C CNN
F 2 "" H 8000 4600 60  0001 C CNN
F 3 "" H 8000 4600 60  0001 C CNN
	1    8000 4600
	0    1    1    0   
$EndComp
NoConn ~ 8000 2900
NoConn ~ 9200 3200
NoConn ~ 9200 3100
NoConn ~ 9200 3000
NoConn ~ 9200 2900
NoConn ~ 9200 2600
NoConn ~ 9200 2500
NoConn ~ 9200 2400
NoConn ~ 9200 2300
NoConn ~ 9200 2200
NoConn ~ 9200 2100
NoConn ~ 8000 3200
NoConn ~ 8000 3100
NoConn ~ 8000 3000
NoConn ~ 8000 2800
NoConn ~ 8000 2700
NoConn ~ 8000 2500
NoConn ~ 8000 2400
NoConn ~ 8000 2300
NoConn ~ 8000 2200
NoConn ~ 8000 2100
NoConn ~ 8000 2000
$Comp
L pic_programmer_schlib:GND #PWR038
U 1 1 443697C7
P 9200 2800
F 0 "#PWR038" H 9200 2800 30  0001 C CNN
F 1 "GND" H 9200 2730 30  0001 C CNN
F 2 "" H 9200 2800 60  0001 C CNN
F 3 "" H 9200 2800 60  0001 C CNN
	1    9200 2800
	0    -1   -1   0   
$EndComp
$Comp
L pic_programmer_schlib:GND #PWR039
U 1 1 443697C3
P 8000 2600
F 0 "#PWR039" H 8000 2600 30  0001 C CNN
F 1 "GND" H 8000 2530 30  0001 C CNN
F 2 "" H 8000 2600 60  0001 C CNN
F 3 "" H 8000 2600 60  0001 C CNN
	1    8000 2600
	0    1    1    0   
$EndComp
Text Label 9350 2000 0    60   ~ 0
CLOCK-RB6
Text Label 9350 1900 0    60   ~ 0
DATA-RB7
Text Label 7350 1900 0    60   ~ 0
VPP-MCLR
$Comp
L pic_programmer_schlib:SUPP28 P2
U 1 1 4436967E
P 8600 2550
F 0 "P2" H 8600 3350 70  0000 C CNN
F 1 "SUPP28" H 8600 1700 70  0000 C CNN
F 2 "Package_DIP:DIP-28_W7.62mm_Socket_LongPads" H 8600 1600 15  0000 C CNN
F 3 "" H 8600 2550 60  0001 C CNN
	1    8600 2550
	1    0    0    -1  
$EndComp
$Comp
L pic_programmer_schlib:GND #PWR040
U 1 1 442AA147
P 3300 6950
F 0 "#PWR040" H 3300 6950 30  0001 C CNN
F 1 "GND" H 3300 6880 30  0001 C CNN
F 2 "" H 3300 6950 60  0001 C CNN
F 3 "" H 3300 6950 60  0001 C CNN
	1    3300 6950
	1    0    0    -1  
$EndComp
$Comp
L pic_programmer_schlib:C C7
U 1 1 442AA145
P 3300 6750
F 0 "C7" H 3350 6850 50  0000 L CNN
F 1 "100nF" H 3350 6650 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D5.1mm_W3.2mm_P5.00mm" H 3500 6600 10  0000 C CNN
F 3 "" H 3300 6750 60  0001 C CNN
	1    3300 6750
	1    0    0    -1  
$EndComp
$Comp
L pic_programmer_schlib:GND #PWR041
U 1 1 442AA138
P 2400 6950
F 0 "#PWR041" H 2400 6950 30  0001 C CNN
F 1 "GND" H 2400 6880 30  0001 C CNN
F 2 "" H 2400 6950 60  0001 C CNN
F 3 "" H 2400 6950 60  0001 C CNN
	1    2400 6950
	1    0    0    -1  
$EndComp
NoConn ~ 9200 5800
NoConn ~ 9200 5700
NoConn ~ 9200 5600
NoConn ~ 9200 5500
NoConn ~ 9200 5400
NoConn ~ 9200 5300
NoConn ~ 9200 5200
NoConn ~ 9200 5100
NoConn ~ 9200 5000
NoConn ~ 9200 4900
NoConn ~ 9200 4600
NoConn ~ 9200 4500
NoConn ~ 9200 4400
NoConn ~ 9200 4300
NoConn ~ 9200 4200
NoConn ~ 9200 4100
NoConn ~ 8000 5800
NoConn ~ 8000 5700
NoConn ~ 8000 5600
NoConn ~ 8000 5500
NoConn ~ 8000 5400
NoConn ~ 8000 5300
NoConn ~ 8000 5200
NoConn ~ 8000 5100
NoConn ~ 8000 4800
NoConn ~ 8000 4700
NoConn ~ 8000 4500
NoConn ~ 8000 4400
NoConn ~ 8000 4300
NoConn ~ 8000 4200
NoConn ~ 8000 4100
NoConn ~ 8000 4000
$Comp
L pic_programmer_schlib:GND #PWR042
U 1 1 442A896A
P 9200 4800
F 0 "#PWR042" H 9200 4800 30  0001 C CNN
F 1 "GND" H 9200 4730 30  0001 C CNN
F 2 "" H 9200 4800 60  0001 C CNN
F 3 "" H 9200 4800 60  0001 C CNN
	1    9200 4800
	0    -1   -1   0   
$EndComp
$Comp
L pic_programmer_schlib:SUPP40 P3
U 1 1 442A88ED
P 8600 4850
F 0 "P3" H 8600 5950 70  0000 C CNN
F 1 "SUPP40" H 8600 3700 70  0000 C CNN
F 2 "footprints:40tex-Ell600" H 8600 3600 15  0000 C CNN
F 3 "" H 8600 4850 60  0001 C CNN
	1    8600 4850
	1    0    0    -1  
$EndComp
NoConn ~ 4400 1200
$Comp
L pic_programmer_schlib:GND #PWR043
U 1 1 442A8838
P 2800 1350
F 0 "#PWR043" H 2800 1350 30  0001 C CNN
F 1 "GND" H 2800 1280 30  0001 C CNN
F 2 "" H 2800 1350 60  0001 C CNN
F 3 "" H 2800 1350 60  0001 C CNN
	1    2800 1350
	1    0    0    -1  
$EndComp
Text Label 4550 1500 0    60   ~ 0
DATA-RB7
Text Label 4550 1400 0    60   ~ 0
CLOCK-RB6
$Comp
L pic_programmer_schlib:24C16 U1
U 1 1 442A87F7
P 3700 1300
F 0 "U1" H 3850 1650 60  0000 C CNN
F 1 "24Cxx" H 3900 950 60  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm_Socket_LongPads" H 3900 900 10  0000 C CNN
F 3 "" H 3700 1300 60  0001 C CNN
	1    3700 1300
	1    0    0    -1  
$EndComp
Text Label 9350 4000 0    60   ~ 0
CLOCK-RB6
Text Label 9350 3900 0    60   ~ 0
DATA-RB7
Text Label 7350 3900 0    60   ~ 0
VPP-MCLR
NoConn ~ 2950 4150
NoConn ~ 4500 5200
NoConn ~ 4500 5050
NoConn ~ 4500 4300
NoConn ~ 4500 4450
NoConn ~ 4500 4150
NoConn ~ 4500 4000
NoConn ~ 2950 5200
NoConn ~ 2950 5050
NoConn ~ 2950 4900
NoConn ~ 2950 4750
NoConn ~ 2950 4300
NoConn ~ 2950 4000
$Comp
L pic_programmer_schlib:PIC16F54 U5
U 1 1 442A81A7
P 3750 4600
F 0 "U5" H 3750 5350 60  0000 C CNN
F 1 "PIC_18_PINS" H 3750 3800 60  0000 C CNN
F 2 "Package_DIP:DIP-18_W7.62mm_Socket_LongPads" H 3800 3700 15  0000 C CNN
F 3 "" H 3750 4600 60  0001 C CNN
	1    3750 4600
	1    0    0    -1  
$EndComp
NoConn ~ 4450 3400
NoConn ~ 3000 3100
NoConn ~ 3000 2700
Text Label 4650 4900 0    60   ~ 0
CLOCK-RB6
Text Label 4650 4750 0    60   ~ 0
DATA-RB7
Text Label 2300 4450 0    60   ~ 0
VPP-MCLR
Text Label 2350 3400 0    60   ~ 0
VPP-MCLR
Text Label 4600 3100 0    60   ~ 0
CLOCK-RB6
Text Label 4600 2700 0    60   ~ 0
DATA-RB7
Text HLabel 1900 6300 0    60   Input ~ 0
VPP-MCLR
Text HLabel 1900 2000 0    60   Input ~ 0
CLOCK-RB6
Text HLabel 1900 2100 0    60   Input ~ 0
DATA-RB7
$Comp
L pic_programmer_schlib:GND #PWR044
U 1 1 442A820F
P 2750 4650
F 0 "#PWR044" H 2750 4650 30  0001 C CNN
F 1 "GND" H 2750 4580 30  0001 C CNN
F 2 "" H 2750 4650 60  0001 C CNN
F 3 "" H 2750 4650 60  0001 C CNN
	1    2750 4650
	1    0    0    -1  
$EndComp
$Comp
L pic_programmer_schlib:GND #PWR045
U 1 1 442A8205
P 4450 2400
F 0 "#PWR045" H 4450 2400 30  0001 C CNN
F 1 "GND" H 4450 2330 30  0001 C CNN
F 2 "" H 4450 2400 60  0001 C CNN
F 3 "" H 4450 2400 60  0001 C CNN
	1    4450 2400
	0    -1   -1   0   
$EndComp
$Comp
L pic_programmer_schlib:PIC12C508A U6
U 1 1 442A81A5
P 3750 2900
F 0 "U6" H 3700 3600 60  0000 C CNN
F 1 "PIC_8_PINS" H 3750 2250 60  0000 C CNN
F 2 "Package_DIP:DIP-8_W7.62mm_Socket_LongPads" H 3750 2150 15  0000 C CNN
F 3 "" H 3750 2900 60  0001 C CNN
	1    3750 2900
	1    0    0    -1  
$EndComp
$Comp
L pic_programmer_schlib:GND #PWR046
U 1 1 52C92629
P 3700 1850
F 0 "#PWR046" H 3700 1850 30  0001 C CNN
F 1 "GND" H 3700 1780 30  0001 C CNN
F 2 "" H 3700 1850 60  0001 C CNN
F 3 "" H 3700 1850 60  0001 C CNN
	1    3700 1850
	1    0    0    -1  
$EndComp
Text Label 3700 800  0    60   ~ 0
VCC_PIC
$Comp
L pic_programmer_schlib:C C6
U 1 1 442AA12B
P 2400 6750
F 0 "C6" H 2450 6850 50  0000 L CNN
F 1 "100nF" H 2450 6650 50  0000 L CNN
F 2 "Capacitor_THT:C_Disc_D5.1mm_W3.2mm_P5.00mm" H 2600 6600 10  0000 C CNN
F 3 "" H 2400 6750 60  0001 C CNN
	1    2400 6750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2400 6200 2400 6500
Text Label 2750 6500 0    60   ~ 0
VCC_PIC
Wire Wire Line
	5300 1500 5300 2100
Connection ~ 5300 2700
Wire Wire Line
	5400 1400 5400 2000
Connection ~ 5400 3100
Wire Wire Line
	10000 1500 10000 1900
Connection ~ 5300 1500
Connection ~ 10000 1900
Wire Wire Line
	10100 1400 10100 2000
Connection ~ 10100 2000
Connection ~ 5400 1400
Wire Wire Line
	10200 6200 10200 4700
Wire Wire Line
	5500 6200 5500 4600
Wire Wire Line
	1900 6200 2100 6200
Connection ~ 10200 4700
Wire Wire Line
	5500 800  3700 800 
Connection ~ 5500 4600
Wire Wire Line
	2200 3400 2200 4450
Wire Wire Line
	7100 6300 2200 6300
Wire Wire Line
	7100 1900 7100 3900
Connection ~ 2200 4450
Connection ~ 7100 3900
Wire Wire Line
	7200 4900 7200 6200
Connection ~ 7200 6200
Connection ~ 5500 6200
Connection ~ 2200 6300
Wire Wire Line
	2100 2400 2100 6200
Connection ~ 2100 6200
Wire Wire Line
	1900 2000 5400 2000
Connection ~ 5400 2000
Wire Wire Line
	1900 2100 5300 2100
Connection ~ 5300 2100
Connection ~ 2400 6500
Connection ~ 2400 6200
Wire Wire Line
	3700 1850 3700 1800
Wire Wire Line
	2400 6900 2400 6950
Wire Wire Line
	3300 6900 3300 6950
$Comp
L pic_programmer_schlib:PWR_FLAG #FLG047
U 1 1 5558FA28
P 3300 6500
F 0 "#FLG047" H 3300 6595 30  0001 C CNN
F 1 "PWR_FLAG" H 3300 6680 30  0000 C CNN
F 2 "" H 3300 6500 60  0000 C CNN
F 3 "" H 3300 6500 60  0000 C CNN
	1    3300 6500
	0    1    1    0   
$EndComp
Wire Wire Line
	2800 1200 2800 1300
Wire Wire Line
	2800 1300 2800 1350
Wire Wire Line
	5300 2700 5300 4750
Wire Wire Line
	5400 3100 5400 4900
Wire Wire Line
	5300 1500 10000 1500
Wire Wire Line
	10000 1900 10000 3900
Wire Wire Line
	10100 2000 10100 4000
Wire Wire Line
	5400 1400 10100 1400
Wire Wire Line
	10200 4700 10200 2700
Wire Wire Line
	5500 4600 5500 800 
Wire Wire Line
	2200 4450 2200 6300
Wire Wire Line
	7100 3900 7100 6300
Wire Wire Line
	7200 6200 10200 6200
Wire Wire Line
	5500 6200 7200 6200
Wire Wire Line
	2200 6300 1900 6300
Wire Wire Line
	2100 6200 2400 6200
Wire Wire Line
	5400 2000 5400 3100
Wire Wire Line
	5300 2100 5300 2700
Wire Wire Line
	2400 6500 2400 6600
Wire Wire Line
	2400 6200 5500 6200
Connection ~ 3300 6500
$EndSCHEMATC
