printf "\n\n"
printf "\t=========== Deploying Bounty Smart Contract ===========\n\n"

"$CONTRACTS_DIR"/bounty/scripts/unlock_wallet.sh
cleos set contract bounty "$CONTRACTS_DIR"/bounty
