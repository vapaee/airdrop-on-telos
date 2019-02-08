#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

using namespace eosio;
using namespace std;


namespace vapaee {

CONTRACT vapaeetokens : public eosio::contract {
    private:
       // TOKEN-TABLES ------------------------------------------------------------------------------------------------------

        TABLE account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
        };

        TABLE currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
        };

        typedef eosio::multi_index< "accounts"_n, account > accounts;
        typedef eosio::multi_index< "stat"_n, currency_stats > stats;

        void sub_balance( name owner, asset value );
        void add_balance( name owner, asset value, name ram_payer );

    public:
       // TOKEN-ACTOINS ------------------------------------------------------------------------------------------------------

        using contract::contract;
        // cleos push action vapaeetokens create '["vapaeetokens","500000000.0000 CNT"]' -p vapaeetokens@active
        ACTION create( name issuer, asset  maximum_supply);
        ACTION issue( name to, const asset& quantity, string memo );
        ACTION retire( asset quantity, string memo );
        ACTION transfer(name from, name to, asset quantity, string  memo );
        ACTION open( name owner, const symbol& symbol, name ram_payer );
        ACTION close( name owner, const symbol& symbol );

    private:
        // SNAPSHOT TABLES ------------------------------------------------------------------------------------------------------
        // auxiliar structure to query the real snapshot table on other contract. Holds an amount of TLOS of each account.
        TABLE snapshot_table {
            name account;
            int64_t amount;
            uint64_t primary_key() const { return account.value; }
        };
        typedef eosio::multi_index< "snapshots"_n, snapshot_table > snapshots;

        // this will have only 1 row setted by calling action setsnapshot
        TABLE spanshot_source {
            name contract;
            uint64_t scope;
            int64_t cap;
            int64_t min;
            uint64_t primary_key() const { return contract.value; }
            uint64_t by_scope_key() const { return scope; }
        };
        typedef eosio::multi_index< "source"_n, spanshot_source,
            indexed_by<"scope"_n, const_mem_fun<spanshot_source, uint64_t, &spanshot_source::by_scope_key>>
        > source;

        // Every user has this table with 1 entry for each token symbol_code claimed
        TABLE claimed_table {
            symbol_code sym_code;
            uint64_t primary_key() const { return sym_code.raw(); }
        };
        typedef eosio::multi_index< "claimed"_n, claimed_table > claimed;
        
    public:
        // AIRDROP-ACTOINS  ------------------------------------------------------------------------------------------------------
        // cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"CNT",0,0]' -p vapaeetokens@active
        ACTION setsnapshot (name contract, uint64_t scope, const symbol_code& symbolcode, int64_t cap, int64_t min);
        ACTION nosnapshot (const symbol_code& symbolcode);
        ACTION claim (name owner, const symbol_code & symbolcode, name ram_payer);

    private:
        // MARKET-TABLES ------------------------------------------------------------------------------------------------------
        TABLE reg_token_table {
            symbol_code symbol;
            name contract;
            uint64_t primary_key() const { return symbol.raw(); }
            uint64_t by_contract_key() const { return contract.value; }
        };

        typedef eosio::multi_index< "tokens"_n, reg_token_table,
            indexed_by<"contract"_n, const_mem_fun<reg_token_table, uint64_t, &reg_token_table::by_contract_key>>
        > tokens;



    public:
        // MARKET-ACTOINS  ------------------------------------------------------------------------------------------------------
        ACTION addtoken (name contract, const symbol_code & symbol, name ram_payer);



    private:
        // BANKING-TABLES ------------------------------------------------------------------------------------------------------
        // scope: owner
        TABLE user_stakes_table {
            asset quantity;
            name to;
            uint64_t since;
            uint64_t primary_key() const { return quantity.symbol.code().raw(); }
            uint64_t by_to_key() const { return to.value; }
        };
        typedef eosio::multi_index< "stakes"_n, user_stakes_table,
            indexed_by<"to"_n, const_mem_fun<user_stakes_table, uint64_t, &user_stakes_table::by_to_key>>
        > stakes;

        // scope: owner
        TABLE user_unstakes_table {
            asset quantity;
            uint64_t block;
            uint64_t primary_key() const { return quantity.symbol.code().raw(); }
            uint64_t by_block_key() const { return block; }
        };
        typedef eosio::multi_index< "unstakes"_n, user_unstakes_table,
            indexed_by<"block"_n, const_mem_fun<user_unstakes_table, uint64_t, &user_unstakes_table::by_block_key>>
        > unstakes;

        // scope: owner
        TABLE unstake_time_table {
            symbol_code sym_code;
            uint64_t min_time;
            uint64_t max_time;
            uint64_t primary_key() const { return sym_code.raw(); }
        };
        typedef eosio::multi_index< "config"_n, unstake_time_table > config;

    public:
        // BANKING-ACTOINS  ------------------------------------------------------------------------------------------------------
        // se genera una transacción desde el owner al contrato pagándole la cantidad quantity
        ACTION stake (name owner, const asset & quantity, name to);

        // se fija cuanto tiempo corresponde según la cantidad.
        // Resta esa cantidad de la entrada de la tabla stakes. Si llega a cero la elimina
        // coloca una entrada en la tabla unstakes con el vencimiento calculado en block
        ACTION unstake (name owner, const asset & quantity, name from);

        // recorre la tabla de unstakes y todos los que encuentra que tienen el block menor al block actual entonces lo saca de la lista y los suma
        // luego hace una sola transacción desde el contrato hacia el owner
        ACTION unstakeback (name owner);

        // setea el tiempo máximo y mínimo a esperar al hacer unstake. El timepo final dependerá de la cantidad en porcentaje sobre el total staked
        // si el auto_stake es > 0, entonces por cada interaccción con el contrato se registrará el bloque (de la última actividad)
        // Si el owner quiere hacer un transfer y la cantidad de blockes desde el ultimo registro supera auto_stake,
        // entonces se auto stakea toda la quita y no se permite transferir fondos
        ACTION unstaketime (name owner, const symbol_code & sym_code, uint64_t min_time, uint64_t max_time, uint64_t auto_stake);



};

}; // namespace