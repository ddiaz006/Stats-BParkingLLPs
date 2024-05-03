
path=../Lim_WP19_IT/ #SigPi0_M 0p3 _low_ctau 1/higgsCombineTest.AsymptoticLimits.mH120.root
ctaus=(\
10 \
100 \
1000 \
3 \
30 \
300 \
3000 \
5 \
50 \
500 \
5000 \
7 \
70 \
700 \
)

mass=(\
"M0p3_low" \
"M0p3_high" \
"M0p3_low" \
"M0p5_high" \
"M0p5_low" \
"M1p0_high" \
"M1p0_low" \
"M2p0_high" \
"M2p0_low" \
"M3p0_high" \
"M3p0_low" \
)

proc=(\
"SigPi0_" \
"SigPiPlusPiMinus_" \
)

for m in ${mass[@]} 
do
  rm -f PiAll_${version}_${m}.list
  touch PiAll_${version}_${m}.list
  for p in ${proc[@]}
  do
    for ct in ${ctaus[@]} 
    do
      echo 0.000001 ${path}${p}${m}"_ctau"${ct}"/higgsCombineTest.AsymptoticLimits.mH120.root" >>PiAll_${version}_${m}.list 
    done
  done 
done
