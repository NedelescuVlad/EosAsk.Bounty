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
        void insert(name from, asset quantity, int question_id, std::string memo);

        [[eosio::action]]
        void reclaim(name claimant, int question_id);

        [[eosio::action]]
        void reclaimf(name mod, name bounty_owner, int question_id);

        [[eosio::action]]
        void payout(name bounty_owner, name answerer, int question_id);

        [[eosio::action]]
        void addans(name answerer, int question_id);

        [[eosio::action]]
        void rmans(name bounty_owner, int question_id, int answer_id, std::string reason);

        // struct [[eosio::table]] bounties
        // {
            // uint64_t key;
            // uint64_t questionId;
            // double eosWorth;

            // uint64_t primary_key() const { return key; }
            // uint64_t by_question_id() const { return questionId; }
        // };

        struct [[eosio::table]] bounties2
        {
            uint64_t key;
            uint64_t questionId;
            asset worth;

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
        typedef multi_index<"bounties2"_n, bounties2, indexed_by<"questionid"_n, const_mem_fun<bounties2, uint64_t, &bounties2::by_question_id>>> bounty_index2;
        typedef multi_index<"answers"_n, answers, indexed_by<"questionid"_n, const_mem_fun<answers, uint64_t, &answers::by_question_id>>> answer_index2;

};

// TODO: Add the other actions
//
EOSIO_DISPATCH(bounty, (insert)(reclaim)(reclaimf)(payout)(addans)(rmans));
