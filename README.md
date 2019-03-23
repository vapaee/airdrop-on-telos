# airdrop-on-telos

This repository holds all the software generated to perform the CNT Token Airdrop on TELOS network based on the original TLOS snapshot. The final CNT Token smart contract will include some features that excede this demo.    
--------------------------------------------------------------------------------------

```
cd /var/www
git clone https://github.com/vapaee/airdrop-on-telos.git
cd airdrop-on-telos
npm i
```

You need a valid EOS-public-key already imported in your keosd wallet. If you dont simply execute:
```
cleos create key --to-console
cleos wallet import
```
then copy (Ctrl+Shift+C) and paste (Ctrl+Shift+V) in the same terminal the private key just generated. 


edit the `scripts/create_accounts.sh` file and change the EOS_PUB_KEY variable with your generated EOS-public-key
```
gedit scripts/create_accounts.sh
```

if you did clone the repository in a path different from `/var/www/airdrop-on-telos` then edit the `scripts/deploy_contracts.sh` and change the variable HOME with the correct path to the cloned repo.
```
gedit scripts/deploy_contracts.sh
```

Make sure you have nodeos running and then excecute the init script. This will create the accounts needed, compile the code and deploy it on the blockchain and then will define 3 Tokens with different airdrop configurations (CNT, BOX, VPE)
```
cd scripts
sudo chmod +x *.sh
./init.sh
```

There's a script with the claim commands for the 4 user accounts (bob, alice, kate, tom).
```
cd scripts
./claim_tokens.sh
```

Also there's a script to consult the tokens state and balances for all users
```
cd scripts
./tables_tokens.sh
```

# Aidrop Script
Finally, if you wants to perform an actual airdrop 'the old-school way' refer to [perform_airdrop.js node script](https://github.com/vapaee/airdrop-on-telos/blob/master/perform_airdrop.js).
