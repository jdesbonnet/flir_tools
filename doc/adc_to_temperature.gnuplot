

# Flir Ex constants
Ex_R1 = 14226.111;
Ex_R2 = 0.027153991;
Ex_O  = -7518;
Ex_B  = 1387.2;
Ex_F  = 2.5;

# Flir A320 constants
R1 = 14809.436;
R2 = 0.010635848;
B = 1379.6;
O = -5846;
F = 1;

A320_T(x) = B /  log(R1/(R2*(x+O))+F)

Ex_T(x) = Ex_B / log(Ex_R1 / (Ex_R2 * (x + Ex_O))+F)

set grid
set title "Conversion of Flir Ex and A320 raw ADC to estimated temperature with  T = B / log(R1/(R2*(x+O))+F)"
set xlabel "ADC value (16 bit unsigned)"
set ylabel "temperature K"
set xrange [0:65535]

set terminal pngcairo size 1024,800
set output "adc_to_temperature.png"
plot Ex_T(x) linewidth 2 title 'Flir E4/E8', A320_T(x) linewidth 2 title 'Flir A320'


#pause -1
