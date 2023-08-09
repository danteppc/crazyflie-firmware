import os
import json
import pickle
from .input import *
from .default import *

PKL_NAME = "cache.pkl"
JSON_NAME = "info.json"

expDir = os.path.join(Meta.date, Meta.timestamp)
joinDir = lambda p: os.path.join(expDir, p)


def getFilledExperiment() -> Experiment:
	experiment = defaultExperiment()

	os.makedirs(Meta.date, exist_ok=True)

	first_time = True
	try:
		with open(PKL_NAME, "rb") as pklFile:
			print("using cache...")
			experiment = pickle.load(pklFile)
			first_time = False
			pklFile.close()
	except FileNotFoundError:
		print("using first time default configuration...")

	if first_time or click.confirm('run questionnaire?', default=True):
		generate_questionnaire(experiment)

	jsonData = json.dumps(experiment, indent=4, cls=DictJSONEncoder)
	print(jsonData)

	os.makedirs(Meta.date, exist_ok=True)
	os.makedirs(expDir)

	pklFile = open(PKL_NAME, "wb")
	pickle.dump(experiment, pklFile)
	pklFile.close()

	with open(joinDir(JSON_NAME), 'w') as jsonFile:
		jsonFile.write(jsonData)

	return experiment
