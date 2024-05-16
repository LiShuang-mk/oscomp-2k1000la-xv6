#!/bin/bash

wc -l `find ./kernel -name '*.cc' -or -name '*.c' -or -name '*.S' -or -name '*.hh' -or -name '*.h' -or -name 'Makefile'`