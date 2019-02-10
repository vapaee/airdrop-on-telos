#!/bin/bash
# rm /var/www/blockchain/eosio/data -fr

./create_accounts.sh

./deploy_contracts.sh force

./load_tokens.sh

