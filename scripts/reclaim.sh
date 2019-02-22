#!/bin/bash

cleos push action bounty reclaim "[\"alice2\", \"$1\"]" -p alice2@active
