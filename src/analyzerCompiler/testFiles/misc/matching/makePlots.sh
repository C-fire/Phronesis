#!/bin/bash
dot -Tpng /tmp/file.dot -o /tmp/file.png; dot -Tpng /tmp/db.dot -o /tmp/db.png; evince /tmp/file.png & evince /tmp/db.png
