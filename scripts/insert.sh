#!/bin/bash

cleos push action bounty insert "[\"alice2\", \"$1.0000 SYS\", $2, \"test\"]" -p alice2@active
