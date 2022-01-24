#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: User Segment TX Test
# Author: dev
# GNU Radio version: 3.9.3.0

from distutils.version import StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio.filter import firdes
import sip
from gnuradio import blocks
import pmt
from gnuradio import digital
from gnuradio import fec
from gnuradio import gr
from gnuradio.fft import window
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from usersegment_tx import usersegment_tx  # grc-generated hier_block



from gnuradio import qtgui

class test_usersegment_tx(gr.top_block, Qt.QWidget):

    def __init__(self, MTU=1500, frame_size=30):
        gr.top_block.__init__(self, "User Segment TX Test", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("User Segment TX Test")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "test_usersegment_tx")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Parameters
        ##################################################
        self.MTU = MTU
        self.frame_size = frame_size

        ##################################################
        # Variables
        ##################################################
        self.sps = sps = 4
        self.rep = rep = 3
        self.nfilts = nfilts = 32
        self.eb = eb = .22
        self.samp_rate = samp_rate = 1e6
        self.rx_rrc_taps = rx_rrc_taps = firdes.root_raised_cosine(nfilts, nfilts*sps,1.0, eb, 11*sps*nfilts)
        self.dec_rep = dec_rep = fec.repetition_decoder.make(MTU*8,rep, 0.5)
        self.constel = constel = digital.constellation_calcdist(digital.psk_2()[0], digital.psk_2()[1],
        2, 1, digital.constellation.AMPLITUDE_NORMALIZATION).base()
        self.constel.gen_soft_dec_lut(8)
        self.MTU_0 = MTU_0 = 1500

        ##################################################
        # Blocks
        ##################################################
        self.usersegment_tx_0 = usersegment_tx()
        self.qtgui_sink_x_0 = qtgui.sink_c(
            1024, #fftsize
            window.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            samp_rate, #bw
            "", #name
            True, #plotfreq
            True, #plotwaterfall
            True, #plottime
            True, #plotconst
            None # parent
        )
        self.qtgui_sink_x_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_win = sip.wrapinstance(self.qtgui_sink_x_0.qwidget(), Qt.QWidget)

        self.qtgui_sink_x_0.enable_rf_freq(False)

        self.top_layout.addWidget(self._qtgui_sink_x_0_win)
        self.fec_async_decoder_0_1 = fec.async_decoder(dec_rep, True, False, MTU)
        self.digital_map_bb_0_1 = digital.map_bb([-1,1])
        self.digital_crc32_async_bb_0 = digital.crc32_async_bb(True)
        self.digital_correlate_access_code_xx_ts_0 = digital.correlate_access_code_bb_ts("00011010110011111111110000011101",
          3, 'packet_len')
        self.blocks_unpack_k_bits_bb_0_0 = blocks.unpack_k_bits_bb(8)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_tagged_stream_to_pdu_1_0 = blocks.tagged_stream_to_pdu(blocks.float_t, 'packet_len')
        self.blocks_random_pdu_0 = blocks.random_pdu(80, 80, 0xFF, 2)
        self.blocks_message_strobe_0_0 = blocks.message_strobe(pmt.intern("TEST"), 1000)
        self.blocks_message_debug_0_0 = blocks.message_debug(True)
        self.blocks_message_debug_0 = blocks.message_debug(True)
        self.blocks_char_to_float_0_1_0 = blocks.char_to_float(1, 1)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_message_strobe_0_0, 'strobe'), (self.blocks_random_pdu_0, 'generate'))
        self.msg_connect((self.blocks_random_pdu_0, 'pdus'), (self.usersegment_tx_0, 'pdu'))
        self.msg_connect((self.blocks_tagged_stream_to_pdu_1_0, 'pdus'), (self.fec_async_decoder_0_1, 'in'))
        self.msg_connect((self.digital_crc32_async_bb_0, 'out'), (self.blocks_message_debug_0_0, 'print_pdu'))
        self.msg_connect((self.fec_async_decoder_0_1, 'out'), (self.digital_crc32_async_bb_0, 'in'))
        self.connect((self.blocks_char_to_float_0_1_0, 0), (self.blocks_tagged_stream_to_pdu_1_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.qtgui_sink_x_0, 0))
        self.connect((self.blocks_unpack_k_bits_bb_0_0, 0), (self.digital_correlate_access_code_xx_ts_0, 0))
        self.connect((self.digital_correlate_access_code_xx_ts_0, 0), (self.digital_map_bb_0_1, 0))
        self.connect((self.digital_map_bb_0_1, 0), (self.blocks_char_to_float_0_1_0, 0))
        self.connect((self.usersegment_tx_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.usersegment_tx_0, 1), (self.blocks_unpack_k_bits_bb_0_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "test_usersegment_tx")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_MTU(self):
        return self.MTU

    def set_MTU(self, MTU):
        self.MTU = MTU

    def get_frame_size(self):
        return self.frame_size

    def set_frame_size(self, frame_size):
        self.frame_size = frame_size

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.set_rx_rrc_taps(firdes.root_raised_cosine(self.nfilts, self.nfilts*self.sps, 1.0, self.eb, 11*self.sps*self.nfilts))

    def get_rep(self):
        return self.rep

    def set_rep(self, rep):
        self.rep = rep

    def get_nfilts(self):
        return self.nfilts

    def set_nfilts(self, nfilts):
        self.nfilts = nfilts
        self.set_rx_rrc_taps(firdes.root_raised_cosine(self.nfilts, self.nfilts*self.sps, 1.0, self.eb, 11*self.sps*self.nfilts))

    def get_eb(self):
        return self.eb

    def set_eb(self, eb):
        self.eb = eb
        self.set_rx_rrc_taps(firdes.root_raised_cosine(self.nfilts, self.nfilts*self.sps, 1.0, self.eb, 11*self.sps*self.nfilts))

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.qtgui_sink_x_0.set_frequency_range(0, self.samp_rate)

    def get_rx_rrc_taps(self):
        return self.rx_rrc_taps

    def set_rx_rrc_taps(self, rx_rrc_taps):
        self.rx_rrc_taps = rx_rrc_taps

    def get_dec_rep(self):
        return self.dec_rep

    def set_dec_rep(self, dec_rep):
        self.dec_rep = dec_rep

    def get_constel(self):
        return self.constel

    def set_constel(self, constel):
        self.constel = constel

    def get_MTU_0(self):
        return self.MTU_0

    def set_MTU_0(self, MTU_0):
        self.MTU_0 = MTU_0



def argument_parser():
    parser = ArgumentParser()
    parser.add_argument(
        "--MTU", dest="MTU", type=intx, default=1500,
        help="Set MTU [default=%(default)r]")
    parser.add_argument(
        "--frame-size", dest="frame_size", type=intx, default=30,
        help="Set Frame Size [default=%(default)r]")
    return parser


def main(top_block_cls=test_usersegment_tx, options=None):
    if options is None:
        options = argument_parser().parse_args()

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(MTU=options.MTU, frame_size=options.frame_size)

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()

if __name__ == '__main__':
    main()
