#!/bin/sh

if [ "$#" -eq 0 ]; then
    echo "Usage: $0 <lattice-dir> <result-file-name> <sent-lattice-ext> <set-scores-ext>"
    echo "    <lattice-dir> - the directory with the lattice files"
    echo "    <result-file-name> - the file name to be used for the combined lattice data"
    echo "    <sent-lattice-ext> - the lattice file extension for a sentence, default is 'lattice'"
    echo "    <set-scores-ext> - the feature scores file extension for a sentence, default is 'feature_scores'"
    echo "    <number-of-batches> - the number of the batch files to be created as the result of combining lattice or feature score files"
    exit 1
fi

#Store the directory in which the lattice files reside
export LATTICES_DERECTORY=${1}
if ! [ -e "${LATTICES_DERECTORY}" ]; then
    echo "ERROR: ${LATTICES_DERECTORY} not found"
    exit 1
fi
if ! [ -d "${LATTICES_DERECTORY}" ]; then
    echo "ERROR: ${LATTICES_DERECTORY} not a directory"
    exit 1
fi

#Store the name of the output file
export OUTPUT_FILE_NAME=${2}
if [ -z "${OUTPUT_FILE_NAME}" ]; then
    echo "ERROR: <result-file-name> is not defined"
    exit 1
fi

#Store the lattice file extention
export LATTICE_FILE_EXT=${3}
if [ -z "${LATTICE_FILE_EXT}" ]; then
    export LATTICE_FILE_EXT="lattice"
fi

#Store the feature scores file extention
export SCORE_FILE_EXT=${4}
if [ -z "${SCORE_FILE_EXT}" ]; then
    export SCORE_FILE_EXT="feature_scores"
fi

#Store the number of the batches
export NUMBER_OF_BATCHES=${5}
if [ -z "${NUMBER_OF_BATCHES}" ]; then
    export NUMBER_OF_BATCHES=1
fi

echo "Searhcing for the lattices in the folder:" ${LATTICES_DERECTORY}
echo "------------------------------------"

#Move to the lattices folder
export INITIAL_DIRECTORY=`pwd`
cd ${LATTICES_DERECTORY}

#Collect the lattice file names
export SENTENCE_FILES=`ls *.${LATTICE_FILE_EXT} | cut -f 1 -d '.' | sort -k 1n`
#Obtain the minimum sentence id
export MIN_FILE_NAME=`echo ${SENTENCE_FILES} | sed -e "s/ .*//"`
export MAX_FILE_NAME=`echo ${SENTENCE_FILES} | sed -e "s/.* //"`
export NUMBER_OF_SENTENCES=$(( ${MAX_FILE_NAME} - ${MIN_FILE_NAME} + 1 ))
export BATCH_SIZE=$(( $NUMBER_OF_SENTENCES / ${NUMBER_OF_BATCHES} ))
export LAST_BATCH_SIZE=$(( $NUMBER_OF_SENTENCES % $NUMBER_OF_BATCHES ))
if [ $LAST_BATCH_SIZE -ne 0 ]; then
    let BATCH_SIZE=$BATCH_SIZE+1
fi
#export TEMP=$(( ${NUMBER_OF_SENT_IN_BATCH} / ${NUMBER_OF_BATCHES} ))
echo "last item:${MAX_FILE_NAME}"
echo "number of sentences in a batch:${BATCH_SIZE}"
echo "last file size:$LAST_BATCH_SIZE"

index=0
SENTENCE_COUNTER=0
#Define the lattices and scores output file names
export LATTICES_OUT_FILE="${OUTPUT_FILE_NAME}.lattices.batch.$index"
export SCORES_OUT_FILE="${OUTPUT_FILE_NAME}.feature_scores.batch.$index"

#Iterate through the files and combine them into lattices and scores file
for FILE_NAME in ${SENTENCE_FILES}; do
    #Re-set the files
    #rm -f ${LATTICES_OUT_FILE} ${SCORES_OUT_FILE}
    if [ $SENTENCE_COUNTER -ge $BATCH_SIZE ]; then
        SENTENCE_COUNTER=0
        let index=$index+1
        LATTICES_OUT_FILE="${OUTPUT_FILE_NAME}.lattices.batch.$index"
        SCORES_OUT_FILE="${OUTPUT_FILE_NAME}.feature_scores.batch.$index"
    fi

    #Compute the sentence id
    export SENT_ID=$(expr ${FILE_NAME} - ${MIN_FILE_NAME})

    echo "Processing ${FILE_NAME}.${LATTICE_FILE_EXT} and ${FILE_NAME}.${SCORE_FILE_EXT}, sentence id: ${SENT_ID}"

    #Append the lattice data
    echo "<SENT ID=${SENT_ID}>" >> ${LATTICES_OUT_FILE}
    cat "${FILE_NAME}.${LATTICE_FILE_EXT}" >> ${LATTICES_OUT_FILE}
    echo "</SENT>" >> ${LATTICES_OUT_FILE}

    #Append the scores data
    echo "<SENT ID=${SENT_ID}>" >> ${SCORES_OUT_FILE}
    cat ${FILE_NAME}.${SCORE_FILE_EXT} | sort -k1n >> ${SCORES_OUT_FILE}
    echo "</SENT>" >> ${SCORES_OUT_FILE}
    let SENTENCE_COUNTER=$SENTENCE_COUNTER+1
    #Report on the results
    echo "------------------------------------"
    echo "The resulting files are: ${LATTICES_OUT_FILE} and ${SCORES_OUT_FILE}"
    echo "++++++++++++++++++++++++++++++++++++"

done

#Re-set the gzip files
#rm -f ${LATTICES_OUT_FILE}.gz ${SCORES_OUT_FILE}.gz

#Archive the resulting files
#echo "Archiving: ${LATTICES_OUT_FILE}"
#gzip ${LATTICES_OUT_FILE}
#echo "Archiving: ${SCORES_OUT_FILE}"
#gzip ${SCORES_OUT_FILE}

#Move back to the original folder
cd ${INITIAL_DIRECTORY}
