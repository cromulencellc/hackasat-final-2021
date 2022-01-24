"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
import pmt
from gnuradio import gr


class blk(gr.basic_block):
	def __init__(self, enabled=0):
		gr.basic_block.__init__(
			self,
			name="Packet Counter",
			in_sig=None,
			out_sig=None)

		self.message_port_register_in(pmt.intern('tx_in'))
		self.message_port_register_in(pmt.intern('rx_in'))
		
		self.set_msg_handler(pmt.intern('tx_in'), self.tx)
		self.set_msg_handler(pmt.intern('rx_in'), self.rx)
		
		self.tx_count = 0
		self.rx_count = 0
		self.enabled = enabled

	def tx(self, msg):
		if self.enabled:
			self.tx_count += 1
		else:
			self.rx_count = 0
			self.tx_count = 0
	
	def rx(self, msg):
		if self.enabled and self.tx_count:
			self.rx_count += 1	
			print("Packets RX/TX: ",self.rx_count,"/",self.tx_count, " (",self.rx_count/self.tx_count*100,"%)",sep='')
