import sys, os

app_dict = { "magic": "magic",
			 'bqb-addlogo': 'bqb-addlogo',
			 "blink": 'picMaskZoom',
			 "monoC" : "monoC-matting",
			 "find-logo" : "find-logo",
			 "od-app" : "od-app", 
			 "Implant" : "Implant", 
			 "iqa" : "IQA",
			"vs_ppt" : "vs_ppt",
			"v-sfp" : "v-sfp"
}

basedir = ""

def makeApp(app):
	print("start build : " + app)
	os.chdir(basedir + '/' + app_dict[app])
	os.system("make clean")
	os.system("make")
	os.system("make install")
	print(app + " build done\n")

if __name__=="__main__":
	basedir = os.getcwd()
	print(basedir)
	apptype = sys.argv[1]
	if (apptype=="all"):
		for key in app_dict:
			makeApp(key)
	else:
		val = app_dict.get(apptype, None)
		if (val==None):
			print("not support : ", apptype)
		makeApp(apptype)
