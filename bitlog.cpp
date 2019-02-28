#include <ctime>
#include <eosiolib/asset.hpp>
#include <eosiolib/currency.hpp>
#include <eosiolib/db.h>
#include <eosiolib/eosio.hpp>
#include <eosiolib/multi_index.hpp>
#include <eosiolib/transaction.hpp>
#include <string>
#include <vector>

using namespace eosio;
using namespace std;

class bitlog : public eosio::contract {
public:
  using contract::contract;

  // @abi table chatmsg i64
  struct chatmsg {
    uint64_t id;
    uint64_t type;
    account_name user;
    string msg;
    string data;
    uint64_t ctime;
    uint64_t primary_key() const { return id; }
    EOSLIB_SERIALIZE(chatmsg, (id)(type)(user)(msg)(data)(ctime))
  };
  typedef eosio::multi_index<N(chatmsg), chatmsg> chatmsgs;
  //

  /// @abi action
  void hi() {

  }

  /// @abi action
  void postmsg(account_name user,uint64_t type,string msg) {
    require_auth(user);
    eosio_assert(type==0, "type 不正确");
    eosio_assert(msg.length()>0, "msg 长度不合法");
    chatmsgs chatmsg_inx(_self,_self);
    chatmsg_inx.emplace(_self, [&](auto &a) {
        a.id = chatmsg_inx.available_primary_key();
        a.type = type;
        a.user=user;
        a.msg=msg;
        a.ctime=now();
    });
  }

  void on(const currency::transfer &t, account_name code) {

  }
  void apply(uint64_t code, uint64_t action) {
    if (action == N(transfer) && code == N(eosio.token)) {
      on(unpack_action_data<currency::transfer>(), code);
      return;
    }
    auto &thiscontract = *this;
    switch (action) { EOSIO_API(bitlog, (hi)(postmsg)); };
  }
};

extern "C" {
[[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  bitlog gtb(receiver);
  gtb.apply(code, action);
  eosio_exit(0);
}
}