set xrange [.05:1]
set terminal jpeg
set output "./img/ejer1/hypByHyp2-1.jpg"

plot "./dat/dat-ejer1/hypByHyp2-5.dat" title "P = 5" with line,\
     "./dat/dat-ejer1/hypByHyp2-10.dat" title "P = 10" with line,\
     "./dat/dat-ejer1/hypByHyp2-20.dat" title "P = 20" with line
