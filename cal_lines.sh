#!/bin/bash

wc -l `find ./kernel ./user -name '*.cc' -or -name '*.c' -or -name '*.S' -or -name '*.hh' -or -name '*.h' -or -name '*.inc' -or -name 'Makefile'`