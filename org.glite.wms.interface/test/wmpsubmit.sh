#! /bin/bash

######
#	Copyright (c) Members of the EGEE Collaboration. 2004.
#	See http://www.eu-egee.org/partners/ for details on the copyright holders.
#
#	Licensed under the Apache License, Version 2.0 (the "License");
#	you may not use this file except in compliance with the License.
#	You may obtain a copy of the License at
#
#	    http://www.apache.org/licenses/LICENSE-2.0
#
#	Unless required by applicable law or agreed to in writing, software
#	distributed under the License is distributed on an "AS IS" BASIS,
#	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#	See the License for the specific language governing permissions and
#	limitations under the License.
#########

#
# Author: Giuseppe Avellino <giuseppe.avellino@datamat.it>
#
# This script file requires WMProxy client to be installed in order to submit jobs via WMProxy server
# Before running the script, a proxy MUST be delegated, and the used delegation id MUST be equal to
# DELEGATIONID value (or viceversa ;-) see below)
# JDLDIR MUST be the directory where JDL file to submit are stored (10 files are possible - it is a really simple script file!)
# CONFFILE is the conf file used by the client (for instance is used to get the WMProxy endpoint to contact)
# FILE0 - FILE9 are the names of the 10 files in JDLDIR
#
# If a JDL file name is provided, this file will be submitted <iteration number> times
#

# JDL to submit directory name
DELEGATIONID=deleg
JDLDIR=jdltosubmit
CONFFILE=wmpendpoint.conf

# File names chosen in a randomic way
FILE0=$JDLDIR/collection.jdl
FILE1=$JDLDIR/collection_isb.jdl
FILE2=$JDLDIR/collection_usertag.jdl
FILE3=$JDLDIR/normal.jdl
FILE4=$JDLDIR/normal_isb.jdl
FILE5=$JDLDIR/normal_usertag.jdl
FILE6=$JDLDIR/parametric.jdl
FILE7=$JDLDIR/parametric_isb.jdl
FILE8=$JDLDIR/parametric_usertag.jdl
FILE9=$JDLDIR/normal_perusal.jdl

echo "Command line arguments: $#"

if (( $#<1 ))
then
  echo "Please provide at least the iteration number"
  echo "Usage: wmpsubmit.sh <iteration number> [<jdl file name>]"
  exit -1
fi

echo "Submitting job to Workload Manager Proxy..."
echo "Today is `date`"

if [ -z "$2" ]
then
	RANGE=10
	for ((  i = 0 ;  i < $1;  i++  ))
	do
		number=$RANDOM
		let "number %= $RANGE"
		echo "Generated number: $number"
		case $number in
			0) echo "Submitting $FILE0 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE0 &;;
			1) echo "Submitting $FILE1 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE1 &;;
			2) echo "Submitting $FILE2 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE2 &;;
			3) echo "Submitting $FILE3 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE3 &;;
			4) echo "Submitting $FILE4 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE4 &;;
			5) echo "Submitting $FILE5 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE5 &;;
			6) echo "Submitting $FILE6 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE6 &;;
			7) echo "Submitting $FILE7 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE7 &;;
			8) echo "Submitting $FILE8 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE8 &;;
			9) echo "Submitting $FILE9 ..."
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $FILE9 &;;
			*) echo "Ops, something bad has happened :-) !!"
			exit -1;;
		esac
	done
else
	if [ -f "$2" ]
	then
		for ((  i = 0 ;  i < $1;  i++  ))
		do
			glite-wms-job-submit -d $DELEGATIONID -c $CONFFILE $2 &
		done
	else
		echo "File not found: $2"
		exit -1
	fi
fi
exit 0
