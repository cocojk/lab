#!/bin/bash 


cd data
for i in $(ls)
do
    echo ${i};
    var=$var" ";
    var=$var"data/"${i};
done
cd ..

echo $var

./a.out $var

 

