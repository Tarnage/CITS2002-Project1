Here's a python script to test for you if all your exes are running the same as on the site. Some notes on usage:
- Must be run with 
python3 test.py <yourStackMachineExe>
- Must be in the same directory as your stack machine exe
- In the same directory there also should be a passwd file with the single line
password <yourPassword> username <yourUsername>
- It looks for .coolexe files in ./exe/ (so in the same directory as the python script and your exe should be a folder called exe with all your .coolexes in it)
- If you are onto doing the cache then you need to set usingCache to True