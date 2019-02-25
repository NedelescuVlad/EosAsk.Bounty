#!/bin/bash

if [ -z "$1" ]
then
    cleos get table bounty bounty bounties3
else
    cleos get table "$1" bounty bounty bounties3
fi
