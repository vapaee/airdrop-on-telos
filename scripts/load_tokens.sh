#!/bin/bash


cleos push action snapsnapsnap add '["bob",250000000,1]' -p bob@active
cleos push action snapsnapsnap add '["alice",10000000000,1]' -p alice@active
cleos push action snapsnapsnap add '["tom",1000,1]' -p tom@active
cleos push action snapsnapsnap add '["kate",9999999,1]' -p kate@active

cleos push action vapaeetokens create '["vapaeetokens","500000000.0000 CNT"]' -p vapaeetokens@active
cleos push action vapaeetokens create '["vapaeetokens","500000000.0000 BOX"]' -p vapaeetokens@active
cleos push action vapaeetokens create '["vapaeetokens","1000000.0000 VPE"]' -p vapaeetokens@active


# ratio 1:1
# (everyone should get exactly the same amount of TLOS in CNT)
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"CNT",0,0,10000,0,"CNT Token Airdrop - Cards & Tokens - http://cardsandtokens.com"]' -p vapaeetokens@active

# ratio 1:1 - min 1000.0000 TLOS - cap 40000.0000 TLOS
# (only accounts with more than 1000 TLOS will be capped at 40K and will receive 10% in BOX)
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"BOX",400000000,10000000,1000,0,"BOX Token Airdrop - Board Game Box"]' -p vapaeetokens@active

# ratio 1:0 - min 1000.0000 TLOS - base 1.0000 VPE
# (only accounts with more than 1000 TLOS will receive just 1 VPE)
cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"VPE",0,10000000,0,10000,"VPE Token Airdrop - Vapaee"]' -p vapaeetokens@active

