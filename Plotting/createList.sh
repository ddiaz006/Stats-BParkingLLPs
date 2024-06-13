
path=../Lim_${version}_IT/ #SigPi0_M 0p3 _low_ctau 1/higgsCombineTest.AsymptoticLimits.mH120.root
echo "inputs at " $path 
ctaus=(\
10 \
100 \
1000 \
30 \
300 \
3000 \
50 \
500 \
5000 \
70 \
700 \
)

mass=(\
"M0p3_low" \
"M0p3_high" \
"M0p5_high" \
"M0p5_low" \
"M1p0_high" \
"M1p0_low" \
"M2p0_high" \
"M2p0_low" \
"M3p0_high" \
"M3p0_low" \
)
#"M0p3_high" \
#"M0p3_low" \
#"M0p5_high" \
#"M0p5_low" \
#"M1p0_high" \
#"M1p0_low" \
#"M2p0_high" \
#"M2p0_low" \
#"M3p0_high" \
#"M3p0_low" \
#)

#"SigPi0_" \
proc=(\
"SigPiPlusPiMinus_" \
)

for m in ${mass[@]} 
do
  rm -f PiAll_${version}_${m}.list
  touch PiAll_${version}_${m}.list
  rm -f PiAll_${version}_${m}_CSC.list
  touch PiAll_${version}_${m}_CSC.list
  rm -f PiAll_${version}_${m}_DT.list
  touch PiAll_${version}_${m}_DT.list
  for p in ${proc[@]}
  do
    for ct in ${ctaus[@]} 
    do
      echo ${path}${p}${m}"_ctau"${ct}"/higgsCombineTest.AsymptoticLimits.mH120.root" >>PiAll_${version}_${m}.list 
      echo ${path}${p}${m}"_ctau"${ct}"/CSC/higgsCombineTest.AsymptoticLimits.mH120.root" >>PiAll_${version}_${m}_CSC.list 
      echo ${path}${p}${m}"_ctau"${ct}"/DT/higgsCombineTest.AsymptoticLimits.mH120.root" >>PiAll_${version}_${m}_DT.list 
    done
  done 
done
