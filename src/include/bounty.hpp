#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/symbol.hpp>
#include </home/vlad/contracts/eosio.contracts/eosio.token/include/eosio.token/eosio.token.hpp>
#include <eosiolib/transaction.hpp>

using namespace eosio;

// TODO: There are no checks to see if question IDs represent questions that
// actually exist in the application. This might be an issue if a bounty is posted
// on a question that doesn't exist.
class [[eosio::contract]] bounty : public eosio::contract 
{

    public:
        using contract::contract;

        bounty(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds) {}

        [[eosio::action]]
        void insert(name from, asset quantity, int question_id, std::string memo)
        {
            require_auth(from);
            eosio_assert(true, "it's not true");

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

        void reclaim(name claimant, int bounty_id)
        {
            require_auth(claimant);
            // read from the bounties table by the bounty id
            // if bounty is_active and claimant is the owner issue eosio.token transfer from "bounty" to "from"
        }

        void mod_reclaim(name mod, name bounty_owner, int bounty_id)
        {
            // only moderators can do this action (special permissions should be enforced on this action)
            // do the same as reclaim, but don't care about good_answers
        }

        void payout(name bounty_owner, name answerer, int bounty_id)
        {
            require_auth(bounty_owner);
            // read from the table and get the amount(qty) to payout
            // if bounty is_active and bounty_owner is the owner issue eosio.token transfer from "from" to "to"
        }

        void add_answer(name answerer, int bounty_id)
        {
            require_auth(answerer);
            // read from the table and get the amount(qty) to payout
            // if bounty is_active issue increment good_answers
            // else do nothing
        }

        void remove_answer(name bounty_owner, int bounty_id, int answer_id, std::string reason)
        {
            require_auth(bounty_owner);
            // check if bounty_owner is the owner of bounty_id
            // check if answer.bounty_id == bounty_id 
            // mark answer as bad with given reason
        }

    private:
        struct [[eosio::table]] bounties
        {
            uint64_t key;
            uint64_t questionId;
            double eosWorth;

            uint64_t primary_key() const { return key; }
            uint64_t by_question_id() const { return questionId; }
        };

        struct [[eosio::table]] answers
        {
            uint64_t key;
            uint64_t questionId;
            uint64_t answererId;
            uint64_t status = 2; // 2 == "Undecided", 1 == "Awarded Bounty", 0 == "Decided Bad" (by bounty poster)
            uint64_t eosEarned; // How much EOS has this answer received? Doesn't have to be status == 1 to have received eos

            uint64_t primary_key() const { return key; }
            uint64_t by_question_id() const { return key; }
        };

        // Defining the tables with typedef; indexing on question id for fast lookups by question id.
        typedef multi_index<"bounties"_n, bounties, indexed_by<"questionid"_n, const_mem_fun<bounties, uint64_t, &bounties::by_question_id>>> bounty_index;
        typedef multi_index<"answers"_n, answers, indexed_by<"questionid"_n, const_mem_fun<answers, uint64_t, &answers::by_question_id>>> answer_index;

};

// TODO: Add the other actions
EOSIO_DISPATCH(bounty, (insert));
