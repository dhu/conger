#!/bin/sh

########################################################################
#
#  File:  killer.sh
#
#  Lists your active processes and then prompts for each to be killed.
#  The kills are not performed until after the last prompt so you can
#  <Control> C out if you change your mind.
#
########################################################################


LIST=/tmp/ps_${USER}.list

if [ -z "$1" ]; then
   if [ "`uname`" = "OSF1" ]; then
      /bin/ps -u ${USER} | grep -v csh | grep -v '/bin/ps'        \
                         | grep -v 'killer.sh' > ${LIST}

   elif [ "`uname`" = "Linux" ]; then
      /bin/ps h -u ${USER} | grep -v csh  | grep -v '/bin/ps'     \
                           | grep -v sshd | grep -v 'killer.sh' > ${LIST}

   elif [ "`uname`" = "SunOS" ]; then
      /bin/ps | grep -v csh | grep -v '/bin/ps'                   \
              | grep -v 'killer.sh' > ${LIST}

   elif [ "`uname`" = "ULTRIX" ]; then
      /bin/ps -t | grep -v csh | grep -v '/bin/ps'                \
                 | grep -v 'killer.sh' > ${LIST}

   elif [ "`uname`" = "Darwin" ]; then
      /bin/ps -t | grep -v csh | grep -v '/bin/ps'                \
                 | grep -v 'killer.sh' > ${LIST}

   else
      echo "unsupported platform: `uname`"
      exit
   fi

   cat ${LIST}

   Kill_List=""
   export Kill_List

   for i in `cat ${LIST} | grep -v 'TIME COMMAND' | awk '{ print $1 }'`; do
       echo -n " Kill ${i} [No]: "

       read Reply

       if [ "${Reply}" = "y" ]; then
          Kill_List="${Kill_List} ${i}"
          export Kill_List
       fi
   done

   if [ "${Kill_List}" != "" ]; then
      echo "Killing ${Kill_List}"
      kill -9 ${Kill_List}
   fi

else
   for i in $*; do
      kill -9 `ps -ef|grep ${i}|awk '{print $2}'`
   done
fi

rm -f  ${LIST}

###########################  end  killer.sh  ###################################
