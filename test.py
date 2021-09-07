from os import listdir
from requests import get, session
from bs4 import BeautifulSoup
from subprocess import run, PIPE, STDOUT, TimeoutExpired
from sys import argv

# Change this to "true" when we start working on the cache
usingCache = False
stackMachineExe = ""

if len(argv) != 2:
	print("Please use the following command to run the test")
	print("\tpython3 test.py <stackMachineExe>")
	quit()
elif argv[1] not in listdir("."):
		print("stackMachineExe argument incorrect")
		quit()
else:
	stackMachineExe = argv[1]

def loadUserCreds():
	username = ""
	password = ""

	if "passwd" not in listdir("."):
		print("Please include a file named 'passwd' in this directory")
		print("It must be formatted like")
		print("\tusername <username> password <password>")
		quit()
	else:
		with open("./passwd", "r") as file:
			myList = file.read().split(" ")
			for index, item in enumerate(myList):
				if item == "username":
					username = myList[index+1]
				elif item == "password":
					password = myList[index+1]
			assert username != "", "Username field is empty or not included"
			assert password != "", "Password field is empty or not included"

	return username, password

def getProgramStatsFromWeb(response):
	response = BeautifulSoup(response.content, features="lxml")
	programStats = ""
	
	for tag in response.findAll('td'):
		tagText = tag.getText()
		if "Sample solution(s) say..." in tagText:
			programStats = tagText
	
	assert programStats != "", "Unable to find program stats on page"

	programStats = list(filter(lambda item: item != "" and item != "Sample solution(s) say...", [item.strip() for item in programStats.split("\n")]))
	for index, item in enumerate(programStats):
		if usingCache == False and item == "sample solution with fast-jeq and no cache":
			return [programStats[index+offset] for offset in range(1, 11) if "percentage" not in programStats[index+offset]]
		elif usingCache == True and item == "sample solution with fast-jeq and write-back cache":
			return [programStats[index+offset] for offset in range(1, 11) if "percentage" not in programStats[index+offset]]

def getProgramStatsFromExe(exe):
	# If the program is ending too soon change timeout to 15
	try:
		programStats = run(["./" + stackMachineExe, "exe/" + exe], stdout=PIPE, stderr=STDOUT, timeout=15).stdout.decode('utf-8').split("\n")
	except TimeoutExpired:
		return -2
	if "ERROR" in programStats:
		return -1
	return [item for item in programStats[-12:-1] if item != ""]

def compareProgramStats(webStats, exeStats):
	mismatches = []
	for webStat, exeStat in zip(webStats, exeStats):
		if "exit" not in webStat:
			stat, webValue = webStat.split()
			exeValue = exeStat.split()[1]
			if webValue != exeValue:
				mismatches.append([stat, webValue, exeValue])
		else:
			webValue = webStat[-2]
			exeValue = exeStat[-2]
			if webValue != exeValue:
				mismatches.append(["Exit", webValue, exeValue])
	return mismatches
			

exes = list(filter(lambda item: ".coolexe" in item, listdir("./exe")))
username, password = loadUserCreds()
session = session()
loginURL = "https://secure.csse.uwa.edu.au/run/coolc"
authPayload = {
	'trylogin' : 'y',
	'tryloginname'	: username,
	'trypassword'	: password
}

loginResponse = session.post(loginURL, data=authPayload)
loginResponse.raise_for_status()

length = len(exes)
errors = []
for counter, exe in enumerate(exes, start=1):
	webpage = session.get(f'https://secure.csse.uwa.edu.au/run/coolc?p=np&demo={exe.replace("exe", "")}')
	webpage.raise_for_status()
	webStats = getProgramStatsFromWeb(webpage)
	exeStats = getProgramStatsFromExe(exe)

	if exeStats == -1:
		print(f'[X] [{counter}/{length}] Program "{exe}" did not complete due to illegal instruction error\n')
		continue
		errors.append(exe)
	elif exeStats == -2:
		print(f'[X] {counter}/{length}] Program "{exe}" did not complete due to likely infinite loop\n')
		continue
		errors.append(exe)

	mismatches = compareProgramStats(webStats, exeStats)
	if mismatches != []:
		print(f'[X] [{counter}/{length}] Program "{exe}" completed with the following deviations:')
		errors.append(exe)
		for mismatch in mismatches:
			print(f'\t{mismatch[0]} expected {mismatch[1]}, received {mismatch[2]}')
	else:
		print(f'[✓] [{counter}/{length}] Program "{exe}" completed without error or deviation')
	print()

print("Erroneous Programs:")
for counter, exe in enumerate(errors, start=1):
	print(f'{counter}: {exe}')
