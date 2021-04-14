#!/bin/sh
diff <(1511 cs_beats_ext < testing/$1) <(./ext_beats < testing/$1)
