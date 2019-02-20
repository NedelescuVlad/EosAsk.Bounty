#!/bin/bash

if [ -z "$1" ]
then
    cleos get table bounty bounty bounties2
else
    cleos get table "$1" bounty bounty bounties2
fi
