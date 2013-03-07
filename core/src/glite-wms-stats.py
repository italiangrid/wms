#!/usr/bin/env python

# Copyright (c) Members of the EGEE Collaboration. 2004. 
# See http://www.eu-egee.org/partners/ for details on the copyright
# holders.  
# 
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at 
# 
#     http://www.apache.org/licenses/LICENSE-2.0 
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.

"""
glite-wms-stats is a python scrypt who retrieves informations about a glite
based resource broker's status by querying the lbserver20 database,
the Condorg system and by parsing files.

Once Executed it returns to stdout a line wich sintax is as follow:
<key1>=<NumericValue1>:<key2>=<NumericValue2>: ...

Following is an example list of <key>=value returned by current
glite-wms-stats version.
Meaning is intended to be self explaining. For a more detailed
explanation refer to http://goc.grid.sinica.edu.tw/gocwiki/RB_Passive_Sensor.

WMS_Sensor_Version
CG_EndedJobs1H
CG_HeldJobs
CG_RunningJobs
CG_SubmittedJobs1H
CG_WaitingJobs
JC_InputFileListSize
JC_WaitingRequests
WMS_Jobs_Aborted
WMS_Jobs_Cancelled
WMS_Jobs_Cleared
WMS_Jobs_Done
WMS_Jobs_Purged
WMS_Jobs_Ready
WMS_Jobs_Running
WMS_Jobs_Scheduled
WMS_Jobs_Submitted
WMS_Jobs_Unknown
WMS_Jobs_Waiting
WMS_Jobs_Aborted1H
WMS_Jobs_Cancelled1H
WMS_Jobs_Cleared1H
WMS_Jobs_Done1H
WMS_Jobs_Purged1H
WMS_Jobs_Ready1H
WMS_Jobs_Running1H
WMS_Jobs_Scheduled1H
WMS_Jobs_Submitted1H
WMS_Jobs_Unknown1H
WMS_Jobs_Waiting1H
WMS_SandBox_InputSandBoxMaxSize
WMS_SandBox_InputSandBoxNumber
WMS_SandBox_InputSandBoxSizeTotal
WMS_SandBox_OutputSandBoxMaxSize
WMS_SandBox_OutputSandBoxNumber
WMS_SandBox_OutputSandBoxSizeTotal
WM_InputFileListSize
WM_WaitingRequests

"""

#per setacciare ini files 
#import ConfigParser 
"""legenda commenti:
#OK_0 la funzione esegue senza errori
#OK_1 verificato corretto comportamento in alcuni casi
#OK_2  verificato corretto comportamento in tutti i casi
#OK_3 gestione eccezioni implementata (try: except: etc)

#D: difetto
#Q: domanda
"""

#DEBUG=0

import os
import time


#OK_0
def WM_From_fl_file_get():
    """
    Gets a few data from input.fl or output.fl.
    Returns a 2 elements tuple:
    (WM.WaitingRequests, WM_InputFileListSize)
    Development status: OK_0
    """
    fname=basedir+'/workload_manager/input.fl'
    f=open(fname,'r')
    filesize=os.fstat(f.fileno())[6] 
    l=' '
    count=0
    while l:
        l=f.readlines()
        if len(l)>1 and l[-2]=='g':
            count += 1
    f.close()
    return (count,filesize)

def CreatePastDate(secs):
    """
create a date shifted of 'secs' 
seconds respect to 'now' compatible with mysql queries
Note that using positive 'secs' you'll get a 'future' time!
    """
    T=time.time()+time.timezone+secs
    tup=time.gmtime(T)
    #when="%d-%d-%d %d:%d:%d"%(tup[0],tup[1],tup[2],tup[3],tup[4],tup[5])
    when="%d-%d-%d %d:%d:%d"%tup[0:6]
    return when

def dirscan(dirpath):
    """
    returns in a tuple:
    [0] timestamp of <path>/. dir
    [1] sum of bytesize of files in directory.
    """
    f=os.popen('ls -la --full-time '+dirpath)
    f.readline() #skip the "total n" thing
    l=f.readline()
    t=l.split()
    when=t[5]+' '+t[6].split('.')[0] #creation time of dir
    size=0
    f.readline() #skip the '..' dir
    l=f.readline()
    while l:
        t=l.split()
        size+=int(t[4])
        l=f.readline()
    f.close()
    return (when,size)
        
class WMSps(object):
    """
WMS Passive sensor class
Retrieves and serves infos as specified in
http://goc.grid.sinica.edu.tw/gocwiki/RB_Passive_Sensor

    """
    def __init__(self):
        """
        x is the 'status' which is a dictionary
        {StatusNameKey:StatusValue}
        """
        self._esegui_all={'WM_From_fl_file_get':self.WM_From_fl_file_get,
              'WM_From_condorq':self.WM_From_condorq,
              'WMS_scan_SandBoxDir':self.WMS_scan_SandBoxDir,
              'WMS_LivingJobs':self.WMS_LivingJobs,
              'WMS_Jobs_LastHour':self.WMS_Jobs_LastHour
              }
        self._esegui={}
        self.__DEBUG=0
        self._env={}
        self._env['GLITE_WMS_TMP']=os.getenv('GLITE_WMS_TMP')
        self._env['GLITE_LOCATION_LOG']=os.getenv('GLITE_LOCATION_LOG')
        self._env['input.fl']=self._env['GLITE_WMS_TMP']+'/workload_manager/input.fl'
        self._env['output.fl']=self._env['GLITE_WMS_TMP']+'/jobcontrol/queue.fl'
        self._env['CONDORG_INSTALL_PATH']=os.getenv('CONDORG_INSTALL_PATH')+'/bin/'
        self._env['CONDOR_CONFIG']=os.getenv('CONDOR_CONFIG')
        self._version='0.9'
        self.x={}
        #self.outputstring=''
        self._timestamp=1        
        self.JobStates={'1':'Submitted',
                        '2':'Waiting',
                        '3':'Ready',
                        '4':'Scheduled',
                        '5':'Running',
                        '6':'Done',
                        '7':'Cleared',
                        '8':'Aborted',
                        '9':'Cancelled',
                        '10':'Unknown',
                        '11':'Purged'
                        }
        self._helpstring="""
                -h prints this help
                -v prints the version
                -plain plain text output

                -xml xml output - Not yet supported
                -sbox reads info from SandboxDir
                -condor gets info from condor_q
                -livejobs ask for jobs known by LB
                -lasth ask for jobs known by LB in last hour
                -flfile ask for jobs from input.fl and queue.fl
                """
        self._exectimes={}

    def dbgprint(self,msg):
        if self.__DEBUG:
            print msg

    def printhelp(self):
        print self._helpstring
        #self.outputstring+=self._helpstring

    def SetDebug(self):
        self.__DEBUG=1

    def printversion(self):
        self._timestamp=0
        #self.outputstring+="WMS_Sensor_Version=%s:"%self._version
        return "WMS_Sensor_Version=%s:"%self._version
        
    def flfileparse(self,fname):
        """
        Reads data from fname,
        which must be one of
        <path>/input.fl or <path>/output.fl
        Returns a 2-uple (count,filesize)
        """
        f=open(fname,'r')
        filesize=os.fstat(f.fileno())[6] 
        l=' '
        count=0
        while l:
            l=f.readline()
            if len(l)>1 and l[-2]=='g':
                count += 1
        f.close()
        return (count,filesize)
    
    def WM_From_fl_file_get(self):
        """
        Gets a few data from input.fl or output.fl.
        Returns a 2 elements tuple:
        (WM.WaitingRequests, WM_InputFileListSize)
        """
        (count,filesize)=self.flfileparse(self._env['input.fl'])
        self.x['WM_WaitingRequests']=count
        self.x['WM_InputFileListSize']=filesize
        self.dbgprint("(count,filesize)=%s"%str((count,filesize)))
        (count,filesize)=self.flfileparse(self._env['output.fl'])
        self.dbgprint("(count,filesize)=%s"%str((count,filesize)))
        self.x['JC_WaitingRequests']=count
        self.x['JC_InputFileListSize']=filesize

    def WM_From_condorq(self):
        self.x['CG_WaitingJobs']='0'
        self.x['CG_RunningJobs']='0'
        self.x['CG_HeldJobs']='0'
        f=os.popen(self._env['CONDORG_INSTALL_PATH']+"condor_q")
        ris=f.readlines()[-1].split(' ')
        f.close()
        n=len(ris)
        if n>0:
            self.x['CG_WaitingJobs']=ris[2]
        else:
            self.x['CG_WaitingJobs']='-1'
        if len(ris)>3:
            self.x['CG_RunningJobs']=ris[4]
        else:
            self.x['CG_RunningJobs']='-1'
        if len(ris)>5:
            self.x['CG_HeldJobs']=ris[6]
        else:
            self.x['CG_HeldJobs']='-1'
        """calcolo job sottomessi ultima ora, es:
        53 jobs; 4 idle, 17 running, 32 held"""
        f=os.popen(self._env['CONDORG_INSTALL_PATH']+'condor_q -constraint "QDate>(`date +%s`-3600)" | wc -l')
        ris=f.readline()
        f.close()
        """WARNING: weak assumption that exactly 4 header lines of `condor_q -constraint` output are descriptive only. safer would be counting lines by parsing them"""
        self.x['CG_SubmittedJobs1H']=str(int(ris)-4)
        f=os.popen(self._env['CONDORG_INSTALL_PATH']+'condor_history -constraint "QDate>(`date +%s`-3600)" | wc -l')
        ris=f.readline()
        f.close()
        """WARNING: weak assumption that exactly 2 header lines of `condor_q -constraint` output are descriptive only. safer would be counting lines by parsing them"""
        self.x['CG_EndedJobs1H']=str(int(ris)-2)

    def WMS_from_log(self):
        pass
        f=os.popen(self._env['CONDORG_INSTALL_PATH']+"condor_q")
        ris=f.readlines()[-1].split(' ')
        f.close()                                

    def WMS_LivingJobs(self):
        f=os.popen('echo "select status,count(*) from states group by status;" | mysql -u lbserver20 lbserver20')
        states={}
        for k in self.JobStates.values():
            states[k]=0
        l=f.readline() #via la prima linea
        l=f.readline()            
        while l:
            S=l.split()
            states[self.JobStates[S[0]]]=S[1]
            l=f.readline()
        self.x['WMS_Jobs']=states.copy()
        f.close()
        
    def WMS_Jobs_LastHour(self):
        self.dbgprint("sono WMS_Jobs_LastHour")
        query="""select states.status,count(*) from events,states where events.arrived>'%s' and states.jobid=events.jobid group by states.status;"""%CreatePastDate(-3600)
        cmd="""echo "%s" | mysql -u lbserver20 lbserver20"""%query
        f=os.popen(cmd)
        for k in self.JobStates.values():
            self.x['WMS_Jobs_'+k+'1H']=0
        l=f.readline() #via la prima linea
        l=f.readline()
        while l:
            S=l.split()
            self.x['WMS_Jobs_'+self.JobStates[S[0]]+'1H']=S[1]
            l=f.readline()
        f.close()
        
    def WMS_scan_SandBoxDir(self):
        """
        This will search for non empty sandboxes. It aims to retrieve these infos:
        OutputSandBoxNumber = how many in the system
        OutputSandBoxSizeTotal = the sum filesize in sandboxes
        OutputSandBoxMaxSize = the bigger
        InputSandBoxMaxSize = ...
        InputSandBoxNumber = ...
        InputSandBoxSizeTotal = ...
        """
#        f=os.popen("find self._env['GLITE_WMS_TMP']"+"/SandboxDir"+" -type d -maxdepth 3 -mindepth 3 -name output ! -empty -mmin -60 -print -exec ls -l {} \; ")
        d={'OutputSandBoxNumber':0,
           'OutputSandBoxMaxSize':0,
           'OutputSandBoxSizeTotal':0,
           'InputSandBoxNumber':0,
           'InputSandBoxMaxSize':0,
           'InputSandBoxSizeTotal':0
           }
        f=os.popen("find %s/SandboxDir "%self._env['GLITE_WMS_TMP'] + r"-type d -maxdepth 3 -mindepth 3 ! -empty \( -name input -o -name output \) -depth -print")
        l=f.readline()
        while l:
            dirdata=dirscan(l)
            if l[-7:-2]=='/inpu':
                d['InputSandBoxNumber'] += 1
                d['InputSandBoxSizeTotal'] += dirdata[1]
                if d['InputSandBoxMaxSize'] < dirdata[1]:
                    d['InputSandBoxMaxSize'] = dirdata[1]
            else:
                d['OutputSandBoxNumber'] += 1
                d['OutputSandBoxSizeTotal'] += dirdata[1]
                if d['OutputSandBoxMaxSize'] < dirdata[1]:
                    d['OutputSandBoxMaxSize'] = dirdata[1]
            l=f.readline()
        f.close()
        self.x['WMS_SandBox']=d.copy()
        
    def ExecuteAll_new(self):
        self._exectimes['TimestampStart']=T0=time.time()        
        for (k,v) in self._esegui.iteritems():
            T1=time.time()
            v()
            self._exectimes[k]=T1-T0
            T0=T1
        self._exectimes['TotalExecutionTime']=time.time()-self._exectimes['TimestampStart']
        self.PlainOutput()

    def AddToEsegui(self,metodo):
        if metodo == 'All':
            self._esegui=self._esegui_all.copy()
            return
        self._esegui[metodo]=eval('self.'+metodo)
        
    def PlainOutput(self):
        strout=self.printversion()
        self.dbgprint("-"*50+'\n'+str(self.x)+'\n'+"-"*50+'\n')
        if self._timestamp:
            strout+="%s=%d:%s=%d:"%('TimestampStart',int(round(self._exectimes['TimestampStart'])),'TotalExecutionTime',int(round(self._exectimes['TotalExecutionTime'])))
        L=self.x.keys()
        L.sort()
        for k in L:
            v=self.x[k]
            if v is not None and type(v) is dict:
                Lv=v.keys()
                Lv.sort()
                for kk in Lv:
                  if self.__DEBUG :
                    strout+=' , %s==>%s secs\n'%(k,self._exectimes[k])
                  strout+="%s_%s=%s:"%(k,kk,v[kk])
            else:
                if v is not None:
                    strout+="%s=%s:"%(k,v)
        print strout[:-1]

if __name__ == '__main__':
    import os,sys,time 
    W=WMSps()
    opts={'-h':(W.printhelp,''),
          '-v':(W.printversion,''),
          '-plain':(W.AddToEsegui,'All'),
          '-sbox':(W.AddToEsegui,'WMS_scan_SandBoxDir'),
          '-condor':(W.AddToEsegui,'WM_From_condorq'),
          '-livejobs':(W.AddToEsegui,'WMS_LivingJobs'),
          '-lasth':(W.AddToEsegui,'WMS_Jobs_LastHour'),
          '-flfile':(W.AddToEsegui,'WM_From_fl_file_get'),
          '-dbg':(W.SetDebug,'')
          }
    
    if len(sys.argv) == 1 or sys.argv.count('-h') > 0 :
        opts['-h'][0]()
    else:
        for opt in sys.argv[1:]:
            if not opts.has_key(opt):
                print "opzione '%s' sconosciuta"%opt
                opts['-h'][0]()
                exit
            else:
                if opts[opt][1]:
                    opts[opt][0](opts[opt][1])
                else:
                    opts[opt][0]()
                    exit
        W.ExecuteAll_new()
