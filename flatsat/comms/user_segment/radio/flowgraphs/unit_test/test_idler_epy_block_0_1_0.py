"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
import pmt
from gnuradio import gr


class blk(gr.sync_block):  # other base classes are basic_block, decim_block, interp_block
    """Embedded Python Block example - a simple multiply const"""

    def __init__(self, pattern=[0x55]*10):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""
        gr.sync_block.__init__(
            self,
            name='HAS Idle Inserter',   # will show up in GRC
            in_sig=None,
            out_sig=[np.byte]
        )
        
        self.message_port_register_in(pmt.intern('pdu'))
        self.set_msg_handler(pmt.intern('pdu'), self.buffer_items)
        self.set_min_output_buffer(len(pattern))
        
        self.buffer = []
        
        # if an attribute with the same name as a parameter is found,
        # a callback is registered (properties work, too).
        self.pattern = pattern

    def work(self, input_items, output_items):
        #print(self.pattern)
        #output_items[0][0:len(self.pattern)] = self.pattern
        output_items[0][:] = self.pattern
        numItems = len(output_items[0])
        #print("Output Len:",len(output_items[0]))
        
        if len(self.buffer)<numItems:
            numItems = len(self.buffer)
        i = 0
        #print("Input/Output Size:", numItems,"/",len(output_items[0]))
        while i < numItems and numItems>0:
            #pdu = pmt.to_python(self.pdu)
            #data = pdu[1]
            #print("PDU:",pdu)
            #print("Data:",data)
            #framesize = len(frame)
            
            output_items[0][i] = self.buffer[0]
            self.buffer.pop(0)
            i = i+1
            #pmt.u8vector_ref(pdu, pmt.size(pdu)) → uint8_t
            #self.pdu = None
        #print("Buffered items:",len(self.frames))
        	
        return len(output_items[0])

    def buffer_items(self, msg):
        pdu = pmt.to_python(msg)
        items = pdu[1]
        
        self.buffer.extend(items)
        print("Buffer Len:",len(self.buffer))
    	
