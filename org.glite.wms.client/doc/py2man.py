# CONFIGURATION VALUES:
"""
DEBUG values:
0 - no debug
1 - stream debug (all toghether)
2 - step debug
"""
RASKMAN_DEBUG = 0
# Detemrine which documentation to be generated:
GENERATE_MAN=True #MAN PAGES
GENERATE_TXT=True #LATEC
GENERATE_WKI=True #WIKI PAGES
GENERATE_PLT=False #Plain Text PAGES
#Output for generated pages PATH:
MAN_OUTPUT=""
TXT_OUTPUT="../../org.egee.jra1.deliverables/users-guide/WMPROXY/"
WKI_OUTPUT=""
PLT_OUTPUT=""
# OPTIONS and COMMANDS description files:
options2descriptionFile  = "options_description.txt"
commands2descriptionFile = "commands_description.txt"


options2short = { \
"all": ["all","",""] ,\
"autm-delegation": ["autm-delegation","a",""] ,\
"cancel-input": ["input","i","filepath"] ,\
"info-input": ["input","i","filepath"] ,\
"cancel-output": ["output","o","filepath"] ,\
"collection": ["collection","c","dirpath"] ,\
"config":["config","c","configfile"] ,\
"dag": ["dag","","dirpath"] ,\
"debug": ["debug","",""] ,\
"default-jdl": ["default-jdl","","filepath"] ,\
"delegationid": ["delegationid","d","idstring"] ,\
"deleg-output": ["output","o","filepath"] ,\
"endpoint": ["endpoint","e","serviceURL"] ,\
"filename": ["filename","f","filename"] ,\
"get": ["get","",""] ,\
"help": ["help","",""] ,\
"info-output" : ["output","o","filename"],\
"info-delegationid": ["delegationid","d","idstring"] ,\
"jobout-dir": ["dir","","directorypath"] ,\
"jobout-input": ["input","i","filepath"] ,\
"jdl":["jdl","","jobid"], \
"jdl-original":["jdl-original","j","jobid"], \
"list-only": ["list-only","",""] ,\
"logfile": ["logfile","","filepath"] ,\
"lrms": ["lrms","","lrmstype"] ,\
"match-output": ["output","o","filepath"] ,\
"nodes-resource": ["nodes-resource","","ceid"] ,\
"nodisplay": ["nodisplay","",""] ,\
"noint": ["noint","",""] ,\
"nolisten": ["nolisten","",""] ,\
"nomsg": ["nomsg","",""] ,\
"perusal-dir": ["dir","","directorypath"] ,\
"perusal-output": ["output","o","filepath"] ,\
"proto": ["proto","","protocol"] ,\
"proto-submit": ["proto","","protocol"] ,\
"proxy": ["proxy","p","jobid"] ,\
"rank": ["rank","",""] ,\
"register-only": ["register-only","",""] ,\
"resource": ["resource","r", "ceid"] ,\
"set": ["set","",""] ,\
"start": ["start","","jobid"] ,\
"submit-input": ["input","i","filepath"] ,\
"perusal-inputfile": ["input-file","","filepath"] ,\
"submit-output": ["output","o","filepath"] ,\
"to": ["to ","","[MM:DD:]hh:mm[:[CC]YY]"] ,\
"transfer-files": ["transfer-files","",""] ,\
"unset": ["unset","",""] ,\
"valid": ["valid","v","hh:mm"] ,\
"version": ["version","",""] ,\
"vo": ["vo","","voname"] ,\
}

common_options=[ "version", "help", "config", "vo", "debug", "logfile", "noint"]

commands2options={
"glite-wms-job-delegate-proxy":["delegationid", "autm-delegation", "endpoint", "deleg-output"], \
"glite-wms-job-submit":["delegationid", "autm-delegation","submit-input", "resource","nodes-resource",  "nolisten", "nomsg","lrms", "to", "valid", "register-only", \
   "transfer-files", "proto-submit", "start", "submit-output", "collection","dag","default-jdl"], \
"glite-wms-job-cancel":["cancel-input","cancel-output"], \
"glite-wms-job-list-match":["delegationid", "autm-delegation", "endpoint", "rank","match-output"], \
"glite-wms-job-output":["jobout-input", "jobout-dir", "proto", "list-only"], \
"glite-wms-job-perusal": ["perusal-inputfile","set", "get", "unset", "filename",\
   "all", "perusal-dir", "proto", "perusal-output", "nodisplay"],\
"glite-wms-job-info":["info-input","info-delegationid","proxy","jdl","jdl-original","endpoint","info-output"],\
}
