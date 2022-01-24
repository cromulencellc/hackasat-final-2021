#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Payload Flatsat RX
# GNU Radio version: 3.9.3.0

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from flatsat_rx import flatsat_rx  # grc-generated hier_block
from gnuradio import analog
from gnuradio import blocks
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import uhd
import time
from gnuradio.filter import pfb
from rms_agc import rms_agc  # grc-generated hier_block




class payload_flatsat_rx(gr.top_block):

    def __init__(self, team_num=1):
        gr.top_block.__init__(self, "Payload Flatsat RX", catch_exceptions=True)

        ##################################################
        # Parameters
        ##################################################
        self.team_num = team_num

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 1e6
        self.num_chan = num_chan = 18
        self.freq = freq = 901e6
        self.rx_samp_rate_0 = rx_samp_rate_0 = samp_rate
        self.rx_samp_rate = rx_samp_rate = samp_rate
        self.rx_gain = rx_gain = 15
        self.rx_freq = rx_freq = freq
        self.lpf_taps = lpf_taps = firdes.low_pass(1.0, samp_rate, samp_rate/num_chan*0.8,10e3, window.WIN_HAMMING, 6.76)

        ##################################################
        # Blocks
        ##################################################
        self.uhd_usrp_source_0 = uhd.usrp_source(
            ",".join(("", "")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,1)),
            ),
        )
        self.uhd_usrp_source_0.set_subdev_spec('A:B', 0)
        self.uhd_usrp_source_0.set_samp_rate(rx_samp_rate)
        self.uhd_usrp_source_0.set_time_now(uhd.time_spec(time.time()), uhd.ALL_MBOARDS)

        self.uhd_usrp_source_0.set_center_freq(rx_freq, 0)
        self.uhd_usrp_source_0.set_antenna('RX2', 0)
        self.uhd_usrp_source_0.set_bandwidth(rx_samp_rate, 0)
        self.uhd_usrp_source_0.set_gain(rx_gain, 0)
        self.rms_agc_0 = rms_agc(
            alpha=1e-2,
            reference=1.0,
        )
        self.pfb_channelizer_ccf_0 = pfb.channelizer_ccf(
            num_chan,
            lpf_taps,
            1,
            100)
        self.pfb_channelizer_ccf_0.set_channel_map([0,10,12,14,16,1,3,5,7,11,13,15,17,2,4,6,8,9])
        self.pfb_channelizer_ccf_0.declare_sample_delay(0)
        self.flatsat_rx_0 = flatsat_rx(
            damping=1,
            deviation=1.5,
            timing_bw=0.045,
        )
        self.blocks_socket_pdu_1 = blocks.socket_pdu('UDP_CLIENT', 'localhost', '2303', 256, False)
        self.blocks_selector_0 = blocks.selector(gr.sizeof_gr_complex*1,team_num-1,0)
        self.blocks_selector_0.set_enabled(True)
        self.blocks_selector_0.set_block_alias("team_selector")
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_message_debug_0_0 = blocks.message_debug(True)
        self.analog_pwr_squelch_xx_0 = analog.pwr_squelch_cc(-70, 1e-4, 0, True)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.flatsat_rx_0, 'pdu'), (self.blocks_message_debug_0_0, 'print_pdu'))
        self.msg_connect((self.flatsat_rx_0, 'pdu'), (self.blocks_socket_pdu_1, 'pdus'))
        self.connect((self.analog_pwr_squelch_xx_0, 0), (self.rms_agc_0, 0))
        self.connect((self.blocks_selector_0, 0), (self.analog_pwr_squelch_xx_0, 0))
        self.connect((self.pfb_channelizer_ccf_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.pfb_channelizer_ccf_0, 8), (self.blocks_selector_0, 7))
        self.connect((self.pfb_channelizer_ccf_0, 11), (self.blocks_selector_0, 10))
        self.connect((self.pfb_channelizer_ccf_0, 1), (self.blocks_selector_0, 0))
        self.connect((self.pfb_channelizer_ccf_0, 15), (self.blocks_selector_0, 14))
        self.connect((self.pfb_channelizer_ccf_0, 9), (self.blocks_selector_0, 8))
        self.connect((self.pfb_channelizer_ccf_0, 6), (self.blocks_selector_0, 5))
        self.connect((self.pfb_channelizer_ccf_0, 5), (self.blocks_selector_0, 4))
        self.connect((self.pfb_channelizer_ccf_0, 13), (self.blocks_selector_0, 12))
        self.connect((self.pfb_channelizer_ccf_0, 3), (self.blocks_selector_0, 2))
        self.connect((self.pfb_channelizer_ccf_0, 16), (self.blocks_selector_0, 15))
        self.connect((self.pfb_channelizer_ccf_0, 17), (self.blocks_selector_0, 16))
        self.connect((self.pfb_channelizer_ccf_0, 4), (self.blocks_selector_0, 3))
        self.connect((self.pfb_channelizer_ccf_0, 12), (self.blocks_selector_0, 11))
        self.connect((self.pfb_channelizer_ccf_0, 2), (self.blocks_selector_0, 1))
        self.connect((self.pfb_channelizer_ccf_0, 14), (self.blocks_selector_0, 13))
        self.connect((self.pfb_channelizer_ccf_0, 7), (self.blocks_selector_0, 6))
        self.connect((self.pfb_channelizer_ccf_0, 10), (self.blocks_selector_0, 9))
        self.connect((self.rms_agc_0, 0), (self.flatsat_rx_0, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.pfb_channelizer_ccf_0, 0))


    def get_team_num(self):
        return self.team_num

    def set_team_num(self, team_num):
        self.team_num = team_num
        self.blocks_selector_0.set_input_index(self.team_num-1)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.set_lpf_taps(firdes.low_pass(1.0, self.samp_rate, self.samp_rate/self.num_chan*0.8, 10e3, window.WIN_HAMMING, 6.76))
        self.set_rx_samp_rate(self.samp_rate)
        self.set_rx_samp_rate_0(self.samp_rate)

    def get_num_chan(self):
        return self.num_chan

    def set_num_chan(self, num_chan):
        self.num_chan = num_chan
        self.set_lpf_taps(firdes.low_pass(1.0, self.samp_rate, self.samp_rate/self.num_chan*0.8, 10e3, window.WIN_HAMMING, 6.76))

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.set_rx_freq(self.freq)

    def get_rx_samp_rate_0(self):
        return self.rx_samp_rate_0

    def set_rx_samp_rate_0(self, rx_samp_rate_0):
        self.rx_samp_rate_0 = rx_samp_rate_0

    def get_rx_samp_rate(self):
        return self.rx_samp_rate

    def set_rx_samp_rate(self, rx_samp_rate):
        self.rx_samp_rate = rx_samp_rate
        self.uhd_usrp_source_0.set_samp_rate(self.rx_samp_rate)
        self.uhd_usrp_source_0.set_bandwidth(self.rx_samp_rate, 0)

    def get_rx_gain(self):
        return self.rx_gain

    def set_rx_gain(self, rx_gain):
        self.rx_gain = rx_gain
        self.uhd_usrp_source_0.set_gain(self.rx_gain, 0)

    def get_rx_freq(self):
        return self.rx_freq

    def set_rx_freq(self, rx_freq):
        self.rx_freq = rx_freq
        self.uhd_usrp_source_0.set_center_freq(self.rx_freq, 0)

    def get_lpf_taps(self):
        return self.lpf_taps

    def set_lpf_taps(self, lpf_taps):
        self.lpf_taps = lpf_taps
        self.pfb_channelizer_ccf_0.set_taps(self.lpf_taps)



def argument_parser():
    parser = ArgumentParser()
    parser.add_argument(
        "--team-num", dest="team_num", type=intx, default=1,
        help="Set Team Number Selector [default=%(default)r]")
    return parser


def main(top_block_cls=payload_flatsat_rx, options=None):
    if options is None:
        options = argument_parser().parse_args()
    tb = top_block_cls(team_num=options.team_num)

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    tb.start()

    try:
        input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()


if __name__ == '__main__':
    main()
