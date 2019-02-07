#include "vapaeetokens.hpp"
using namespace eosio;

namespace vapaee {

// TOKEN --------------------------------------------------------------------------------------------
void vapaeetokens::create(name issuer, asset maximum_supply) {
    print("\nACTION vapaeetokens.create()\n");
    require_auth( issuer );

    auto sym = maximum_supply.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( maximum_supply.is_valid(), "invalid supply");
    eosio_assert( maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable( _self, sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    eosio_assert( existing == statstable.end(), "token with symbol already exists" );

    statstable.emplace( _self, [&]( auto& s ) {
        s.supply.symbol = maximum_supply.symbol;
        s.max_supply    = maximum_supply;
        s.issuer        = issuer;
    });
}


// void vapaeetokens::issue( name to, const asset& quantity, string memo ) {
void vapaeetokens::issue( name to, const asset& quantity, string memo ) {
    print("\nACTION vapaeetokens.issue()\n");
    print("to: ", to.to_string(), "\n");
    print("quantity: ", quantity.to_string(), "\n");
    print("memo: ", memo.c_str(), "\n");
    
    // check on symbol
    auto sym = quantity.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    // check token existance
    stats statstable( _self, sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    eosio_assert( existing != statstable.end(), "token with symbol does not exist, create token before issue" );
    const auto& st = *existing;

    // check quantity
    require_auth( st.issuer );
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must issue positive quantity" );

    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");
    
    // update current supply
    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply += quantity;
    });

    // update issuer balance silently
    add_balance( st.issuer, quantity, st.issuer );

    // if target user is not issuer the send an inline action
    if( to != st.issuer ) {
        SEND_INLINE_ACTION( *this, transfer, { {st.issuer, "active"_n} },
                            { st.issuer, to, quantity, memo }
        );
    }
    //print("vapaeetokens.issue() ...\n");
}

void vapaeetokens::retire( asset quantity, string memo ) {
    print("\nACTION vapaeetokens.retire()\n");

    auto sym = quantity.symbol;
    eosio_assert( sym.is_valid(), "invalid symbol name" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    stats statstable( _self, sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    eosio_assert( existing != statstable.end(), "token with symbol does not exist" );
    const auto& st = *existing;

    require_auth( st.issuer );
    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must retire positive quantity" );

    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );

    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply -= quantity;
    });

    sub_balance( st.issuer, quantity );
}

void vapaeetokens::transfer(name from, name to, asset quantity, string memo) {
    print("\nACTION vapaeetokens.transfer()\n");

    eosio_assert( from != to, "cannot transfer to self" );
    require_auth( from );
    eosio_assert( is_account( to ), "to account does not exist");
    auto sym = quantity.symbol.code();
    stats statstable( _self, sym.raw() );
    const auto& st = statstable.get( sym.raw() );

    require_recipient( from );
    require_recipient( to );

    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
    eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    auto ram_payer = has_auth( to ) ? to : from;

    sub_balance( from, quantity );
    add_balance( to, quantity, ram_payer );

}

void vapaeetokens::sub_balance( name owner, asset value ) {
    accounts from_acnts( _self, owner.value );

    const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found" );
    eosio_assert( from.balance.amount >= value.amount, "overdrawn balance" );

    from_acnts.modify( from, owner, [&]( auto& a ) {
        a.balance -= value;
    });
}

void vapaeetokens::add_balance( name owner, asset value, name ram_payer ) {
    accounts to_acnts( _self, owner.value );
    auto to = to_acnts.find( value.symbol.code().raw() );
    if( to == to_acnts.end() ) {
        to_acnts.emplace( ram_payer, [&]( auto& a ){
            a.balance = value;
        });
    } else {
        to_acnts.modify( to, same_payer, [&]( auto& a ) {
            a.balance += value;
        });
    }
}

void vapaeetokens::open( name owner, const symbol& symbol, name ram_payer ) {
    print("\nACTION vapaeetokens.open()\n");    
    require_auth( ram_payer );

    auto sym_code_raw = symbol.code().raw();

    stats statstable( _self, sym_code_raw );
    const auto& st = statstable.get( sym_code_raw, "symbol does not exist" );
    eosio_assert( st.supply.symbol == symbol, "symbol precision mismatch" );

    accounts acnts( _self, owner.value );
    auto it = acnts.find( sym_code_raw );
    if( it == acnts.end() ) {
        acnts.emplace( ram_payer, [&]( auto& a ){
            a.balance = asset{0, symbol};
        });
    } else {
        acnts.modify( it, ram_payer, [&]( auto& a ) {
            a.balance += a.balance;
        });
    }
    print("vapaeetokens.open() ...\n");    
}

void vapaeetokens::close( name owner, const symbol& symbol ) {
    print("\nACTION vapaeetokens.close()\n");

    require_auth( owner );
    accounts acnts( _self, owner.value );
    auto it = acnts.find( symbol.code().raw() );
    eosio_assert( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect." );
    eosio_assert( it->balance.amount == 0, "Cannot close because the balance is not zero." );
    acnts.erase( it );
}

#define TOKEN_ACTIONS (create)(issue)(transfer)(open)(close)(retire)

// AIRDROP --------------------------------------------------------------------------------------------


void vapaeetokens::setsnapshot(name contract, uint64_t scope, const symbol_code& sym_code, int64_t cap, int64_t min) {
    print("\nACTION vapaeetokens.setsnapshot()\n");

    require_auth( _self );
    source table( _self, sym_code.raw() );
    auto it = table.begin();
    eosio_assert(it == table.end(), "source table is not empty");

    table.emplace( _self, [&]( auto& a ){
        a.contract = contract;
        a.scope = scope;
        a.min = min;
        a.cap = cap;
    });
}

void vapaeetokens::nosnapshot(const symbol_code& sym_code) {
    print("\nACTION vapaeetokens.nosnapshot()\n");

    require_auth( _self );
    source table( _self, sym_code.raw() );
    auto it = table.begin();
    eosio_assert(it != table.end(), "source table is empty");

    table.erase(it);
}

void vapaeetokens::claim(name owner, const symbol_code& sym_code, name ram_payer) {
    print("\nACTION vapaeetokens.claim()\n");

    require_auth( ram_payer );
    eosio::symbol symbol{sym_code,4};
    auto sym_code_raw = symbol.code().raw();
 
    // check symbol exists
    stats statstable( _self, sym_code_raw );
    const auto& st = statstable.get( sym_code_raw, "symbol does not exist" );
    eosio_assert( st.supply.symbol == symbol, "symbol precision mismatch" );
    
    // get airdrop amount
    source table( _self, sym_code_raw );
    auto srcit = table.begin();
    eosio_assert(srcit != table.end(), "SOURCE table is EMPTY. execute action setsnapshot before.");

    snapshots snaptable( srcit->contract, srcit->scope );
    auto snapit = snaptable.find(owner.value);
    eosio_assert(snapit != snaptable.end(), "SNAPSHOTS table does not have an entry for owner");
    int64_t amount = snapit->amount;
    int64_t cap = srcit->cap;
    int64_t min = srcit->min;
    
    // filter
    if (cap > 0) if (amount > cap) {
        amount = cap;
    }    
    eosio_assert(amount >= min, (owner.to_string() + " account does NOT reach the minimun amount of " + std::to_string((float)min)).c_str());
    
    // check if already claimed
    claimed claimedtable( _self, owner.value );
    auto it = claimedtable.find( sym_code_raw );
    eosio_assert(it == claimedtable.end(), "You already claimed this token airdrop");

    // set calimed 
    claimedtable.emplace(ram_payer, [&]( auto& a ){
        a.sym_code = sym_code;
    });

    // perform the airdrop
    asset quantity = asset{(int64_t)amount, symbol};

    action(
        permission_level{ram_payer,"active"_n},
        get_self(),
        "open"_n,
        std::make_tuple(owner, symbol, ram_payer)
    ).send();
   
    action(
        permission_level{st.issuer,"active"_n},
        get_self(),
        "issue"_n,
        std::make_tuple(owner, quantity, string("airdrop"))
    ).send();
    
    print("vapaeetokens.claim() ...\n");
}

#define AIRDROP_ACTIONS (setsnapshot)(nosnapshot)(claim)

// MARKET --------------------------------------------------------------------------------------------
void vapaeetokens::addtoken(name contract, const symbol_code & symbol, name ram_payer) {
    print("\nACTION vapaeetokens.addtoken()\n");

    tokens tokenstable(get_self(), get_self().value);
    auto itr = tokenstable.find(symbol.raw());
    eosio_assert(itr == tokenstable.end(), "Token already registered");
    tokenstable.emplace( ram_payer, [&]( auto& a ){
        a.contract = contract;
        a.symbol = symbol;
    });
}



#define MARKET_ACTIONS (addtoken)



} /// namespace vapaee

EOSIO_DISPATCH( vapaee::vapaeetokens, TOKEN_ACTIONS AIRDROP_ACTIONS MARKET_ACTIONS )