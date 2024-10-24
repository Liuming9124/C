set terminal png font arial 16 size 800,600
set key bmargin

set xlabel 'frame number'
set ylabel 'mine string disk write'
set output 'minedi.png'

set xtics ('10' 0, '20' 1, '30' 2, '40' 3, '50' 4, '60' 5, '70' 6, '80' 7, '90' 8, '100' 9)

plot    './diskwrite/enhance.txt' pt 1 lw 3 ps 1 lc rgb "blue"  title 'enhance' with lines ,\
        './diskwrite/fifo.txt'    pt 1 lw 3 ps 1 lc rgb "red"   title 'fifo'    with lines ,\
        './diskwrite/mine.txt'    pt 1 lw 3 ps 1 lc rgb "green" title 'mine'    with lines ,\
        './diskwrite/optimal.txt' pt 1 lw 3 ps 1 lc rgb "black" title 'optimal' with lines,

set output