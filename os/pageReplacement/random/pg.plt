set terminal png font arial 14 size 800,600
set key bmargin

#set xrange [0:10000]
#set yrange [-1:10]

set xlabel 'page size'
set ylabel 'page fault'
set output 'localpg.png'

plot    './pagefault/enhance.txt' pt 1 lw 3 ps 1 lc rgb "blue"  title 'enhance' with lines ,\
        './pagefault/fifo.txt'    pt 1 lw 3 ps 1 lc rgb "red"   title 'fifo'    with lines ,\
        './pagefault/mine.txt'    pt 1 lw 3 ps 1 lc rgb "green" title 'mine'    with lines,\
        './pagefault/optimal.txt' pt 1 lw 3 ps 1 lc rgb "black" title 'optimal' with lines,

set output