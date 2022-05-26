#!/usr/bin/python

'''apport hook for openssh-server

(c) 2010 Canonical Ltd.
Author: Chuck Short <chuck.short@canonical.com>

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.  See http://www.gnu.org/copyleft/gpl.html for
the full text of the license.
'''

from apport.hookutils import *

def add_info(report, ui):
    response = ui.yesno("The contents of your /etc/ssh/sshd_config file "
                        "may help developers diagnose your bug more "
                        "quickly.  However, it may contain sensitive "
                        "information.  Do you want to include it in your "
                        "bug report?")

    if response == None: # user cancelled
        raise StopIteration

    elif response == True:
        report['SSHDConfig'] = root_command_output(['/usr/sbin/sshd', '-T'])
