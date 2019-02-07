#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

using namespace eosio;
using namespace std;


namespace telos {

CONTRACT snapsnapsnap : public eosio::contract {
    private:
        TABLE snapshot_table {
            name account;
            uint64_t amount;
            uint64_t primary_key() const { return account.value; }
        };

        typedef eosio::multi_index< "snapshots"_n, snapshot_table > snapshots;

    public:
        using contract::contract;

        ACTION add(name account, uint64_t  amount, uint64_t scope);

        ACTION remove(name account, uint64_t scope);

};

}; // namespace