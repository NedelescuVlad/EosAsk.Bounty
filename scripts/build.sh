#! /bin/bash

printf "\t=========== Building AskEos Bounty Smart Contract ===========\n\n"

eosio-cpp -o bounty.wasm "$CONTRACTS_DIR"/bounty/src/bounty.cpp --abigen

mv -f bounty.wasm "$CONTRACTS_DIR"/bounty
mv -f bounty.abi "$CONTRACTS_DIR"/bounty

