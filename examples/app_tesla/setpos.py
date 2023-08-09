# -*- coding: utf-8 -*-
#
#     ||          ____  _ __
#  +------+      / __ )(_) /_______________ _____  ___
#  | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
#  +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
#   ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
#
#  Copyright (C) 2017 Bitcraze AB
#
#  Crazyflie Nano Quadcopter Client
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
"""
Simple example that connects to the first Crazyflie found, and then sends the
reboot signals to 6 anchors ID from 0 to 5. The reset signals is sent
10 times in a row to make sure all anchors are reset to bootloader.
"""
import logging
import time
from threading import Thread

import cflib
from cflib.crazyflie import Crazyflie
from cflib.utils import uri_helper
from lpslib.lopoanchor import LoPoAnchor
import time
import yaml

uri = uri_helper.uri_from_env(default='radio://0/80/250K/E7E7E7E7E7')

logging.basicConfig(level=logging.ERROR)

counter = 0

class LpsRebootToBootloader:
    """Example that connects to a Crazyflie and ramps the motors up/down and
    the disconnects"""


    def __init__(self, link_uri):
        """ Initialize and run the example with the specified link_uri """

        self._cf = Crazyflie()

        self._cf.connected.add_callback(self._connected)
        self._cf.disconnected.add_callback(self._disconnected)
        self._cf.connection_failed.add_callback(self._connection_failed)
        self._cf.connection_lost.add_callback(self._connection_lost)

        self._cf.open_link(link_uri)


        print('Connecting to %s' % link_uri)

    def console_callback(text: str):
        '''A callback to run when we get console text from Crazyflie'''
        # We do not add newlines to the text received, we get them from the
        # Crazyflie at appropriate places.
        print(text, end='')

    def _connected(self, link_uri):
        """ This callback is called form the Crazyflie API when a Crazyflie
        has been connected and the TOCs have been downloaded."""

        Thread(target=self._sync_thread).start()
        self._cf.console.receivedChar.add_callback(console_callback)
        
    def _connection_failed(self, link_uri, msg):
        """Callback when connection initial connection fails (i.e no Crazyflie
        at the specified address)"""
        print('Connection to %s failed: %s' % (link_uri, msg))

    def _connection_lost(self, link_uri, msg):
        """Callback when disconnected after a connection has been made (i.e
        Crazyflie moves out of range)"""
        print('Connection to %s lost: %s' % (link_uri, msg))

    def _disconnected(self, link_uri):
        """Callback when the Crazyflie is disconnected (called in all cases)"""
        print('Disconnected from %s' % link_uri)

    def _sync_thread(self):
        
        anchors = LoPoAnchor(self._cf)
        
        #anchors.set_position(0, (-1.0, -1.0, 0.0))
        #anchors.set_position(1, (-1.0, 1.0, 1.0))
        #anchors.set_position(2, (1.0, 1.0, 0.0))
        #anchors.set_position(3, (1.0, -1.0, 1.0))
        #anchors.set_position(4, (-1.0, -1.0, 1.0))
        #anchors.set_position(5, (-1.0, 1.0, 0.0))
        #anchors.set_position(6, (1.0, 1.0, 1.0))
        #anchors.set_position(7, (1.0, -1.0, 0.0))
        
                
        with open('/Users/mahyar/Developer/crazyflie/new_coordinate_system_update.yaml', 'r') as f:
            data = yaml.safe_load(f)
            
        print(data)
        
        for key, value in data.items():
            
            anchors.set_position(int(key), (value['x'],value['y'],value['z']))
            
            
            #anchors.set_position(0, (-2.1230,-3.6221,0.3586))
            #anchors.set_position(1, (-2.0414,4.0449,3.0240))
            #anchors.set_position(2, (2.6391,3.4173,0.3626))
            #anchors.set_position(3, (2.5791,-3.4709,3.1023))
            #anchors.set_position(4, (-1.9667,-3.6053,2.9467))
            #anchors.set_position(5, (-2.2824,3.4097,0.3657))
            #anchors.set_position(6, (2.6057,3.9284,2.8900))
            #anchors.set_position(7, (2.6056,-2.9680,0.3632))
        #anchors.set_position(1, (10,10,10))
        #anchors.set_position(2, (10,10,10))
        #anchors.set_position(3, (10,10,10))
        
        self._cf.close_link()


def console_callback(text: str):
    '''A callback to run when we get console text from Crazyflie'''
    # We do not add newlines to the text received, we get them from the
    # Crazyflie at appropriate places.
    print(text, end='')
    
if __name__ == '__main__':
    # Initialize the low-level drivers
    cflib.crtp.init_drivers()
    
    le = LpsRebootToBootloader(uri)
    