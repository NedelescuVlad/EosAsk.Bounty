#!/bin/bash

if [ -z "$1" ]
then
    cleos get table bounty bounty answers12
else
    cleos get table "$1" bounty bounty answers12
fi
