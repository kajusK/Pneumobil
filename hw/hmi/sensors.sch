EESchema Schematic File Version 4
LIBS:hmi-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 5 5
Title "BUT Pneumobil - HMI sensors"
Date "2018-12-23"
Rev "0.1"
Comp "Brno University of Technology"
Comment1 "Jakub Kaderka"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Sensor:BME280 U?
U 1 1 5C645A9B
P 3550 3150
AR Path="/5C645A9B" Ref="U?"  Part="1" 
AR Path="/5C5A63AB/5C645A9B" Ref="U9"  Part="1" 
F 0 "U9" H 3250 3600 50  0000 R CNN
F 1 "BME280" H 3350 2700 50  0000 R CNN
F 2 "Package_LGA:Bosch_LGA-8_2.5x2.5mm_P0.65mm_ClockwisePinNumbering" H 3550 2950 50  0001 C CNN
F 3 "https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf" H 3550 2950 50  0001 C CNN
	1    3550 3150
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5C645AA2
P 3550 3850
AR Path="/5C645AA2" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C645AA2" Ref="#PWR05"  Part="1" 
F 0 "#PWR05" H 3550 3600 50  0001 C CNN
F 1 "GND" H 3555 3677 50  0000 C CNN
F 2 "" H 3550 3850 50  0001 C CNN
F 3 "" H 3550 3850 50  0001 C CNN
	1    3550 3850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3550 3850 3550 3800
Wire Wire Line
	3550 3800 3450 3800
Wire Wire Line
	3450 3800 3450 3750
Wire Wire Line
	3550 3800 3650 3800
Wire Wire Line
	3650 3800 3650 3750
Connection ~ 3550 3800
$Comp
L power:+3V3 #PWR?
U 1 1 5C645AAE
P 3550 2450
AR Path="/5C645AAE" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C645AAE" Ref="#PWR04"  Part="1" 
F 0 "#PWR04" H 3550 2300 50  0001 C CNN
F 1 "+3V3" H 3565 2623 50  0000 C CNN
F 2 "" H 3550 2450 50  0001 C CNN
F 3 "" H 3550 2450 50  0001 C CNN
	1    3550 2450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3550 2450 3550 2500
Wire Wire Line
	3550 2500 3650 2500
Wire Wire Line
	3650 2500 3650 2550
Wire Wire Line
	3550 2500 3450 2500
Wire Wire Line
	3450 2500 3450 2550
Connection ~ 3550 2500
Wire Wire Line
	3750 2500 3650 2500
Connection ~ 3650 2500
$Comp
L power:GND #PWR?
U 1 1 5C645AC3
P 4100 2500
AR Path="/5C645AC3" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C645AC3" Ref="#PWR06"  Part="1" 
F 0 "#PWR06" H 4100 2250 50  0001 C CNN
F 1 "GND" V 4105 2372 50  0000 R CNN
F 2 "" H 4100 2500 50  0001 C CNN
F 3 "" H 4100 2500 50  0001 C CNN
	1    4100 2500
	0    -1   1    0   
$EndComp
Wire Wire Line
	4100 2500 4050 2500
Wire Wire Line
	2950 3450 2750 3450
Wire Wire Line
	2950 3250 2750 3250
Wire Wire Line
	2950 3050 2750 3050
Wire Wire Line
	2950 2850 2750 2850
$Comp
L Sensor_Motion:LSM303C U?
U 1 1 5C6A6629
P 9600 3200
AR Path="/5C6A6629" Ref="U?"  Part="1" 
AR Path="/5C5A63AB/5C6A6629" Ref="U5"  Part="1" 
F 0 "U5" H 9150 3800 50  0000 C CNN
F 1 "LSM303C" H 9250 3700 50  0000 C CNN
F 2 "Package_LGA:LGA-12_2x2mm_P0.5mm" H 9200 2500 50  0001 L CNN
F 3 "www.st.com/resource/en/datasheet/lsm303c.pdf" H 9700 2550 50  0001 C CNN
	1    9600 3200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5C6A6630
P 9650 3900
AR Path="/5C6A6630" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C6A6630" Ref="#PWR012"  Part="1" 
F 0 "#PWR012" H 9650 3650 50  0001 C CNN
F 1 "GND" H 9655 3727 50  0000 C CNN
F 2 "" H 9650 3900 50  0001 C CNN
F 3 "" H 9650 3900 50  0001 C CNN
	1    9650 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 3800 9600 3850
Wire Wire Line
	9600 3850 9650 3850
Wire Wire Line
	9700 3850 9700 3800
Wire Wire Line
	9650 3900 9650 3850
Connection ~ 9650 3850
Wire Wire Line
	9650 3850 9700 3850
Wire Wire Line
	8900 3000 8500 3000
Wire Wire Line
	8900 3100 8500 3100
$Comp
L Device:R R?
U 1 1 5C6A6640
P 8600 3200
AR Path="/5C6A6640" Ref="R?"  Part="1" 
AR Path="/5C5A63AB/5C6A6640" Ref="R10"  Part="1" 
F 0 "R10" V 8550 3400 50  0000 C CNN
F 1 "1k" V 8600 3200 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric" V 8530 3200 50  0001 C CNN
F 3 "~" H 8600 3200 50  0001 C CNN
	1    8600 3200
	0    1    1    0   
$EndComp
Wire Wire Line
	8750 3200 8800 3200
$Comp
L power:+3V3 #PWR?
U 1 1 5C6A6648
P 8400 3200
AR Path="/5C6A6648" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C6A6648" Ref="#PWR010"  Part="1" 
F 0 "#PWR010" H 8400 3050 50  0001 C CNN
F 1 "+3V3" V 8415 3328 50  0000 L CNN
F 2 "" H 8400 3200 50  0001 C CNN
F 3 "" H 8400 3200 50  0001 C CNN
	1    8400 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8400 3200 8450 3200
$Comp
L Device:C C?
U 1 1 5C6A664F
P 10350 3600
AR Path="/5C6A664F" Ref="C?"  Part="1" 
AR Path="/5C5A63AB/5C6A664F" Ref="C30"  Part="1" 
F 0 "C30" H 10465 3646 50  0000 L CNN
F 1 "100n" H 10465 3555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 10388 3450 50  0001 C CNN
F 3 "~" H 10350 3600 50  0001 C CNN
	1    10350 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	10300 3400 10350 3400
Wire Wire Line
	10350 3400 10350 3450
$Comp
L power:GND #PWR?
U 1 1 5C6A6658
P 10350 3800
AR Path="/5C6A6658" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C6A6658" Ref="#PWR014"  Part="1" 
F 0 "#PWR014" H 10350 3550 50  0001 C CNN
F 1 "GND" H 10355 3627 50  0000 C CNN
F 2 "" H 10350 3800 50  0001 C CNN
F 3 "" H 10350 3800 50  0001 C CNN
	1    10350 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	10350 3800 10350 3750
Wire Wire Line
	8900 3300 8800 3300
Wire Wire Line
	8800 3300 8800 3200
Connection ~ 8800 3200
Wire Wire Line
	8800 3200 8900 3200
$Comp
L power:+3V3 #PWR?
U 1 1 5C6A6663
P 9600 2500
AR Path="/5C6A6663" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C6A6663" Ref="#PWR011"  Part="1" 
F 0 "#PWR011" H 9600 2350 50  0001 C CNN
F 1 "+3V3" H 9615 2673 50  0000 C CNN
F 2 "" H 9600 2500 50  0001 C CNN
F 3 "" H 9600 2500 50  0001 C CNN
	1    9600 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	9600 2600 9600 2550
Wire Wire Line
	9700 2550 9700 2600
Wire Wire Line
	9600 2500 9600 2550
Connection ~ 9600 2550
Wire Wire Line
	9600 2550 9700 2550
NoConn ~ 10300 3000
NoConn ~ 10300 3100
NoConn ~ 10300 3200
$Comp
L Device:C C?
U 1 1 5C6A6671
P 9900 2550
AR Path="/5C6A6671" Ref="C?"  Part="1" 
AR Path="/5C5A63AB/5C6A6671" Ref="C29"  Part="1" 
F 0 "C29" V 9750 2550 50  0000 C CNN
F 1 "100n" V 10050 2550 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 9938 2400 50  0001 C CNN
F 3 "~" H 9900 2550 50  0001 C CNN
	1    9900 2550
	0    1    1    0   
$EndComp
Wire Wire Line
	9750 2550 9700 2550
Connection ~ 9700 2550
$Comp
L power:GND #PWR?
U 1 1 5C6A667A
P 10100 2550
AR Path="/5C6A667A" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C6A667A" Ref="#PWR013"  Part="1" 
F 0 "#PWR013" H 10100 2300 50  0001 C CNN
F 1 "GND" V 10105 2422 50  0000 R CNN
F 2 "" H 10100 2550 50  0001 C CNN
F 3 "" H 10100 2550 50  0001 C CNN
	1    10100 2550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10100 2550 10050 2550
Text Notes 9400 4250 0    50   ~ 0
Accelerometer
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 5C6D4BF4
P 6500 3300
AR Path="/5C6D4BF4" Ref="J?"  Part="1" 
AR Path="/5C5A63AB/5C6D4BF4" Ref="J3"  Part="1" 
F 0 "J3" H 6579 3292 50  0000 L CNN
F 1 "GPS" H 6579 3201 50  0000 L CNN
F 2 "Connector_Molex:Molex_KK-254_AE-6410-04A_1x04_P2.54mm_Vertical" H 6500 3300 50  0001 C CNN
F 3 "~" H 6500 3300 50  0001 C CNN
	1    6500 3300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5C6D4BFF
P 6250 3550
AR Path="/5C6D4BFF" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C6D4BFF" Ref="#PWR09"  Part="1" 
F 0 "#PWR09" H 6250 3300 50  0001 C CNN
F 1 "GND" H 6255 3377 50  0000 C CNN
F 2 "" H 6250 3550 50  0001 C CNN
F 3 "" H 6250 3550 50  0001 C CNN
	1    6250 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 3550 6250 3500
Wire Wire Line
	6250 3500 6300 3500
$Comp
L power:+3V3 #PWR?
U 1 1 5C6D4C07
P 6250 3050
AR Path="/5C6D4C07" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C6D4C07" Ref="#PWR08"  Part="1" 
F 0 "#PWR08" H 6250 2900 50  0001 C CNN
F 1 "+3V3" H 6265 3223 50  0000 C CNN
F 2 "" H 6250 3050 50  0001 C CNN
F 3 "" H 6250 3050 50  0001 C CNN
	1    6250 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 3050 6250 3100
Wire Wire Line
	6250 3200 6300 3200
$Comp
L Device:C C?
U 1 1 5C6D4C0F
P 6000 3100
AR Path="/5C6D4C0F" Ref="C?"  Part="1" 
AR Path="/5C5A63AB/5C6D4C0F" Ref="C17"  Part="1" 
F 0 "C17" V 6252 3100 50  0000 C CNN
F 1 "100n" V 6161 3100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 6038 2950 50  0001 C CNN
F 3 "~" H 6000 3100 50  0001 C CNN
	1    6000 3100
	0    1    -1   0   
$EndComp
Wire Wire Line
	6150 3100 6250 3100
Connection ~ 6250 3100
Wire Wire Line
	6250 3100 6250 3200
$Comp
L power:GND #PWR?
U 1 1 5C6D4C19
P 5800 3100
AR Path="/5C6D4C19" Ref="#PWR?"  Part="1" 
AR Path="/5C5A63AB/5C6D4C19" Ref="#PWR07"  Part="1" 
F 0 "#PWR07" H 5800 2850 50  0001 C CNN
F 1 "GND" V 5805 2972 50  0000 R CNN
F 2 "" H 5800 3100 50  0001 C CNN
F 3 "" H 5800 3100 50  0001 C CNN
	1    5800 3100
	0    1    1    0   
$EndComp
Wire Wire Line
	5800 3100 5850 3100
Text HLabel 8500 3000 0    50   Input ~ 0
SCL
Text HLabel 8500 3100 0    50   Input ~ 0
SDA
Text HLabel 2750 2850 0    50   Output ~ 0
MISO
Text HLabel 2750 3250 0    50   Input ~ 0
MOSI
Text HLabel 2750 3050 0    50   Input ~ 0
SCK
Text HLabel 2750 3450 0    50   Input ~ 0
BME_CS
Text Notes 2950 4200 0    50   ~ 0
Pressure, Humidity, Temperature
Text HLabel 5700 3300 0    50   Output ~ 0
GPS_RX
Text HLabel 5700 3400 0    50   Input ~ 0
GPS_TX
Wire Wire Line
	5700 3300 6300 3300
Wire Wire Line
	6300 3400 5700 3400
$Comp
L Device:C C43
U 1 1 5C9D716A
P 3900 2500
F 0 "C43" V 3648 2500 50  0000 C CNN
F 1 "100n" V 3739 2500 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric" H 3938 2350 50  0001 C CNN
F 3 "~" H 3900 2500 50  0001 C CNN
	1    3900 2500
	0    1    1    0   
$EndComp
$EndSCHEMATC
