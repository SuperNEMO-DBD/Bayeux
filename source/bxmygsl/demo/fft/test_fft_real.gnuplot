

t1=0
t2=100.
a1=-0.5
a2=5.5
set xrange [t1:t2]
set yrange [a1:a2]
set xrange [*:*]
set yrange [*:*]
set grid

set xlabel "time (ns)"
set ylabel "ADC (mV)"
plot \
     'test_fft_real.data' u 1:2 index 0 title "Original signal" with lines, \
     'test_fft_real.data' u 1:(($2)+0.01) index 2 title "FFT-restored signal" with lines

#, \
#     'test_fft_real.data' u 1:(($2)-0.01) index 3 with lines	 

pause -1 "Hit return..."
set xlabel "frequency (GHz)"
set ylabel ""
set xrange [0:*]
set yrange [-100:+100]
set yrange [*:*]
plot \
     'test_fft_real.data' index 1 title "FFT-frequency spectrum" with impulses


# end
