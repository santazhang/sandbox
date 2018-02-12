# -*- coding: utf-8 -*-
#
# This file is part of Glances.
#
# Copyright (C) 2017 Nicolargo <nicolas@nicolargo.com>
#
# Glances is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Glances is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

"""Process count plugin."""

from glances.processes import glances_processes
from glances.plugins.glances_plugin import GlancesPlugin

# Note: history items list is not compliant with process count
#       if a filter is applyed, the graph will show the filtered processes count


class Plugin(GlancesPlugin):

    """Glances process count plugin.

    stats is a list
    """

    def __init__(self, args=None):
        """Init the plugin."""
        super(Plugin, self).__init__(args=args)

        # We want to display the stat in the curse interface
        self.display_curse = True

        # Note: 'glances_processes' is already init in the glances_processes.py script

    def reset(self):
        """Reset/init the stats."""
        self.stats = {}

    def update(self):
        """Update processes stats using the input method."""
        # Reset stats
        self.reset()

        if self.input_method == 'local':
            # Update stats using the standard system lib
            # Here, update is call for processcount AND processlist
            glances_processes.update()

            # Return the processes count
            self.stats = glances_processes.getcount()
        elif self.input_method == 'snmp':
            # Update stats using SNMP
            # !!! TODO
            pass

        return self.stats

    def msg_curse(self, args=None):
        """Return the dict to display in the curse interface."""
        # Init the return message
        ret = []

        # Only process if stats exist and display plugin enable...
        if args.disable_process:
            msg = "PROCESSES DISABLED (press 'z' to display)"
            ret.append(self.curse_add_line(msg))
            return ret

        if not self.stats:
            return ret

        # Display the filter (if it exists)
        if glances_processes.process_filter is not None:
            msg = 'Processes filter:'
            ret.append(self.curse_add_line(msg, "TITLE"))
            msg = ' {} '.format(glances_processes.process_filter)
            if glances_processes.process_filter_key is not None:
                msg += 'on column {} '.format(glances_processes.process_filter_key)
            ret.append(self.curse_add_line(msg, "FILTER"))
            msg = '(\'ENTER\' to edit, \'E\' to reset)'
            ret.append(self.curse_add_line(msg))
            ret.append(self.curse_new_line())

        # Build the string message
        # Header
        msg = 'TASKS'
        ret.append(self.curse_add_line(msg, "TITLE"))
        # Compute processes
        other = self.stats['total']
        msg = '{:>4}'.format(self.stats['total'])
        ret.append(self.curse_add_line(msg))

        if 'thread' in self.stats:
            msg = ' ({} thr),'.format(self.stats['thread'])
            ret.append(self.curse_add_line(msg))

        if 'running' in self.stats:
            other -= self.stats['running']
            msg = ' {} run,'.format(self.stats['running'])
            ret.append(self.curse_add_line(msg))

        if 'sleeping' in self.stats:
            other -= self.stats['sleeping']
            msg = ' {} slp,'.format(self.stats['sleeping'])
            ret.append(self.curse_add_line(msg))

        msg = ' {} oth '.format(other)
        ret.append(self.curse_add_line(msg))

        # Display sort information
        if glances_processes.auto_sort:
            msg = 'sorted automatically'
            ret.append(self.curse_add_line(msg))
            msg = ' by {}'.format(glances_processes.sort_key)
            ret.append(self.curse_add_line(msg))
        else:
            msg = 'sorted by {}'.format(glances_processes.sort_key)
            ret.append(self.curse_add_line(msg))
        ret[-1]["msg"] += ", %s view" % ("tree" if glances_processes.is_tree_enabled() else "flat")
        # if args.disable_irix:
        #     ret[-1]["msg"] += " - IRIX off"

        # Return the message with decoration
        return ret
