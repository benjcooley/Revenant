@echo off
copy %2%1 diftmp
diff %1 diftmp | more


