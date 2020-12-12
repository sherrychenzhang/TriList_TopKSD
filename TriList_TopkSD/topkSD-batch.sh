#!/bin/bash

for dir in "CA-GrQc" "CA-CondMat" "Email-EuAll" "Epinions" "slashdot" "dblp" "Amazon0601" "web-Google" "WikiTalk" "as-skitter" "LiveJournal" "uk-2002"
do
    	for alg in "AB" "DB"
	   do
             for num in "5" "20" "100" "500" "2000" "10000" "100000"
               do 
                    echo ${num}>>log.txt
                   ./Method_Comparision  ../data/${dir} -k${num} -t2 -m${alg} >>log.txt      

               done  
 
              for size in "1" "2" "3" "5" "7"
                do
                    echo ${size}>>log.txt
                    ./Method_Comparision  ../data/${dir} -k100 -t${size} -m${alg} >>log.txt
                done
          done
done
