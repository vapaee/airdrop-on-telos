#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>
#include <math.h>

using namespace eosio;
using namespace std;


namespace vapaee {
    namespace bgbox {
        static uint128_t combine( uint64_t key1, uint64_t key2 ) {
            return (uint128_t{key1} << 64) | uint128_t{key2};
        }        
    }    

CONTRACT vapaeetokens : public eosio::contract {
    private:
       // TOKEN-TABLES ------------------------------------------------------------------------------------------------------

        TABLE account {
            asset    balance;

            uint64_t primary_key()const { return balance.symbol.code().raw(); }
        };

        TABLE currency_stats {
            eosio::asset           supply;
            eosio::asset           max_supply;
            name                   issuer;
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
            uint64_t amount;
            uint64_t primary_key() const { return account.value; }
        };
        typedef eosio::multi_index< "snapshots"_n, snapshot_table > snapshots;

        // this will have only 1 row setted by calling action setsnapshot
        TABLE spanshot_source {
            name contract;
            uint64_t scope;
            int64_t cap;
            int64_t min;
            int64_t ratio;
            int64_t base;
            string memo;
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
        ACTION setsnapshot (name contract, uint64_t scope, const symbol_code& sym_code, int64_t cap, int64_t min, int64_t ratio, int64_t base, const std::string & memo);
        ACTION claim (name owner, const symbol_code & symbolcode, name ram_payer);


}; // contract

}; // namespace