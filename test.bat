:: SPDX-License-Identifier: 0BSD
@echo off

set /a RET=0

echo TEST 1: MODE = E, GEN = 1
for /f %%A in ('tntpsp_keygen.exe e 1 2b3e295e109bbe0dd4b0bc39c90773e9') do set "out=%%A"
if "%out%"=="07ac0d1b506d64cb0529a5b5bb7eace4f968" (
  echo   PASS
) else (
  echo   FAIL
  set /a RET=1
)
echo(

echo TEST 2: MODE = D, GEN = 1
for /f %%A in ('tntpsp_keygen.exe d 1 07ac0d1b506d64cb0529a5b5bb7eace4f968') do set "out=%%A"
if "%out%"=="2b3e295e109bbe0dd4b0bc39c90773e9" (
  echo   PASS
) else (
  echo   FAIL
  set /a RET=1
)
echo(

echo TEST 3: MODE = E, GEN = 2
for /f %%A in ('tntpsp_keygen.exe e 2 2b3e295e109bbe0dd4b0bc39c90773e9') do set "out=%%A"
if "%out%"=="13a60ab295a958a07d41870abd663f5562d9" (
  echo   PASS
) else (
  echo   FAIL
  set /a RET=1
)
echo(

echo TEST 4: MODE = D, GEN = 2
for /f %%A in ('tntpsp_keygen.exe d 2 13a60ab295a958a07d41870abd663f5562d9') do set "out=%%A"
if "%out%"=="2b3e295e109bbe0dd4b0bc39c90773e9" (
  echo   PASS
) else (
  echo   FAIL
  set /a RET=1
)
echo(

if "%RET%"=="0" (
  echo *** ALL TESTS PASSED ***
) else (
  echo *** ONE OR MORE TESTS FAILED ***
)
echo(

exit /b %RET%
