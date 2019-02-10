#!/bin/bash

cleos push action vapaeetokens claim '["bob","CNT","bob"]' -p bob@active
cleos push action vapaeetokens claim '["alice","CNT","bob"]' -p bob@active
cleos push action vapaeetokens claim '["kate","CNT","bob"]' -p bob@active
cleos push action vapaeetokens claim '["tom","CNT","bob"]' -p bob@active

cleos push action vapaeetokens claim '["bob","BOX","bob"]' -p bob@active
cleos push action vapaeetokens claim '["alice","BOX","bob"]' -p bob@active
cleos push action vapaeetokens claim '["kate","BOX","bob"]' -p bob@active
cleos push action vapaeetokens claim '["tom","BOX","bob"]' -p bob@active

cleos push action vapaeetokens claim '["bob","VPE","bob"]' -p bob@active
cleos push action vapaeetokens claim '["alice","VPE","bob"]' -p bob@active
cleos push action vapaeetokens claim '["kate","VPE","bob"]' -p bob@active
cleos push action vapaeetokens claim '["tom","VPE","bob"]' -p bob@active
