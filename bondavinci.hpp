/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

using namespace eosio;
using std::string;

class token : public contract {
  public:
      token( account_name self ):contract(self){}

      void create( account_name issuer,
                  asset        maximum_supply);

      void unlock( asset unlock );

      void issue( account_name to, asset quantity, string memo );

      void transfer( account_name from,
                    account_name to,
                    asset        quantity,
                    string       memo );

      void addblacklist( account_name auth, account_name user );
      void rmblacklist( account_name auth, account_name user );


      inline asset get_supply( symbol_name sym )const;

      inline asset get_balance( account_name owner, symbol_name sym )const;

  private:
      // @abi table accounts i64
      struct account {
        asset    balance;

        uint64_t primary_key()const { return balance.symbol.name(); }
      };

      // @abi table stat i64
      struct stat {
        asset          supply;
        asset          max_supply;
        account_name   issuer;
        bool           transfer_locked = true;

        uint64_t primary_key()const { return supply.symbol.name(); }
      };

      // @abi table blacklist i64
      struct blacklist {
        account_name   user;

        uint64_t primary_key()const { return user; }
      };

      typedef eosio::multi_index<N(accounts), account> accounts;
      typedef eosio::multi_index<N(stat), stat> stats;
      typedef eosio::multi_index<N(blacklist), blacklist> blacklists;

      void sub_balance( account_name owner, asset value );
      void add_balance( account_name owner, asset value, account_name ram_payer );
      bool is_blacklisted(account_name user);
  public:
      struct transfer_args {
        account_name  from;
        account_name  to;
        asset         quantity;
        string        memo;
      };
};

asset token::get_supply( symbol_name sym )const
{
  stats statstable( _self, sym );
  const auto& st = statstable.get( sym );
  return st.supply;
}

asset token::get_balance( account_name owner, symbol_name sym )const
{
  accounts accountstable( _self, owner );
  const auto& ac = accountstable.get( sym );
  return ac.balance;
}

