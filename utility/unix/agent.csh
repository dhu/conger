#! /bin/csh -f


#############################  .agent.csh  #####################################
#
#  Start up ssh-agent.  Skips if the agent is already running.
#
#  <<< begin .cshrc >>>
#  alias     agent         source  <utility>unix/agent.csh
#  alias     kill.agent    <utility>unix/kill.agent.csh
#  <<< begin .cshrc >>>
#...............................................................................


set  sshAgent=/usr/bin/ssh-agent
set  sshAgentArgs="-c"
set  tmpFile=exportAgentEnv

if ( -x "${sshAgent}" ) then
   if ( ! ${?SSH_AUTH_SOCK} ) then
      setenv  SSH_AUTH_SOCK  /tmp/.ssh-socket
      ${sshAgent}  ${sshAgentArgs} | head -2 > ${tmpFile}

      source     ${tmpFile}
      rm  -f     ${tmpFile}

      ssh-add
      echo  "ssh agent started [pid = ${SSH_AGENT_PID}]"

   else
      echo "ssh agent already running [pid = ${SSH_AGENT_PID}]"
   endif

else
   echo "ERROR:  Can not find the ssh agent program."
endif

#
###########################  end .agent.csh  ###################################
