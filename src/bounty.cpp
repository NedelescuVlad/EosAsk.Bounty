#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/symbol.hpp>
#include </home/vlad/contracts/eosio.contracts/eosio.token/include/eosio.token/eosio.token.hpp>
#include <eosiolib/transaction.hpp>
#include "include/bounty.hpp"

using namespace eosio;

void bounty::insert(name from, asset quantity, int question_id, std::string memo)
{
    require_auth(from);

    // Check if there isn't already a bounty for that question id
    bounty_index bounties(_code, _code.value);
    auto questionid_index = bounties.get_index<"questionid"_n>();
    auto itr = questionid_index.find(question_id);

    eosio_assert(itr == questionid_index.end(), "Bounty already exists for question");
    eosio_assert(has_enough_funds(from, quantity), "Insufficient funds");

    //TODO: Add support for placing a bounty on another user's question
    //
    // a bounty is already placed for that question

    // from == payer
    bounties.emplace(from, [&](auto &row) {
        row.key = bounties.available_primary_key();
        row.questionId = question_id;
        row.eosWorth = quantity.amount;
    });

    action(
        permission_level{from, "active"_n},
        "eosio.token"_n, 
        "transfer"_n,
        std::make_tuple(
            from,
            get_self(),
            quantity,
            memo
        )
    ).send();
}

void bounty::reclaim(name claimant, int bounty_id)
{
    require_auth(claimant);
    // read from the bounties table by the bounty id
    // if bounty is_active and claimant is the owner issue eosio.token transfer from "bounty" to "from"
}

void bounty::mod_reclaim(name mod, name bounty_owner, int bounty_id)
{
    // only moderators can do this action (special permissions should be enforced on this action)
    // do the same as reclaim, but don't care about good_answers
}

void bounty::payout(name bounty_owner, name answerer, int bounty_id)
{
    require_auth(bounty_owner);
    // read from the table and get the amount(qty) to payout
    // if bounty is_active and bounty_owner is the owner issue eosio.token transfer from "from" to "to"
}

void bounty::add_answer(name answerer, int bounty_id)
{
    require_auth(answerer);
    // read from the table and get the amount(qty) to payout
    // if bounty is_active issue increment good_answers
    // else do nothing
}

void bounty::remove_answer(name bounty_owner, int bounty_id, int answer_id, std::string reason)
{
    require_auth(bounty_owner);
    // check if bounty_owner is the owner of bounty_id
    // check if answer.bounty_id == bounty_id 
    // mark answer as bad with given reason
}

bool has_enough_funds(name account, asset quantity)
{
    return (get_balance(account, quantity.symbol.code()) - quantity).amount >= 1;
}

asset get_balance(name account, symbol_code code)
{
    return token::get_balance("eosio.token"_n, account, code);
}
