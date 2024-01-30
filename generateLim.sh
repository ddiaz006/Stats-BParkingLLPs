#region=csc
Type=IT
outDir=Lim_${Type}  #_${region}

model=SigPi0
#model=SigPiPlusPiMinus

inputs=inputs_OPT/datacards

#rm -rf $outDir
mkdir -p $outDir
touch names.txt

sig=( \
  ${model}_M0p3_ctau1 \
  ${model}_M0p3_ctau3 \
  ${model}_M0p3_ctau5 \
  ${model}_M0p3_ctau7 \
  ${model}_M0p3_ctau10 \
  ${model}_M0p3_ctau30 \
  ${model}_M0p3_ctau50 \
  ${model}_M0p3_ctau70 \
  ${model}_M0p3_ctau100 \
  ${model}_M0p3_ctau300 \
  ${model}_M0p3_ctau500 \
  ${model}_M0p3_ctau700 \
  ${model}_M0p3_ctau1000 \
  ${model}_M0p3_ctau3000 \
  ${model}_M0p3_ctau5000 \
  ${model}_M0p3_ctau10000 \
)

for s in ${sig[@]}
do
  echo ${s} >> names.txt
  echo "############################ Working on " $s
  mkdir -p ${outDir}/${s}
  cp ${inputs}/${s}-${Type}_csc.txt ./${outDir}/${s}/ 
  cp ${inputs}/${s}-${Type}_dt.txt  ./${outDir}/${s}/ 
  cd ${outDir}/${s}
  combineCards.py ${s}-${Type}_csc.txt ${s}-${Type}_dt.txt > card.txt
  combine -M AsymptoticLimits --run expected card.txt  --rMin 0 --rMax 1
  echo "############################ done with " $s
  cd -
done


