EESchema Schematic File Version 4
LIBS:hmi-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 5
Title "BUT Pneumobil - HMI Power supply"
Date "2018-11-13"
Rev "0.1"
Comp "Brno University of Technology"
Comment1 "Jakub Kaderka"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 2700 3000 0    50   Input ~ 0
VIN
$Comp
L Regulator_Linear:LD1117S33TR_SOT223 U7
U 1 1 5C379855
P 5800 3000
F 0 "U7" H 5800 3242 50  0000 C CNN
F 1 "TS1117BCW33" H 5800 3151 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 5800 3200 50  0001 C CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00000544.pdf" H 5900 2750 50  0001 C CNN
	1    5800 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C33
U 1 1 5C3799B9
P 6250 3200
F 0 "C33" H 6365 3246 50  0000 L CNN
F 1 "100n" H 6365 3155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6288 3050 50  0001 C CNN
F 3 "~" H 6250 3200 50  0001 C CNN
	1    6250 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 3000 6250 3000
Wire Wire Line
	6250 3050 6250 3000
$Comp
L power:GND #PWR0159
U 1 1 5C379AA7
P 5800 3450
F 0 "#PWR0159" H 5800 3200 50  0001 C CNN
F 1 "GND" H 5805 3277 50  0000 C CNN
F 2 "" H 5800 3450 50  0001 C CNN
F 3 "" H 5800 3450 50  0001 C CNN
	1    5800 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 3450 5800 3400
Wire Wire Line
	5800 3400 6250 3400
Connection ~ 5800 3400
Wire Wire Line
	5800 3400 5800 3300
Wire Wire Line
	6250 3350 6250 3400
$Comp
L Device:LED D4
U 1 1 5C37AFE9
P 4000 3900
F 0 "D4" V 4038 3783 50  0000 R CNN
F 1 "3.3 V" V 3947 3783 50  0000 R CNN
F 2 "LED_SMD:LED_1206_3216Metric" H 4000 3900 50  0001 C CNN
F 3 "~" H 4000 3900 50  0001 C CNN
	1    4000 3900
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R14
U 1 1 5C37B05F
P 4000 3550
F 0 "R14" H 4070 3596 50  0000 L CNN
F 1 "220" H 4070 3505 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 3930 3550 50  0001 C CNN
F 3 "~" H 4000 3550 50  0001 C CNN
	1    4000 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 3700 4000 3750
$Comp
L power:GND #PWR0160
U 1 1 5C37B20A
P 4000 4200
F 0 "#PWR0160" H 4000 3950 50  0001 C CNN
F 1 "GND" H 4005 4027 50  0000 C CNN
F 2 "" H 4000 4200 50  0001 C CNN
F 3 "" H 4000 4200 50  0001 C CNN
	1    4000 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 4200 4000 4050
$Comp
L power:+3V3 #PWR0161
U 1 1 5C37B3FC
P 4000 3350
F 0 "#PWR0161" H 4000 3200 50  0001 C CNN
F 1 "+3V3" H 4015 3523 50  0000 C CNN
F 2 "" H 4000 3350 50  0001 C CNN
F 3 "" H 4000 3350 50  0001 C CNN
	1    4000 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 3350 4000 3400
$Comp
L Device:LED D6
U 1 1 5C37C0FF
P 4450 3900
F 0 "D6" V 4488 3782 50  0000 R CNN
F 1 "5 V" V 4397 3782 50  0000 R CNN
F 2 "LED_SMD:LED_1206_3216Metric" H 4450 3900 50  0001 C CNN
F 3 "~" H 4450 3900 50  0001 C CNN
	1    4450 3900
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R16
U 1 1 5C37C105
P 4450 3550
F 0 "R16" H 4520 3596 50  0000 L CNN
F 1 "470" H 4520 3505 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 4380 3550 50  0001 C CNN
F 3 "~" H 4450 3550 50  0001 C CNN
	1    4450 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3700 4450 3750
$Comp
L power:GND #PWR0163
U 1 1 5C37C10C
P 4450 4200
F 0 "#PWR0163" H 4450 3950 50  0001 C CNN
F 1 "GND" H 4455 4027 50  0000 C CNN
F 2 "" H 4450 4200 50  0001 C CNN
F 3 "" H 4450 4200 50  0001 C CNN
	1    4450 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 4200 4450 4050
$Comp
L power:+3.3VADC #PWR0164
U 1 1 5C42217C
P 8250 2950
F 0 "#PWR0164" H 8400 2900 50  0001 C CNN
F 1 "+3.3VADC" H 8270 3093 50  0000 C CNN
F 2 "" H 8250 2950 50  0001 C CNN
F 3 "" H 8250 2950 50  0001 C CNN
	1    8250 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 3000 3000 3000
Wire Wire Line
	4450 3400 4450 3000
Connection ~ 4450 3000
$Comp
L Regulator_Switching:ADP2108AUJ-3.3 U8
U 1 1 5C4CBB20
P 5750 4050
F 0 "U8" H 5900 3800 50  0000 C CNN
F 1 "MCP1603T-330" H 5600 4250 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:TSOT-23-5" H 5800 3800 50  0001 L CNN
F 3 "http://www.analog.com/media/en/technical-documentation/data-sheets/ADP2108.pdf" H 5500 3700 50  0001 C CNN
	1    5750 4050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0166
U 1 1 5C4CC4B4
P 5750 4550
F 0 "#PWR0166" H 5750 4300 50  0001 C CNN
F 1 "GND" H 5755 4377 50  0000 C CNN
F 2 "" H 5750 4550 50  0001 C CNN
F 3 "" H 5750 4550 50  0001 C CNN
	1    5750 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4350 5000 4450
Wire Wire Line
	5000 4450 5750 4450
Wire Wire Line
	5750 4450 5750 4550
Wire Wire Line
	5750 4450 5750 4350
Connection ~ 5750 4450
Wire Wire Line
	5450 3950 5350 3950
Wire Wire Line
	5000 3950 5000 4050
Wire Wire Line
	5450 4050 5350 4050
Wire Wire Line
	5350 4050 5350 3950
Connection ~ 5350 3950
Wire Wire Line
	5350 3950 5000 3950
$Comp
L Device:L L1
U 1 1 5C4CD722
P 6350 3950
F 0 "L1" V 6540 3950 50  0000 C CNN
F 1 "LPS4018-103MRB" V 6449 3950 50  0000 C CNN
F 2 "Inductor_SMD:L_Coilcraft_LPS4018" H 6350 3950 50  0001 C CNN
F 3 "~" H 6350 3950 50  0001 C CNN
	1    6350 3950
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C31
U 1 1 5C4CE1F1
P 5000 4200
F 0 "C31" H 5115 4246 50  0000 L CNN
F 1 "4.7u" H 5115 4155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5038 4050 50  0001 C CNN
F 3 "~" H 5000 4200 50  0001 C CNN
	1    5000 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 3950 6050 3950
Wire Wire Line
	6050 4150 6600 4150
Wire Wire Line
	6600 4150 6600 3950
Wire Wire Line
	6600 3950 6500 3950
$Comp
L Device:C C35
U 1 1 5C4CF073
P 6800 4200
F 0 "C35" H 6915 4246 50  0000 L CNN
F 1 "4.7u" H 6915 4155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6838 4050 50  0001 C CNN
F 3 "~" H 6800 4200 50  0001 C CNN
	1    6800 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 4450 6800 4450
Wire Wire Line
	6800 4450 6800 4350
Wire Wire Line
	6800 3950 6800 4050
Connection ~ 6600 3950
Wire Wire Line
	6800 3900 6800 3950
Connection ~ 6800 3950
$Comp
L power:+3.3VADC #PWR0169
U 1 1 5C4D449A
P 10100 900
F 0 "#PWR0169" H 10250 850 50  0001 C CNN
F 1 "+3.3VADC" H 10120 1043 50  0000 C CNN
F 2 "" H 10100 900 50  0001 C CNN
F 3 "" H 10100 900 50  0001 C CNN
	1    10100 900 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0170
U 1 1 5C4D4597
P 10550 950
F 0 "#PWR0170" H 10550 700 50  0001 C CNN
F 1 "GND" H 10555 777 50  0000 C CNN
F 2 "" H 10550 950 50  0001 C CNN
F 3 "" H 10550 950 50  0001 C CNN
	1    10550 950 
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5C4D4639
P 9650 1050
F 0 "#FLG0101" H 9650 1125 50  0001 C CNN
F 1 "PWR_FLAG" H 9650 1223 50  0000 C CNN
F 2 "" H 9650 1050 50  0001 C CNN
F 3 "~" H 9650 1050 50  0001 C CNN
	1    9650 1050
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5C4D467F
P 10100 1050
F 0 "#FLG0102" H 10100 1125 50  0001 C CNN
F 1 "PWR_FLAG" H 10100 1223 50  0000 C CNN
F 2 "" H 10100 1050 50  0001 C CNN
F 3 "~" H 10100 1050 50  0001 C CNN
	1    10100 1050
	-1   0    0    1   
$EndComp
$Comp
L power:PWR_FLAG #FLG0103
U 1 1 5C4D46A9
P 10550 1050
F 0 "#FLG0103" H 10550 1125 50  0001 C CNN
F 1 "PWR_FLAG" H 10550 1223 50  0000 C CNN
F 2 "" H 10550 1050 50  0001 C CNN
F 3 "~" H 10550 1050 50  0001 C CNN
	1    10550 1050
	-1   0    0    1   
$EndComp
Wire Wire Line
	9650 1050 9650 900 
Wire Wire Line
	10100 1050 10100 900 
Wire Wire Line
	10550 1050 10550 950 
$Comp
L Device:L L2
U 1 1 5BE3E6E6
P 7550 3000
F 0 "L2" V 7740 3000 50  0000 C CNN
F 1 "BLM21AG601SN1D" V 7649 3000 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric" H 7550 3000 50  0001 C CNN
F 3 "~" H 7550 3000 50  0001 C CNN
	1    7550 3000
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C34
U 1 1 5BE3E796
P 7800 3200
F 0 "C34" H 7915 3246 50  0000 L CNN
F 1 "100n" H 7915 3155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7838 3050 50  0001 C CNN
F 3 "~" H 7800 3200 50  0001 C CNN
	1    7800 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 3000 7800 3000
$Comp
L Device:CP C36
U 1 1 5BE40030
P 8250 3200
F 0 "C36" H 8368 3246 50  0000 L CNN
F 1 "1u" H 8368 3155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 8288 3050 50  0001 C CNN
F 3 "~" H 8250 3200 50  0001 C CNN
	1    8250 3200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R17
U 1 1 5C97A9EA
P 4800 4900
F 0 "R17" H 4870 4946 50  0000 L CNN
F 1 "4k7" H 4870 4855 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 4730 4900 50  0001 C CNN
F 3 "~" H 4800 4900 50  0001 C CNN
	1    4800 4900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R18
U 1 1 5C97A9F1
P 4800 5350
F 0 "R18" H 4870 5396 50  0000 L CNN
F 1 "9k1" H 4870 5305 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 4730 5350 50  0001 C CNN
F 3 "~" H 4800 5350 50  0001 C CNN
	1    4800 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 5050 4800 5150
$Comp
L power:GND #PWR0171
U 1 1 5C97AA00
P 4800 5600
F 0 "#PWR0171" H 4800 5350 50  0001 C CNN
F 1 "GND" H 4805 5427 50  0000 C CNN
F 2 "" H 4800 5600 50  0001 C CNN
F 3 "" H 4800 5600 50  0001 C CNN
	1    4800 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 5500 4800 5550
Wire Wire Line
	4800 5150 5150 5150
Connection ~ 4800 5150
Wire Wire Line
	4800 5150 4800 5200
$Comp
L Device:D_Zener D7
U 1 1 5C97AA0A
P 5150 5350
F 0 "D7" V 5104 5429 50  0000 L CNN
F 1 "3.3 V" V 5195 5429 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-523" H 5150 5350 50  0001 C CNN
F 3 "~" H 5150 5350 50  0001 C CNN
	1    5150 5350
	0    1    1    0   
$EndComp
Wire Wire Line
	4800 5550 5150 5550
Wire Wire Line
	5150 5550 5150 5500
Connection ~ 4800 5550
Wire Wire Line
	4800 5550 4800 5600
Wire Wire Line
	5150 5200 5150 5150
Connection ~ 5150 5150
Wire Wire Line
	5150 5150 5550 5150
$Comp
L Device:C C32
U 1 1 5C97AA18
P 5550 5350
F 0 "C32" H 5665 5396 50  0000 L CNN
F 1 "100n" H 5665 5305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5588 5200 50  0001 C CNN
F 3 "~" H 5550 5350 50  0001 C CNN
	1    5550 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 5500 5550 5550
Wire Wire Line
	5550 5550 5150 5550
Connection ~ 5150 5550
Wire Wire Line
	5550 5200 5550 5150
Connection ~ 5550 5150
Wire Wire Line
	5550 5150 5700 5150
Wire Wire Line
	4450 3000 4800 3000
Wire Wire Line
	4800 3000 4800 3950
Connection ~ 4800 3000
Wire Wire Line
	4800 3000 5200 3000
Wire Wire Line
	5000 3950 4800 3950
Connection ~ 5000 3950
Connection ~ 4800 3950
Wire Wire Line
	4800 3950 4800 4750
Text HLabel 5700 5150 2    50   Output ~ 0
ADC_5V
Connection ~ 6250 3000
Wire Wire Line
	7800 3000 7800 3050
Wire Wire Line
	8250 3050 8250 3000
Wire Wire Line
	8250 3000 7800 3000
Connection ~ 7800 3000
Wire Wire Line
	8250 2950 8250 3000
Connection ~ 8250 3000
$Comp
L Device:D D8
U 1 1 5BEE85DB
P 5800 2600
F 0 "D8" H 5800 2816 50  0000 C CNN
F 1 "BAT60" H 5800 2725 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-323" H 5800 2600 50  0001 C CNN
F 3 "~" H 5800 2600 50  0001 C CNN
	1    5800 2600
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0181
U 1 1 5C9DEE90
P 9200 950
F 0 "#PWR0181" H 9200 800 50  0001 C CNN
F 1 "+5V" H 9215 1123 50  0000 C CNN
F 2 "" H 9200 950 50  0001 C CNN
F 3 "" H 9200 950 50  0001 C CNN
	1    9200 950 
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0104
U 1 1 5C9DEF2D
P 9200 1050
F 0 "#FLG0104" H 9200 1125 50  0001 C CNN
F 1 "PWR_FLAG" H 9200 1223 50  0000 C CNN
F 2 "" H 9200 1050 50  0001 C CNN
F 3 "~" H 9200 1050 50  0001 C CNN
	1    9200 1050
	-1   0    0    1   
$EndComp
Wire Wire Line
	9200 1050 9200 950 
$Comp
L Device:CP C37
U 1 1 5C207C40
P 3000 3200
F 0 "C37" H 3118 3246 50  0000 L CNN
F 1 "100u" H 3118 3155 50  0000 L CNN
F 2 "Capacitor_SMD:CP_Elec_6.3x7.7" H 3038 3050 50  0001 C CNN
F 3 "~" H 3000 3200 50  0001 C CNN
	1    3000 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 3050 3000 3000
Connection ~ 3000 3000
Wire Wire Line
	3000 3000 3450 3000
$Comp
L power:GND #PWR0130
U 1 1 5C2090CE
P 3000 3400
F 0 "#PWR0130" H 3000 3150 50  0001 C CNN
F 1 "GND" H 3005 3227 50  0000 C CNN
F 2 "" H 3000 3400 50  0001 C CNN
F 3 "" H 3000 3400 50  0001 C CNN
	1    3000 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 3400 3000 3350
$Comp
L Device:C C40
U 1 1 5C20C385
P 3450 3200
F 0 "C40" H 3565 3246 50  0000 L CNN
F 1 "100n" H 3565 3155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3488 3050 50  0001 C CNN
F 3 "~" H 3450 3200 50  0001 C CNN
	1    3450 3200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0162
U 1 1 5C20C466
P 3450 3400
F 0 "#PWR0162" H 3450 3150 50  0001 C CNN
F 1 "GND" H 3455 3227 50  0000 C CNN
F 2 "" H 3450 3400 50  0001 C CNN
F 3 "" H 3450 3400 50  0001 C CNN
	1    3450 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 3400 3450 3350
Wire Wire Line
	3450 3050 3450 3000
Connection ~ 3450 3000
Wire Wire Line
	3450 3000 4450 3000
$Comp
L Device:C C41
U 1 1 5C122697
P 5200 3200
F 0 "C41" H 5315 3246 50  0000 L CNN
F 1 "100n" H 5315 3155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5238 3050 50  0001 C CNN
F 3 "~" H 5200 3200 50  0001 C CNN
	1    5200 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5200 3050 5200 3000
Connection ~ 5200 3000
Wire Wire Line
	5200 3000 5500 3000
Wire Wire Line
	5800 3400 5200 3400
Wire Wire Line
	5200 3400 5200 3350
$Comp
L Device:C C42
U 1 1 5C12C9C6
P 6750 3200
F 0 "C42" H 6865 3246 50  0000 L CNN
F 1 "10u" H 6865 3155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6788 3050 50  0001 C CNN
F 3 "~" H 6750 3200 50  0001 C CNN
	1    6750 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6750 3000 6750 3050
Wire Wire Line
	6250 3000 6750 3000
Wire Wire Line
	6750 3350 6750 3400
Wire Wire Line
	6750 3400 6250 3400
Connection ~ 6250 3400
Wire Wire Line
	6750 3000 7000 3000
Connection ~ 6750 3000
$Comp
L power:+3.3V #PWR0131
U 1 1 5C131872
P 7000 2950
F 0 "#PWR0131" H 7000 2800 50  0001 C CNN
F 1 "+3.3V" H 7015 3123 50  0000 C CNN
F 2 "" H 7000 2950 50  0001 C CNN
F 3 "" H 7000 2950 50  0001 C CNN
	1    7000 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	7000 2950 7000 3000
Connection ~ 7000 3000
Wire Wire Line
	7000 3000 7400 3000
Wire Wire Line
	8250 3350 8250 3400
Wire Wire Line
	8250 3400 7800 3400
Connection ~ 6750 3400
Wire Wire Line
	7800 3350 7800 3400
Connection ~ 7800 3400
Wire Wire Line
	7800 3400 6750 3400
Wire Wire Line
	6600 3950 6800 3950
Wire Wire Line
	5650 2600 5200 2600
Wire Wire Line
	5200 2600 5200 3000
Wire Wire Line
	5950 2600 6250 2600
Wire Wire Line
	6250 2600 6250 3000
$Comp
L power:+3.3VP #PWR0165
U 1 1 5C141841
P 6800 3900
F 0 "#PWR0165" H 6950 3850 50  0001 C CNN
F 1 "+3.3VP" H 6820 4043 50  0000 C CNN
F 2 "" H 6800 3900 50  0001 C CNN
F 3 "" H 6800 3900 50  0001 C CNN
	1    6800 3900
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3VP #PWR0167
U 1 1 5C177A9A
P 9650 900
F 0 "#PWR0167" H 9800 850 50  0001 C CNN
F 1 "+3.3VP" H 9670 1043 50  0000 C CNN
F 2 "" H 9650 900 50  0001 C CNN
F 3 "" H 9650 900 50  0001 C CNN
	1    9650 900 
	1    0    0    -1  
$EndComp
$EndSCHEMATC
