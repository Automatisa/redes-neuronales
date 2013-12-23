set xrange [1:50]
set terminal jpeg
set output "./img/ejer1/hypByLog2-2.jpg"

plot "./dat/dat-ejer1/hypByLog2-5.dat" title "P = 5" with line,\
     "./dat/dat-ejer1/hypByLog2-10.dat" title "P = 10" with line,\
     "./dat/dat-ejer1/hypByLog2-20.dat" title "P = 20" with line

