#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <atomicassets.hpp>

CONTRACT niftyshopper : public eosio::contract
{
public:
    using eosio::contract::contract;

    struct cfg_params {
        uint32_t timeout_seconds = 0;
    };

    [[eosio::on_notify("*::transfer")]] void receive_token_transfer(
        eosio::name & from,
        eosio::name & to,
        eosio::asset & quantity,
        std::string & memo);

    // atomicassets listener in order to prevent issues on *::transfer
    // this has priortiy over the wildcard *
    // this notify actions does nothing functionality wise
    [[eosio::on_notify("atomicassets::transfer")]] void _dummy_aa(
        eosio::name & from,
        eosio::name & to,
        std::vector<uint64_t> & asset_ids,
        std::string & memo);

    [[eosio::action]] void setstore(
        uint64_t template_id,

        eosio::name & token_contract,
        eosio::asset & buy_price,
        bool burn_token);

    [[eosio::action]] void rmstore(
        uint64_t template_id);

    [[eosio::action]] void init();
    [[eosio::action]] void destruct();
    [[eosio::action]] void maintenance(bool maintenance);
    [[eosio::action]] void setparams(cfg_params & params);

private:
    struct [[eosio::table("config")]] _config_entity
    {
        bool maintenance = true;
        cfg_params params;
    };
    typedef eosio::singleton<eosio::name("config"), _config_entity> _config;

    struct [[eosio::table("store")]] _store_entity
    {
        uint64_t template_id;

        eosio::name token_contract;
        eosio::asset buy_price;

        bool burn_token;

        uint64_t primary_key() const { return template_id; }
    };
    typedef eosio::multi_index<eosio::name("store"), _store_entity> _store;

    struct [[eosio::table("users")]] _users_entity
    {
        eosio::name user;
        eosio::time_point_sec last_buy;

        uint64_t primary_key() const { return user.value; }
    };
    typedef eosio::multi_index<eosio::name("users"), _users_entity> _users;

    _config get_config()
    {
        return _config(get_self(), get_self().value);
    }

    _store get_store()
    {
        return _store(get_self(), get_self().value);
    }

    _users get_users()
    {
        return _users(get_self(), get_self().value);
    }

    void maintenace_check();
};
