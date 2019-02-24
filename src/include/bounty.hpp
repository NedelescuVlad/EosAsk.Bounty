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

        enum AnswerStatus {Undecided = 2, Awarded = 1, Incorrect = 0};

        bounty(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds), _bounties(_code, _code.value), _answers(_code, _code.value)
        {}

        [[eosio::action]]
        void insert(name from, asset quantity, uint64_t question_id, std::string memo);

        [[eosio::action]]
        void reclaim(name claimant, uint64_t question_id);

        [[eosio::action]]
        void reclaimf(name mod, name from, uint64_t question_id);

        [[eosio::action]]
        void payout(name from, uint64_t question_id, uint64_t answer_id);

        [[eosio::action]]
        void addans(name answerer, uint64_t question_id);

        [[eosio::action]]
        void rmans(name bounty_owner, uint64_t answer_id);

        [[eosio::action]]
        void erase();

        struct [[eosio::table]] bounties3
        {
            uint64_t key;
            uint64_t questionId;
            asset worth;
            name owner;

            uint64_t primary_key() const { return key; }
            uint64_t by_question_id() const { return questionId; }
        };

        struct [[eosio::table]] answers2
        {
            uint64_t key;
            uint64_t questionId;
            name owner;
            uint64_t status = AnswerStatus::Undecided;
            double eosTipped = 0;

            uint64_t primary_key() const { return key; }
            uint64_t by_question_id() const { return key; }
        };

        // Defining the tables with typedef; indexing on question id for fast lookups by question id.
        typedef multi_index<"bounties3"_n, bounties3, indexed_by<"questionid"_n, const_mem_fun<bounties3, uint64_t, &bounties3::by_question_id>>> bounty_index;
        typedef multi_index<"answers1"_n, answers1, indexed_by<"questionid"_n, const_mem_fun<answers1, uint64_t, &answers1::by_question_id>>> answer_index;

        // local instances of the multi index tables
        bounty_index _bounties;
        answer_index _answers;
};

// TODO: Add the other actions
//
EOSIO_DISPATCH(bounty, (insert)(reclaim)(reclaimf)(payout)(addans)(rmans)(erase));
