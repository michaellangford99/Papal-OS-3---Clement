#!/bin/bash

sudo mount -o loop $1 ~/media/iso --read-write

echo "mounted $1 to ~/media/iso"
