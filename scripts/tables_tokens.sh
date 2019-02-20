#!/bin/bash

show_table() {
    code=$1
    scope=$2
    table=$3

    echo "--------- $code::$table($scope) -------------------------------------------------"
    cleos get table $code $scope $table
}

show_balance() {
    code=$1
    scope=$2
    symbol=$3
    echo "--------- $scope ($symbol) -------------------------------------------------"
    cleos get currency balance $code $scope $symbol
}

show_table snapsnapsnap 1 snapshots

echo ""
echo "********* vapaeetokens CNT *******************************************************"
cleos get currency stats vapaeetokens CNT
show_table vapaeetokens CNT source

show_balance vapaeetokens bob CNT
show_balance vapaeetokens alice CNT
show_balance vapaeetokens tom CNT
show_balance vapaeetokens kate CNT

echo ""
echo "********* vapaeetokens BOX *******************************************************"
cleos get currency stats vapaeetokens BOX
show_table vapaeetokens BOX source

show_balance vapaeetokens bob BOX
show_balance vapaeetokens alice BOX
show_balance vapaeetokens tom BOX
show_balance vapaeetokens kate BOX

echo ""
echo "********* vapaeetokens VPE *******************************************************"
cleos get currency stats vapaeetokens VPE
show_table vapaeetokens VPE source

show_balance vapaeetokens bob VPE
show_balance vapaeetokens alice VPE
show_balance vapaeetokens tom VPE
show_balance vapaeetokens kate VPE
