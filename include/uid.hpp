#pragma once

CONTRACT uid : public contract {
public:
    using contract::contract;

    /******************* Debug Interface ***********************/

    ACTION clearall();

    ACTION clearglobal();

    ACTION clearstats();

    ACTION clearuser();

    ACTION cleardev();

    ACTION clearapp();

    ACTION clearublack();

    ACTION clearablack();

    /******************* Manage Interface ***********************/

    ACTION setreg(name reg);

    ACTION setcaller(name caller);

    ACTION setmanager(name manager);

    ACTION setfee(name fee);

    ACTION setminfee(asset quantity);

    ACTION setinfeerate(symbol_code code, double rate);

    ACTION setoufeerate(symbol_code code, double rate);

    ACTION setaminfee(name contract, asset quantity);

    ACTION setafeerate(name contract, symbol_code code, double rate);

    ACTION addublack(name username);

    ACTION delublack(name username);

    ACTION addablack(name contract);

    ACTION delablack(name contract);


    /******************* User Interface ***********************/

    ACTION signup(name username, public_key pubkey, signature sig);

    ACTION update(name username, string nickname, string avatar, uint8_t gender, string brief,
            uint32_t expire_time, signature sig);

    ACTION chpubkey(name username, public_key pubkey, uint32_t expire_time, signature sig);

    ACTION signin(name username, name devname, string limits, uint32_t expire_in,
            public_key pubkey, uint32_t unlimited, uint32_t expire_time, signature sig);

    ACTION signout(name username, vector<name> contracts, string limits,
            uint32_t expire_time, signature sig);

    // ACTION transfer: using eosio.token transfer, tf^username^original_memo

    ACTION transfer(name username, name to, asset quantity, string memo,
            uint32_t expire_time, signature sig);

    ACTION createact(name username, name newaccount, public_key owner_key,
            public_key active_key, uint32_t expire_time, signature sig);

    // defer
    ACTION removeauth(name username, vector<name> contracts, public_key pubkey);

    // defer
    ACTION rmuserhash(name username, uint32_t hash);

    /******************* Developer Interface ***********************/

    // ACTION reg: using eosio.token transfer, reg^original_memo

    ACTION devupdate(name account, string devname, string avatar, string url, string brief);

    // ACTION recharge: using eosio.token rc^account^original_memo

    ACTION withdraw(name account, asset quantity, string memo);

    ACTION verifyapp(name contract, uint32_t status, string memo);

    ACTION devquit(name account, string memo);

    /******************* DApp Interface ***********************/

    ACTION appreg(name contract, name developer, string memo);

    ACTION appupdate(name contract, name developer, string appname, string owner,
            string logo_url, string url, string brief, string desc);

    ACTION setappinfo(name contract, string appname, string owner, string logo_url,
            string url, string brief, string desc, string language);

    ACTION appquit(name contract, string memo);

    ACTION charge(name username, name contract, asset quantity, string memo,
            uint32_t expire_time, signature sig);

    ACTION pushaction(name username, name contract, name action, string args,
            uint32_t expire_time, signature sig);

    // defer
    ACTION rmchargehash(name username, name contract, uint32_t hash);

    /******************* Transfer ***********************/
    /**
     * 1. transfer: tf^username^original_memo
     * 2. devreg: reg^original_memo
     * 3. recharge: rc^account^original_memo
     *
     **/
    void transfer_received(name from, name to, asset quantity, string memo);

public:
    TABLE global_info {
        uint64_t id = 0;
        uint64_t user_num = 0;
        uint64_t dev_num = 0;
        uint64_t app_num = 0;
        uint64_t created_num = 0;
        name reg;
        name caller;
        name manager;
        name fee;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(global_info, (id)(user_num)(dev_num)(app_num)(created_num)(reg)(caller)(manager)(fee))
    };

    TABLE stats_info {
        asset min_fee;
        asset total_fee;
        double in_fee_rate;
        double out_fee_rate;
        asset user_balance;
        asset dev_balance;

        uint64_t primary_key() const { return total_fee.symbol.code().raw(); }

        EOSLIB_SERIALIZE(stats_info, (min_fee)(total_fee)(in_fee_rate)(out_fee_rate)(user_balance)(dev_balance))
    };

    TABLE user_info {
        name username;
        string nickname; // 32bytes
        string avatar; // 128 bytes
        uint8_t gender; // 0 unkown, 1: male 2 femal
        string brief; // 128 bytes
        public_key pubkey;
        set<uint32_t> sig_hashs; //used hashs

        uint64_t primary_key() const { return username.value; }

        EOSLIB_SERIALIZE(user_info, (username)(nickname)(avatar)(gender)(brief)(pubkey)(sig_hashs))
    };

    TABLE developer_info {
        name developer;
        asset balance;
        uint32_t status = 0; // 0: normal, 1: quited
        string name; // 32 bytes
        string avatar; // 128 bytes
        string url; // 128 bytes
        string brief; //  128 bytes

        uint64_t primary_key() const { return developer.value; }
        uint64_t by_balance_desc() const { return -balance.amount; }

        EOSLIB_SERIALIZE(developer_info, (developer)(balance)(status)(name)(avatar)(url)(brief))
    };

    TABLE dapp_info {
        name contract;
        name developer;
        uint32_t category = 0;
        uint32_t status = 0; // 0: initial 1: verified
        string appname; // 32 bytes
        string owner; // 64 bytes
        string logo_url; // 128 bytes
        string url; // 128 bytes
        string brief;   // 64 bytes
        string desc; // 512 bytes

        uint64_t primary_key() const { return contract.value; }
        uint64_t by_developer() const { return developer.value; }

        EOSLIB_SERIALIZE(dapp_info, (contract)(developer)(category)(status)(appname)(owner)(logo_url)(url)(brief)(desc))
    };

    TABLE dapp_info_cn {
        name contract;
        string appname; // 32 bytes
        string owner; // 64 bytes
        string logo_url; // 128 bytes
        string url; // 128 bytes
        string brief;   // 64 bytes
        string desc; // 512 bytes

        uint64_t primary_key() const { return contract.value; }

        EOSLIB_SERIALIZE(dapp_info_cn, (contract)(appname)(owner)(logo_url)(url)(brief)(desc))
    };

    TABLE dapp_info_en {
        name contract;
        string appname; // 32 bytes
        string owner; // 64 bytes
        string logo_url; // 128 bytes
        string url; // 128 bytes
        string brief;   // 64 bytes
        string desc; // 512 bytes

        uint64_t primary_key() const { return contract.value; }

        EOSLIB_SERIALIZE(dapp_info_en, (contract)(appname)(owner)(logo_url)(url)(brief)(desc))
    };

    TABLE dapp_fee {
        asset total_fee;
        asset min_fee;
        double fee_rate;

        uint64_t primary_key() const { return total_fee.symbol.code().raw(); };
        uint64_t by_total_fee() const { return -total_fee.amount; }

        EOSLIB_SERIALIZE(dapp_fee, (total_fee)(min_fee)(fee_rate));
    };

    TABLE auth_info {
        struct credit_info {
            asset limit; // limit amount
            asset used; // used amount

            credit_info() = default;
            credit_info(asset l, asset u)
                :limit(l), used(u)
            {}

            EOSLIB_SERIALIZE(credit_info, (limit)(used))
        };

        name contract;
        public_key pubkey;
        time_point_sec expire;
        vector<credit_info> credits;
        set<uint32_t> sig_hashs;

        uint64_t primary_key() const { return contract.value; }
        uint64_t by_expire() const { return static_cast<uint64_t>(expire.sec_since_epoch()); }

        EOSLIB_SERIALIZE(auth_info, (contract)(pubkey)(expire)(credits)(sig_hashs))
    };

    TABLE user_balance {
        asset balance;

        uint64_t primary_key() const { return balance.symbol.code().raw(); }

        EOSLIB_SERIALIZE(user_balance, (balance))
    };

    TABLE user_blacklist {
        name username;
        time_point_sec time;

        uint64_t primary_key() const { return username.value; }
        uint64_t by_time() const { return static_cast<uint64_t>(time.sec_since_epoch()); }

        EOSLIB_SERIALIZE(user_blacklist, (username)(time))
    };

    TABLE app_blacklist {
        name contract;
        time_point_sec time;

        uint64_t primary_key() const { return contract.value; }
        uint64_t by_time() const { return static_cast<uint64_t>(time.sec_since_epoch()); }

        EOSLIB_SERIALIZE(app_blacklist, (contract)(time))
    };
};

using global_table = multi_index<"global"_n, uid::global_info>;
using stats_table = multi_index<"stats"_n, uid::stats_info>;
using user_table = multi_index<"users"_n, uid::user_info>;
using developer_table = multi_index<"developers"_n, uid::developer_info,
      indexed_by<"balance"_n, const_mem_fun<uid::developer_info, uint64_t, &uid::developer_info::by_balance_desc>>>;
using dev_balance_table =  multi_index<"devbalance"_n, uid::user_balance>;
using dapp_table = multi_index<"dapps"_n, uid::dapp_info,
      indexed_by<"developer"_n, const_mem_fun<uid::dapp_info, uint64_t, &uid::dapp_info::by_developer>>>;
using dapp_info_cn_table  = multi_index<"dinfocn"_n, uid::dapp_info_cn>;
using dapp_info_en_table  = multi_index<"dinfoen"_n, uid::dapp_info_en>;

using dappfee_table = multi_index<"dappfee"_n,  uid::dapp_fee,
      indexed_by<"totalfee"_n, const_mem_fun<uid::dapp_fee, uint64_t, &uid::dapp_fee::by_total_fee>>>;
using auth_table = multi_index<"auths"_n, uid::auth_info,
      indexed_by<"expire"_n, const_mem_fun<uid::auth_info, uint64_t, &uid::auth_info::by_expire>>>;
using balance_table = multi_index<"balance"_n, uid::user_balance>;
using user_blacklist_table = multi_index<"ublacklist"_n, uid::user_blacklist,
      indexed_by<"time"_n, const_mem_fun<uid::user_blacklist, uint64_t, &uid::user_blacklist::by_time>>>;
using app_blacklist_table = multi_index<"ablacklist"_n, uid::app_blacklist,
      indexed_by<"time"_n, const_mem_fun<uid::app_blacklist, uint64_t, &uid::app_blacklist::by_time>>>;

