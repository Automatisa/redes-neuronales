set hidden3d
set dgrid3d 50,50
set yrange [-10:10]
set xrange [-10:10]
set terminal jpeg
set output "./img/ejer2/grapMultByTanh-1.jpg"
splot "./dat/dat-ejer2/multBySimple12-m10.dat" u 1:2:3 with lines title "Interpolation Error of FFNN"
