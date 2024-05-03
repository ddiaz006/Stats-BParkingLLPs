Type=IT
outDir=Lim_${version}_${Type} 

models=( \
    SigPi0 \
    SigPiPlusPiMinus \
)
#    SigPi0 \
#    SigPiPlusPiMinus \

inputs=inputs_${version}/datacards

#rm -rf $outDir
mkdir -p $outDir
#touch names.txt

for model in ${models[@]}
do
  sig=( \
    ${model}_M0p3_high_ctau3 \
    ${model}_M0p3_high_ctau5 \
    ${model}_M0p3_high_ctau7 \
    ${model}_M0p3_high_ctau10 \
    ${model}_M0p3_high_ctau30 \
    ${model}_M0p3_high_ctau50 \
    ${model}_M0p3_high_ctau70 \
    ${model}_M0p3_high_ctau100 \
    ${model}_M0p3_high_ctau300 \
    ${model}_M0p3_high_ctau500 \
    ${model}_M0p3_high_ctau700 \
    ${model}_M0p3_high_ctau1000 \
    ${model}_M0p3_high_ctau3000 \
    ${model}_M0p3_high_ctau5000 \
    ${model}_M0p3_high_ctau10000 \
    ${model}_M0p3_low_ctau3 \
    ${model}_M0p3_low_ctau5 \
    ${model}_M0p3_low_ctau7 \
    ${model}_M0p3_low_ctau10 \
    ${model}_M0p3_low_ctau30 \
    ${model}_M0p3_low_ctau50 \
    ${model}_M0p3_low_ctau70 \
    ${model}_M0p3_low_ctau100 \
    ${model}_M0p3_low_ctau300 \
    ${model}_M0p3_low_ctau500 \
    ${model}_M0p3_low_ctau700 \
    ${model}_M0p3_low_ctau1000 \
    ${model}_M0p3_low_ctau3000 \
    ${model}_M0p3_low_ctau5000 \
    ${model}_M0p3_low_ctau10000 \

    ${model}_M0p5_high_ctau3 \
    ${model}_M0p5_high_ctau5 \
    ${model}_M0p5_high_ctau7 \
    ${model}_M0p5_high_ctau10 \
    ${model}_M0p5_high_ctau30 \
    ${model}_M0p5_high_ctau50 \
    ${model}_M0p5_high_ctau70 \
    ${model}_M0p5_high_ctau100 \
    ${model}_M0p5_high_ctau300 \
    ${model}_M0p5_high_ctau500 \
    ${model}_M0p5_high_ctau700 \
    ${model}_M0p5_high_ctau1000 \
    ${model}_M0p5_high_ctau3000 \
    ${model}_M0p5_high_ctau5000 \
    ${model}_M0p5_high_ctau10000 \
 
    ${model}_M1p0_high_ctau3 \
    ${model}_M1p0_high_ctau5 \
    ${model}_M1p0_high_ctau7 \
    ${model}_M1p0_high_ctau10 \
    ${model}_M1p0_high_ctau30 \
    ${model}_M1p0_high_ctau50 \
    ${model}_M1p0_high_ctau70 \
    ${model}_M1p0_high_ctau100 \
    ${model}_M1p0_high_ctau300 \
    ${model}_M1p0_high_ctau500 \
    ${model}_M1p0_high_ctau700 \
    ${model}_M1p0_high_ctau1000 \
    ${model}_M1p0_high_ctau3000 \
    ${model}_M1p0_high_ctau5000 \
    ${model}_M1p0_high_ctau10000 \
 
    ${model}_M2p0_high_ctau3 \
    ${model}_M2p0_high_ctau5 \
    ${model}_M2p0_high_ctau7 \
    ${model}_M2p0_high_ctau10 \
    ${model}_M2p0_high_ctau30 \
    ${model}_M2p0_high_ctau50 \
    ${model}_M2p0_high_ctau70 \
    ${model}_M2p0_high_ctau100 \
    ${model}_M2p0_high_ctau300 \
    ${model}_M2p0_high_ctau500 \
    ${model}_M2p0_high_ctau700 \
    ${model}_M2p0_high_ctau1000 \
    ${model}_M2p0_high_ctau3000 \
    ${model}_M2p0_high_ctau5000 \
    ${model}_M2p0_high_ctau10000 \
 
    ${model}_M3p0_high_ctau3 \
    ${model}_M3p0_high_ctau5 \
    ${model}_M3p0_high_ctau7 \
    ${model}_M3p0_high_ctau10 \
    ${model}_M3p0_high_ctau30 \
    ${model}_M3p0_high_ctau50 \
    ${model}_M3p0_high_ctau70 \
    ${model}_M3p0_high_ctau100 \
    ${model}_M3p0_high_ctau300 \
    ${model}_M3p0_high_ctau500 \
    ${model}_M3p0_high_ctau700 \
    ${model}_M3p0_high_ctau1000 \
    ${model}_M3p0_high_ctau3000 \
    ${model}_M3p0_high_ctau5000 \
    ${model}_M3p0_high_ctau10000 \
 
    ${model}_M0p3_low_ctau3 \
    ${model}_M0p3_low_ctau5 \
    ${model}_M0p3_low_ctau7 \
    ${model}_M0p3_low_ctau10 \
    ${model}_M0p3_low_ctau30 \
    ${model}_M0p3_low_ctau50 \
    ${model}_M0p3_low_ctau70 \
    ${model}_M0p3_low_ctau100 \
    ${model}_M0p3_low_ctau300 \
    ${model}_M0p3_low_ctau500 \
    ${model}_M0p3_low_ctau700 \
    ${model}_M0p3_low_ctau1000 \
    ${model}_M0p3_low_ctau3000 \
    ${model}_M0p3_low_ctau5000 \
    ${model}_M0p3_low_ctau10000 \
 
    ${model}_M0p5_low_ctau3 \
    ${model}_M0p5_low_ctau5 \
    ${model}_M0p5_low_ctau7 \
    ${model}_M0p5_low_ctau10 \
    ${model}_M0p5_low_ctau30 \
    ${model}_M0p5_low_ctau50 \
    ${model}_M0p5_low_ctau70 \
    ${model}_M0p5_low_ctau100 \
    ${model}_M0p5_low_ctau300 \
    ${model}_M0p5_low_ctau500 \
    ${model}_M0p5_low_ctau700 \
    ${model}_M0p5_low_ctau1000 \
    ${model}_M0p5_low_ctau3000 \
    ${model}_M0p5_low_ctau5000 \
    ${model}_M0p5_low_ctau10000 \
 
    ${model}_M1p0_low_ctau3 \
    ${model}_M1p0_low_ctau5 \
    ${model}_M1p0_low_ctau7 \
    ${model}_M1p0_low_ctau10 \
    ${model}_M1p0_low_ctau30 \
    ${model}_M1p0_low_ctau50 \
    ${model}_M1p0_low_ctau70 \
    ${model}_M1p0_low_ctau100 \
    ${model}_M1p0_low_ctau300 \
    ${model}_M1p0_low_ctau500 \
    ${model}_M1p0_low_ctau700 \
    ${model}_M1p0_low_ctau1000 \
    ${model}_M1p0_low_ctau3000 \
    ${model}_M1p0_low_ctau5000 \
    ${model}_M1p0_low_ctau10000 \
 
    ${model}_M2p0_low_ctau3 \
    ${model}_M2p0_low_ctau5 \
    ${model}_M2p0_low_ctau7 \
    ${model}_M2p0_low_ctau10 \
    ${model}_M2p0_low_ctau30 \
    ${model}_M2p0_low_ctau50 \
    ${model}_M2p0_low_ctau70 \
    ${model}_M2p0_low_ctau100 \
    ${model}_M2p0_low_ctau300 \
    ${model}_M2p0_low_ctau500 \
    ${model}_M2p0_low_ctau700 \
    ${model}_M2p0_low_ctau1000 \
    ${model}_M2p0_low_ctau3000 \
    ${model}_M2p0_low_ctau5000 \
    ${model}_M2p0_low_ctau10000 \
 
     ${model}_M3p0_low_ctau3 \
     ${model}_M3p0_low_ctau5 \
     ${model}_M3p0_low_ctau7 \
     ${model}_M3p0_low_ctau10 \
     ${model}_M3p0_low_ctau30 \
     ${model}_M3p0_low_ctau50 \
     ${model}_M3p0_low_ctau70 \
     ${model}_M3p0_low_ctau100 \
     ${model}_M3p0_low_ctau300 \
     ${model}_M3p0_low_ctau500 \
     ${model}_M3p0_low_ctau700 \
     ${model}_M3p0_low_ctau1000 \
     ${model}_M3p0_low_ctau3000 \
     ${model}_M3p0_low_ctau5000 \
     ${model}_M3p0_low_ctau10000 \

   )

  
  for s in ${sig[@]}
  do
    #echo ${s} >> names.txt
    echo "############################ Working on " ${outDir}/$s
    mkdir -p ${outDir}/${s}
    cp ${inputs}/${s}-${Type}_csc.txt ./${outDir}/${s}/ 
    cp ${inputs}/${s}-${Type}_dt.txt  ./${outDir}/${s}/ 
    cd ${outDir}/${s}
    combineCards.py CSC=${s}-${Type}_csc.txt DT=${s}-${Type}_dt.txt > card.txt
    # Extracting the ctau value
    ctau_value=$(echo $s | sed -E 's/.*_ctau([0-9]+).*/\1/')
    
    if [ "$ctau_value" -le 10 ]; then
      rMax=5000
    elif [ "$ctau_value" -gt 10 ] && [ "$ctau_value" -le 100 ]; then
      rMax=100
    elif [ "$ctau_value" -gt 100 ] && [ "$ctau_value" -le 1000 ]; then
      rMax=500
    elif [ "$ctau_value" -gt 1000 ] && [ "$ctau_value" -le 10000 ]; then
      rMax=10000
    fi
    
    # Run combine with the set rMax value
    combine -M AsymptoticLimits --run expected card.txt  --rMin 0 --rMax $rMax
    echo "############################ done with " ${outDir}/$s
    cd -
  done
done
