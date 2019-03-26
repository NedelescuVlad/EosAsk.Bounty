#!/bin/bash

cleos push action bounty insert "[\"alice2\", \"$1.0000 EOS\", $2, \"test\"]" -p alice2@active
