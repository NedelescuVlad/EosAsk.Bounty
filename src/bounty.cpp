#include "include/bounty.hpp"

using namespace eosio;

asset get_balance(name account, symbol_code code)
{
    return token::get_balance("eosio.token"_n, account, code);
}

bool has_enough_funds(name account, asset quantity)
{
    return (get_balance(account, quantity.symbol.code()) - quantity).amount;
}

void bounty::bountyadd(name from, asset quantity, uint64_t question_id, std::string memo)
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
    _bounties.emplace(get_self(), [&](auto &row) {
        row.key = _bounties.available_primary_key();
        row.questionId = question_id;
        row.worth = quantity;
        row.owner = from;
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

void bounty::reclaim(name from, uint64_t question_id)
{
    require_auth(from);

    auto bounties = _bounties.get_index<"questionid"_n>();
    auto itr_bounties = bounties.find(question_id);
    eosio_assert(itr_bounties != bounties.end(), "No bounty exists for that question");
    eosio_assert(itr_bounties->owner == from, "You're not the owner of that bounty");

    auto answers = _answers.get_index<"questionid"_n>();
    auto itr_answers = answers.find(question_id);

    bool has_only_bad_answers = true;
    for (auto &answer : _answers) 
    {
        if (answer.questionId == question_id && answer.status != AnswerStatus::Incorrect) 
        {
            has_only_bad_answers = false;
        }
    }

    eosio_assert(has_only_bad_answers, "There are answers to this bounty that might be appropriate");
    asset quantity = itr_bounties->worth;

    action(
        permission_level{get_self(), "active"_n},
        "eosio.token"_n, 
        "transfer"_n,
        std::make_tuple(
            get_self(),
            from,
            quantity,
            std::string("r")
        )
    ).send();

    _bounties.erase(*itr_bounties);
}

// TODO: Only allow mods to use this action
void bounty::reclaimf(name mod, name from, uint64_t question_id)
{
    require_auth(mod);

    auto bounties = _bounties.get_index<"questionid"_n>();
    auto itr_bounties = bounties.find(question_id);
    eosio_assert(itr_bounties != bounties.end(), "No bounty exists for that question");
    eosio_assert(itr_bounties->owner == from, "You're not the owner of that bounty");

    asset quantity = itr_bounties->worth;

    action(
        permission_level{get_self(), "active"_n},
        "eosio.token"_n, 
        "transfer"_n,
        std::make_tuple(
            get_self(),
            from,
            quantity,
            std::string("r")
        )
    ).send();

    _bounties.erase(*itr_bounties);
}

void bounty::payout(name from, uint64_t question_id, uint64_t answer_id)
{
    require_auth(from);

    auto bounties = _bounties.get_index<"questionid"_n>();
    auto itr_bounties = bounties.find(question_id);
    eosio_assert(itr_bounties != bounties.end(), "No bounty exists for that question");
    eosio_assert(itr_bounties->owner == from, "You're not the owner of that bounty");

    auto answers = _answers.get_index<"answerid"_n>();
    auto itr_answers = answers.find(answer_id);
    eosio_assert(itr_answers != answers.end(), "Invalid answer ID");
    eosio_assert(itr_bounties -> owner != itr_answers->owner, "Cannout payout your own answer");

    action(
        permission_level{get_self(), "active"_n},
        "eosio.token"_n, 
        "transfer"_n,
        std::make_tuple(
            get_self(),
            itr_answers->owner,
            itr_bounties->worth,
            std::string("Bounty Payout")
        )
    ).send();

    answers.modify(itr_answers, get_self(), [&](auto &ans) {
        ans.status = bounty::AnswerStatus::Awarded; // Awarded Bounty
    });
    bounties.erase(itr_bounties);
}

void bounty::ansadd(name answerer, uint64_t question_id, uint64_t answer_id)
{
    require_auth(answerer);

    auto answers = _answers.get_index<"answerid"_n>();
    auto itr = answers.find(answer_id);
    eosio_assert(itr == answers.end(), "Answer already added");

    _answers.emplace(get_self(), [&](auto &row) {
        row.key = _answers.available_primary_key();
        row.questionId = question_id;
        row.answerId = answer_id;
        row.owner = answerer;
    });
}

void bounty::anstip(name from, asset quantity, uint64_t answer_id) 
{
    require_auth(from);

    auto answers = _answers.get_index<"answerid"_n>();
    auto itr_answers = answers.find(answer_id);
    eosio_assert(itr_answers != answers.end(), "Invalid answer ID");
    eosio_assert(from != itr_answers->owner, "Cannot tip your own answer");

    answers.modify(itr_answers, get_self(), [&](auto &ans) {
        if (ans.eosTipped.amount == 0)
        {
            ans.eosTipped = quantity;
        }
        else 
        {
            ans.eosTipped = ans.eosTipped + quantity; 
        }
    });
}

void bounty::ansrm(name answerer, uint64_t answer_id)
{
    require_auth(answerer);

    auto answers = _answers.get_index<"answerid"_n>();
    auto itr = answers.find(answer_id);

    eosio_assert(itr != answers.end(), "Invalid answer ID");
    eosio_assert(itr->owner == answerer, "You do not own that answer");

    answers.erase(itr);
}

void bounty::ansbad(name bounty_owner, uint64_t answer_id, uint64_t reason)
{
    require_auth(bounty_owner);

    auto answers = _answers.get_index<"answerid"_n>();
    auto itr_answers = answers.find(answer_id);
    eosio_assert(itr_answers != answers.end(), "Invalid answer ID");

    auto bounties = _bounties.get_index<"questionid"_n>();
    auto itr_bounties = bounties.find(itr_answers->questionId);
    eosio_assert(itr_bounties != bounties.end(), "Invalid bounty ID");

    eosio_assert(itr_bounties->owner == bounty_owner, "You are not the owner of this bounty");

    answers.modify(itr_answers, get_self(), [&](auto &ans) {
        ans.status = bounty::AnswerStatus::Incorrect; 
        ans.statusReason = reason;
    });
}

//TODO: Remove this action as it is only for local testing
void bounty::erase()
{
    // require_auth(get_self());

    for (auto bounty_itr = _bounties.begin(); bounty_itr != _bounties.end();)
    {
        bounty_itr = _bounties.erase(bounty_itr);
    }

    for (auto answer_itr = _answers.begin(); answer_itr != _answers.end();)
    {
        answer_itr = _answers.erase(answer_itr);
    }
}
