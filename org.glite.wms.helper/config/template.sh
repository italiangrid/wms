#!/bin/sh

trap 'fatal_error "Job has been terminated by the batch system"' TERM

jw_echo() # 1 - msg
{
  echo "$1"
  echo "$1" >> "${maradona}"
}

log_event() # 1 - event
{
  GLITE_WMS_SEQUENCE_CODE=`$lb_logevent\
    --jobid="$GLITE_WMS_JOBID"\
    --source=LRMS\
    --sequence="$GLITE_WMS_SEQUENCE_CODE"\
    --event="$1"\
    --node=$host\
  || echo $GLITE_WMS_SEQUENCE_CODE`
}

fatal_error() # 1 - reason
{
  jw_echo "$1"

  GLITE_WMS_SEQUENCE_CODE=`$lb_logevent\
    --jobid="$GLITE_WMS_JOBID"\
    --source=LRMS\
    --sequence="$GLITE_WMS_SEQUENCE_CODE"\
    --event="Done"\
    --reason="$1"\
    --status_code=FAILED\
    --exit_code=0\
  || echo $GLITE_WMS_SEQUENCE_CODE`

  doExit 1
}

truncate() # 1 - file name, 2 - bytes num., 3 - name of the truncated file
{
  tail "$1" --bytes=$2>$3 2>/dev/null
  return $?
}

sort_by_size() # 1 - file names vector, 2 - directory
{
  tmp_sort_file=`mktemp -q tmp.XXXXXX`
  if [ $? != 0 ]; then
    jw_echo "Cannot generate temporary file"
    unset tmp_sort_file
    return $?
  fi
  eval tmpvar="$1[@]"
  eval elements="\${$tmpvar}"
  for fname in "${elements}"; do
    fsize=`stat -t $2/$fname 2>/dev/null | awk '{print $2}'`||0
    echo "$fsize $fname" >> "$tmp_sort_file"
  done
  unset $1
  eval "$1=(`sort -n $tmp_sort_file|awk '{print $2}'`)"
  rm -f "$tmp_sort_file"
  unset tmp_sort_file
}

retry_copy() # 1 - command, 2 - source, 3 - dest
{
  count=0
  succeded=1
  sleep_time=0
  while [ ${count} -le ${__copy_retry_count} -a ${succeded} -ne 0 ];
  do
    time_left=`grid-proxy-info -timeleft 2> /dev/null` || echo 0;
    if [ $time_left -lt $sleep_time ]; then
      return 1
    fi
    sleep "$sleep_time"
    if [ $sleep_time -eq 0 ]; then
      sleep_time=${__copy_retry_first_wait}
    else
      sleep_time=`expr $sleep_time \* 2`
    fi
    $1 "$2" "$3" 2>std_err
    succeded=$?
    if [ $succeded != 0 ]; then
      log_event "`cat std_err`"
    fi
    rm -f std_err
    count=`expr $count + 1`
  done
  return ${succeded}
}

doExit() # 1 - status
{
  jw_status=$1
  jw_echo "jw exit status = ${jw_status}"

  retry_copy "globus-url-copy" "file://${workdir}/${maradona}" "${__maradonaprotocol}"
  globus_copy_status=$?

  cd ..
  rm -rf "${newdir}"

  if [ ${jw_status} -eq 0 ]; then
    exit ${globus_copy_status}
  else
    exit ${jw_status}
  fi
}

doDSUploadTmp() # 1 - filename.tmp
{
  echo "#" >> "$1"
  echo "# Autogenerated by JobWrapper!" >> "$1"
  echo "#" >> "$1"
  echo "# The file contains the results of the upload and registration" >> "$1"
  echo "# process in the following format:" >> "$1"
  echo "# <outputfile> <lfn|guid|Error>" >> "$1"
  echo >> "$1"
}

doDSUpload() # 1 - filename
{
  mv -fv "$1.tmp" "$1"
}

doCheckReplicaFile() # 1 - source file, 2 - filename.tmp
{
  exit_status=0
  if [ ! -f "${workdir}/$1" ]; then
    echo "$1    Error: File $1 has not been found on the WN $host" >> "$2"
    exit_status=1
  fi
  echo >> "$2"
  return ${exit_status}
}

doReplicaFile() # 1 - source file, 2 - filename.tmp
{
  exit_status=0
  local=`$GLITE_WMS_LOCATION/bin/edg-rm --vo=${__vo} copyAndRegisterFile "file://${workdir}/$1" 2>&1`
  result=$?
  if [ $result -eq 0 ]; then
    echo "$1    $local" >> "$2"
  else
    echo "$1    Error: $local" >> "$2"
    exit_status=1
  fi

  echo >> "$2"
  return ${exit_status}
}

doReplicaFilewithLFN() # 1 - source file, 2 - lfn, 3 - filename.tmp
{
  exit_status=0
  local=`$GLITE_WMS_LOCATION/bin/edg-rm --vo=${__vo} copyAndRegisterFile "file://${workdir}/$1" -l "$2" 2>&1`
  result=$?
  if [ $result -eq 0 ]; then
    echo "$1    $2" >> "$3"
  else
    localnew=`$GLITE_WMS_LOCATION/bin/edg-rm --vo=${__vo} copyAndRegisterFile "file://${workdir}/$1" 2>&1`
    result=$?
    if [ $result -eq 0 ]; then
      echo "$1 $localnew" >> "$3"
    else
      echo "$1 Error: $local; $localnew" >> "$3"
      exit_status=1
    fi
  fi

  echo >> "$3"
  return ${exit_status}
}

doReplicaFilewithSE() # 1 - source file, 2 - se, 3 - filename.tmp
{
  exit_status=0
  local=`$GLITE_WMS_LOCATION/bin/edg-rm --vo=${__vo} copyAndRegisterFile "file://${workdir}/$1" -d "$2" 2>&1`
  result=$?
  if [ $result -eq 0 ]; then
    echo "$1   $local" >> "$3"
  else
    localnew=`$GLITE_WMS_LOCATION/bin/edg-rm --vo=${__vo} copyAndRegisterFile "file://${workdir}/$1" 2>&1`
    result=$?
    if [ $result -eq 0 ]; then
      echo "$1 $localnew" >> "$3"
    else
      echo "$1 Error: $local; $localnew" >> "$3"
      exit_status=1
    fi
  fi

  echo >> "$3"
  return ${exit_status}
}

doReplicaFilewithLFNAndSE() # 1 - source file, 2 - lfn, 3 - se, 4 - filename.tmp
{
  exit_status=0
  local=`$GLITE_WMS_LOCATION/bin/edg-rm --vo=${__vo} copyAndRegisterFile "file://${workdir}/$1" -l "$2" -d "$3" 2>&1`
  result=$?
  if [ $result -eq 0 ]; then
    echo "$1    $2" >> "$4"
  else
    localse=`$GLITE_WMS_LOCATION/bin/edg-rm --vo=${__vo} copyAndRegisterFile "file://${workdir}/$1" -d "$3" 2>&1`
    result=$?
    if [ $result -eq 0 ]; then
      echo "$1    $localse" >> "$4"
    else
      locallfn=`$GLITE_WMS_LOCATION/bin/edg-rm --vo=${__vo} copyAndRegisterFile "file://${workdir}/$1" -l "$2" 2>&1`
      result=$?
      if [ $result -eq 0 ]; then
        echo "$1    $locallfn" >> "$4"
      else
        localnew=`$GLITE_WMS_LOCATION/bin/edg-rm --vo=${__vo} copyAndRegisterFile "file://${workdir}/$1" 2>&1`
        result=$?
        if [ $result -eq 0 ]; then
          echo "$1    $localnew" >> "$4"
        else
          echo "$1    Error: $local; $localse; $locallfn; $localnew" >> "$4"
          exit_status=1
        fi
      fi
    fi
  fi

  echo >> "$4"
  return ${exit_status}
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
    if [ -r "${TRIGGERFILE}" ]; then
      if [ "${TRIGGERFILE:0:9}" == "gsiftp://" ]; then
        retry_copy "globus-url-copy" ${TRIGGERFILE} file://${LISTFILE}
      elif [ "${TRIGGERFILE:0:8}" == "https://" -o "${TRIGGERFILE:0:7}" == "http://" ]; then
        retry_copy "htcp" "${f}" "file://${workdir}/${file}"
      else
        false
      fi
    fi
    # Skip iteration if unable to get the list
    # (can be used to switch off monitoring)
    if [ "$?" -ne "0" ] ; then
      continue
    fi
    for SRCFILE in `cat $LISTFILE` ; do
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
          if [ -r "${DESTURL}" ]; then
            if [ "${DESTURL:0:9}" == "gsiftp://" ]; then
              retry_copy "globus-url-copy" "file://$SLICENAME" "${DESTURL}/`basename $SLICENAME`"
            elif [ "${DESTURL:0:8}" == "https://" -o "${DESTURL:0:7}" == "http://" ]; then
              retry_copy "htcp" "file://$SLICENAME" "${DESTURL}/`basename $SLICENAME`"
            else
              false
            fi
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

if [ -n "${__gatekeeper_hostname}" ]; then
  export GLITE_WMS_LOG_DESTINATION="${__gatekeeper_hostname}"
fi

if [ -n "${__jobid}" ]; then
  export GLITE_WMS_JOBID="${__jobid}"
fi

export GLITE_WMS_SEQUENCE_CODE="$1"
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

host=`hostname -f`

log_event "Running"

if [ "${__input_base_url:-1}" != "/" ]; then
  __input_base_url="${__input_base_url}/"
fi

if [ "${__output_base_url:-1}" != "/" ]; then
  __output_base_url="${__output_base_url}/"
fi

if [ -z "${GLITE_LOCAL_COPY_RETRY_COUNT}" ]; then
  __copy_retry_count=6
else
  __copy_retry_count=${GLITE_LOCAL_COPY_RETRY_COUNT}
fi

if [ -z "${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT}" ]; then
  __copy_retry_first_wait=300
else
  __copy_retry_first_wait=${GLITE_LOCAL_COPY_RETRY_FIRST_WAIT}
fi

if [ -z "${GLITE_LOCAL_MAX_OSB_SIZE}" ]; then
  __max_osb_size=-1 # unlimited
else
  __max_osb_size=${GLITE_LOCAL_MAX_OSB_SIZE}
fi

#customization point #1
if [ -n "${GLITE_LOCAL_CUSTOMIZATION_DIR}" ]; then
  if [ -f "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_1.sh" ]; then
    . "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_1.sh"
  fi
fi

if [ ${__create_subdir} -eq 1 ]; then
  if [ ${__job_type} -eq 0 -o ${__job_type} -eq 3 ]; then
    #normal or interactive
    newdir="${__jobid_to_filename}"
    mkdir ${newdir}
    cd ${newdir}
  elif [ ${__job_type} -eq 1 -o ${__job_type} -eq 2 ]; then
    #MPI (LSF or PBS)
    newdir="${__jobid_to_filename}"
    mkdir -p .mpi/${newdir}
    if [ $? != 0 ]; then
      fatal_error "Cannot create .mpi/${newdir} directory"
    fi
    cd .mpi/${newdir}
  fi
fi

tmpfile=`mktemp -q ./tmp.XXXXXX`
if [ $? != 0 ]; then
  fatal_error "Working directory not writable"
else
  rm ${tmpfile}
fi
unset tmpfile

workdir="`pwd`"

if [ -n "${__brokerinfo}" ]; then
  export GLITE_WMS_RB_BROKERINFO="`pwd`/${__brokerinfo}"
fi

maradona="${__jobid_to_filename}.output"
touch "${maradona}"

if [ -z "${GLOBUS_LOCATION}" ]; then
  fatal_error "GLOBUS_LOCATION undefined"
elif [ -r "${GLOBUS_LOCATION}/etc/globus-user-env.sh" ]; then
  . ${GLOBUS_LOCATION}/etc/globus-user-env.sh
else
  fatal_error "${GLOBUS_LOCATION}/etc/globus-user-env.sh not found or unreadable"
fi

for env in ${__environment[@]}
do
  eval export $env
done

umask 022

if [ ${__wmp_support} -eq 0 ]; then
  for f in ${__input_file[@]}
  do
    retry_copy "globus-url-copy" "${__input_base_url}${f}" "file://${workdir}/${f}"
    if [ $? != 0 ]; then
      fatal_error "Cannot download ${f} from ${__input_base_url}"
    fi
  done
else
  #WMP support
  index=0
  for f in ${__wmp_input_base_file[@]}
  do
    if [ -z "${__wmp_input_base_dest_file}" ]; then
      file=`basename ${f}`
    else
      file=`basename ${__wmp_input_base_dest_file[$index]}`
    fi
    if [ "${f:0:9}" == "gsiftp://" ]; then
      retry_copy "globus-url-copy" "${f}" "file://${workdir}/${file}"
    elif [ "${f:0:8}" == "https://" -o "${f:0:7}" == "http://" ]; then
      retry_copy "htcp" "${f}" "file://${workdir}/${file}"
    else
      false
    fi
    if [ $? != 0 ]; then
      fatal_error "Cannot download ${workdir}/${file} from ${f}"
    fi
    let "++index"
  done
fi

if [ -f "${__job}" ]; then
  chmod +x "${__job}" 2>/dev/null
else
  fatal_error "${__job} not found or unreadable"
fi

#user script (before taking the token, shallow-sensitive)
if [ -f "${__prologue}" ]; then
  chmod +x "${__prologue}" 2>/dev/null
  ${__prologue} "${__prologue_arguments}"
  prologue_status=$?
  if [ ${prologue_status} -ne 0 ]; then
    fatal_error "prologue failed with error ${prologue_status}"
  fi
fi

if [ ${__job_type} -eq 3 ]; then #interactive jobs
  #extracts 'scheme://host' from the full URL
  base_url=${__input_base_url:0:`expr match "$__input_base_url" '[[:alpha:]][[:alnum:]+.-]*://[[:alnum:]_.~!$&-]*'`}
  for f in  "glite-wms-pipe-input" "glite-wms-pipe-output" "glite-wms-job-agent" ; do
    retry_copy "globus-url-copy" "${base_url}/${GLITE_LOCATION}/bin/${f}" "file://${workdir}/${f}"
    chmod +x ${workdir}/${f}
  done
  retry_copy "globus-url-copy" "${base_url}/${GLITE_LOCATION}/lib/libglite-wms-grid-console-agent.so.0" "file://${workdir}/libglite-wms-grid-console-agent.so.0"
fi

if [ ${__perusal_support} -eq 1 ]; then
  send_partial_file ${__perusal_listfileuri} ${__perusal_filesdesturi} ${__perusal_timeinterval} & send_pid=$!
fi

if [ -n ${__shallow_resubmission_token} ]; then
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
    fatal_error "No *ftp for rm command found"
  else
    is_uberftp=`expr match "${gridftp_rm_command}" '.*uberftp'`
    if [ $is_uberftp -eq 0 ]; then
      $gridftp_rm_command ${__shallow_resubmission_token}
    else #uberftp
      tkn=${__shallow_resubmission_token} # will reduce lines length
      scheme=${tkn:0:`expr match "${tkn}" '[[:alpha:]][[:alnum:]+.-]*://'`}
      remaining=${tkn:${#scheme}:${#tkn}-${#scheme}}
      hostname=${remaining:0:`expr match "$remaining" '[[:alnum:]_.~!$&()-]*'`}
      token_fullpath=${remaining:${#hostname}:${#remaining}-${#hostname}}
      $gridftp_rm_command $hostname -a gsi "\"rm ${token_fullpath}\""
    fi
    result=$?
    if [ $result -eq 0 ]; then
      log_event "ReallyRunning"
      jw_echo "Take token: ${GLITE_WMS_SEQUENCE_CODE}"
    else
      fatal_error "Cannot take token for ${GLITE_WMS_JOBID}"
    fi
  fi
fi

if [ ${__job_type} -eq 1 ]; then
  HOSTFILE=host$$
  touch ${HOSTFILE}
  for host in ${LSB_HOSTS}
    do echo $host >> ${HOSTFILE}
  done
elif [ ${__job_type} -eq 2 ]; then
  HOSTFILE=${PBS_NODEFILE}
fi
if [ ${__job_type} -eq 1 -o ${__job_type} -eq 2 ]; then #MPI LSF, PBS
  for i in `cat $HOSTFILE`; do
    ssh ${i} mkdir -p `pwd`
    /usr/bin/scp -rp ./* "${i}:`pwd`"
    ssh ${i} chmod 755 `pwd`/${__job}
  done
fi

if [ ${__job_type} -eq 0 ]; then #normal
  cmd_line="${__job} ${__arguments} $*"
elif [ ${__job_type} -eq 1 -o ${__job_type} -eq 2 ]; then #MPI LSF, PBS
  cmd_line="mpirun -np ${__nodes} -machinefile ${HOSTFILE} ${__job} ${__arguments} $*"
elif [ ${__job_type} -eq 3 ]; then #interactive
  cmd_line="./glite-wms-job-agent $BYPASS_SHADOW_HOST $BYPASS_SHADOW_PORT '${__job} ${__arguments} $*'"
fi

if [ ${__job_type} -ne 3 ]; then #all but interactive
  if [ -n "${__standard_input}" ]; then
    cmd_line="$cmd_line < ${__standard_input}"
  fi
  if [ -n "${__standard_output}" ]; then
    cmd_line="$cmd_line > ${__standard_output}"
  else
    cmd_line="$cmd_line > /dev/null "
  fi
  if [ -n "${__standard_error}" ]; then
    if [ -n "${__standard_output}" ]; then
      if [ "${__standard_error}" = "${__standard_output}" ]; then
        cmd_line="$cmd_line 2>&1"
      else
        cmd_line="$cmd_line 2>${__standard_error}"
      fi
    fi
  else
    cmd_line="$cmd_line 2>/dev/null"
  fi
fi

perl -e '
  unless (defined($ENV{"EDG_WL_NOSETPGRP"})) {
    $SIG{"TTIN"} = "IGNORE";
    $SIG{"TTOU"} = "IGNORE";
    setpgrp(0, 0);
  }
  exec(@ARGV);
  warn "could not exec $ARGV[0]: $!\n";
  exit(127);
' "$cmd_line" &
user_job=$!

exec 2> /dev/null

perl -e '
  while (1) {
    $time_left = `grid-proxy-info -timeleft 2> /dev/null` || echo 0;
    last if ($time_left <= 0);
    sleep($time_left);
  }
  kill(defined($ENV{"EDG_WL_NOSETPGRP"}) ? 9 : -9, '"$user_job"');
  my $maradona = "'$maradona'";
  my $logger = "'$lb_logevent'";
  print STDERR $err_msg;
  if (open(DAT,">> ".$maradona)) {
    print DAT $err_msg;
    close(DAT);
  }
  if (open(CMD, $logger.
                " --jobid=\"".$ENV{GLITE_WMS_JOBID}."\"".
                " --source=LRMS".
                " --sequence=".$ENV{GLITE_WMS_SEQUENCE_CODE}.
                " --event=\"Done\"".
                " --reason=\"Job killed because of user proxy expiration\"".
                " --status_code=FAILED".
                " --exit_code=0 2>/dev/null |")) {
    chomp(my $value = <CMD>);
    close(CMD);
    my $result = $?;
    my $exit_value = $result >> 8;
    $ENV{GLITE_WMS_SEQUENCE_CODE} = $value if ($exit_value == 0);
  }
  exit(1);
' &

watchdog=$!
wait $user_job
status=$?
#the bash kill command doesn't appear to work properly on process groups
/bin/kill -9 $watchdog $user_job -$user_job

#customization point #2
if [ -n "${GLITE_LOCAL_CUSTOMIZATION_DIR}" ]; then
  if [ -f "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_2.sh" ]; then
    . "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_2.sh"
  fi
fi

if [ ${__perusal_support} -eq 1 ]; then
  kill -USR2 $send_pid
  wait $send_pid
fi

if [ ${__output_data} -eq 1 ]; then
  return_value=0
  if [ $status -eq 0 ]; then
    local=`doDSUploadTmp "${__dsupload}.tmp"`
    status=$?
    return_value=$status
    local_cnt=0
    for outputfile in ${__output_file[@]}
    do
      local=`doCheckReplicaFile ${__output_file} "${__dsupload}.tmp"`
      status=$?
      if [ $status -ne 0 ]; then
        return_value=1
      else
        if [ -z "${__output_lfn}" -a -z "${__output_se}"] ; then
         local=`doReplicaFile ${outputfile} "${__dsupload}.tmp"`
        elif [ -n "${__output_lfn}" -a -z "${__output_se}"] ; then
         local=`doReplicaFilewithLFN ${outputfile} ${__output_lfn[$local_cnt]} "${__dsupload}.tmp"`
        elif [ -z "${__output_lfn}" -a -n "${__output_se}"] ; then
          local=`doReplicaFilewithSE ${outputfile} ${__output_se[$local_cnt]} "${__dsupload}.tmp"`
        else
         local=`doReplicaFilewithLFNAndSE ${outputfile} ${__output_lfn[$local_cnt]} ${__output_se[$local_cnt]} "${__dsupload}.tmp"`
        fi
        status=$?
      fi
      let "++local_cnt"
    done
    local=`doDSUpload ${__dsupload}`
    status=$?
  fi
fi

jw_echo "job exit status = ${status}"

if [ -f "${__epilogue}" ]; then
  chmod +x "${__epilogue}" 2>/dev/null
  ${__epilogue} "${__epilogue_arguments}"
  epilogue_status=$?
  if [ ${epilogue_status} -ne 0 ]; then
    fatal_error "epilogue failed with error ${epilogue_status}"
  fi
fi

# uncomment this one below if the order in the osb file list originally
# specified is not of some relevance to the user
#sort_by_size __output_file ${workdir}

file_size_acc=0
current_file=0
if [ ${__wmp_support} -eq 0 ]; then
  total_files=${#__output_file[@]}
  for f in "${__output_file[@]}"
  do
    if [ -r "${f}" ]; then
      ff=${f##*/}
      if [ ${__max_osb_size} -ge 0 ]; then
        #todo
        #if hostname=wms
          file_size=`stat -t $f | awk '{print $2}'`
          let "file_size_acc += $file_size"
        #fi
        if [ $file_size_acc -le ${__max_osb_size} ]; then
          retry_copy "globus-url-copy" "file://${workdir}/${f}" "${__output_base_url}${ff}"
        else
          jw_echo "OSB quota exceeded for file://${workdir}/${f}, truncating needed"
          # $current_file is zero-based (being used even
          # below as an array index), + 1 again because of the
          # difference between $total and $current (i.e. 20-19=2 more files)
          remaining_files=`expr $total_files \- $current_file + 2`
          remaining_space=`expr $__max_osb_size \- $file_size_acc`
          trunc_len=`expr $remaining_space / $remaining_files`||0
          if [ $trunc_len -lt 10 ]; then #non trivial truncation
            jw_echo "Not enough room for a significant truncation on file ${f}, not sending"
          else
            truncate "${workdir}/${f}" $trunc_len "${workdir}/${f}.tail"
            if [ $? != 0 ]; then
              jw_echo "Could not truncate output sandbox file ${f}, not sending"
            else
              jw_echo "Truncated last $trunc_len bytes for file ${f}"
              retry_copy "globus-url-copy" "file://${workdir}/${f}.tail" "${__output_base_url}${ff}.tail"
            fi
          fi
        fi
      else
        retry_copy "globus-url-copy" "file://${workdir}/${f}" "${__output_base_url}${ff}"
      fi
      if [ $? != 0 ]; then
        fatal_error "Cannot upload ${f} into ${__output_base_url}" "Done"
      fi
    fi #if [ -r "${f}" ]; then
    let "++current_file"
  done
else #WMP support
  total_files=${#__wmp_output_dest_file[@]}
  for f in "${__wmp_output_dest_file[@]}"
  do
    if [ -r "${__wmp_output_file[$current_file]}" ]; then
      file=`basename $f`
      s="${workdir}/${__wmp_output_file[$current_file]}"
      if [ ${__osb_wildcards_support} -eq 0 ]; then
        d="${f}"
      else
        d="${__output_sandbox_base_dest_uri}/${file}"
      fi
      if [ ${__max_osb_size} -ge 0 ]; then
        #todo
        #if hostname=wms
          file_size=`stat -t $f | awk '{print $2}'`
          file_size_acc=`expr $file_size_acc + $file_size`
        #fi
        if [ $file_size_acc -le ${__max_osb_size} ]; then
          if [ "${f:0:9}" == "gsiftp://" ]; then
            retry_copy "globus-url-copy" "file://$s" "$d"
          elif [ "${f:0:8}" == "https://" -o "${f:0:7}" == "http://" ]; then
            retry_copy "htcp" "file://$s" "$d"
          else
            false
          fi
        else
          jw_echo "OSB quota exceeded for $s, truncating needed"
          remaining_files=`expr $total_files \- $current_file + 2`
          remaining_space=`expr $__max_osb_size \- $file_size_acc`
          trunc_len=`expr $remaining_space / $remaining_files`||0
          if [ $trunc_len -lt 10 ]; then #non trivial truncation
            jw_echo "Not enough room for a significant truncation on file ${f}, not sending"
          else
            truncate "$s" $trunc_len "$s.tail"
            if [ $? != 0 ]; then
              jw_echo "Could not truncate output sandbox file ${f}, not sending"
            else
              jw_echo "Truncated last $trunc_len bytes for file ${f}"
              if [ "${f:0:9}" == "gsiftp://" ]; then
                retry_copy "globus-url-copy" "file://$s.tail" "$d.tail"
              elif [ "${f:0:8}" == "https://" -o "${f:0:7}" == "http://" ]; then
                retry_copy "htcp" "file://$s.tail" "$d.tail"
              else
                false
              fi
            fi
          fi
        fi
      else #unlimited osb
        if [ "${f:0:9}" == "gsiftp://" ]; then
          retry_copy "globus-url-copy" "file://$s" "$d"
        elif [ "${f:0:8}" == "https://" -o "${f:0:7}" == "http://" ]; then
          retry_copy "htcp" "file://$s" "$d"
        else
          false
        fi
      fi
      if [ $? != 0 ]; then
        fatal_error "Cannot upload ${file} into ${f}" "Done"
      fi
    fi
    let "++current_file"
  done
fi #WMP support

log_event "Done"

if [ -n "${LSB_JOBID}" ]; then
  cat "${X509_USER_PROXY}" | ${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient -s ${__gatekeeper_hostname}:56569: -L lsf_${LSB_JOBID} -G ${GLITE_WMS_JOBID} -C ${__globus_resource_contact_string} -H "$HLR_LOCATION"
  if [ $? != 0 ]; then
    jw_echo "Error transferring gianduia with command: cat ${X509_USER_PROXY} | ${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient -s ${__gatekeeper_hostname}:56569: -L lsf_${LSB_JOBID} -G ${GLITE_WMS_JOBID} -C ${__globus_resource_contact_string} -H $HLR_LOCATION"
  fi
fi

if [ -n "${PBS_JOBID}" ]; then
  cat ${X509_USER_PROXY} | ${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient -s ${__gatekeeper_hostname}:56569: -L pbs_${PBS_JOBID} -G ${GLITE_WMS_JOBID} -C ${__globus_resource_contact_string} -H "$HLR_LOCATION"
  if [ $? != 0 ]; then
    jw_echo "Error transferring gianduia with command: cat ${X509_USER_PROXY} | ${GLITE_WMS_LOCATION}/libexec/glite_dgas_ceServiceClient -s ${__gatekeeper_hostname}:56569: -L pbs_${PBS_JOBID} -G ${GLITE_WMS_JOBID} -C ${__globus_resource_contact_string} -H $HLR_LOCATION"
  fi
fi

#customization point #3
if [ -n "${GLITE_LOCAL_CUSTOMIZATION_DIR}" ]; then
  if [ -f "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_3.sh" ]; then
    . "${GLITE_LOCAL_CUSTOMIZATION_DIR}/cp_3.sh"
  fi
fi

doExit 0
