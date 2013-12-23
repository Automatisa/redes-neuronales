set hidden3d
set dgrid3d 50,50
set terminal jpeg
set output "./img/ejer2/grapMultByPow2-2.jpg"

splot "./dat/dat-ejer2/multByPow2-3-m10.dat" u 1:2:3 with lines title "Extrapolation Error of FFNN"
