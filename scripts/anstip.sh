#!/bin/bash

cleos push action bounty anstip "[\"alice2\", \"$1 EOS\", $2]" -p alice2@active
