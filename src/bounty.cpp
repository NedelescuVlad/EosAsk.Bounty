#include "include/bounty.hpp"

using namespace eosio;

asset get_balance(name account, symbol_code code)
{
    return token::get_balance("eosio.token"_n, account, code);
}

bool has_enough_funds(name account, asset quantity)
{
    return (get_balance(account, quantity.symbol.code()) - quantity).amount >= 1;
}

void bounty::insert(name from, asset quantity, uint64_t question_id, std::string memo)
{
    require_auth(from);

    // Check if there isn't already a bounty for that question id
    auto questionid_index = _bounties.get_index<"questionid"_n>();
    auto itr = questionid_index.find(question_id);

    eosio_assert(itr == questionid_index.end(), "Bounty already exists for question");
    eosio_assert(has_enough_funds(from, quantity), "Insufficient funds");

    //TODO: Add support for placing a bounty on another user's question
    //
    // a bounty is already placed for that question

    // payer == from
    _bounties.emplace(from, [&](auto &row) {
        row.key = _bounties.available_primary_key();
        row.questionId = question_id;
        row.worth = quantity;
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

void bounty::reclaim(name claimant, uint64_t question_id)
{
    require_auth(claimant);

    auto questionid_index = _bounties.get_index<"questionid"_n>();
    auto itr = questionid_index.find(question_id);

    eosio_assert(itr != questionid_index.end(), "No bounty exists for that question");

    //eosio_assert(itr == questionid_index.end(), "Bounty already exists for question");

    // Don't care about the answers in this first implementation; just allow the reclaim
    //
    
    // read from the bounties table by the bounty id
    // if bounty is_active and claimant is the owner issue eosio.token transfer from "bounty" to "from"
}

void bounty::reclaimf(name mod, name bounty_owner, uint64_t question_id)
{
    // only moderators can do this action (special permissions should be enforced on this action)
    // do the same as reclaim, but don't care about good_answers
}

void bounty::payout(name bounty_owner, name answerer, uint64_t question_id)
{
    require_auth(bounty_owner);
    // read from the table and get the amount(qty) to payout
    // if bounty is_active and bounty_owner is the owner issue eosio.token transfer from "from" to "to"
}

void bounty::addans(name answerer, uint64_t question_id)
{
    require_auth(answerer);
    // read from the table and get the amount(qty) to payout
    // if bounty is_active issue increment good_answers
    // else do nothing
}

void bounty::rmans(name bounty_owner, uint64_t question_id, uint64_t answer_id, std::string reason)
{
    require_auth(bounty_owner);
    // check if bounty_owner is the owner of bounty_id
    // check if answer.bounty_id == bounty_id 
    // mark answer as bad with given reason
}
