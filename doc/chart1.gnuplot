set title "Raw radiometric ADC converted to temperature (°C) estimate"
set xrange[0:320]
set yrange[0:240]

set terminal pngcairo size 640,480
set output "chart1.png"

plot '1m_still_temp.dat' using ($1):(-$2+238):($3/100) matrix with image title ''



