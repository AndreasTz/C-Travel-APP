#!/bin/bash

#Elegxetai an to plithos twn orismatwn einai swsto.
if [ "$#" -ne 3 ] ; then

	echo "Must give 3 arguments: input-file, input-directory, number-of-files-per-directory."

#Elegxetai an mporei na anoixtei to arxeio me tis eggrafes politwn.
elif ! ( test -f "$1" ) ; then
	
	echo "Can't open input file"

#Elegxetai an yparxei hdh o katalogos me toys ypokatalogoyw kai ta arxeia.		
elif ( test -f "$2" ) ; then
	
	echo "Input directory already there."

#Elegxetai an to plhthos twn arxeiwn ana ypokatalogo einai mh arhtiko.
elif [[ ! ( $3 =~ ^[0-9]+$ ) ]] ; then

	echo "Number of files per directory is not a non-negative integer."			

#Elegxetai an to plhthos twn arxeiwn ana ypokatalogo einaimhden.
elif [[ $3 -eq "0" ]] ; then

	echo "Number of files per directory can't be equal to zero."	
	
else
		
	#Diavazontai oles oi xwres
	allCountries=($(cat $1 | cut -d " " -f 4 | sort  | uniq))
	
	#Apothikeyetai to plhthos tous		
	numberOfAllCountries=${#allCountries[*]}

	#Diavazontai oles oi eggrafes politwn
	readarray -t allCitizens < $1
	
	#Apothikeyetai to plhthos tous	
	numberOfAllCitizens=${#allCitizens[*]}
				
	#Elegxetai an to plhthos twn xwrwn einai mhden.
	if [[ $numberOfAllCountries -eq "0" ]]; then
	
		echo "Number of countries can't be equal to zero."	
	
	else
		
		# Dhmioyrgeitai o kentrikos katalogos pou tha graftoun ypokatalogoi kai  arxeia
		mkdir $2
		
		# Gia kathe xwra
		for (( i=0; i<$numberOfAllCountries; i++ ));do
			
			# Dhmioyrgeitai sxetikos ypokatalogos 
			mkdir ./$2/${allCountries[$i]}
						
			# Amews meta ta sxetika arxeia
			for (( j=1; j<=$3; j++ ));do
			
				touch ./$2/${allCountries[$i]}/${allCountries[$i]}-$j.txt	
											
			done
			
			# Epishs dhmioyrgeitai sxetikos deikths round robin
			roundRobinIndexes[$i]=1
			
		done
		
		# Gia kathe polith
		for (( i=0; i<$numberOfAllCitizens; i++ ));do
			
			# Ypologizetai h xwra toy polith 
			country=$(echo ${allCitizens[$i]} | cut -d " " -f 4)
			
			# Ypologizetai h thesh toy antistoixoy round robin deikth
			roundRobinIndexPosition=0
			
			while [  ${allCountries[$roundRobinIndexPosition]} != $country  ];do
				roundRobinIndexPosition=$(($roundRobinIndexPosition+1))
			done	
			
			echo ${allCitizens[$i]} >> ./$2/$country/$country-${roundRobinIndexes[$roundRobinIndexPosition]}.txt
			
			# Aykshsh toy round robin deikth kata 1
			roundRobinIndexes[$roundRobinIndexPosition]=$((${roundRobinIndexes[$roundRobinIndexPosition]}+1))
			
			# An h ayjhsh prepei na "kyklwsei", tote gyrizei sto 1
			if [[ ${roundRobinIndexes[$roundRobinIndexPosition]} -gt $3 ]]; then
				roundRobinIndexes[$roundRobinIndexPosition]=1
			fi
			
		done
		
	fi							
	
fi	
		
