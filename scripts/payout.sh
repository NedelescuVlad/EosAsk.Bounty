#!/bin/bash

cleos push action bounty payout "[\"alice2\", $1, $2]" -p alice2@active
