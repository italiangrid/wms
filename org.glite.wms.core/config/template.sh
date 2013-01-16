# Copyright (c) Members of the EGEE Collaboration. 2004.
# See http://project.eu-egee.org/partners for details on the
# copyright holders.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#!/bin/sh

trap 'fatal_error "Job has been terminated (got SIGXCPU)" "1" "OSB"' XCPU # parsed 'as is' by LM to cause a resubmit
trap 'fatal_error "Job has been terminated (got SIGQUIT)" "1" "OSB"' QUIT # parsed 'as is' by LM to cause a resubmit
trap 'fatal_error "Job has been terminated (got SIGINT)" "1" "OSB"' INT # parsed 'as is' by LM to cause a resubmit
trap 'fatal_error "Job has been terminated (got SIGTERM)" "1" "OSB"' TERM # parsed 'as is' by LM to cause a resubmit
#trap 'warning "Job has been signalled (got SIGUSR1)" "OSB"' SIGUSR1

# the bash builtin kill command is sometimes buggy with process groups
enable -n kill

##
## initializations
##

# explicitly addresses interoperability with OSG
if [ -r "${OSG_GRID}/setup.sh" ]; then
  source "${OSG_GRID}/setup.sh" &>/dev/null
fi

if [ -n "${__jobid}" ]; then
  export GLITE_WMS_JOBID="${__jobid}"
fi

export GLITE_WMS_SEQUENCE_CODE="`eval echo $1`"
shift

if [ -z "${GLITE_WMS_LOCATION}" ]; then
  export GLITE_WMS_LOCATION="${GLITE_LOCATION:-/opt/glite}"
fi

if [ -z "${EDG_WL_LOCATION}" ]; then
  export EDG_WL_LOCATION="${EDG_LOCATION:-/opt/edg}"
fi

lb_logevent=${GLITE_WMS_LOCATION}/bin/glite-lb-logevent
if [ ! -x "$lb_logevent" ]; then
  lb_logevent="${EDG_WL_LOCATION}/bin/edg-wl-logev"
fi

#lb_register_sandbox=

if [ "X${__input_base_url##*/}" != "X" ]; then
  __input_base_url="${__input_base_url}/"
fi

if [ "X${__output_base_url##*/}" != "X" ]; then
  __output_base_url="${__output_base_url}/"
fi

if [ -z "${GLITE_LOCAL_COPY_RETRY_COUNT_ISB}" ]; then
  if [ -z "${GLITE_LOCAL_COPY_RETRY_COUNT}" ]; then
    __copy_retry_count_isb=2
  else
    __copy_retry_count_isb=${GLITE_LOCAL_COPY_RETRY_COUNT}
  fi
else
  __copy_retry_count_isb=${GLITE_LOCAL_COPY_RETRY_COUNT_ISB}
fi

if [ -z "${GLITE_LOCAL_COPY_RETRY_COUNT_OSB}" ]; then
  if [ -z "${GLITE_LOCAL_COPY_RETRY_COUNT}" ]; then
    __copy_retry_count_osb=6
  else
    __copy_retry_count_osb=${GLITE_LOCAL_COPY_RETRY_COUNT}
  fi
else
  __copy_retry_count_osb=${GLITE_LOCAL_COPY_RETRY_COUNT_OSB}
fi
  
if [ -z "${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT_ISB}" ]; then
  if [ -z "${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT}" ]; then
    __copy_retry_first_wait_isb=60
  else
    __copy_retry_first_wait_isb=${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT}
  fi
else
  __copy_retry_first_wait_isb=${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT_ISB}
fi

if [ -z "${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT_OSB}" ]; then
  if [ -z "${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT}" ]; then
    __copy_retry_first_wait_osb=300
  else
    __copy_retry_first_wait_osb=${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT}
  fi
else
  __copy_retry_first_wait_osb=${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT_OSB}
fi

jw_host="`hostname -f`"
jw_newdir="${__jobid_to_filename}"
jw_maradona="${jw_newdir}.output"
jw_workdir="`pwd`"
LM_done_failed_reason=

##
# functions definitions
##

kill_with_children() { # 1 - parent PID
  if [ -n "$1" ]
  then
    local ppid=$1
    kill -STOP $ppid
    local kill_list=$ppid
    for i in `ps -ef | awk '$3 == '${ppid}' { print $2 }'`
    do
      kill_list="$kill_list $i"
    done
    kill -9 $kill_list
  else
    echo "kill_with_children() called with an empty pid"
  fi
}

do_transfer() # 1 - command, 2 - source, 3 - dest, 4 - std err, 5 - exit code file
{
  $1 "$2" "$3" 2>"$4"
  echo $? > "$5"
}

proxy_checker()
{
  while [ 1 -eq 1 ]; do
    time_left=`grid-proxy-info -timeleft 2>/dev/null || echo 0`
    if [ $time_left -lt 0 ]; then
      break;
    elif [ $time_left -gt 1000 ]; then
      time_left=1000 
    fi
    sleep $time_left
  done

  fatal_error "Job killed by the jobwrapper because of user proxy expiration" # parsed 'as is' by LM to cause an abort
}

jw_echo() # 1 - msg
{
  echo "$1"
  echo "$1" >> "${jw_maradona}"
}

do_log_event() # 1 - event, 2 - resultfile, 3 - sequencefile
{
  export TMP_SEQUENCE_CODE=`eval "$lb_logevent $1 2>/dev/null"`
  echo $? >$2
  echo "$TMP_SEQUENCE_CODE" >$3
}

log_event() # 1 - event, 2 - sequence code variable
{
  if [ -n "${__gatekeeper_hostname}" ]; then
    export GLITE_WMS_LOG_DESTINATION="${__gatekeeper_hostname}"
  fi
  export tmp_result_file=`mktemp -q tmp.XXXXXXXXXX`
  export tmp_sequence_file=`mktemp -q tmp.XXXXXXXXXX`
  do_log_event "$1" "$tmp_result_file" "$tmp_sequence_file" &
  log_watchdog=$!
  log_timeout=60
  while [ $log_timeout -gt 0 ];
  do
    if [ -z `ps -p $log_watchdog -o pid=` ]; then
      break;
    fi
    sleep 1
    let "log_timeout--"
  done
  if [ $log_timeout -le 0 ]; then
    jw_echo "Killing log watchdog (pid=$log_watchdog)..."
    kill_with_children $log_watchdog
    push_in_LM_done_reason "Hanging log"
  fi    
  log_result=`cat $tmp_result_file 2>/dev/null`
  if [ "$log_result" == "0" ]; then
    eval "export $2=`cat $tmp_sequence_file`"
  else
    # Try with the locallogger on the LB responsible for the job
    local match_index=`expr match "$GLITE_WMS_JOBID" '[[:alpha:]][[:alnum:]+.-]*://'`
    local scheme=${GLITE_WMS_JOBID:0:${match_index}}
    local remaining=${GLITE_WMS_JOBID:${#scheme}:${#GLITE_WMS_JOBID}-${#scheme}}
    local lb_hostname=${remaining:0:`expr match "$remaining" '[[:alnum:]_.~!$&()-]*'`}
    export GLITE_WMS_LOG_DESTINATION="$lb_hostname"
    do_log_event "$1" "$tmp_result_file" "$tmp_sequence_file" &
    log_watchdog=$!
    log_timeout=60
    while [ $log_timeout -gt 0 ];
    do
      if [ -z `ps -p $log_watchdog -o pid=` ]; then
        break;
      fi
      sleep 1
      let "log_timeout--"
    done
    if [ $log_timeout -le 0 ]; then
      jw_echo "Killing log watchdog (pid=$log_watchdog)..."
      kill_with_children $log_watchdog
      push_in_LM_done_reason "Hanging log"
    fi
    log_result=`cat $tmp_result_file 2>/dev/null`
    if [ "$log_result" == "0" ]; then
      eval "export $2=`cat $tmp_sequence_file`"
    fi  
  fi
}

log_bare_event() # 1 - event
{
  log_event "--jobid=\"$GLITE_WMS_JOBID\"\
    --source=LRMS\
    --sequence=\"$GLITE_WMS_SEQUENCE_CODE\"\
    --event=\"$1\"\
    --node=\"$jw_host\"" "GLITE_WMS_SEQUENCE_CODE"
}

log_user_event() # 1 - reason
{
  log_event "--jobid=\"$GLITE_WMS_JOBID\"\
    --source=LRMS\
    --sequence=\"$GLITE_WMS_SEQUENCE_CODE\"\
    --event=\"UserTag\"\
    --name=\"notice\"\
    --value=\"$1\"\
    --node=\"$jw_host\"" "GLITE_WMS_SEQUENCE_CODE"
}

log_done_ok() # 1 - exit code
{
  log_event "--jobid=\"$GLITE_WMS_JOBID\"\
    --source=LRMS\
    --sequence=\"$GLITE_WMS_SEQUENCE_CODE\"\
    --event=\"Done\"\
    --reason=\"$done_reason\"\
    --status_code=OK\
    --exit_code=\"$1\"" "GLITE_WMS_SEQUENCE_CODE"
}

log_done_failed() # 1 - exit code
{
  log_event "--jobid=\"$GLITE_WMS_JOBID\"\
    --source=LRMS\
    --sequence=\"$GLITE_WMS_SEQUENCE_CODE\"\
    --event=\"Done\"\
    --reason=\"$done_reason\"\
    --status_code=FAILED\
    --exit_code=\"$1\"" "GLITE_WMS_SEQUENCE_CODE"
}

push_in_LM_done_reason() #  1 - reason
{
  LM_done_failed_reason="$LM_done_failed_reason`date`: $1"$'\n'
}

log_resource_usage() # 1 - resource, 2 - quantity, 3 - unit
{
  log_event "--jobid=\"$GLITE_WMS_JOBID\"\
    --source=LRMS\
    --sequence=\"$GLITE_WMS_SEQUENCE_CODE\"\
    --event=ResourceUsage\
    --resource=\"$1\"\
    --quantity=\"$2\"\
    --unit=\"$3\"" "GLITE_WMS_SEQUENCE_CODE"
}

register_sandbox() # 1 - input/output, 2 - from, 3 - to
{
  eval `$lb_register_sandbox \
        --jobid "$GLITE_WMS_JOBID" \
        --"$1" \
        --from "$2" \
        --to "$3" \
        --sequence "$GLITE_WMS_SEQUENCE_CODE"`
}

log_ISB() # 1 result, 2 # reason(optional)
{
  log_event "--source LRMS \
        --jobid \"$GLITE_LB_ISB_JOBID\" \
        --sequence \"$GLITE_LB_ISB_SEQUENCE\" \
        --event FileTransfer \
        --result \"$1\"
        --reason \"$2\"" "GLITE_LB_ISB_SEQUENCE"
}

log_OSB() # 1 result 2 # reason(optional)
{
  log_event "--source LRMS \
        --jobid \"$GLITE_LB_OSB_JOBID\" \
        --sequence \"$GLITE_LB_OSB_SEQUENCE\" \
        --event FileTransfer \
        --result \"$1\"
        --reason \"$2\"" "GLITE_LB_ISB_SEQUENCE"
}

warning()
{
  local term_delay=10
  push_in_LM_done_reason "job received SIGUSR1 as warning, terminating in $term_delay seconds"
  kill -USR1 -$user_job_pid # forwarding to the user job (just in case)
  sleep $term_delay
  fatal_error "Job has been terminated $term_delay seconds after having being warned" "1" $2 # parsed 'as is' by LM to cause a resubmit
}

fatal_error() # 1 - reason, 2 - transfer OSB
{
  if [ "x${LM_done_failed_reason}" != "x" ]; then
    jw_echo "LM_log_done_begin" # parsed 'as is' by LM
    jw_echo "${LM_done_failed_reason}"
    jw_echo "LM_log_done_end" # parsed 'as is' by LM
  fi
  jw_echo "$1"
  log_done_failed 1 "$1"
  if [ "x$2" == "xOSB" ]; then
    OSB_transfer
  fi
  doExit 1
}

truncate() # 1 - file name, 2 - bytes num., 3 - name of the truncated file
{
  tail "$1" --bytes=$2>$3 2>/dev/null
  return $?
}

sort_by_size() # 1 - file names vector, 2 - directory
{
  local tmp_sort_file=`mktemp -q tmp.XXXXXXXXXX`
  if [ ! -f "$tmp_sort_file" ]; then
    push_in_LM_done_reason "Cannot generate temporary file"
    return $?
  fi
  eval tmpvar="$1[@]"
  eval elements="\${$tmpvar}"
  for fname in "${elements}"; do
    fsize=`stat -t $2/$fname 2>/dev/null | awk '{print $2}'`
    if [ -z "$fsize" ]; then
      fsize=0
    fi
    echo "$fsize $fname" >> "$tmp_sort_file"
  done
  unset "$1"
  eval "$1=(`sort -n $tmp_sort_file|awk '{print $2}'`)"
  rm -f "$tmp_sort_file"
}

is_integer() { # 1 - value to be checked
  if [ -z "${1//[0-9]/}" ] && [ -n "$1" ] ; then
    return 0
  else
    return 1
  fi
}

retry_copy() # 1 - source, 2 - dest
{
  local source=$1
  local dest=$2
  local count=0
  local succeded=1
  local sleep_time=0
  # If a space separated list of transports is specified in the _same_ vector element 
  # (i.e. the schemes correspond to the same transport client), then 
  # only the scheme specified by the caller is considered. If the caller doesn't specify it, 
  # only the first scheme in the space separated list is considered.
  local transport[0]="gsiftp"
  local transport[1]="https"
  local transport[2]="http"
  local transport_client[0]="globus-url-copy"
  local transport_client[1]="htcp"
  local transport_client[2]="htcp"
  for uberftp_cmd in $GLOBUS_LOCATION/bin/uberftp \
                            `which uberftp 2>/dev/null`; do
    if [ -x "${uberftp_cmd}" ]; then
      break;
    fi
  done
  if [ -n ${uberftp_cmd} ]; then
    local transport_client[3]="$uberftp_cmd"
    local transport[3]="gsiftp"
  fi

  local match_index=`expr match "${source}" '[[:alpha:]][[:alnum:]+.-]*://'`
  if [ ${match_index} -gt 0 ]; then
    match_index=`expr ${match_index} - 3`
  fi
  local scheme_src=${source:0:${match_index}}
  local remaining_src=${source:${#scheme_src}:${#source}-${#scheme_src}}

  local match_index=`expr match "${dest}" '[[:alpha:]][[:alnum:]+.-]*://'`
  if [ ${match_index} -gt 0 ]; then
    match_index=`expr ${match_index} - 3`
  fi
  local scheme_dest=${dest:0:${match_index}}
  local remaining_dest=${dest:${#scheme_dest}:${#dest}-${#scheme_dest}}

  if [ "x${scheme_src}" == "xfile" -o "x${scheme_src}" == "x" ]; then
    local scheme=${scheme_dest}
    local remaining=${remaining_dest}
    local remote="dest"
    __copy_retry_count=${__copy_retry_count_osb}
    __copy_retry_first_wait=${__copy_retry_first_wait_osb}
  elif [ "x${scheme_dest}" == "xfile" -o "x${scheme_dest}" == "x" ]; then
    local scheme=${scheme_src}   
    local remaining=${remaining_src}
    local remote="source"
    __copy_retry_count=${__copy_retry_count_isb}
    __copy_retry_first_wait=${__copy_retry_first_wait_isb}
  else 
    push_in_LM_done_reason "Expected 'file://' or no scheme in either source or destination"
    return 1
  fi    

  local ischeme=0
  while [ $ischeme -lt ${#transport[@]} ]; do
    if [ "x${transport[$ischeme]}" == "x${scheme}" ]; then
      break
    fi
  ischeme=`expr $ischeme + 1`
  done 
  # ischeme points to the transport specified in the remote resource (either source or dest)
  if [ ${ischeme} -eq ${#transport[@]} ]; then
    push_in_LM_done_reason "Specified transport protocol is not available"
    return 1
  fi 

  while [ $count -le ${__copy_retry_count} -a $succeded -ne 0 ];
  do
    time_left=`grid-proxy-info -timeleft 2>/dev/null || echo 0`;
    if [ $time_left -lt $sleep_time ]; then
      return 1
    fi
    sleep $sleep_time
    if [ $sleep_time -eq 0 ]; then
      sleep_time=${__copy_retry_first_wait}
    else
      sleep_time=`expr $sleep_time \* 2`
    fi
    transfer_stderr=`mktemp -q std_err.XXXXXXXXXX`
    if [ ! -f "$transfer_stderr" ]; then
      transfer_stderr="/dev/null"
    fi
    transfer_exitcode=`mktemp -q tr_exit_code.XXXXXXXXXX`
    if [ ! -f "$transfer_exitcode" ]; then
      transfer_exitcode="/dev/null"
    fi
    
    # cycle through the different transports starting from the one specified by dest (ischeme)
    i=0
    while [ $i -lt ${#transport[@]} ]; 
    do
      if [[ "$succeded" -eq 0 ]] || [[ "${__retry_different_transports}" == 0 ]] && [[ "$i" -gt 0 ]]; then
        break
      fi
      local index=$((($ischeme+$i)%${#transport[@]})) 

      if [ "x${remote}" == "xsource" ]; then 
        do_transfer "${transport_client[$index]}" "${transport[$index]}${remaining}" "${dest}" "$transfer_stderr" "$transfer_exitcode"&
      else
        do_transfer "${transport_client[$index]}" "${source}" "${transport[$index]}${remaining}" "$transfer_stderr" "$transfer_exitcode"&
      fi
      transfer_watchdog=$!
      transfer_timeout=3600
      while [ $transfer_timeout -gt 0 ];
      do
        if [ -z `ps -p $transfer_watchdog -o pid=` ]; then
          break;
        fi
        sleep 1
        let "transfer_timeout--"
      done
      if [ $transfer_timeout -le 0 ]; then
        echo "Killing transfer watchdog (pid=$transfer_watchdog)..."
        kill_with_children $transfer_watchdog
        push_in_LM_done_reason "Hanging transfer"
        succeded=1
      else
        succeded=`cat $transfer_exitcode 2>/dev/null`
        if [ -z $succeded ]; then
          push_in_LM_done_reason "Cannot retrieve return value for transfer"
          return 1 # will cause a fatal_error
        else
          if [ "$succeded" -ne "0" ]; then
            push_in_LM_done_reason "Error during transfer"
          fi
        fi
      fi
      rm -f "$transfer_stderr" "$transfer_exitcode"
      i=`expr $i + 1`
    done
    count=`expr $count + 1`
  done
  return ${succeded}
}

doExit() # 1 - status
{
  local jw_status=$1

  jw_echo "jw exit status = ${jw_status}" # parsed 'as is' by LM

  if [ -n "${jw_maradona}" ]; then
    if [ -r "${jw_maradona}" ]; then
      retry_copy "file://${jw_workdir}/${jw_maradona}" "${__maradona_url}"
      globus_copy_status=$?
    else
      push_in_LM_done_reason "jw_maradona not readable, so not sent"
      globus_copy_status=0
    fi
  else
    push_in_LM_done_reason "jw_maradona was found unset"
    globus_copy_status=0
  fi

  if [ -n "${jw_newdir}" ]; then
    rm -rf "../${jw_newdir}"
  fi

  if [ -n "$proxy_watchdog" ]
  then
    push_in_LM_done_reason "Killing proxy watchdog (pid=$proxy_watchdog)..."
    kill_with_children $proxy_watchdog 
  fi
  kill -9 -$user_job_pid 2>/dev/null

  # customization point #3
  if [ -n "${GLITE_LOCAL_CUSTOMIZATION_DIR}" ]; then
    if [ -r "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_3.sh" ]; then
      . "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_3.sh"
    fi
  fi

  if [ ${jw_status} -eq 0 ]; then
    exit ${globus_copy_status}
  else
    exit ${jw_status}
  fi
}

doDSUploadTmp()
{
  local filename="${__dsupload}"
  echo "#" >> "$filename.tmp"
  echo "# Autogenerated by JobWrapper!" >> "$filename.tmp"
  echo "#" >> "$filename.tmp"
  echo "# The file contains the results of the upload and registration" >> "$filename.tmp"
  echo "# process in the following format:" >> "$filename.tmp"
  echo "# <outputfile> <lfn|guid|Error>" >> "$filename.tmp"
  echo "" >> "$filename.tmp"
}

doDSUpload()
{
  local filename="${__dsupload}"
  mv -fv "$filename.tmp" "$filename"
}

doCheckReplicaFile()
{
  local sourcefile=$1
  local filename="${__dsupload}"
  local exit_status=0
  if [ ! -f "${jw_workdir}/$sourcefile" ]; then
    echo "$sourcefile    Error: File $sourcefile has not been found on the WN $jw_host" >> "$filename.tmp"
    exit_status=1
  fi
  echo >> "$filename.tmp"
  return $exit_status
}

doReplicaFile()
{
  local sourcefile=$1
  local filename="${__dsupload}"
  local exit_status=0
  local localf=`${edg_rm_command} --vo=${__vo} copyAndRegisterFile "file://${jw_workdir}/$sourcefile" 2>&1`
  result=$?
  if [ $result -eq 0 ]; then
    echo "$sourcefile    $localf" >> "$filename.tmp"
  else
    echo "$sourcefile    Error: $localf" >> "$filename.tmp"
    exit_status=1
  fi
  
  echo >> "$filename.tmp"
  return $exit_status
}

doReplicaFilewithLFN()
{
  local sourcefile="$1"
  local lfn="$2"
  local filename="${__dsupload}"
  local exit_status=0
  local localf=`${edg_rm_command} --vo=${__vo} copyAndRegisterFile "file://${jw_workdir}/$sourcefile" -l "$lfn" 2>&1`
  result=$?
  if [ $result -eq 0 ]; then
    echo "$sourcefile    $lfn" >> "$filename.tmp"
  else
    local localnew=`${edg_rm_command} --vo=${__vo} copyAndRegisterFile "file://${jw_workdir}/$sourcefile" 2>&1`
    result=$?
    if [ $result -eq 0 ]; then
      echo "$sourcefile $localnew" >> "$filename.tmp"
    else
      echo "$sourcefile Error: $localf; $localnew" >> "$filename.tmp"
      exit_status=1
    fi
  fi
  
  echo >> "$filename.tmp"
  return $exit_status
}

doReplicaFilewithSE()
{
  local sourcefile="$1"
  local se="$2"
  local filename="${__dsupload}"
  local exit_status=0
  local localf=`${edg_rm_command} --vo=${__vo} copyAndRegisterFile "file://${jw_workdir}/$sourcefile" -d "$se" 2>&1`
  result=$?
  if [ $result -eq 0 ]; then
    echo "$sourcefile  $localf" >> "$filename.tmp"
  else
    local localnew=`${edg_rm_command} --vo=${__vo} copyAndRegisterFile "file://${jw_workdir}/$sourcefile" 2>&1`
    result=$?
    if [ $result -eq 0 ]; then
      echo "$sourcefile $localnew" >> "$filename.tmp"
    else
      echo "$sourcefile Error: $localf; $localnew" >> "$filename.tmp"
      exit_status=1
    fi
  fi

  echo >> "$filename.tmp"
  return $exit_status
}

doReplicaFilewithLFNAndSE()
{
  local sourcefile="$1"
  local lfn="$2"
  local se="$3"
  local filename="${__dsupload}"
  local exit_status=0
  local localf=`${edg_rm_command} --vo=${__vo} copyAndRegisterFile "file://${jw_workdir}/$sourcefile" -l "$lfn" -d "$se" 2>&1`
  result=$?
  if [ $result -eq 0 ]; then
    echo "$sourcefile    $localf" >> "$filename.tmp"
  else
    local localse=`${edg_rm_command} --vo=${__vo} copyAndRegisterFile "file://${jw_workdir}/$sourcefile" -d "$se" 2>&1`
    result=$?
    if [ $result -eq 0 ]; then
      echo "$sourcefile    $localse" >> "$filename.tmp"
    else
      local locallfn=`${edg_rm_command} --vo=${__vo} copyAndRegisterFile "file://${jw_workdir}/$sourcefile" -l "$lfn" 2>&1`
      result=$?
      if [ $result -eq 0 ]; then 
        echo "$sourcefile    $locallfn" >> "$filename.tmp"
      else
        local localnew=`${edg_rm_command} --vo=${__vo} copyAndRegisterFile "file://${jw_workdir}/$sourcefile" 2>&1`
        result=$?
        if [ $result -eq 0 ]; then
          echo "$sourcefile    $localnew" >> "$filename.tmp"
        else
          echo "$sourcefile    Error: $localf; $localse; $locallfn; $localnew" >> "$filename.tmp"
          exit_status=1
        fi    
      fi
    fi
  fi
    
  echo >> "$filename.tmp"
  return $exit_status
}

function send_partial_file
{
  # Use local variables to avoid conflicts with main program
  local TRIGGERFILE=$1
  local DESTURL=$2
  local POLL_INTERVAL=$3
  local FILESUFFIX=partialtrf
  local GLOBUS_RETURN_CODE
  local SLICENAME
  local LISTFILE=`pwd`/listfile.$$
  local LAST_CYCLE=""
  local SLEEP_PID
  local MD5
  local OLDSIZE
  local NEWSIZE
  local COUNTER
  # Loop forever (need to be killed by main program)
  while [ -z "$LAST_CYCLE" ] ; do
    # Go to sleep, but be ready to wake up when the user job finishes
    sleep $POLL_INTERVAL & SLEEP_PID=$!
    trap 'LAST_CYCLE="y"; kill -ALRM $SLEEP_PID >/dev/null 2>&1' USR2
    wait $SLEEP_PID >/dev/null 2>&1
    # Retrieve the list of files to be monitored
    if [ "${TRIGGERFILE:0:9}" == "gsiftp://" ]; then
      globus-url-copy "${TRIGGERFILE}" "file://${LISTFILE}"
    elif [ "${TRIGGERFILE:0:8}" == "https://" -o "${TRIGGERFILE:0:7}" == "http://" ]; then
      htcp "${TRIGGERFILE}" "file://${LISTFILE}"
    else
      false
    fi
    # Skip iteration if unable to get the list
    # (can be used to switch off monitoring)
    if [ "$?" -ne "0" ] ; then
      continue
    fi
    for SRCFILE in `cat "$LISTFILE"` ; do
      # SRCFILE must contain the full path
      if [ "$SRCFILE" == "`basename $SRCFILE`" ] ; then
        SRCFILE=`pwd`/$SRCFILE
      fi
      if [ -f $SRCFILE ] ; then
        # Point to the "state" variables of the current file
        # (we will use indirect reference)
        MD5=`expr "$(echo $SRCFILE | md5sum)" : '\([^ ]*\).*'`
        OLDSIZE="OLDSIZE_$MD5"
        COUNTER="COUNTER_$MD5"
        # Initialize variables if referenced for the first time
        if [ -z "${!OLDSIZE}" ]; then eval local $OLDSIZE=0; fi
        if [ -z "${!COUNTER}" ]; then eval local $COUNTER=1; fi
        # Make a snapshot of the current file
        cp $SRCFILE ${SRCFILE}.${FILESUFFIX}
        NEWSIZE=`stat -c %s ${SRCFILE}.${FILESUFFIX}`
        if [ "${NEWSIZE}" -gt "${!OLDSIZE}" ] ; then
          let "DIFFSIZE = NEWSIZE - $OLDSIZE"
          SLICENAME=$SRCFILE.`date +%Y%m%d%H%M%S`_${!COUNTER}
          tail -c "$DIFFSIZE" "${SRCFILE}.${FILESUFFIX}" > "$SLICENAME"
          if [ "${DESTURL:0:9}" == "gsiftp://" ]; then
            globus-url-copy "file://$SLICENAME" "${DESTURL}/`basename $SLICENAME`"
          elif [ "${DESTURL:0:8}" == "https://" -o "${DESTURL:0:7}" == "http://" ]; then
            htcp "file://$SLICENAME" "${DESTURL}/`basename $SLICENAME`"
          else
            false
          fi
          GLOBUS_RETURN_CODE=$?
          rm -f "${SRCFILE}.${FILESUFFIX}" "$SLICENAME"
          if [ "$GLOBUS_RETURN_CODE" -eq "0" ] ; then
            let "$OLDSIZE = NEWSIZE"
            let "$COUNTER += 1"
          fi # else we will send this chunk toghether with the next one
        fi # else the file size didn't increase
      fi
    done
  done
  rm -f "$LISTFILE" # some cleanup
}

OSB_transfer()
{
  # uncomment this one below if the order in the osb originally 
  # specified is not of some relevance to the user
  #sort_by_size __output_file ${jw_workdir}
  # if [ $? != 0 ]; then
  # handle error
  # fi

  local file_size_acc=0
  local current_file=0
  local total_files=${#__wmp_output_dest_file[@]}
  local error
  for f in "${__wmp_output_dest_file[@]}"
  do
    if [ -r "${__wmp_output_file[$current_file]}" ]; then
      file=`basename $f`
      s="${jw_workdir}/${__wmp_output_file[$current_file]}"
      if [ ${__osb_wildcards_support} -eq 0 ]; then
        d="${f}"
      else
        d="${__output_sandbox_base_dest_uri}/${file}"
      fi
      local match_index=`expr match "${d}" '[[:alpha:]][[:alnum:]+.-]*://'`
      local scheme_dest=${d:0:${match_index}}
      local remaining_dest=${d:${#scheme_dest}:${#d}-${#scheme_dest}}
      local hostname=${remaining_dest:0:`expr match "$remaining_dest" '[[:alnum:]_.~!$&()-]*'`}

      if [[ ${max_osb_size} -ge 0 ]] && [[ $hostname == $__broker_hostname ]]; then
        # TODO
        #if hostname=wms
          file_size=`stat -t $s | awk '{print $2}'`
          if [ -z "$file_size" ]; then
            file_size=0
          fi
          file_size_acc=`expr $file_size_acc + $file_size`
        #fi
        if [[ $file_size_acc -le ${max_osb_size} ]]; then
          retry_copy "file://$s" "$d"
        else
          error="OSB quota exceeded for $s, truncating needed"
          log_user_event "$error"
          push_in_LM_done_reason "$error"
          file_size_acc=`expr $file_size_acc - $file_size`
          remaining_files=`expr $total_files \- $current_file`
          remaining_space=`expr $max_osb_size \- $file_size_acc`
          trunc_len=`expr $remaining_space / $remaining_files`
          if [ $? != 0 ]; then
            trunc_len=0
          fi
          file_size_acc=`expr $file_size_acc + $trunc_len`
            truncate "$s" $trunc_len "$s.tail"
            if [ $? != 0 ]; then
              error="Could not truncate output sandbox file ${file}, not sending"
              log_user_event "$error"
              push_in_LM_done_reason "$error"
            else
              error="Truncated last $trunc_len bytes for file ${file}"
              log_user_event "$error"
              push_in_LM_done_reason "$error"
              retry_copy "file://$s.tail" "$d.tail"
            fi
          #fi
        fi
      else # unlimited osb
        retry_copy "file://$s" "$d"
      fi
      if [ $? != 0 ]; then
        fatal_error "Cannot upload file://$s into $d" # parsed 'as is' by LM to cause a resubmit
      fi
    else
      error="Cannot read or missing file ${__wmp_output_file[$current_file]}"
      log_user_event "$error"
      push_in_LM_done_reason "$error"
    fi
    let "++current_file"
  done
}

##
## let's start it up
##

# customization point #1
# Be sure to update workdir as it may be changed by cp_1.sh
if [ -n "${GLITE_LOCAL_CUSTOMIZATION_DIR}" ]; then
  if [ -r "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_1.sh" ]; then
    . "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_1.sh"
  fi
fi
jw_workdir="`pwd`"

if [ ${__job_type} -eq 1 -o ${__job_type} -eq 2 ]; then
  # MPI (LSF or PBS)
  mkdir -p .mpi/${jw_newdir}
  if [ $? != 0 ]; then
    fatal_error "Cannot create directory .mpi/${jw_newdir}" # parsed 'as is' by LM to cause a resubmit
  fi
  cd .mpi/${jw_newdir}
else #if [ ${__job_type} -eq 0 -o ${__job_type} -eq 3 ]; then
  mkdir ${jw_newdir}
  if [ $? != 0 ]; then
    fatal_error "Cannot create directory ${jw_newdir}" # parsed 'as is' by LM to cause a resubmit
  fi
  cd ${jw_newdir}
fi
jw_workdir="`pwd`"

max_osb_size=${__max_outputsandbox_size}
is_integer ${GLITE_LOCAL_MAX_OSB_SIZE}
if [ $? -eq 0 ]; then
  if [ ${GLITE_LOCAL_MAX_OSB_SIZE} -lt $max_osb_size ]; then
    max_osb_size=${GLITE_LOCAL_MAX_OSB_SIZE}
  fi
fi

vo_hook="lcg-jobwrapper-hook.sh" # common-agreed now hard-coded
if [ -n "${__ce_application_dir}" ]; then
  if [ -d "${__ce_application_dir}" ]; then
    if [ -r "${__ce_application_dir}/${vo_hook}" ]; then
      . "${__ce_application_dir}/${vo_hook}"
    elif [ -r "${__ce_application_dir}/${__vo}/${vo_hook}" ]; then
      . "${__ce_application_dir}/${__vo}/${vo_hook}"
    else
      push_in_LM_done_reason "${vo_hook} not readable or not present"
    fi
  else
    push_in_LM_done_reason "${__ce_application_dir} not found or not a directory"
  fi
fi
unset vo_hook

# GLITE_WMS_RB_BROKERINFO must be defined after execution of cp_1.sh in case
# workdir (used as destination when downloading sandbox) has been updated
if [ -n "${__brokerinfo}" ]; then
  export GLITE_WMS_RB_BROKERINFO="$jw_workdir/${__brokerinfo}"
fi

# the test -w on work dir is unsuitable on AFS machines
tmpfile=`mktemp -q tmp.XXXXXXXXXX`
if [ ! -f "$tmpfile" ]; then
  fatal_error "Working directory not writable" # parsed 'as is' by LM to cause a resubmit
else
  rm "$tmpfile"
fi
unset tmpfile

touch "${jw_maradona}"

if [ -z "${GLOBUS_LOCATION}" ]; then
  fatal_error "GLOBUS_LOCATION undefined" # parsed 'as is' by LM to cause a resubmit
elif [ -r "${GLOBUS_LOCATION}/etc/globus-user-env.sh" ]; then
  . "${GLOBUS_LOCATION}/etc/globus-user-env.sh"
else
  fatal_error "${GLOBUS_LOCATION}/etc/globus-user-env.sh unreadable or not found" # parsed 'as is' by LM to cause a resubmit
fi

umask 022

# input sandbox download
for f in ${__wmp_input_base_file[@]}
do
  if [ -z "${__wmp_input_base_dest_file}" ]; then
    file=`basename ${f}`
  else
    file=`basename ${__wmp_input_base_dest_file[$index]}`
  fi
  retry_copy "${f}" "file://${jw_workdir}/${file}"
  if [ $? != 0 ]; then
    fatal_error "Cannot download ${file} from ${f}" # parsed 'as is' by LM to cause a resubmit
  fi
done

if [ -r "${__job}" ]; then
  chmod +x "${__job}" 2>/dev/null
else
  fatal_error "${__job} not found or unreadable" # parsed 'as is' by LM to cause an abort
fi

# user script (before taking the token, shallow-sensitive)
if [ -n "${__prologue}" ]; then
  if [ -r "${__prologue}" ]; then
    (
    for env in "${__environment[@]}"
    do
      eval export $env
    done
    chmod +x "${__prologue}" 2>/dev/null
    ${__prologue} "${__prologue_arguments}" >/dev/null 2>&1
    exit $?
    )
    prologue_status=$?
    if [ ${prologue_status} -ne 0 ]; then
      fatal_error "prologue failed with error ${prologue_status}" # parsed 'as is' by LM to cause an abort
    fi
  else
    fatal_error "prologue failed: ${__prologue} not found" # parsed 'as is' by LM to cause an abort
  fi
fi

if [ ${__perusal_support} -eq 1 ]; then
  send_partial_file ${__perusal_listfileuri} ${__perusal_filesdesturi} ${__perusal_timeinterval} & send_pid=$!
fi

if [ -n "${__shallow_resubmission_token}" ]; then

  # Look for an executable gridftp_rm command
  for gridftp_rm_command in $GLITE_LOCATION/bin/glite-gridftp-rm \
                            `which glite-gridftp-rm 2>/dev/null` \
                            $EDG_LOCATION/bin/edg-gridftp-rm \
                            `which edg-gridftp-rm 2>/dev/null` \
                            $GLOBUS_LOCATION/bin/uberftp \
                            `which uberftp 2>/dev/null`; do
    if [ -x "${gridftp_rm_command}" ]; then
      break;
    fi
  done

  if [ ! -x "${gridftp_rm_command}" ]; then
    fatal_error "No *ftp for rm command found" # parsed 'as is' by LM to cause a resubmit
  else
    is_uberftp=`expr match "${gridftp_rm_command}" '.*uberftp'`
    if [ $is_uberftp -eq 0 ]; then
      $gridftp_rm_command ${__shallow_resubmission_token} &>/dev/null
    else # uberftp
      tkn=${__shallow_resubmission_token} # will reduce lines length
      scheme=${tkn:0:`expr match "${tkn}" '[[:alpha:]][[:alnum:]+.-]*://'`}
      remaining=${tkn:${#scheme}:${#tkn}-${#scheme}}
      hostname=${remaining:0:`expr match "$remaining" '[[:alnum:]_.~!$&()-]*'`}
      token_fullpath=${remaining:${#hostname}:${#remaining}-${#hostname}}
      $gridftp_rm_command $hostname "quote dele ${token_fullpath}" &>/dev/null
    fi
    result=$?
    if [ $result -eq 0 ]; then
      log_bare_event "Running"
      push_in_LM_done_reason "Taken token ${__shallow_resubmission_token}"
    else
      fatal_error "Cannot take shallow resubmission token ${__shallow_resubmission_token}" # parsed 'as is' by LM to cause a resubmit
    fi
  fi
fi

jw_echo "Sequence code: ${GLITE_WMS_SEQUENCE_CODE}" # parsed 'as is' by LM

if [ ${__job_type} -eq 1 ]; then # MPI LSF
  hostfile="host$$"
  # touch $hostfile
  for mpi_host in "${LSB_HOSTS}"
    do echo "$mpi_host" >> $hostfile
  done
elif [ ${__job_type} -eq 2 ]; then
  hostfile="${PBS_NODEFILE}"
fi
if [ ${__job_type} -eq 1 -o ${__job_type} -eq 2 ]; then # MPI LSF, PBS
  for i in `cat "$hostfile"`; do
    ssh ${i} mkdir -p `pwd`
    /usr/bin/scp -rp ./* "${i}:`pwd`"
    ssh ${i} chmod 755 `pwd`/${__job}
  done
fi

if [ 1 -eq 1 ]; then # dump variable to be set?
  time_cmd=`which time 2>/dev/null`
  if [ -x "$time_cmd" ]; then
    time_cmd="$time_cmd -p"
    tmp_time_file=`mktemp -q tmp.XXXXXXXXXX`
    if [ $? -ne 0 ]; then
      push_in_LM_done_reason "Cannot generate temporary file"
      unset tmp_time_file 
    fi
  else
    push_in_LM_done_reason "Cannot find 'time' command"
  fi
fi

if [ ${__job_type} -eq 0 ]; then # normal
  executable="${__job}"
elif [ ${__job_type} -eq 1 -o ${__job_type} -eq 2 ]; then # MPI LSF, PBS
  executable="mpirun"
  __arguments="-np ${__nodes} -machinefile ${hostfile} ${__job} ${__arguments}"
fi

if [ ${__job_type} -ne 3 ]; then # all but interactive
  if [ -n "${__standard_input}" ]; then
    std_input=" <\"${__standard_input}\" "
  fi
  if [ -n "${__standard_output}" ]; then
    std_output=" >\"${__standard_output}\" "
  else
    std_output=" >/dev/null "
  fi
  if [ -n "${__standard_error}" ]; then
    std_error=" 2>\"${__standard_error}\" "
  else
    std_error=" 2>/dev/null "
  fi
fi

log_bare_event "ReallyRunning"
(
  for env in "${__environment[@]}"
  do
    eval export $env
  done

  if [ -f "$tmp_time_file" ]; then
    full_cmd_line="($time_cmd -o \"$tmp_time_file\" \"${executable}\" ${__arguments} ${std_input} ${std_output} ${std_error})"
  else
    full_cmd_line="\"${executable}\" ${__arguments} ${std_input} ${std_output} ${std_error}"
  fi

  if [ -z "$EDG_WL_NOSETPGRP" ]; then
    trap 'IGNORING' TTIN
    trap 'IGNORING' TTOU
  fi

  # finally getting to the core business
  set -m
  eval "${full_cmd_line}" &
  user_job_pid=$!
  set +m

  # customization point
  if [ -n "${GLITE_LOCAL_CUSTOMIZATION_DIR}" ]; then
    if [ -r "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_1_5.sh" ]; then
      . "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_1_5.sh"
    fi
  fi

  proxy_checker &
  proxy_watchdog=$!

  wait $user_job_pid
  user_job_status=$?
  echo "Killing proxy watchdog (pid=$proxy_watchdog)..."
  kill_with_children $proxy_watchdog
  exit $user_job_status
)
user_job_status=$?
jw_echo "job exit status = ${user_job_status}" # parsed 'as is' by LM

# reports the time usage
if [ -f "$tmp_time_file" -a -n "$time_cmd" ]; then
  log_resource_usage "real" "`grep real $tmp_time_file | cut -d' ' -f 2`" "s"
  log_resource_usage "user" "`grep user $tmp_time_file | cut -d' ' -f 2`" "s"
  log_resource_usage "sys" "`grep sys $tmp_time_file | cut -d' ' -f 2`" "s"
  rm -f "$tmp_time_file"
fi 

# customization point #2
if [ -n "${GLITE_LOCAL_CUSTOMIZATION_DIR}" ]; then
  if [ -r "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_2.sh" ]; then
    . "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_2.sh"
  fi
fi

if [ ${__perusal_support} -eq 1 ]; then
  kill -USR2 $send_pid
  wait $send_pid 
fi

if [ ${__output_data} -eq 1 ]; then
  return_value=0
  if [ $user_job_status -eq 0 ]; then
    local=`doDSUploadTmp`
    status=$?
    return_value=$status
    local_cnt=0
    for edg_rm_command in "${GLITE_LOCATION}/bin/lcg-replica-manager" \
                          "/opt/lcg/bin/lcg-replica-manager" \
                          "${LCG_LOCATION}/bin/lcg-replica-manager" \
                          "/opt/lcg/bin/lcg-replica-manager" \
                          "${GLITE_LOCATION}/bin/edg-rm" \
                          "${EDG_LOCATION}/bin/edg-rm" \
                          "`which lcg-replica-manager 2>/dev/null`" \
                          "`which edg-rm 2>/dev/null`"; do
      if [ -x "${edg_rm_command}" ]; then
        break;
      fi
    done
    if [ ! -x "${edg_rm_command}" ]; then
      fatal_error "Cannot find edg-rm command" # parsed 'as is' by LM to cause a resubmit
    else
      for outputfile in ${__output_file[@]}
      do
        local=`doCheckReplicaFile ${__output_file}`
        status=$?
        if [ $status -ne 0 ]; then
          return_value=1
        else
          if [ -z "${__output_lfn}" -a -z "${__output_se}"] ; then
            local=`doReplicaFile $outputfile`
          elif [ -n "${__output_lfn}" -a -z "${__output_se}"] ; then
            local=`doReplicaFilewithLFN $outputfile ${__output_lfn[$local_cnt]}`
          elif [ -z "${__output_lfn}" -a -n "${__output_se}"] ; then
            local=`doReplicaFilewithSE $outputfile ${__output_se[$local_cnt]}`
          else
            local=`doReplicaFilewithLFNAndSE $outputfile ${__output_lfn[$local_cnt]} ${__output_se[$local_cnt]}`
          fi
          status=$?
        fi
        let "++local_cnt"
      done
      local=`doDSUpload`
      status=$?
    fi
  fi
fi

if [ -n "${__epilogue}" ]; then
  if [ -r "${__epilogue}" ]; then
    (
    for env in "${__environment[@]}"
    do
      eval export $env
    done
    chmod +x "${__epilogue}" 2>/dev/null
    ${__epilogue} "${__epilogue_arguments}" >/dev/null 2>&1
    exit $?
    )
    epilogue_status=$?
    if [ ${epilogue_status} -ne 0 ]; then
      fatal_error "epilogue failed with error ${epilogue_status}" # parsed 'as is' by LM to cause an abort
    fi
  else
    fatal_error "epilogue failed: ${__epilogue} not found" # parsed 'as is' by LM to cause an abort
  fi
fi

OSB_transfer

log_done_ok "${user_job_status}"

# DGAS specific stuff: do it only if DGAS client is installed (thanks to Michel Jouvin)
if [ -x ${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient ]; then
  if [ -n "${LSB_JOBID}" ]; then
    cat "${X509_USER_PROXY}" | ${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient -s ${__gatekeeper_hostname}:56569: -L lsf_${LSB_JOBID} -G ${GLITE_WMS_JOBID} -C ${__globus_resource_contact_string} -H "$HLR_LOCATION"
    if [ $? != 0 ]; then
    push_in_LM_done_reason "Error transferring gianduia with command: cat ${X509_USER_PROXY} | ${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient -s ${__gatekeeper_hostname}:56569: -L lsf_${LSB_JOBID} -G ${GLITE_WMS_JOBID} -C ${__globus_resource_contact_string} -H $HLR_LOCATION"
    fi
  fi

  if [ -n "${PBS_JOBID}" ]; then
    cat ${X509_USER_PROXY} | ${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient -s ${__gatekeeper_hostname}:56569: -L pbs_${PBS_JOBID} -G ${GLITE_WMS_JOBID} -C ${__globus_resource_contact_string} -H "$HLR_LOCATION"
    if [ $? != 0 ]; then
      push_in_LM_done_reason "Error transferring gianduia with command: cat ${X509_USER_PROXY} | ${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient -s ${__gatekeeper_hostname}:56569: -L pbs_${PBS_JOBID} -G ${GLITE_WMS_JOBID} -C ${__globus_resource_contact_string} -H $HLR_LOCATION"
    fi
  fi
else
  push_in_LM_done_reason "${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient not installed: ignoring gianduia transfer"
fi

doExit 0
