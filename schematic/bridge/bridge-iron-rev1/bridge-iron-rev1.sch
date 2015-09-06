EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:MCU-board-connectors
LIBS:linStab-(1-GND,Adj,2-Out,3-in)
LIBS:nrf24l01
LIBS:bridge-iron-rev1-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "5 sep 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L MCU1 P1
U 1 1 55EAD13E
P 4000 2150
F 0 "P1" H 6350 3050 60  0000 C CNN
F 1 "MCU1" H 6350 2950 60  0000 C CNN
F 2 "~" H 4050 2050 60  0000 C CNN
F 3 "~" H 4050 2050 60  0000 C CNN
	1    4000 2150
	-1   0    0    -1  
$EndComp
$Comp
L MCU2 P4
U 1 1 55EAD14D
P 8300 2150
F 0 "P4" H 10650 2500 60  0000 C CNN
F 1 "MCU2" H 10650 2400 60  0000 C CNN
F 2 "~" H 8300 2150 60  0000 C CNN
F 3 "~" H 8300 2150 60  0000 C CNN
	1    8300 2150
	1    0    0    -1  
$EndComp
$Comp
L LINSTAB-(1-GND/ADJ,2-OUT,3-IN) U1
U 1 1 55EAD15D
P 3800 4250
F 0 "U1" H 3800 4400 60  0000 C CNN
F 1 "LINSTAB-(1-GND/ADJ,2-OUT,3-IN)" H 3850 4600 60  0000 C CNN
F 2 "~" H 3800 4250 60  0000 C CNN
F 3 "~" H 3800 4250 60  0000 C CNN
	1    3800 4250
	1    0    0    -1  
$EndComp
$Comp
L CONN_2 Power1
U 1 1 55EAD16C
P 1200 4350
F 0 "Power1" V 1150 4350 40  0000 C CNN
F 1 "CONN_2" V 1250 4350 40  0000 C CNN
F 2 "~" H 1200 4350 60  0000 C CNN
F 3 "~" H 1200 4350 60  0000 C CNN
	1    1200 4350
	-1   0    0    1   
$EndComp
$Comp
L CP1 C2
U 1 1 55EAD187
P 4800 4500
F 0 "C2" H 4850 4600 50  0000 L CNN
F 1 "CP1" H 4850 4400 50  0000 L CNN
F 2 "~" H 4800 4500 60  0000 C CNN
F 3 "~" H 4800 4500 60  0000 C CNN
	1    4800 4500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR01
U 1 1 55EAD833
P 2050 5000
F 0 "#PWR01" H 2050 5000 30  0001 C CNN
F 1 "GND" H 2050 4930 30  0001 C CNN
F 2 "" H 2050 5000 60  0000 C CNN
F 3 "" H 2050 5000 60  0000 C CNN
	1    2050 5000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 55EAD842
P 3800 5050
F 0 "#PWR02" H 3800 5050 30  0001 C CNN
F 1 "GND" H 3800 4980 30  0001 C CNN
F 2 "" H 3800 5050 60  0000 C CNN
F 3 "" H 3800 5050 60  0000 C CNN
	1    3800 5050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 55EAD851
P 4800 5050
F 0 "#PWR03" H 4800 5050 30  0001 C CNN
F 1 "GND" H 4800 4980 30  0001 C CNN
F 2 "" H 4800 5050 60  0000 C CNN
F 3 "" H 4800 5050 60  0000 C CNN
	1    4800 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 4850 3800 5050
Wire Wire Line
	4800 4700 4800 5050
Wire Wire Line
	4400 4250 5100 4250
Wire Wire Line
	4800 4250 4800 4300
Wire Wire Line
	2250 4250 3250 4250
$Comp
L +3.3V #PWR04
U 1 1 55EAD88D
P 5100 4250
F 0 "#PWR04" H 5100 4210 30  0001 C CNN
F 1 "+3.3V" H 5100 4360 30  0000 C CNN
F 2 "" H 5100 4250 60  0000 C CNN
F 3 "" H 5100 4250 60  0000 C CNN
	1    5100 4250
	0    1    1    0   
$EndComp
Connection ~ 4800 4250
$Comp
L GND #PWR05
U 1 1 55EAD8A4
P 4600 1100
F 0 "#PWR05" H 4600 1100 30  0001 C CNN
F 1 "GND" H 4600 1030 30  0001 C CNN
F 2 "" H 4600 1100 60  0000 C CNN
F 3 "" H 4600 1100 60  0000 C CNN
	1    4600 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4350 1200 4450 1200
Wire Wire Line
	4450 1200 4450 1000
Wire Wire Line
	4450 1000 4600 1000
Wire Wire Line
	4600 1000 4600 1100
$Comp
L +3.3V #PWR06
U 1 1 55EAD8BF
P 4600 1300
F 0 "#PWR06" H 4600 1260 30  0001 C CNN
F 1 "+3.3V" H 4600 1410 30  0000 C CNN
F 2 "" H 4600 1300 60  0000 C CNN
F 3 "" H 4600 1300 60  0000 C CNN
	1    4600 1300
	0    1    1    0   
$EndComp
Wire Wire Line
	4350 1300 4600 1300
$Comp
L R R1
U 1 1 55EAD8EA
P 5200 2700
F 0 "R1" V 5280 2700 40  0000 C CNN
F 1 "R" V 5207 2701 40  0000 C CNN
F 2 "~" V 5130 2700 30  0000 C CNN
F 3 "~" H 5200 2700 30  0000 C CNN
	1    5200 2700
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 55EAD8F9
P 5200 3350
F 0 "R2" V 5280 3350 40  0000 C CNN
F 1 "R" V 5207 3351 40  0000 C CNN
F 2 "~" V 5130 3350 30  0000 C CNN
F 3 "~" H 5200 3350 30  0000 C CNN
	1    5200 3350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 55EAD908
P 5200 3700
F 0 "#PWR07" H 5200 3700 30  0001 C CNN
F 1 "GND" H 5200 3630 30  0001 C CNN
F 2 "" H 5200 3700 60  0000 C CNN
F 3 "" H 5200 3700 60  0000 C CNN
	1    5200 3700
	1    0    0    -1  
$EndComp
$Comp
L +BATT #PWR08
U 1 1 55EAD917
P 2800 4150
F 0 "#PWR08" H 2800 4100 20  0001 C CNN
F 1 "+BATT" H 2800 4250 30  0000 C CNN
F 2 "" H 2800 4150 60  0000 C CNN
F 3 "" H 2800 4150 60  0000 C CNN
	1    2800 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 4150 2800 4250
Connection ~ 2800 4250
Wire Wire Line
	4350 2700 5050 2700
Wire Wire Line
	5050 2700 5050 3050
Wire Wire Line
	5050 3050 5200 3050
Wire Wire Line
	5200 2950 5200 3100
Connection ~ 5200 3050
Wire Wire Line
	5200 3600 5200 3700
$Comp
L +BATT #PWR09
U 1 1 55EAD96F
P 5200 2350
F 0 "#PWR09" H 5200 2300 20  0001 C CNN
F 1 "+BATT" H 5200 2450 30  0000 C CNN
F 2 "" H 5200 2350 60  0000 C CNN
F 3 "" H 5200 2350 60  0000 C CNN
	1    5200 2350
	1    0    0    -1  
$EndComp
$Comp
L NRF24L01 P5
U 1 1 55EAD994
P 8500 4100
F 0 "P5" H 8500 4150 60  0000 C CNN
F 1 "NRF24L01" H 8500 4300 60  0000 C CNN
F 2 "~" H 8500 4100 60  0000 C CNN
F 3 "~" H 8500 4100 60  0000 C CNN
	1    8500 4100
	0    1    -1   0   
$EndComp
Wire Wire Line
	7950 3850 7600 3850
Wire Wire Line
	7600 3850 7600 1600
Wire Wire Line
	7600 1600 7950 1600
Wire Wire Line
	7950 3950 7500 3950
Wire Wire Line
	7500 3950 7500 1700
Wire Wire Line
	7500 1700 7950 1700
Wire Wire Line
	7950 4050 7400 4050
Wire Wire Line
	7400 4050 7400 1500
Wire Wire Line
	7400 1500 7950 1500
Wire Wire Line
	7950 1400 7300 1400
Wire Wire Line
	7300 1400 7300 4150
Wire Wire Line
	7300 4150 7950 4150
Wire Wire Line
	7950 3750 7700 3750
Wire Wire Line
	7700 3750 7700 3000
Wire Wire Line
	7700 3000 7950 3000
Wire Wire Line
	7950 4250 7200 4250
Wire Wire Line
	7200 4250 7200 2900
Wire Wire Line
	7200 2900 7950 2900
$Comp
L GND #PWR010
U 1 1 55EADA65
P 7800 4600
F 0 "#PWR010" H 7800 4600 30  0001 C CNN
F 1 "GND" H 7800 4530 30  0001 C CNN
F 2 "" H 7800 4600 60  0000 C CNN
F 3 "" H 7800 4600 60  0000 C CNN
	1    7800 4600
	1    0    0    -1  
$EndComp
$Comp
L +3.3V #PWR011
U 1 1 55EADA74
P 7650 4350
F 0 "#PWR011" H 7650 4310 30  0001 C CNN
F 1 "+3.3V" H 7650 4460 30  0000 C CNN
F 2 "" H 7650 4350 60  0000 C CNN
F 3 "" H 7650 4350 60  0000 C CNN
	1    7650 4350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7650 4350 7950 4350
Wire Wire Line
	7800 4600 7800 4450
Wire Wire Line
	7800 4450 7950 4450
$Comp
L CONN_4 P3
U 1 1 55EADADC
P 6200 2250
F 0 "P3" V 6150 2250 50  0000 C CNN
F 1 "UART1" V 6250 2250 50  0000 C CNN
F 2 "~" H 6200 2250 60  0000 C CNN
F 3 "~" H 6200 2250 60  0000 C CNN
	1    6200 2250
	-1   0    0    1   
$EndComp
Wire Wire Line
	6550 2100 7950 2100
Wire Wire Line
	6550 2200 6650 2200
Wire Wire Line
	6650 2200 6650 2000
Wire Wire Line
	6650 2000 7950 2000
$Comp
L GND #PWR012
U 1 1 55EADB4F
P 6650 2500
F 0 "#PWR012" H 6650 2500 30  0001 C CNN
F 1 "GND" H 6650 2430 30  0001 C CNN
F 2 "" H 6650 2500 60  0000 C CNN
F 3 "" H 6650 2500 60  0000 C CNN
	1    6650 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6550 2400 6650 2400
Wire Wire Line
	6650 2400 6650 2500
$Comp
L CONN_6 P2
U 1 1 55EADB8E
P 5050 950
F 0 "P2" V 5000 950 60  0000 C CNN
F 1 "Bluetooth" V 5100 950 60  0000 C CNN
F 2 "~" H 5050 950 60  0000 C CNN
F 3 "~" H 5050 950 60  0000 C CNN
	1    5050 950 
	-1   0    0    1   
$EndComp
$Comp
L +BATT #PWR013
U 1 1 55EADB9D
P 5700 1100
F 0 "#PWR013" H 5700 1050 20  0001 C CNN
F 1 "+BATT" H 5700 1200 30  0000 C CNN
F 2 "" H 5700 1100 60  0000 C CNN
F 3 "" H 5700 1100 60  0000 C CNN
	1    5700 1100
	0    1    1    0   
$EndComp
Wire Wire Line
	5400 1100 5700 1100
$Comp
L GND #PWR014
U 1 1 55EADBED
P 5950 1100
F 0 "#PWR014" H 5950 1100 30  0001 C CNN
F 1 "GND" H 5950 1030 30  0001 C CNN
F 2 "" H 5950 1100 60  0000 C CNN
F 3 "" H 5950 1100 60  0000 C CNN
	1    5950 1100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 1000 5950 1000
Wire Wire Line
	5950 1000 5950 1100
Wire Wire Line
	5400 900  5500 900 
Wire Wire Line
	5500 900  5500 2200
Wire Wire Line
	5500 2200 4350 2200
Wire Wire Line
	4350 2100 5600 2100
Wire Wire Line
	5600 2100 5600 800 
Wire Wire Line
	5600 800  5400 800 
Wire Wire Line
	4350 2000 5650 2000
Wire Wire Line
	5650 2000 5650 700 
Wire Wire Line
	5650 700  5400 700 
Wire Wire Line
	4350 1500 5450 1500
Wire Wire Line
	5450 1500 5450 1200
Wire Wire Line
	5450 1200 5400 1200
$Comp
L C C1
U 1 1 55EADD1A
P 3050 4600
F 0 "C1" H 3050 4700 40  0000 L CNN
F 1 "C" H 3056 4515 40  0000 L CNN
F 2 "~" H 3088 4450 30  0000 C CNN
F 3 "~" H 3050 4600 60  0000 C CNN
	1    3050 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 4400 3050 4250
Connection ~ 3050 4250
$Comp
L GND #PWR015
U 1 1 55EADD6A
P 3050 5050
F 0 "#PWR015" H 3050 5050 30  0001 C CNN
F 1 "GND" H 3050 4980 30  0001 C CNN
F 2 "" H 3050 5050 60  0000 C CNN
F 3 "" H 3050 5050 60  0000 C CNN
	1    3050 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 4800 3050 5050
$Comp
L MOSFET_P Q1
U 1 1 55EADD1E
P 2050 4350
F 0 "Q1" H 2050 4540 60  0000 R CNN
F 1 "MOSFET_P" H 2050 4170 60  0000 R CNN
F 2 "~" H 2050 4350 60  0000 C CNN
F 3 "~" H 2050 4350 60  0000 C CNN
	1    2050 4350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1550 4250 1850 4250
Wire Wire Line
	2050 4550 2050 5000
Wire Wire Line
	1550 4450 1700 4450
Wire Wire Line
	1700 4450 1700 4750
Wire Wire Line
	1700 4750 2050 4750
Connection ~ 2050 4750
Wire Wire Line
	5200 2450 5200 2350
$EndSCHEMATC
