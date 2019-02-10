#!/bin/bash

# checking 'force' param
force=false
NET=
if [ "$1" == "force" ]; then
   force=true
fi

HOME=/var/www/airdrop-on-telos
VAPAEETOKENS_HOME=$HOME/contracts/vapaeetokens
SNAPSNAPSNAP_HOME=$HOME/contracts/snapsnapsnap

echo "-------- vapaeetokens ---------"
cd $VAPAEETOKENS_HOME
if [[ vapaeetokens.cpp -nt vapaeetokens.wasm || vapaeetokens.hpp -nt vapaeetokens.wasm || $force == true ]]; then
    eosio-cpp -o vapaeetokens.wasm vapaeetokens.cpp --abigen
    cleos $NET set contract vapaeetokens $PWD -p vapaeetokens@active
fi

echo "-------- snapsnapsnap ---------"
cd $SNAPSNAPSNAP_HOME
if [[ snapsnapsnap.cpp -nt snapsnapsnap.wasm || snapsnapsnap.hpp -nt snapsnapsnap.wasm || $force == true ]]; then
    eosio-cpp -o snapsnapsnap.wasm snapsnapsnap.cpp --abigen
    cleos $NET set contract snapsnapsnap $PWD -p snapsnapsnap@active
fi