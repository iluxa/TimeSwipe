#!/usr/bin/python
import timeswipe
import time
import json
import sys
import signal

import optparse

parser = optparse.OptionParser()

parser.add_option('--config', dest="config", help="config file name", default="config.json")
parser.add_option('--input', dest="input", help="input name; default is first one from config", default="NORM")
parser.add_option('--output', dest="output", help="output filename to save data", default=None)

options, args = parser.parse_args()

tswipe = timeswipe.TimeSwipe()

json_file = open(options.config)
data = json.load(json_file)
item = data[options.input]
output = None
if options.output:
    sys.stdout = open(options.output, "w")

# old style
#tswipe.SetBridge(item["U_BRIDGE"]);
#tswipe.SetSensorOffsets(item["SENSOR_OFFSET"]);
#tswipe.SetSensorGains(item["SENSOR_GAIN"]);
#tswipe.SetSensorTransmissions(item["SENSOR_TRANSMISSION"]);

#new style
tswipe.Init(item["U_BRIDGE"], item["SENSOR_OFFSET"], item["SENSOR_GAIN"], item["SENSOR_TRANSMISSION"]);

def signal_handler(sig, frame):
    tswipe.Stop()
    sys.exit(1)

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)

def process(records, errors):
    if errors:
        print >> sys.stderr, "errors: ", errors
    for record in records:
        print('\t'.join([str(int(x)) for x in record]))
    if len(records) == 0 and errors==0:
        time.sleep(1.0/100)

tswipe.Start(process)


