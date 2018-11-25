EESchema Schematic File Version 4
LIBS:hmi-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 4
Title "BUT Pneumobil - HMI Power supply"
Date "2018-11-13"
Rev "0.1"
Comp "Brno University of Technology"
Comment1 "Jakub Kaderka"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 3350 3000 0    50   Input ~ 0
VIN
$Comp
L Regulator_Linear:LD1117S33TR_SOT223 U7
U 1 1 5C379855
P 5750 3000
F 0 "U7" H 5750 3242 50  0000 C CNN
F 1 "TS1117BCW33" H 5750 3151 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 5750 3200 50  0001 C CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00000544.pdf" H 5850 2750 50  0001 C CNN
	1    5750 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C33
U 1 1 5C3799B9
P 6200 3200
F 0 "C33" H 6315 3246 50  0000 L CNN
F 1 "100n" H 6315 3155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6238 3050 50  0001 C CNN
F 3 "~" H 6200 3200 50  0001 C CNN
	1    6200 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 3000 6200 3000
Wire Wire Line
	6200 3050 6200 3000
$Comp
L power:GND #PWR0159
U 1 1 5C379AA7
P 5750 3450
F 0 "#PWR0159" H 5750 3200 50  0001 C CNN
F 1 "GND" H 5755 3277 50  0000 C CNN
F 2 "" H 5750 3450 50  0001 C CNN
F 3 "" H 5750 3450 50  0001 C CNN
	1    5750 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 3450 5750 3400
Wire Wire Line
	5750 3400 6200 3400
Connection ~ 5750 3400
Wire Wire Line
	5750 3400 5750 3300
Wire Wire Line
	6200 3350 6200 3400
$Comp
L Device:LED D4
U 1 1 5C37AFE9
P 3400 3900
F 0 "D4" V 3438 3783 50  0000 R CNN
F 1 "3.3 V" V 3347 3783 50  0000 R CNN
F 2 "LED_SMD:LED_1206_3216Metric" H 3400 3900 50  0001 C CNN
F 3 "~" H 3400 3900 50  0001 C CNN
	1    3400 3900
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R14
U 1 1 5C37B05F
P 3400 3550
F 0 "R14" H 3470 3596 50  0000 L CNN
F 1 "220" H 3470 3505 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 3330 3550 50  0001 C CNN
F 3 "~" H 3400 3550 50  0001 C CNN
	1    3400 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 3700 3400 3750
$Comp
L power:GND #PWR0160
U 1 1 5C37B20A
P 3400 4200
F 0 "#PWR0160" H 3400 3950 50  0001 C CNN
F 1 "GND" H 3405 4027 50  0000 C CNN
F 2 "" H 3400 4200 50  0001 C CNN
F 3 "" H 3400 4200 50  0001 C CNN
	1    3400 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 4200 3400 4050
$Comp
L power:+3V3 #PWR0161
U 1 1 5C37B3FC
P 3400 3350
F 0 "#PWR0161" H 3400 3200 50  0001 C CNN
F 1 "+3V3" H 3415 3523 50  0000 C CNN
F 2 "" H 3400 3350 50  0001 C CNN
F 3 "" H 3400 3350 50  0001 C CNN
	1    3400 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 3350 3400 3400
$Comp
L Device:LED D5
U 1 1 5C37B5E1
P 3950 3900
F 0 "D5" V 3988 3783 50  0000 R CNN
F 1 "Vadc" V 3897 3783 50  0000 R CNN
F 2 "LED_SMD:LED_1206_3216Metric" H 3950 3900 50  0001 C CNN
F 3 "~" H 3950 3900 50  0001 C CNN
	1    3950 3900
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R15
U 1 1 5C37B5E7
P 3950 3550
F 0 "R15" H 4020 3596 50  0000 L CNN
F 1 "220" H 4020 3505 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 3880 3550 50  0001 C CNN
F 3 "~" H 3950 3550 50  0001 C CNN
	1    3950 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 3700 3950 3750
$Comp
L power:GND #PWR0162
U 1 1 5C37B5EE
P 3950 4200
F 0 "#PWR0162" H 3950 3950 50  0001 C CNN
F 1 "GND" H 3955 4027 50  0000 C CNN
F 2 "" H 3950 4200 50  0001 C CNN
F 3 "" H 3950 4200 50  0001 C CNN
	1    3950 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3950 4200 3950 4050
Wire Wire Line
	3950 3300 3950 3400
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
P 7250 2950
F 0 "#PWR0164" H 7400 2900 50  0001 C CNN
F 1 "+3.3VADC" H 7270 3093 50  0000 C CNN
F 2 "" H 7250 2950 50  0001 C CNN
F 3 "" H 7250 2950 50  0001 C CNN
	1    7250 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 3000 4450 3000
$Comp
L power:+3.3VADC #PWR0165
U 1 1 5C4232C5
P 3950 3300
F 0 "#PWR0165" H 4100 3250 50  0001 C CNN
F 1 "+3.3VADC" H 3970 3443 50  0000 C CNN
F 2 "" H 3950 3300 50  0001 C CNN
F 3 "" H 3950 3300 50  0001 C CNN
	1    3950 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3400 4450 3000
Connection ~ 4450 3000
$Comp
L Regulator_Switching:ADP2108AUJ-3.3 U8
U 1 1 5C4CBB20
P 5750 4500
F 0 "U8" H 5900 4250 50  0000 C CNN
F 1 "MCP1603T-330" H 5600 4700 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:TSOT-23-5" H 5800 4250 50  0001 L CNN
F 3 "http://www.analog.com/media/en/technical-documentation/data-sheets/ADP2108.pdf" H 5500 4150 50  0001 C CNN
	1    5750 4500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0166
U 1 1 5C4CC4B4
P 5750 5000
F 0 "#PWR0166" H 5750 4750 50  0001 C CNN
F 1 "GND" H 5755 4827 50  0000 C CNN
F 2 "" H 5750 5000 50  0001 C CNN
F 3 "" H 5750 5000 50  0001 C CNN
	1    5750 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 4800 5000 4900
Wire Wire Line
	5000 4900 5750 4900
Wire Wire Line
	5750 4900 5750 5000
Wire Wire Line
	5750 4900 5750 4800
Connection ~ 5750 4900
Wire Wire Line
	5450 4400 5350 4400
Wire Wire Line
	5000 4400 5000 4500
Wire Wire Line
	5450 4500 5350 4500
Wire Wire Line
	5350 4500 5350 4400
Connection ~ 5350 4400
Wire Wire Line
	5350 4400 5000 4400
$Comp
L Device:L L1
U 1 1 5C4CD722
P 6350 4400
F 0 "L1" V 6540 4400 50  0000 C CNN
F 1 "LPS4018-103MRB" V 6449 4400 50  0000 C CNN
F 2 "Inductor_SMD:L_Coilcraft_LPS4018" H 6350 4400 50  0001 C CNN
F 3 "~" H 6350 4400 50  0001 C CNN
	1    6350 4400
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C31
U 1 1 5C4CE1F1
P 5000 4650
F 0 "C31" H 5115 4696 50  0000 L CNN
F 1 "4.7u" H 5115 4605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5038 4500 50  0001 C CNN
F 3 "~" H 5000 4650 50  0001 C CNN
	1    5000 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 4400 6050 4400
Wire Wire Line
	6050 4600 6600 4600
Wire Wire Line
	6600 4600 6600 4400
Wire Wire Line
	6600 4400 6500 4400
$Comp
L Device:C C35
U 1 1 5C4CF073
P 6800 4650
F 0 "C35" H 6915 4696 50  0000 L CNN
F 1 "4.7u" H 6915 4605 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6838 4500 50  0001 C CNN
F 3 "~" H 6800 4650 50  0001 C CNN
	1    6800 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 4900 6800 4900
Wire Wire Line
	6800 4900 6800 4800
Wire Wire Line
	6600 4400 6700 4400
Wire Wire Line
	6800 4400 6800 4500
Connection ~ 6600 4400
$Comp
L power:+3V3 #PWR0167
U 1 1 5C4CFC02
P 6800 4350
F 0 "#PWR0167" H 6800 4200 50  0001 C CNN
F 1 "+3V3" H 6815 4523 50  0000 C CNN
F 2 "" H 6800 4350 50  0001 C CNN
F 3 "" H 6800 4350 50  0001 C CNN
	1    6800 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 4350 6800 4400
Connection ~ 6800 4400
$Comp
L power:+3V3 #PWR0168
U 1 1 5C4D4378
P 9650 950
F 0 "#PWR0168" H 9650 800 50  0001 C CNN
F 1 "+3V3" H 9665 1123 50  0000 C CNN
F 2 "" H 9650 950 50  0001 C CNN
F 3 "" H 9650 950 50  0001 C CNN
	1    9650 950 
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3VADC #PWR0169
U 1 1 5C4D449A
P 10100 950
F 0 "#PWR0169" H 10250 900 50  0001 C CNN
F 1 "+3.3VADC" H 10120 1093 50  0000 C CNN
F 2 "" H 10100 950 50  0001 C CNN
F 3 "" H 10100 950 50  0001 C CNN
	1    10100 950 
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
	9650 1050 9650 950 
Wire Wire Line
	10100 1050 10100 950 
Wire Wire Line
	10550 1050 10550 950 
$Comp
L Device:L L2
U 1 1 5BE3E6E6
P 6550 3000
F 0 "L2" V 6740 3000 50  0000 C CNN
F 1 "BLM21AG601SN1D" V 6649 3000 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric" H 6550 3000 50  0001 C CNN
F 3 "~" H 6550 3000 50  0001 C CNN
	1    6550 3000
	0    -1   -1   0   
$EndComp
$Comp
L Device:C C34
U 1 1 5BE3E796
P 6800 3200
F 0 "C34" H 6915 3246 50  0000 L CNN
F 1 "100n" H 6915 3155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6838 3050 50  0001 C CNN
F 3 "~" H 6800 3200 50  0001 C CNN
	1    6800 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 3000 6800 3000
$Comp
L Device:CP C36
U 1 1 5BE40030
P 7250 3200
F 0 "C36" H 7368 3246 50  0000 L CNN
F 1 "1u" H 7368 3155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 7288 3050 50  0001 C CNN
F 3 "~" H 7250 3200 50  0001 C CNN
	1    7250 3200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R17
U 1 1 5C97A9EA
P 4800 5350
F 0 "R17" H 4870 5396 50  0000 L CNN
F 1 "4k7" H 4870 5305 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 4730 5350 50  0001 C CNN
F 3 "~" H 4800 5350 50  0001 C CNN
	1    4800 5350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R18
U 1 1 5C97A9F1
P 4800 5800
F 0 "R18" H 4870 5846 50  0000 L CNN
F 1 "9k1" H 4870 5755 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 4730 5800 50  0001 C CNN
F 3 "~" H 4800 5800 50  0001 C CNN
	1    4800 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 5500 4800 5600
$Comp
L power:GND #PWR0171
U 1 1 5C97AA00
P 4800 6050
F 0 "#PWR0171" H 4800 5800 50  0001 C CNN
F 1 "GND" H 4805 5877 50  0000 C CNN
F 2 "" H 4800 6050 50  0001 C CNN
F 3 "" H 4800 6050 50  0001 C CNN
	1    4800 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 5950 4800 6000
Wire Wire Line
	4800 5600 5150 5600
Connection ~ 4800 5600
Wire Wire Line
	4800 5600 4800 5650
$Comp
L Device:D_Zener D7
U 1 1 5C97AA0A
P 5150 5800
F 0 "D7" V 5104 5879 50  0000 L CNN
F 1 "3.3 V" V 5195 5879 50  0000 L CNN
F 2 "Diode_SMD:D_SOD-523" H 5150 5800 50  0001 C CNN
F 3 "~" H 5150 5800 50  0001 C CNN
	1    5150 5800
	0    1    1    0   
$EndComp
Wire Wire Line
	4800 6000 5150 6000
Wire Wire Line
	5150 6000 5150 5950
Connection ~ 4800 6000
Wire Wire Line
	4800 6000 4800 6050
Wire Wire Line
	5150 5650 5150 5600
Connection ~ 5150 5600
Wire Wire Line
	5150 5600 5550 5600
$Comp
L Device:C C32
U 1 1 5C97AA18
P 5550 5800
F 0 "C32" H 5665 5846 50  0000 L CNN
F 1 "100n" H 5665 5755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 5588 5650 50  0001 C CNN
F 3 "~" H 5550 5800 50  0001 C CNN
	1    5550 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 5950 5550 6000
Wire Wire Line
	5550 6000 5150 6000
Connection ~ 5150 6000
Wire Wire Line
	5550 5650 5550 5600
Connection ~ 5550 5600
Wire Wire Line
	5550 5600 5700 5600
Wire Wire Line
	4450 3000 4800 3000
Wire Wire Line
	4800 3000 4800 4400
Connection ~ 4800 3000
Wire Wire Line
	4800 3000 5450 3000
Wire Wire Line
	5000 4400 4800 4400
Connection ~ 5000 4400
Connection ~ 4800 4400
Wire Wire Line
	4800 4400 4800 5200
Text HLabel 5700 5600 2    50   Output ~ 0
ADC_5V
Wire Wire Line
	6400 3000 6200 3000
Connection ~ 6200 3000
Wire Wire Line
	6200 3400 6800 3400
Wire Wire Line
	7250 3400 7250 3350
Connection ~ 6200 3400
Wire Wire Line
	6800 3000 6800 3050
Wire Wire Line
	6800 3350 6800 3400
Connection ~ 6800 3400
Wire Wire Line
	6800 3400 7250 3400
Wire Wire Line
	7250 3050 7250 3000
Wire Wire Line
	7250 3000 6800 3000
Connection ~ 6800 3000
Wire Wire Line
	7250 2950 7250 3000
Connection ~ 7250 3000
$Comp
L Device:D D8
U 1 1 5BEE85DB
P 5750 4050
F 0 "D8" H 5750 4266 50  0000 C CNN
F 1 "BAT60" H 5750 4175 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-323" H 5750 4050 50  0001 C CNN
F 3 "~" H 5750 4050 50  0001 C CNN
	1    5750 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 4050 5000 4050
Wire Wire Line
	5000 4050 5000 4400
Wire Wire Line
	5900 4050 6700 4050
Wire Wire Line
	6700 4050 6700 4400
Connection ~ 6700 4400
Wire Wire Line
	6700 4400 6800 4400
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
$EndSCHEMATC
