#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Flatsat RX Test
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
from gnuradio import eng_notation
from gnuradio import qtgui
from gnuradio.filter import firdes
import sip
from flatsat_rx import flatsat_rx  # grc-generated hier_block
from gnuradio import analog
from gnuradio import blocks
import pmt
from gnuradio import digital
from gnuradio import filter
from gnuradio import gr
from gnuradio.fft import window
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import uhd
import time
from gnuradio.filter import pfb
from gnuradio.qtgui import Range, RangeWidget
from PyQt5 import QtCore
from usersegment_tx import usersegment_tx  # grc-generated hier_block
import satellites.hier
import test_flatsat_rx_epy_block_0_0_0_0_0_0 as epy_block_0_0_0_0_0_0  # embedded python block



from gnuradio import qtgui

class test_flatsat_rx(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Flatsat RX Test", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Flatsat RX Test")
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

        self.settings = Qt.QSettings("GNU Radio", "test_flatsat_rx")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 1e6
        self.tx_samp_rate = tx_samp_rate = samp_rate
        self.num_chan = num_chan = 18
        self.freq = freq = 901e6
        self.tx_samp_rate_0 = tx_samp_rate_0 = samp_rate
        self.tx_gain = tx_gain = 70
        self.tx_freq = tx_freq = freq
        self.timing_bw = timing_bw = .045
        self.strobe_delay = strobe_delay = 1000
        self.squelch = squelch = -70
        self.sps = sps = 4
        self.rx_switch = rx_switch = 0
        self.rx_samp_rate_0 = rx_samp_rate_0 = samp_rate
        self.rx_samp_rate = rx_samp_rate = samp_rate
        self.rx_gain = rx_gain = 0
        self.rx_freq = rx_freq = freq
        self.lpf_taps = lpf_taps = firdes.low_pass(1.0, samp_rate, samp_rate/num_chan*0.8,5e3, window.WIN_HAMMING, 6.76)
        self.eb = eb = .22
        self.deviation = deviation = 1.5
        self.damping = damping = 1.0
        self.constel = constel = digital.constellation_bpsk().base()
        self.constel.gen_soft_dec_lut(8)
        self.chan_samp_rate = chan_samp_rate = tx_samp_rate/(num_chan)
        self.chan_cutoff = chan_cutoff = samp_rate/num_chan*0.8

        ##################################################
        # Blocks
        ##################################################
        self._tx_gain_tool_bar = Qt.QToolBar(self)
        self._tx_gain_tool_bar.addWidget(Qt.QLabel('TX Gain' + ": "))
        self._tx_gain_line_edit = Qt.QLineEdit(str(self.tx_gain))
        self._tx_gain_tool_bar.addWidget(self._tx_gain_line_edit)
        self._tx_gain_line_edit.returnPressed.connect(
            lambda: self.set_tx_gain(int(str(self._tx_gain_line_edit.text()))))
        self.top_grid_layout.addWidget(self._tx_gain_tool_bar, 10, 0, 1, 1)
        for r in range(10, 11):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._timing_bw_range = Range(0.001, 0.1, 0.001, .045, 200)
        self._timing_bw_win = RangeWidget(self._timing_bw_range, self.set_timing_bw, 'Timing BW', "counter_slider", float, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._timing_bw_win)
        self.tab1 = Qt.QTabWidget()
        self.tab1_widget_0 = Qt.QWidget()
        self.tab1_layout_0 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab1_widget_0)
        self.tab1_grid_layout_0 = Qt.QGridLayout()
        self.tab1_layout_0.addLayout(self.tab1_grid_layout_0)
        self.tab1.addTab(self.tab1_widget_0, 'TX')
        self.tab1_widget_1 = Qt.QWidget()
        self.tab1_layout_1 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab1_widget_1)
        self.tab1_grid_layout_1 = Qt.QGridLayout()
        self.tab1_layout_1.addLayout(self.tab1_grid_layout_1)
        self.tab1.addTab(self.tab1_widget_1, 'RX')
        self.top_grid_layout.addWidget(self.tab1, 0, 0, 6, 6)
        for r in range(0, 6):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 6):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.tab0 = Qt.QTabWidget()
        self.tab0_widget_0 = Qt.QWidget()
        self.tab0_layout_0 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab0_widget_0)
        self.tab0_grid_layout_0 = Qt.QGridLayout()
        self.tab0_layout_0.addLayout(self.tab0_grid_layout_0)
        self.tab0.addTab(self.tab0_widget_0, 'AGC')
        self.tab0_widget_1 = Qt.QWidget()
        self.tab0_layout_1 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab0_widget_1)
        self.tab0_grid_layout_1 = Qt.QGridLayout()
        self.tab0_layout_1.addLayout(self.tab0_grid_layout_1)
        self.tab0.addTab(self.tab0_widget_1, 'Correlation')
        self.tab0_widget_2 = Qt.QWidget()
        self.tab0_layout_2 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab0_widget_2)
        self.tab0_grid_layout_2 = Qt.QGridLayout()
        self.tab0_layout_2.addLayout(self.tab0_grid_layout_2)
        self.tab0.addTab(self.tab0_widget_2, 'Timing')
        self.tab0_widget_3 = Qt.QWidget()
        self.tab0_layout_3 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab0_widget_3)
        self.tab0_grid_layout_3 = Qt.QGridLayout()
        self.tab0_layout_3.addLayout(self.tab0_grid_layout_3)
        self.tab0.addTab(self.tab0_widget_3, 'Equalizer')
        self.tab0_widget_4 = Qt.QWidget()
        self.tab0_layout_4 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab0_widget_4)
        self.tab0_grid_layout_4 = Qt.QGridLayout()
        self.tab0_layout_4.addLayout(self.tab0_grid_layout_4)
        self.tab0.addTab(self.tab0_widget_4, 'Costas')
        self.tab0_widget_5 = Qt.QWidget()
        self.tab0_layout_5 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.tab0_widget_5)
        self.tab0_grid_layout_5 = Qt.QGridLayout()
        self.tab0_layout_5.addLayout(self.tab0_grid_layout_5)
        self.tab0.addTab(self.tab0_widget_5, 'Tab 5')
        self.top_grid_layout.addWidget(self.tab0, 0, 6, 6, 6)
        for r in range(0, 6):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(6, 12):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._strobe_delay_range = Range(1, 1000, 1, 1000, 200)
        self._strobe_delay_win = RangeWidget(self._strobe_delay_range, self.set_strobe_delay, 'Strobe Delay', "counter_slider", int, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._strobe_delay_win)
        self._squelch_range = Range(-100, 0, 1, -70, 200)
        self._squelch_win = RangeWidget(self._squelch_range, self.set_squelch, 'Power Squelch', "counter_slider", float, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._squelch_win)
        self._rx_switch_range = Range(0, 1, 1, 0, 200)
        self._rx_switch_win = RangeWidget(self._rx_switch_range, self.set_rx_switch, 'RX On/Off', "dial", float, QtCore.Qt.Horizontal)
        self.top_grid_layout.addWidget(self._rx_switch_win, 14, 0, 1, 1)
        for r in range(14, 15):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._rx_gain_tool_bar = Qt.QToolBar(self)
        self._rx_gain_tool_bar.addWidget(Qt.QLabel('RX Gain' + ": "))
        self._rx_gain_line_edit = Qt.QLineEdit(str(self.rx_gain))
        self._rx_gain_tool_bar.addWidget(self._rx_gain_line_edit)
        self._rx_gain_line_edit.returnPressed.connect(
            lambda: self.set_rx_gain(int(str(self._rx_gain_line_edit.text()))))
        self.top_grid_layout.addWidget(self._rx_gain_tool_bar, 11, 0, 1, 1)
        for r in range(11, 12):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self._deviation_range = Range(0.1, 2, 0.1, 1.5, 200)
        self._deviation_win = RangeWidget(self._deviation_range, self.set_deviation, 'Deviation', "counter_slider", float, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._deviation_win)
        self._damping_range = Range(0.1, 2, 0.1, 1.0, 200)
        self._damping_win = RangeWidget(self._damping_range, self.set_damping, 'Damping', "counter_slider", float, QtCore.Qt.Horizontal)
        self.top_layout.addWidget(self._damping_win)
        self.usersegment_tx_0 = usersegment_tx()
        self.usersegment_tx_0.set_max_output_buffer(4096)
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
        self.uhd_usrp_sink_0 = uhd.usrp_sink(
            ",".join(("", "")),
            uhd.stream_args(
                cpu_format="fc32",
                args='',
                channels=list(range(0,1)),
            ),
            '',
        )
        self.uhd_usrp_sink_0.set_samp_rate(tx_samp_rate)
        self.uhd_usrp_sink_0.set_time_now(uhd.time_spec(time.time()), uhd.ALL_MBOARDS)

        self.uhd_usrp_sink_0.set_center_freq(tx_freq, 0)
        self.uhd_usrp_sink_0.set_antenna('TX/RX', 0)
        self.uhd_usrp_sink_0.set_gain(tx_gain, 0)
        self.satellites_rms_agc_0 = satellites.hier.rms_agc(alpha=1e-2, reference=1.0)
        self.qtgui_sink_x_1 = qtgui.sink_c(
            1024, #fftsize
            window.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            samp_rate, #bw
            'TX', #name
            True, #plotfreq
            True, #plotwaterfall
            True, #plottime
            True, #plotconst
            None # parent
        )
        self.qtgui_sink_x_1.set_update_time(1.0/10)
        self._qtgui_sink_x_1_win = sip.wrapinstance(self.qtgui_sink_x_1.qwidget(), Qt.QWidget)

        self.qtgui_sink_x_1.enable_rf_freq(False)

        self.tab1_layout_0.addWidget(self._qtgui_sink_x_1_win)
        self.qtgui_sink_x_0_0_0_0_1 = qtgui.sink_c(
            1024*4, #fftsize
            window.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            chan_samp_rate, #bw
            'Costas', #name
            True, #plotfreq
            False, #plotwaterfall
            True, #plottime
            True, #plotconst
            None # parent
        )
        self.qtgui_sink_x_0_0_0_0_1.set_update_time(1.0/10)
        self._qtgui_sink_x_0_0_0_0_1_win = sip.wrapinstance(self.qtgui_sink_x_0_0_0_0_1.qwidget(), Qt.QWidget)

        self.qtgui_sink_x_0_0_0_0_1.enable_rf_freq(False)

        self.tab0_layout_4.addWidget(self._qtgui_sink_x_0_0_0_0_1_win)
        self.qtgui_sink_x_0_0_0_0 = qtgui.sink_c(
            1024*4, #fftsize
            window.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            chan_samp_rate, #bw
            'Equalizer', #name
            True, #plotfreq
            False, #plotwaterfall
            True, #plottime
            True, #plotconst
            None # parent
        )
        self.qtgui_sink_x_0_0_0_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_0_0_0_win = sip.wrapinstance(self.qtgui_sink_x_0_0_0_0.qwidget(), Qt.QWidget)

        self.qtgui_sink_x_0_0_0_0.enable_rf_freq(False)

        self.tab0_layout_3.addWidget(self._qtgui_sink_x_0_0_0_0_win)
        self.qtgui_sink_x_0_0_0 = qtgui.sink_c(
            1024*4, #fftsize
            window.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            chan_samp_rate, #bw
            'Timing', #name
            True, #plotfreq
            False, #plotwaterfall
            True, #plottime
            True, #plotconst
            None # parent
        )
        self.qtgui_sink_x_0_0_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_0_0_win = sip.wrapinstance(self.qtgui_sink_x_0_0_0.qwidget(), Qt.QWidget)

        self.qtgui_sink_x_0_0_0.enable_rf_freq(False)

        self.tab0_layout_2.addWidget(self._qtgui_sink_x_0_0_0_win)
        self.qtgui_sink_x_0_0 = qtgui.sink_c(
            1024*4, #fftsize
            window.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            chan_samp_rate, #bw
            'AGC', #name
            True, #plotfreq
            False, #plotwaterfall
            True, #plottime
            True, #plotconst
            None # parent
        )
        self.qtgui_sink_x_0_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_0_win = sip.wrapinstance(self.qtgui_sink_x_0_0.qwidget(), Qt.QWidget)

        self.qtgui_sink_x_0_0.enable_rf_freq(False)

        self.tab0_layout_0.addWidget(self._qtgui_sink_x_0_0_win)
        self.qtgui_sink_x_0 = qtgui.sink_c(
            1024*32, #fftsize
            window.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            samp_rate, #bw
            'Wideband RX', #name
            True, #plotfreq
            True, #plotwaterfall
            True, #plottime
            True, #plotconst
            None # parent
        )
        self.qtgui_sink_x_0.set_update_time(1.0/10)
        self._qtgui_sink_x_0_win = sip.wrapinstance(self.qtgui_sink_x_0.qwidget(), Qt.QWidget)

        self.qtgui_sink_x_0.enable_rf_freq(False)

        self.tab1_layout_1.addWidget(self._qtgui_sink_x_0_win)
        self.pfb_synthesizer_ccf_0 = filter.pfb_synthesizer_ccf(
            num_chan,
            lpf_taps,
            False)
        self.pfb_synthesizer_ccf_0.set_channel_map([0,10,12,14,16,1,3,5,7,11,13,15,17,2,4,6,8,9])
        self.pfb_synthesizer_ccf_0.declare_sample_delay(0)
        self.pfb_channelizer_ccf_0 = pfb.channelizer_ccf(
            num_chan,
            lpf_taps,
            1,
            100)
        self.pfb_channelizer_ccf_0.set_channel_map([0,10,12,14,16,1,3,5,7,11,13,15,17,2,4,6,8,9])
        self.pfb_channelizer_ccf_0.declare_sample_delay(0)
        self.flatsat_rx_0 = flatsat_rx(
            damping=damping,
            deviation=deviation,
            timing_bw=timing_bw,
        )
        self.epy_block_0_0_0_0_0_0 = epy_block_0_0_0_0_0_0.blk(enabled=rx_switch)
        self.blocks_socket_pdu_0_0_2_2 = blocks.socket_pdu('UDP_SERVER', '', '3008', 10000, False)
        self.blocks_socket_pdu_0_0_2_1 = blocks.socket_pdu('UDP_SERVER', '', '3007', 10000, False)
        self.blocks_socket_pdu_0_0_2_0 = blocks.socket_pdu('UDP_SERVER', '', '3006', 10000, False)
        self.blocks_socket_pdu_0_0_2 = blocks.socket_pdu('UDP_SERVER', '', '3005', 10000, False)
        self.blocks_socket_pdu_0_0_1 = blocks.socket_pdu('UDP_SERVER', '', '3004', 10000, False)
        self.blocks_socket_pdu_0_0_0 = blocks.socket_pdu('UDP_SERVER', '', '3003', 10000, False)
        self.blocks_socket_pdu_0_0 = blocks.socket_pdu('UDP_SERVER', '', '3002', 10000, False)
        self.blocks_socket_pdu_0 = blocks.socket_pdu('UDP_SERVER', '', '3001', 10000, False)
        self.blocks_random_pdu_0 = blocks.random_pdu(100, 100, 0xFF, 2)
        self.blocks_null_source_0_0_0 = blocks.null_source(gr.sizeof_gr_complex*1)
        self.blocks_null_source_0_0 = blocks.null_source(gr.sizeof_gr_complex*1)
        self.blocks_null_source_0 = blocks.null_source(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_multiply_const_vxx_0 = blocks.multiply_const_cc(0.5)
        self.blocks_message_strobe_0_0 = blocks.message_strobe(pmt.intern("TEST"), strobe_delay)
        self.blocks_message_debug_0_0 = blocks.message_debug(True)
        self.analog_pwr_squelch_xx_0 = analog.pwr_squelch_cc(squelch, 1e-4, 0, True)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.blocks_message_strobe_0_0, 'strobe'), (self.blocks_random_pdu_0, 'generate'))
        self.msg_connect((self.blocks_random_pdu_0, 'pdus'), (self.epy_block_0_0_0_0_0_0, 'tx_in'))
        self.msg_connect((self.blocks_random_pdu_0, 'pdus'), (self.usersegment_tx_0, 'pdu'))
        self.msg_connect((self.blocks_socket_pdu_0, 'pdus'), (self.usersegment_tx_0, 'pdu'))
        self.msg_connect((self.flatsat_rx_0, 'pdu'), (self.blocks_message_debug_0_0, 'print_pdu'))
        self.msg_connect((self.flatsat_rx_0, 'pdu'), (self.epy_block_0_0_0_0_0_0, 'rx_in'))
        self.connect((self.analog_pwr_squelch_xx_0, 0), (self.satellites_rms_agc_0, 0))
        self.connect((self.blocks_multiply_const_vxx_0, 0), (self.uhd_usrp_sink_0, 0))
        self.connect((self.blocks_null_source_0, 0), (self.pfb_synthesizer_ccf_0, 0))
        self.connect((self.blocks_null_source_0_0, 11), (self.pfb_synthesizer_ccf_0, 16))
        self.connect((self.blocks_null_source_0_0, 3), (self.pfb_synthesizer_ccf_0, 8))
        self.connect((self.blocks_null_source_0_0, 2), (self.pfb_synthesizer_ccf_0, 7))
        self.connect((self.blocks_null_source_0_0, 0), (self.pfb_synthesizer_ccf_0, 5))
        self.connect((self.blocks_null_source_0_0, 12), (self.pfb_synthesizer_ccf_0, 17))
        self.connect((self.blocks_null_source_0_0, 8), (self.pfb_synthesizer_ccf_0, 13))
        self.connect((self.blocks_null_source_0_0, 10), (self.pfb_synthesizer_ccf_0, 15))
        self.connect((self.blocks_null_source_0_0, 4), (self.pfb_synthesizer_ccf_0, 9))
        self.connect((self.blocks_null_source_0_0, 7), (self.pfb_synthesizer_ccf_0, 12))
        self.connect((self.blocks_null_source_0_0, 9), (self.pfb_synthesizer_ccf_0, 14))
        self.connect((self.blocks_null_source_0_0, 5), (self.pfb_synthesizer_ccf_0, 10))
        self.connect((self.blocks_null_source_0_0, 6), (self.pfb_synthesizer_ccf_0, 11))
        self.connect((self.blocks_null_source_0_0, 1), (self.pfb_synthesizer_ccf_0, 6))
        self.connect((self.blocks_null_source_0_0_0, 1), (self.pfb_synthesizer_ccf_0, 3))
        self.connect((self.blocks_null_source_0_0_0, 2), (self.pfb_synthesizer_ccf_0, 4))
        self.connect((self.blocks_null_source_0_0_0, 0), (self.pfb_synthesizer_ccf_0, 2))
        self.connect((self.flatsat_rx_0, 0), (self.qtgui_sink_x_0_0_0, 0))
        self.connect((self.flatsat_rx_0, 1), (self.qtgui_sink_x_0_0_0_0, 0))
        self.connect((self.flatsat_rx_0, 2), (self.qtgui_sink_x_0_0_0_0_1, 0))
        self.connect((self.pfb_channelizer_ccf_0, 1), (self.analog_pwr_squelch_xx_0, 0))
        self.connect((self.pfb_channelizer_ccf_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.pfb_channelizer_ccf_0, 9), (self.blocks_null_sink_0_0, 7))
        self.connect((self.pfb_channelizer_ccf_0, 12), (self.blocks_null_sink_0_0, 10))
        self.connect((self.pfb_channelizer_ccf_0, 3), (self.blocks_null_sink_0_0, 1))
        self.connect((self.pfb_channelizer_ccf_0, 4), (self.blocks_null_sink_0_0, 2))
        self.connect((self.pfb_channelizer_ccf_0, 16), (self.blocks_null_sink_0_0, 14))
        self.connect((self.pfb_channelizer_ccf_0, 14), (self.blocks_null_sink_0_0, 12))
        self.connect((self.pfb_channelizer_ccf_0, 7), (self.blocks_null_sink_0_0, 5))
        self.connect((self.pfb_channelizer_ccf_0, 5), (self.blocks_null_sink_0_0, 3))
        self.connect((self.pfb_channelizer_ccf_0, 2), (self.blocks_null_sink_0_0, 0))
        self.connect((self.pfb_channelizer_ccf_0, 11), (self.blocks_null_sink_0_0, 9))
        self.connect((self.pfb_channelizer_ccf_0, 17), (self.blocks_null_sink_0_0, 15))
        self.connect((self.pfb_channelizer_ccf_0, 15), (self.blocks_null_sink_0_0, 13))
        self.connect((self.pfb_channelizer_ccf_0, 8), (self.blocks_null_sink_0_0, 6))
        self.connect((self.pfb_channelizer_ccf_0, 10), (self.blocks_null_sink_0_0, 8))
        self.connect((self.pfb_channelizer_ccf_0, 6), (self.blocks_null_sink_0_0, 4))
        self.connect((self.pfb_channelizer_ccf_0, 13), (self.blocks_null_sink_0_0, 11))
        self.connect((self.pfb_synthesizer_ccf_0, 0), (self.blocks_multiply_const_vxx_0, 0))
        self.connect((self.satellites_rms_agc_0, 0), (self.flatsat_rx_0, 0))
        self.connect((self.satellites_rms_agc_0, 0), (self.qtgui_sink_x_0_0, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.pfb_channelizer_ccf_0, 0))
        self.connect((self.uhd_usrp_source_0, 0), (self.qtgui_sink_x_0, 0))
        self.connect((self.usersegment_tx_0, 0), (self.pfb_synthesizer_ccf_0, 1))
        self.connect((self.usersegment_tx_0, 0), (self.qtgui_sink_x_1, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "test_flatsat_rx")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.set_chan_cutoff(self.samp_rate/self.num_chan*0.8)
        self.set_lpf_taps(firdes.low_pass(1.0, self.samp_rate, self.samp_rate/self.num_chan*0.8, 5e3, window.WIN_HAMMING, 6.76))
        self.set_rx_samp_rate(self.samp_rate)
        self.set_rx_samp_rate_0(self.samp_rate)
        self.set_tx_samp_rate(self.samp_rate)
        self.set_tx_samp_rate_0(self.samp_rate)
        self.qtgui_sink_x_0.set_frequency_range(0, self.samp_rate)
        self.qtgui_sink_x_1.set_frequency_range(0, self.samp_rate)

    def get_tx_samp_rate(self):
        return self.tx_samp_rate

    def set_tx_samp_rate(self, tx_samp_rate):
        self.tx_samp_rate = tx_samp_rate
        self.set_chan_samp_rate(self.tx_samp_rate/(self.num_chan))
        self.uhd_usrp_sink_0.set_samp_rate(self.tx_samp_rate)

    def get_num_chan(self):
        return self.num_chan

    def set_num_chan(self, num_chan):
        self.num_chan = num_chan
        self.set_chan_cutoff(self.samp_rate/self.num_chan*0.8)
        self.set_chan_samp_rate(self.tx_samp_rate/(self.num_chan))
        self.set_lpf_taps(firdes.low_pass(1.0, self.samp_rate, self.samp_rate/self.num_chan*0.8, 5e3, window.WIN_HAMMING, 6.76))

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.set_rx_freq(self.freq)
        self.set_tx_freq(self.freq)

    def get_tx_samp_rate_0(self):
        return self.tx_samp_rate_0

    def set_tx_samp_rate_0(self, tx_samp_rate_0):
        self.tx_samp_rate_0 = tx_samp_rate_0

    def get_tx_gain(self):
        return self.tx_gain

    def set_tx_gain(self, tx_gain):
        self.tx_gain = tx_gain
        Qt.QMetaObject.invokeMethod(self._tx_gain_line_edit, "setText", Qt.Q_ARG("QString", str(self.tx_gain)))
        self.uhd_usrp_sink_0.set_gain(self.tx_gain, 0)

    def get_tx_freq(self):
        return self.tx_freq

    def set_tx_freq(self, tx_freq):
        self.tx_freq = tx_freq
        self.uhd_usrp_sink_0.set_center_freq(self.tx_freq, 0)

    def get_timing_bw(self):
        return self.timing_bw

    def set_timing_bw(self, timing_bw):
        self.timing_bw = timing_bw
        self.flatsat_rx_0.set_timing_bw(self.timing_bw)

    def get_strobe_delay(self):
        return self.strobe_delay

    def set_strobe_delay(self, strobe_delay):
        self.strobe_delay = strobe_delay
        self.blocks_message_strobe_0_0.set_period(self.strobe_delay)

    def get_squelch(self):
        return self.squelch

    def set_squelch(self, squelch):
        self.squelch = squelch
        self.analog_pwr_squelch_xx_0.set_threshold(self.squelch)

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps

    def get_rx_switch(self):
        return self.rx_switch

    def set_rx_switch(self, rx_switch):
        self.rx_switch = rx_switch
        self.epy_block_0_0_0_0_0_0.enabled = self.rx_switch

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
        Qt.QMetaObject.invokeMethod(self._rx_gain_line_edit, "setText", Qt.Q_ARG("QString", str(self.rx_gain)))
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
        self.pfb_synthesizer_ccf_0.set_taps(self.lpf_taps)

    def get_eb(self):
        return self.eb

    def set_eb(self, eb):
        self.eb = eb

    def get_deviation(self):
        return self.deviation

    def set_deviation(self, deviation):
        self.deviation = deviation
        self.flatsat_rx_0.set_deviation(self.deviation)

    def get_damping(self):
        return self.damping

    def set_damping(self, damping):
        self.damping = damping
        self.flatsat_rx_0.set_damping(self.damping)

    def get_constel(self):
        return self.constel

    def set_constel(self, constel):
        self.constel = constel

    def get_chan_samp_rate(self):
        return self.chan_samp_rate

    def set_chan_samp_rate(self, chan_samp_rate):
        self.chan_samp_rate = chan_samp_rate
        self.qtgui_sink_x_0_0.set_frequency_range(0, self.chan_samp_rate)
        self.qtgui_sink_x_0_0_0.set_frequency_range(0, self.chan_samp_rate)
        self.qtgui_sink_x_0_0_0_0.set_frequency_range(0, self.chan_samp_rate)
        self.qtgui_sink_x_0_0_0_0_1.set_frequency_range(0, self.chan_samp_rate)

    def get_chan_cutoff(self):
        return self.chan_cutoff

    def set_chan_cutoff(self, chan_cutoff):
        self.chan_cutoff = chan_cutoff




def main(top_block_cls=test_flatsat_rx, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

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
