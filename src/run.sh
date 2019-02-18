#! /bin/bash

CONTRACTS_DIR="/home/vlad/contracts"

printf "\t=========== Building AskEos Bounty Smart Contract ===========\n\n"

mkdir -p build
eosio-cpp -o bounty.wasm bounty.cpp --abigen

mv -f bounty.wasm "$CONTRACTS_DIR"/bounty
mv -f bounty.abi "$CONTRACTS_DIR"/bounty

printf "\n\n"
printf "\t=========== Deploying Bounty Smart Contract ===========\n\n"

../scripts/unlock_wallet.sh
cleos set contract bounty /home/vlad/contracts/bounty
