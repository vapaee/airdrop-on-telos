#include "snapsnapsnap.hpp"
using namespace eosio;

namespace telos {

// TOKEN --------------------------------------------------------------------------------------------
void snapsnapsnap::add(name account, uint64_t amount, uint64_t scope)
{
    snapshots snapshots_table( _self, scope );
    auto existing = snapshots_table.find( account.value);
    eosio_assert( existing == snapshots_table.end(), "user already exists" );

    snapshots_table.emplace( _self, [&]( auto& s ) {
        s.account = account;
        s.amount = amount;
    });
}


void snapsnapsnap::remove(name account, uint64_t scope)
{
    snapshots snapshots_table( _self, scope );
    auto existing = snapshots_table.find( account.value);
    eosio_assert( existing != snapshots_table.end(), "user not exists" );

    snapshots_table.erase( existing );
}

// TOKEN --------------------------------------------------------------------------------------------

} /// namespace vapaee

EOSIO_DISPATCH( telos::snapsnapsnap, (add)(remove) )