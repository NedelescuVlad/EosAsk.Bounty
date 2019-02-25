#!/bin/bash

cleos push action bounty ansbad "[\"alice2\", \"$1\", $2]" -p alice2@active
