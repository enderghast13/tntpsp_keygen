#!/bin/sh
# SPDX-License-Identifier: 0BSD

RST=$(echo -e "\033[0m")
RED=$(echo -e "\033[31m")
GRN=$(echo -e "\033[32m")

RET=0

echo 'TEST 1: MODE = E, GEN = 1'
if [ $(./tntpsp_keygen e 1 2b3e295e109bbe0dd4b0bc39c90773e9) \
     = 07ac0d1b506d64cb0529a5b5bb7eace4f968 ]; then
  echo "  ${GRN}✔ PASS${RST}"
else
  echo "  ${RED}✘ FAIL${RST}"
  RET=1
fi
echo

echo 'TEST 2: MODE = D, GEN = 1'
if [ $(./tntpsp_keygen d 1 07ac0d1b506d64cb0529a5b5bb7eace4f968) \
     = 2b3e295e109bbe0dd4b0bc39c90773e9 ]; then
  echo "  ${GRN}✔ PASS${RST}"
else
  echo "  ${RED}✘ FAIL${RST}"
  RET=1
fi
echo

echo 'TEST 3: MODE = E, GEN = 2'
if [ $(./tntpsp_keygen e 2 2b3e295e109bbe0dd4b0bc39c90773e9) \
     = 13a60ab295a958a07d41870abd663f5562d9 ]; then
  echo "  ${GRN}✔ PASS${RST}"
else
  echo "  ${RED}✘ FAIL${RST}"
  RET=1
fi
echo

echo 'TEST 4: MODE = D, GEN = 2'
if [ $(./tntpsp_keygen d 2 13a60ab295a958a07d41870abd663f5562d9) \
     = 2b3e295e109bbe0dd4b0bc39c90773e9 ]; then
  echo "  ${GRN}✔ PASS${RST}"
else
  echo "  ${RED}✘ FAIL${RST}"
  RET=1
fi
echo

if [ $RET = 0 ]; then
  echo "${GRN}*** ✔ ALL TESTS PASSED ***${RST}"
else
  echo "${RED}*** ✘ ONE OR MORE TESTS FAILED ***${RST}"
fi
echo

exit $RET
