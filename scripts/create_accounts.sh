#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr
EOS_PUB_KEY=EOS8RoCAXxWYUW2v4xkG19F57BDVBzpt9NN2iDsD1ouQNyV2BkiNc

cleos create account eosio vapaeetokens $EOS_PUB_KEY
cleos create account eosio snapsnapsnap $EOS_PUB_KEY

cleos create account eosio bob $EOS_PUB_KEY
cleos create account eosio alice $EOS_PUB_KEY
cleos create account eosio tom $EOS_PUB_KEY
cleos create account eosio kate $EOS_PUB_KEY