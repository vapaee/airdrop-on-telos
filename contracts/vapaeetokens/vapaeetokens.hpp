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
        
    public:
       // AIRDROP-ACTOINS  ------------------------------------------------------------------------------------------------------
       // cleos push action vapaeetokens setsnapshot '["snapsnapsnap",1,"CNT",0,0]' -p vapaeetokens@active
       ACTION setsnapshot (name contract, uint64_t scope, const symbol_code& symbolcode, int64_t cap, int64_t min);
       ACTION nosnapshot (const symbol_code& symbolcode);
       ACTION claim (name owner, const symbol_code & symbolcode);

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



};

}; // namespace