#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'

#TECH="omp  dpc++  omp_offload"
WIN=(0 1)
TOPLAYER=(0 1)
BOUNDARY="none random cpml sponge"
MPI="normal"
WINCASE="win"
TOPCASE="top"
PROP=(0 1) # 0 -> 3 prop , 1 -> 2 prop
WITH_PROP=(0 1)
PROPCASE=""
DIR="results"
APPROX="isotropic vti tti"
EQU_ORDER="first second"
GPU_OPT="cpu gpu gpu-static gpu-semi-static"
OPTIONS=""

# get the output of terminal in text file having same  directory of results called terminal_output

len_f=${#FALSE_V[@]}
len_v=${#TRUE_V[@]}

while getopts "b:w:ctio" opt; do
  case $opt in
  t)
    echo -e "test set "
    OPTIONS="${OPTIONS} --test"

    ;;
  i)
    echo -e "image set "
    OPTIONS="${OPTIONS} --images"

    ;;
  o)
    echo -e "tools set "
    OPTIONS="${OPTIONS} --tools"

    ;;
  b)
    TECH=${OPTARG}
    echo -e "technology set ${OPTARG}"

    ;;

  c)
    COMP_PATH=${OPTARG}
    echo -e "compression path set ${COMP_PATH}"

    ;;

  w)
    WORKLOAD_PATH=${OPTARG}
    echo "adding the workload path ${WORKLOAD_PATH}"
    ;;
  esac
done

# if workload doesn't exist
if [ -z "$WORKLOAD_PATH" ]; then
  echo -e "${RED}Please use a valid path for workloads"
fi

# if not specific technolgy is set
if [ -z "$TECH" ]; then
  TECH="omp  dpc++  omp_offload"
fi

#echo ${OPTIONS}

#enable all callbacks
sed -i "s/enable\": [a-z][a-z]*/enable\": false/" ${WORKLOAD_PATH}/callback_configuration.json

# set the show each with 2000
sed -i "s/show-each\": [0-9][0-9]*/show-each\": 2000/" ${WORKLOAD_PATH}/callback_configuration.json

isInFile=$(cat ${WORKLOAD_PATH}/callback_configuration.json | grep -c "percentile")

if [ $isInFile -eq 1 ]; then
  #set precentile value to 98.5
  sed -i "s/percentile\": [0-9][0-9]*.[0-9]/percentile\": 98.5/" ${WORKLOAD_PATH}/callback_configuration.json
fi

# set left in case window
sed -i "18s/: [0-9][0-9]*/: 1300/" ${WORKLOAD_PATH}/computation_parameters.json

# set right in case window
sed -i "19s/: [0-9][0-9]*/: 600/" ${WORKLOAD_PATH}/computation_parameters.json

# set depth in case window
sed -i "20s/: [0-9][0-9]*/: 1200/" ${WORKLOAD_PATH}/computation_parameters.json

# set front in case window
sed -i "21s/: [0-9][0-9]*/: 0/" ${WORKLOAD_PATH}/computation_parameters.json

# set back in case window
sed -i "22s/: [0-9][0-9]*/: 0/" ${WORKLOAD_PATH}/computation_parameters.json

# set the blocks for dimnesions ;
# set block-x dim
sed -i "11s/: [0-9][0-9]*/: 16/" ${WORKLOAD_PATH}/computation_parameters.json

# set block-z dim
sed -i "12s/: [0-9][0-9]*/: 16/" ${WORKLOAD_PATH}/computation_parameters.json

# set block-y dim
sed -i "13s/: [0-9][0-9]*/: 1/" ${WORKLOAD_PATH}/computation_parameters.json

# set the shot stride
sed -i "s/shot-stride\": [0-9][0-9]*/shot-stride\": 2/" ${WORKLOAD_PATH}/engine_configuration.json

# set the dip angle into 90
sed -i "s/dip-angle\": [0-9][0-9]*/dip-angle\": 90/" ${WORKLOAD_PATH}/engine_configuration.json

for t in $TECH; do
  for p in ${PROP[@]}; do

    # shellcheck disable=SC2068
    for wp in ${WITH_PROP[@]}; do
      if [ $wp -eq 0 ] && [ $p -eq 0 ]; then # case of 3 prop and no boundary saving
        PROPCASE="3Prop"
        sed -i "36s/type\": \"[a-z][a-z]*/type\": \"three/" ${WORKLOAD_PATH}/engine_configuration.json
        sed -i "s/boundary-saving\": [a-z][a-z]*/boundary-saving\": false/" ${WORKLOAD_PATH}/engine_configuration.json

      elif

        [ $wp -eq 0 ] && [ $p -eq 1 ] # case of 3 prop and boundary saving ( 2.5 prop )
      then
        PROPCASE="2.5Prop"
        sed -i "36s/type\": \"[a-z][a-z]*/type\": \"three/" ${WORKLOAD_PATH}/engine_configuration.json
        sed -i "s/boundary-saving\": [a-z][a-z]*/boundary-saving\": true/" ${WORKLOAD_PATH}/engine_configuration.json

      elif

        [ $wp -eq 1 ] && [ $p -eq 0 ] # case  of 2 prop
      then
        PROPCASE="2Prop"
        sed -i "36s/type\": \"[a-z][a-z]*/type\": \"two/" ${WORKLOAD_PATH}/engine_configuration.json

      elif
        [ $wp -eq 1 ] && [ $p -eq 1 ] # if boundary saving option with 2 prop  you should skip  ???
      then
        continue
      fi

      for w in ${WIN[@]}; do

        if [ $w -eq 0 ]; then
          WINCASE="Full"
          sed -i "s/enable\": [a-z][a-z]*/enable\": false/" ${WORKLOAD_PATH}/computation_parameters.json

        else
          WINCASE="Win"
          sed -i "s/enable\": [a-z][a-z]*/enable\": true/" ${WORKLOAD_PATH}/computation_parameters.json

        fi

        for to in ${TOPLAYER[@]}; do
          if [ $to -eq 0 ]; then
            TOPCASE="NoTop"
            sed -i "s/use-top-layer\": [a-z][a-z]*/use-top-layer\": false/" ${WORKLOAD_PATH}/engine_configuration.json

          else

            TOPCASE="Top"
            sed -i "s/use-top-layer\": [a-z][a-z]*/use-top-layer\": true/" ${WORKLOAD_PATH}/engine_configuration.json

          fi

          for ap in $APPROX; do
            sed -i "s/approximation\": \"[a-z][a-z]*/approximation\": \"${ap}/" ${WORKLOAD_PATH}/engine_configuration.json

            for ord in $EQU_ORDER; do
              sed -i "s/equation-order\": \"[a-z][a-z]*/equation-order\": \"${ord}/" ${WORKLOAD_PATH}/engine_configuration.json

              for b in $BOUNDARY; do

                # set the boundary
                sed -i "22s/type\": \"[a-z][a-z]*/type\": \"${b}/" ${WORKLOAD_PATH}/engine_configuration.json

                if [[ ${t} == "dpc" ]]; then
                  for g in $GPU_OPT; do
                    sed -i "s/algorithm\": \"[a-z][a-z]*\-*[a-z]*/algorithm\": \"${g}/" ${WORKLOAD_PATH}/computation_parameters.json

                    DIR="${DIR}_${t}_${ap}_${ord}_${b}_${PROPCASE}_${WINCASE}_${TOPCASE}_${g}"

                    REP="${DIR}/report.txt"

                    [ ! -z "$COMP_PATH" ] && ./config.sh -b ${t} ${OPTIONS} -c ${COMP_PATH} -w ${DIR} || ./config.sh -b ${t} ${OPTIONS} -w ${DIR}

                    ./clean_build.sh &>>"${REP}"

                    ./bin/Engine -w ${WORKLOAD_PATH} &>>"${REP}"

                    DIR="results"
                  done
                else

                  DIR="${DIR}_${t}_${ap}_${ord}_${b}_${PROPCASE}_${WINCASE}_${TOPCASE}"

                  REP="${DIR^^}/report.txt"
                  #case of having a compression path or not
                  [ ! -z "$COMP_PATH" ] && ./config.sh -b ${t} ${OPTIONS} -c ${COMP_PATH} -w ${DIR} || ./config.sh -b ${t} ${OPTIONS} -w ${DIR^^}

                  ./clean_build.sh &>>"${REP}"

                  ./bin/Engine -m ${WORKLOAD_PATH} &>>"${REP}"

                  DIR="results"

                fi

                echo "******************************************************************************************"

              done
            done
          done
        done
      done
    done
  done

done
