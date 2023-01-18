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
resync signals to all anchors. 
"""
import logging
import time
from threading import Thread
from threading import Timer
import csv

import cflib
from cflib.crazyflie import Crazyflie
from cflib.utils import uri_helper
from cflib.crazyflie.log import LogConfig

#from lpslib.lopoanchor import LoPoAnchor
import time
import struct

shouldSync = True
samplingRate = 40 # in ms
duration = 60*5 # in seconds
row_list = [["timestamp", "pose.x", "pose.y"]]
logging.basicConfig(level=logging.ERROR)

class LoPoAnchor():
    LPP_TYPE_INIT_TESLA = 6 # lpp.h
    
    def __init__(self, crazyflie):
        """
        :param crazyflie: A crazyflie object to be used as a bridge to the LoPo
         system."""
        self.crazyflie = crazyflie
        
    def init_tesla(self, time):
        for i in range(8):
            data = struct.pack('<BL', LoPoAnchor.LPP_TYPE_INIT_TESLA, time) 
            self.crazyflie.loc.send_short_lpp_packet(i, data)
            print(f"sent 'resync tesla_counter' to anchor '{i}'")
            
            
uri = uri_helper.uri_from_env(default='usb://0')

logging.basicConfig(level=logging.ERROR)

counter = 0

class SYNCLOG:

    def __init__(self, link_uri):
        """ Initialize and run the example with the specified link_uri """

        self._cf = Crazyflie()

        self._cf.connected.add_callback(self._connected)
        self._cf.disconnected.add_callback(self._disconnected)
        self._cf.connection_failed.add_callback(self._connection_failed)
        self._cf.connection_lost.add_callback(self._connection_lost)

        self._cf.open_link(link_uri)

        self._cf.console.receivedChar.add_callback(console_callback)

        print('Connecting to %s' % link_uri)

    def console_callback(text: str):
        '''A callback to run when we get console text from Crazyflie'''
        # We do not add newlines to the text received, we get them from the
        # Crazyflie at appropriate places.
        print(text, end='')

    def _connected(self, link_uri):
        """ This callback is called form the Crazyflie API when a Crazyflie
        has been connected and the TOCs have been downloaded."""

        if shouldSync:
            Thread(target=self._sync_thread).start()
        self._cf.console.receivedChar.add_callback(console_callback)
        self._lg_stab = LogConfig(name='Stabilizer', period_in_ms=samplingRate)
        self._lg_stab.add_variable('stateEstimate.x', 'float')
        self._lg_stab.add_variable('stateEstimate.y', 'float')
        # The fetch-as argument can be set to FP16 to save space in the log packet
        self._lg_stab.add_variable('pm.vbat', 'FP16')
        
        # Adding the configuration cannot be done until a Crazyflie is
        # connected, since we need to check that the variables we
        # would like to log are in the TOC.
        try:
            self._cf.log.add_config(self._lg_stab)
            # This callback will receive the data
            self._lg_stab.data_received_cb.add_callback(self._stab_log_data)
            # This callback will be called on errors
            self._lg_stab.error_cb.add_callback(self._stab_log_error)
            # Start the logging
            self._lg_stab.start()
        except KeyError as e:
            print('Could not start log configuration,'
                  '{} not found in TOC'.format(str(e)))
        except AttributeError:
            print('Could not add Stabilizer log config, bad configuration.')
            
        # Start a timer to disconnect in 10s
        t = Timer(duration, self.saveAndClose)
        t.start()
        
    def saveAndClose(self):
        filename = ("posesample-" + time.strftime('%b-%d-%Y_%H%M', time.localtime()) + ".csv")
        with open(filename, 'w', newline='') as file:
            writer = csv.writer(file)
            writer.writerows(row_list)
        print("done")
        time.sleep(1)
        self._cf.close_link()
            
    def _stab_log_error(self, logconf, msg):
        """Callback from the log API when an error occurs"""
        print('Error when logging %s: %s' % (logconf.name, msg))
        
    def _stab_log_data(self, timestamp, data, logconf):
        row = [timestamp,data["stateEstimate.x"],data["stateEstimate.y"]]
        row_list.append(row)
            
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
        resync_period = 1000 # 1 sec        
        global counter

        while True:
            counter += 1
            time.sleep(0.001)
            if (counter%1000==0):
                print("server counter is "+str(counter))
            if (counter%resync_period==0):
                anchors = LoPoAnchor(self._cf)
                anchors.init_tesla(counter)

def console_callback(text: str):
    '''A callback to run when we get console text from Crazyflie'''
    # We do not add newlines to the text received, we get them from the
    # Crazyflie at appropriate places.
    print(text, end='')
    
if __name__ == '__main__':
    # Initialize the low-level drivers
    cflib.crtp.init_drivers()
    le = SYNCLOG(uri)
    