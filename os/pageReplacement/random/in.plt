set terminal png font arial 14 size 800,600
set key bmargin

#set xrange [0:10000]
#set yrange [-1:10]

set xlabel 'page size'
set ylabel 'interrupt'
set output 'randin.png'

plot    './interrupt/enhance.txt' pt 1 lw 3 ps 1 lc rgb "blue"  title 'enhance' with lines ,\
        './interrupt/fifo.txt'    pt 1 lw 3 ps 1 lc rgb "red"   title 'fifo'    with lines ,\
        './interrupt/mine.txt'    pt 1 lw 3 ps 1 lc rgb "green" title 'mine'    with lines,\
        './interrupt/optimal.txt' pt 1 lw 3 ps 1 lc rgb "black" title 'optimal' with lines,

set output